#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
public:
    explicit ThreadPool(size_t threads = std::thread::hardware_concurrency()) { initialize(threads); }
    ~ThreadPool() { shutdown(); }

    void enqueue(std::function<void()>&& task) {
        {
            std::unique_lock lock(m_queueMutex);
            m_tasks.push(std::move(task));
        }
        m_condition.notify_one();
    }

    void initialize(size_t threads) {
        for (size_t i = 0; i < threads; ++i) {
            m_workers.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock lock(m_queueMutex);
                        m_condition.wait(lock, [this] { return m_stop || !m_tasks.empty(); });
                        if (m_stop && m_tasks.empty()) return;
                        task = std::move(m_tasks.front());
                        m_tasks.pop();
                    }
                    task();
                }
            });
        }
    }

private:
    void shutdown() {
        {
            std::unique_lock lock(m_queueMutex);
            m_stop = true;
        }
        m_condition.notify_all();
        for (auto& worker : m_workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
        m_workers.clear();
    }

private:
    std::vector<std::thread> m_workers;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_queueMutex;
    std::condition_variable m_condition;
    bool m_stop = false;
};