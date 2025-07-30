#include "merge_utils.h"
#include "utils.h"
#include <fstream>
#include <filesystem>
#include <deque>

namespace fs = std::filesystem;

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

	std::vector<std::string> linesA = splitLines(contentA);
	std::vector<std::string> linesB = splitLines(contentB);
	
	out << "<<<<< HEAD\n";
	for (const auto& line : linesA) out << line << "\n";

	out << "=====\n";
	for (const auto& line : linesB) out << line << "\n";

	out << ">>>>> MERGE\n";
	out.close();
}

std::unordered_set<std::string> collectAncestors(const std::string& startHash)
{
	std::unordered_set<std::string> visited;
	std::deque<std::string> queue = { startHash };

	while (!queue.empty())
	{
		std::string current = queue.front();
		queue.pop_front();

		if (visited.count(current)) continue;
		visited.insert(current);

		std::ifstream meta(".minigit\\commits\\" + current + "\\meta.txt");
		if (!meta.is_open()) continue;

		std::string line;
		while (std::getline(meta, line))
		{
			if (line.rfind("parent: ", 0) == 0)
			{
				std::string parentsStr = line.substr(8);
				size_t pos = 0;
				while ((pos = parentsStr.find(",") != std::string::npos))
				{
					queue.push_back(parentsStr.substr(0, pos));
					parentsStr.erase(0, pos + 1);
				}

				if (!parentsStr.empty()) queue.push_back(parentsStr);
				break;
			}
		}
	}

	return visited;
}

/**
* @brief 두 커밋의 공통 조상을 찾음 (가장 가까운 조상, LCA 성격)
*
* @details hashA의 모든 조상을 set으로 수집하고,
*          hashB를 위로 탐색하면서 처음 만나는 조상을 반환.
*          -> 순서 없는 set끼리 비교하지 않고, 가장 가까운 조상 반환 보장 목적.
 */
std::string findCommonAncestor(const std::string& hashA, const std::string& hashB)
{
	auto ancestorsA = collectAncestors(hashA);
	std::deque<std::string> queue = { hashB };
	std::unordered_set<std::string> visited;

	while (!queue.empty())
	{
		std::string current = queue.front();
		queue.pop_front();

		if (visited.count(current)) continue;
		visited.insert(current);

		// 가장 가까운 조상 발견
		if (ancestorsA.count(current)) return current;

		std::ifstream meta(".minigit\\commits\\" + current + "\\meta.txt");
		if (!meta.is_open()) continue;

		std::string line;
		while (std::getline(meta, line))
		{
			if (line.rfind("parent: ", 0) == 0)
			{
				std::string parentsStr = line.substr(8);
				size_t pos = 0;
				while ((pos = parentsStr.find(",") != std::string::npos))
				{
					queue.push_back(parentsStr.substr(0, pos));
					parentsStr.erase(0, pos + 1);
				}

				if (!parentsStr.empty()) queue.push_back(parentsStr);
				break;
			}
		}
	}

	return "";
}
