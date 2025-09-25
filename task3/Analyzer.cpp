#include "Analyzer.hpp"

#include <fstream>
#include <iostream>
#include <print>
#include <span>

std::optional<FileInfo> analyze(std::filesystem::path const& path) {
    std::ifstream file;
    file.rdbuf()->pubsetbuf(nullptr, 0);
    file.open(path, std::ios::binary);
    if (!file) {
        std::println(std::cerr, "Failed to open file: {}", path.string());
        return std::nullopt;
    }

    constexpr size_t BUFFER_SIZE = 64 * 1024; // 64 KiB
    std::array<char, BUFFER_SIZE> buffer;

    auto ptr = buffer.data();
    auto end = buffer.data() + buffer.size();

    auto const readNextChunk = [&]() -> bool {
        if (!file || file.eof()) return false;
        file.read(buffer.data(), buffer.size());
        ptr = buffer.data(); // reset pointer
        end = buffer.data() + file.gcount();
        return file.gcount() > 0;
    };

    auto const peek = [&]() -> char {
        if (ptr >= end && !readNextChunk()) return '\0';
        return *ptr;
    };

    auto const advance = [&]() -> char {
        if (ptr >= end && !readNextChunk()) return '\0';
        return *ptr++;
    };

    FileInfo fi;
    bool inBlockComment = false; // /* ... */
    bool inLineComment = false;  // // ...
    bool inString = false;       // " ... "
    bool inChar = false;         // ' ... '
    bool lineNotBlank = false;   // line has non-whitespace characters
    bool hasCode = false;        // line has code (not comment or whitespace)

    auto const finishLine = [&] {
        if (!lineNotBlank) {
            ++fi.blankLines;
        } else if (hasCode) {
            ++fi.codeLines;
        } else {
            ++fi.commentLines;
        }
        lineNotBlank = false;
        hasCode = false;
    };

    readNextChunk(); // initial read

    do {
        char ch = advance();
        if (ch == '\0') break;

        switch (ch) {
            case '\n': {
                finishLine();
                inLineComment = false;
                break;
            }

            case '/': {
                if (inString || inChar) {
                    lineNotBlank = true;
                    hasCode = true;
                    break;
                }

                // check for /*
                if (!inBlockComment && peek() == '*') {
                    inBlockComment = true;
                    lineNotBlank = true;
                    advance();
                    break;
                }

                // check for //
                if (!inBlockComment && peek() == '/') {
                    inLineComment = true;
                    lineNotBlank = true;
                    advance();
                    break;
                }

                if (!inBlockComment && !inLineComment) {
                    lineNotBlank = true;
                    hasCode = true;
                }

                break;
            }

            case '*': {
                // check for */
                if (inBlockComment && peek() == '/') {
                    inBlockComment = false;
                    lineNotBlank = true;
                    advance();
                    break;
                }

                if (!inBlockComment && !inLineComment) {
                    lineNotBlank = true;
                    hasCode = true;
                } else {
                    lineNotBlank = true;
                }

                break;
            }

            case '"': {
                if (inLineComment || inBlockComment || inChar) {
                    lineNotBlank = true;
                    if (!inLineComment && !inBlockComment) hasCode = true;
                    break;
                }

                inString = !inString;
                lineNotBlank = true;
                hasCode = true;
                break;
            }

            case '\'': {
                if (inLineComment || inBlockComment || inString) {
                    lineNotBlank = true;
                    if (!inLineComment && !inBlockComment) hasCode = true;
                    break;
                }

                inChar = !inChar;
                lineNotBlank = true;
                hasCode = true;
                break;
            }

            case '\\': {
                if (inLineComment || inBlockComment) {
                    lineNotBlank = true;
                    break;
                }

                if (inString || inChar) {
                    lineNotBlank = true;
                    hasCode = true;
                    if (peek()) advance(); // skip escaped char
                } else if (peek() == '\n') { // line breaks in macros
                    finishLine();
                    advance();
                } else {
                    lineNotBlank = true;
                    hasCode = true;
                }

                break;
            }

            case ' ':
            case '\t':
            case '\r': {
                // whitespace
                break;
            }

            default: {
                if (!inBlockComment && !inLineComment) {
                    lineNotBlank = true;
                    hasCode = true;
                } else {
                    lineNotBlank = true;
                }
                break;
            }
        }
    } while (true);

    if (lineNotBlank) {
        finishLine();
    }

    return fi;
}