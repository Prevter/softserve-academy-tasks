#include "ArgParser.hpp"

bool ArgParser::hasOption(std::string_view option) const {
    for (int i = 1; i < m_argc; ++i) {
        auto arg = std::string_view(m_argv[i]);
        if (arg.starts_with(option) && arg.size() > option.size() && arg[option.size()] == '=') {
            return true;
        }
    }
    return false;
}

bool ArgParser::hasFlag(std::string_view flag) const {
    for (int i = 1; i < m_argc; ++i) {
        if (std::string_view(m_argv[i]) == flag) {
            return true;
        }
    }
    return false;
}

std::string_view ArgParser::getOptionValue(std::string_view option) const {
    for (int i = 1; i < m_argc; ++i) {
        auto arg = std::string_view(m_argv[i]);
        if (arg.starts_with(option) && arg.size() > option.size() && arg[option.size()] == '=') {
            return arg.substr(option.size() + 1);
        }
    }
    return {};
}

ArgParser::PositionalArgs ArgParser::positionalArgs() const {
    int posCount = 0;
    for (int i = 1; i < m_argc; ++i) {
        if (m_argv[i][0] != '-') {
            ++posCount;
        }
    }

    return PositionalArgs{m_argv + (m_argc - posCount), posCount};
}
