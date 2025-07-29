#include "merge_utils.h"
#include <fstream>
#include <unordered_map>
#include <filesystem>

namespace fs = std::filesystem;

/**
* @brief 특정 커밋의 index 파일을 파싱하여 (파일명 -> 해시) map으로 반환
*
* @param commitHash 커밋 해시
* @return std::unordered_map<std::string, std::string> 파일명과 대응하는 해시
*/
std::unordered_map<std::string, std::string> parseIndex(const std::string& commitHash)
{
    std::unordered_map<std::string, std::string> map;
    std::ifstream index(".minigit\\commits\\" + commitHash + "\\index");
    if (!index.is_open()) return map;

    std::string line;
    while (std::getline(index, line))
    {
        auto delim = line.find(":");
        if (delim == std::string::npos) continue;

        std::string filename = line.substr(0, delim);
        std::string hash = line.substr(delim + 1);
        map[filename] = hash;
    }

    return map;
}

std::vector<std::string> detectConflicts(const std::string& currentHash, const std::string& targetHash)
{
    std::vector<std::string> conflicts;

    auto currentIndex = parseIndex(currentHash);
    auto targetIndex = parseIndex(targetHash);

    for (const auto& [filename, hash] : currentIndex)
    {
        if (targetIndex.count(filename) && targetIndex[filename] != hash)
            conflicts.push_back(filename);
    }

    return conflicts;
}

void markConflict(const std::string& filename, const std::string& contentA, const std::string& contentB)
{
    std::ofstream out(filename);
    if (!out.is_open()) return;

    out << "<<<<<<< HEAD\n";
    out << contentA;
    out << "\n=======\n";
    out << contentB;
    out << "\n>>>>>>> merge\n";
    out.close();
}
