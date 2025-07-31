#include "merge_utils.h"
#include "checkout_utils.h"
#include "branch_utils.h"
#include "utils.h"
#include <fstream>
#include <filesystem>
#include <deque>
#include <iostream>

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

void saveMergeState(const std::string& currentHash, const std::string& targetHash)
{
	std::ofstream out(".minigit\\MERGE_STATE");
	if (!out.is_open()) return;

	out << currentHash << "\n" << targetHash << "\n";
	out.close();
}

void clearMergeState()
{
	fs::remove(".minigit\\MERGE_STATE");
}

bool isMergeInProgress()
{
	return fs::exists(".minigit\\MERGE_STATE");
}

std::pair<std::string, std::string> loadMergeState()
{
	std::ifstream in(".minigit/MERGE_STATE");
	if (!in.is_open()) return { "", "" };

	std::string current, target;
	std::getline(in, current);
	std::getline(in, target);
	return { current, target };
}

void backupCurrentFilesBeforeMerge(const std::string& currentHash)
{
	auto index = parseIndex(currentHash);

	for (const auto& [filename, hash] : index)
	{
		if (fs::exists(filename))
		{
			fs::path backupPath = fs::path(".minigit\\_merge_backup") / filename;
			fs::create_directories(backupPath.parent_path());
			fs::copy_file(filename, ".minigit\\_merge_backup\\" + filename, fs::copy_options::overwrite_existing);
		}
	}
}

void restoreFilesFromBackup()
{
	std::string currentBranchHash = getCurrentBranchHash();
	auto index = parseIndex(currentBranchHash);

	for (const auto& [filename, _] : index)
	{
		std::string backupPath = ".minigit\\_merge_backup";
		restoreFile(backupPath, filename);
	}
}

void clearBackup()
{
	fs::path backupPath(".minigit\\_merge_backup");

	if (!fs::exists(backupPath))
	{
		// 백업 폴더가 애초에 없다면 조용히 넘어감
		return;
	}

	try
	{
		std::uintmax_t count = fs::remove_all(backupPath);
		if (count == 0)
		{
			std::cerr << "[경고] 백업 폴더는 존재하지만 삭제된 항목이 없습니다.\n";
		}
		else
		{
			std::cout << "[info] 백업 폴더 삭제 완료 (" << count << "개 항목 제거됨)\n";
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "[오류] 백업 삭제 중 예외 발생: " << e.what() << "\n";
	}
}

void updateIndexAfterAutoMerge(const std::string& baseHash, const std::string& targetHash)
{
	std::ofstream out(".minigit\\index", std::ios::trunc);

	if (!out.is_open())
	{
		std::cerr << "index 파일을 열 수 없습니다.\n";
		return;
	}

	std::unordered_map<std::string, std::string> currentIndex = parseIndex(baseHash);
	std::unordered_map<std::string, std::string> targetIndex = parseIndex(targetHash);

	std::unordered_set<std::string> allFiles;

	// current + target 파일 목록 합치기
	for (const auto& [file, _] : currentIndex) allFiles.insert(file);
	for (const auto& [file, _] : targetIndex) allFiles.insert(".minigit\\commits\\"+ targetHash + "\\" + file);

	for (const auto& filename : allFiles)
	{
		if (!fs::exists(filename)) continue;
		std::string content = readFileContent(filename);
		std::string hash = simpleHash(content);
		out << filename << ":" << hash << "\n";
	}

	out.close();
}
