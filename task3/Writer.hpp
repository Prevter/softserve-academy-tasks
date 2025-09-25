#pragma once
#include <format>
#include <fstream>
#include <iostream>

class Writer {
public:
    explicit Writer() : m_output(&std::cout) {}
    explicit Writer(std::ofstream& file) : m_output(&file) {}
    explicit Writer(std::ostream& output) : m_output(&output) {}

    template <typename... Args>
    void write(std::format_string<Args...> fmt, Args&&... args) const {
        if constexpr (sizeof...(Args) == 0) *m_output << fmt.get();
        else *m_output << std::format(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void writeln(std::format_string<Args...> fmt, Args&&... args) const {
        if constexpr (sizeof...(Args) == 0) *m_output << fmt.get() << '\n';
        else *m_output << std::format(fmt, std::forward<Args>(args)...) << '\n';
    }

    void flush() const {
        m_output->flush();
    }

private:
    std::ostream* m_output;
};
