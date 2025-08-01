#include "merge/merge_utils.h"
#include "checkout/checkout_utils.h"
#include "branch/branch_utils.h"
#include "index/index_utils.h"
#include "utils/utils.h"

#include <fstream>
#include <filesystem>
#include <deque>
#include <iostream>

namespace fs = std::filesystem;


bool isMergeInProgress()
{
	return fs::exists(".minigit\\MERGE_STATE");
}

void updateIndexAfterAutoMerge(const std::string& baseHash, const std::string& targetHash)
{
	auto currentIndex = parseIndex(".minigit\\commits\\" + baseHash + "\\index");
	auto targetIndex = parseIndex(".minigit\\commits\\" + targetHash + "\\index");

	std::unordered_set<std::string> files;

	// current + target 파일 목록 합치기
	for (const auto& [file, _] : currentIndex) files.insert(file);
	for (const auto& [file, _] : targetIndex) files.insert(file);

	writeIndexFromWorkingDirectory(files, ".minigit\\index");
}

void updateIndexFromWorkingDirectory()
{
	std::string currentHash = getCurrentBranchHash();
	auto baseIndex = parseIndex(".minigit\\commits\\" + currentHash + "\\index");
	std::unordered_set<std::string> files;
	for (const auto& [file, _] : baseIndex) files.insert(file);

	writeIndexFromWorkingDirectory(files, ".minigit\\index");
}

void applyAutoMergeFiles(const std::string& currentHash, const std::string& targetHash)
{
	auto currentIndex = parseIndex(".minigit\\commits\\" + currentHash + "\\index");
	auto targetIndex = parseIndex(".minigit\\commits\\" + targetHash + "\\index");

	for (const auto& [filename, targetHashVal] : targetIndex)
	{
		// current에도 있고, 해시가 동일하면 복사 안함
		if (currentIndex.count(filename) && currentIndex[filename] == targetHashVal)
			continue;

		// 충돌 없이 자동 병합 가능한 파일만 복사
		std::string targetFilePath = ".minigit\\commits\\" + targetHash + "\\" + filename;

		if (!fs::exists(targetFilePath))
		{
			std::cerr << "[경고] 대상 파일 없음: " << targetFilePath << "\n";
			continue;
		}

		try
		{
			fs::copy_file(targetFilePath, filename, fs::copy_options::overwrite_existing);
			std::cout << "[자동 병합] " << filename << "\n";
		}
		catch (const std::exception& e)
		{
			std::cerr << "[오류] 파일 복사 실패 (" << filename << "): " << e.what() << "\n";
		}
	}
}

void backupIndexFiles(const std::unordered_map<std::string, std::string>& indexMap, const std::string& backupDir)
{
	for (const auto& [filename, hash] : indexMap)
	{
		if (fs::exists(filename))
		{
			fs::path backupPath = fs::path(backupDir) / filename;
			fs::create_directories(backupPath.parent_path());
			fs::copy_file(filename, backupDir + filename, fs::copy_options::overwrite_existing);
		}
	}
}