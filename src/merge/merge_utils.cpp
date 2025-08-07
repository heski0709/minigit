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

void updateIndexAfterAutoMerge(const std::string& currentHash, const std::string& targetHash, const std::unordered_set<std::string>& conflictFiles)
{
	auto currentIndex = parseIndex(".minigit\\commits\\" + currentHash + "\\index");
	auto targetIndex = parseIndex(".minigit\\commits\\" + targetHash + "\\index");

	std::unordered_set<std::string> files;

	// current + target ���� ��� ��ġ��
	for (const auto& [file, _] : currentIndex) files.insert(file);
	for (const auto& [file, _] : targetIndex) files.insert(file);

	for (const auto& file : conflictFiles) files.erase(file);

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

void applyAutoMergeFiles(const std::string& currentHash, const std::string& targetHash, const std::unordered_set<std::string>& conflictFiles)
{
	auto currentIndex = parseIndex(".minigit\\commits\\" + currentHash + "\\index");
	auto targetIndex = parseIndex(".minigit\\commits\\" + targetHash + "\\index");

	for (const auto& [filename, targetHashVal] : targetIndex)
	{
		if (conflictFiles.count(filename)) continue;

		// ���� �ؽð� �����ϸ� ���� ����
		if (currentIndex[filename] == targetHashVal) continue;

		// �ؽð� �ٸ��� �浹�� �ƴ϶�� �ڵ� ����
		std::string targetFilePath = ".minigit\\commits\\" + targetHash + "\\" + filename;

		if (!fs::exists(targetFilePath))
		{
			std::cerr << "[���] ��� ���� ����: " << targetFilePath << "\n";
			continue;
		}

		try
		{
			fs::copy_file(targetFilePath, filename, fs::copy_options::overwrite_existing);
			std::cout << "[�ڵ� ����] " << filename << "\n";
		}
		catch (const std::exception& e)
		{
			std::cerr << "[����] ���� ���� ���� (" << filename << "): " << e.what() << "\n";
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
			fs::copy_file(filename, backupPath, fs::copy_options::overwrite_existing);
		}
	}
}