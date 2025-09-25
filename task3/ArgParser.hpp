#pragma once
#include <string_view>

class ArgParser {
public:
    ArgParser(int argc, char *argv[]) : m_argv(argv), m_argc(argc) {}
    ~ArgParser() = default;

    [[nodiscard]] bool hasOption(std::string_view option) const;
    [[nodiscard]] bool hasFlag(std::string_view flag) const;
    [[nodiscard]] std::string_view getOptionValue(std::string_view option) const;

    struct PositionalArgs {
        char** args;
        int count;

        char* operator[](int index) const {
            if (index < 0 || index >= count) return nullptr;
            return args[index];
        }

        [[nodiscard]] int size() const { return count; }

        struct Iterator {
            char** current;
            int remaining;

            char const* operator*() const { return *current; }

            Iterator& operator++() {
                ++current;
                --remaining;
                return *this;
            }

            bool operator!=(Iterator const& other) const {
                return remaining != other.remaining;
            }
        };

        [[nodiscard]] Iterator begin() const { return Iterator{args, count}; }
        [[nodiscard]] Iterator end() const { return Iterator{args + count, 0}; }
    };

    [[nodiscard]] PositionalArgs positionalArgs() const;

private:
    char** m_argv;
    int m_argc;
};
