#include <chrono>
#include <filesystem>
#include <fstream>
#include <map>
#include <print>
#include <unordered_map>
#include <vector>

#include "Analyzer.hpp"
#include "ArgParser.hpp"
#include "ThreadPool.hpp"
#include "Writer.hpp"

enum class FileType {
    Unknown,
    Cpp,          // .cpp, .cxx, .cc
    CppHeader,    // .hpp, .hxx, .hh
    C,            // .c
    CHeader,      // .h
    ObjectiveCpp, // .mm
};

constexpr std::string_view fileTypeToString(FileType type) {
    switch (type) {
        case FileType::Unknown: return "Unknown";
        case FileType::Cpp: return "C++";
        case FileType::CppHeader: return "C++ Header";
        case FileType::C: return "C";
        case FileType::CHeader: return "C Header";
        case FileType::ObjectiveCpp: return "Objective-C++";
        default: return "Invalid Type";
    }
}

FileType getFileType(std::filesystem::path const& path) {
    if (!path.has_extension()) return FileType::Unknown;

    static std::unordered_map<std::string_view, FileType> const extensionMap = {
        {".cpp", FileType::Cpp},
        {".cxx", FileType::Cpp},
        {".cc", FileType::Cpp},
        {".hpp", FileType::CppHeader},
        {".hxx", FileType::CppHeader},
        {".hh", FileType::CppHeader},
        {".c", FileType::C},
        {".h", FileType::CHeader},
        {".mm", FileType::ObjectiveCpp},
    };

    auto ext = path.extension().string();
    auto it = extensionMap.find(ext);
    if (it != extensionMap.end()) {
        return it->second;
    }

    return FileType::Unknown;
}

struct TypeStats {
    FileInfo info;
    size_t fileCount = 0;
};

static std::map<std::filesystem::path, FileInfo> analyzedFiles;
static std::unordered_map<FileType, TypeStats> fileInfos;
static size_t totalFiles = 0;
static FileInfo totalInfo;
static std::mutex infoMutex;
static bool perFileOutput = false;

void enqueueFile(std::filesystem::path path, FileType type, ThreadPool& threadPool) {
    threadPool.enqueue(
        [path = std::move(path), type]() {
            auto info = analyze(path);
            if (!info) return;

            std::lock_guard lock(infoMutex);
            if (perFileOutput) {
                analyzedFiles[path] = *info;
            } else {
                fileInfos[type].info += *info;
                fileInfos[type].fileCount++;
            }
            totalInfo += *info;
            ++totalFiles;
        }
    );
}

void walkDirectory(std::filesystem::path const& path, ThreadPool& threadPool) {
    std::error_code ec;
    for (auto const& entry : std::filesystem::directory_iterator(path, ec)) {
        if (ec) continue;
        if (entry.is_regular_file(ec)) {
            auto fileType = getFileType(entry.path());
            if (fileType != FileType::Unknown) {
                enqueueFile(entry.path(), fileType, threadPool);
            }
        } else if (entry.is_directory(ec)) {
            walkDirectory(entry.path(), threadPool);
        }
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::println("Usage: {} [-h | --help] [-f | --per-file] [-o | --output=<filename>] <path>...", argv[0]);
        return 1;
    }

    ArgParser parser{argc, argv};
    if (parser.hasFlag("--help") || parser.hasFlag("-h")) {
        std::println("Usage: {} [-h | --help] [-f | --per-file] [-o | --output=<filename>] <path>...", argv[0]);
        std::println("Options:");
        std::println("  --help -h        Show this help message");
        std::println("  --per-file -f    Output analysis results per file");
        std::println("  --output -o      Specify output file (default: stdout)");
        return 0;
    }

    if (parser.hasFlag("--per-file") || parser.hasFlag("-f")) {
        perFileOutput = true;
    }

    Writer writer{};
    std::ofstream out;
    auto outputPath1 = parser.getOptionValue("--output");
    auto outputPath2 = parser.getOptionValue("-o");
    if (!outputPath1.empty() || !outputPath2.empty()) {
        auto& outputPath = outputPath1.empty() ? outputPath2 : outputPath1;
        if (!outputPath.empty()) {
            out.open(outputPath.data());
            if (!out) {
                std::println(std::cerr, "Failed to open output file: {}", outputPath);
                return 1;
            }
            writer = Writer{out};
        }
    }

    std::chrono::time_point<std::chrono::system_clock> start, end;
    {
        ThreadPool threadPool;
        start = std::chrono::high_resolution_clock::now();
        for (auto varg : parser.positionalArgs()) {
            std::filesystem::path path(varg);
            std::error_code ec;

        #define CHECK_ERR_CODE \
            if (ec) { std::println(std::cerr, "Error accessing path {}: {}", varg, ec.message()); continue; }

            if (!std::filesystem::exists(path, ec)) {
                std::println(std::cerr, "Path does not exist: {}", varg);
                continue;
            }
            CHECK_ERR_CODE;

            if (std::filesystem::is_directory(path, ec)) {
                walkDirectory(path, threadPool);
            }
            CHECK_ERR_CODE;

            if (std::filesystem::is_regular_file(path, ec)) {
                auto fileType = getFileType(path);
                if (fileType != FileType::Unknown) {
                    enqueueFile(path, fileType, threadPool);
                }
            }
            CHECK_ERR_CODE;
        }
        end = std::chrono::high_resolution_clock::now();
    }

    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    writer.writeln(
        "Analysis completed in {} ns ({} files/s, {} lines/s)",
        duration, (totalFiles * 1'000'000'000LL) / duration,
        ((totalInfo.blankLines + totalInfo.commentLines + totalInfo.codeLines) * 1'000'000'000LL) / duration
    );

    if (perFileOutput) {
        writer.writeln("Total files analyzed: {}", totalFiles);
        writer.writeln("--------------------------------------------------------------------------------------------------------------------------------------------");
        writer.writeln("{:<95} {:>14} {:>14} {:>14}", "file", "blank", "comment", "code");
        writer.writeln("--------------------------------------------------------------------------------------------------------------------------------------------");

        // sort the files by total lines descending
        std::vector<std::pair<std::filesystem::path, FileInfo>> sortedFiles(analyzedFiles.begin(), analyzedFiles.end());
        std::sort(sortedFiles.begin(), sortedFiles.end(), [](auto const& a, auto const& b) {
            return a.second.totalLines() > b.second.totalLines();
        });

        // print per-file stats
        for (auto const& [path, info] : sortedFiles) {
            writer.writeln("{:<95} {:>14} {:>14} {:>14}", path.string(), info.blankLines, info.commentLines, info.codeLines);
        }

        // print summary
        writer.writeln("--------------------------------------------------------------------------------------------------------------------------------------------");
        writer.writeln("{:<95} {:>14} {:>14} {:>14}", "Total", totalInfo.blankLines, totalInfo.commentLines, totalInfo.codeLines);
        writer.writeln("--------------------------------------------------------------------------------------------------------------------------------------------");
    } else {
        writer.writeln("-------------------------------------------------------------------------------");
        writer.writeln("Language                     files          blank        comment           code");
        writer.writeln("-------------------------------------------------------------------------------");

        // sort by total lines descending
        std::vector<std::pair<FileType, TypeStats>> sortedFileInfos(fileInfos.begin(), fileInfos.end());
        std::sort(sortedFileInfos.begin(), sortedFileInfos.end(), [](auto const& a, auto const& b) {
            return a.second.info.totalLines() > b.second.info.totalLines();
        });

        for (auto const& [type, info] : sortedFileInfos) {
            writer.writeln(
                "{:<20} {:>13} {:>14} {:>14} {:>14}",
                fileTypeToString(type),
                info.fileCount, info.info.blankLines,
                info.info.commentLines, info.info.codeLines
            );
        }

        writer.writeln("-------------------------------------------------------------------------------");
        writer.writeln(
            "Total                {:>13} {:>14} {:>14} {:>14}",
            totalFiles,
            totalInfo.blankLines,
            totalInfo.commentLines,
            totalInfo.codeLines
        );
        writer.writeln("-------------------------------------------------------------------------------");
    }

    return 0;
}
