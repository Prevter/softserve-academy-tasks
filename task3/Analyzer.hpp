#pragma once
#include <cstddef>
#include <filesystem>
#include <span>

struct FileInfo {
    size_t blankLines = 0;
    size_t commentLines = 0;
    size_t codeLines = 0;

    [[nodiscard]] size_t totalLines() const {
        return blankLines + commentLines + codeLines;
    }

    constexpr FileInfo& operator+=(FileInfo const& other) {
        blankLines += other.blankLines;
        commentLines += other.commentLines;
        codeLines += other.codeLines;
        return *this;
    }
};

struct TypeStats {
    FileInfo info;
    size_t fileCount = 0;
};

std::optional<FileInfo> analyze(std::filesystem::path const& path);