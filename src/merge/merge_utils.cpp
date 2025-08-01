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
	std::ofstream out(".minigit\\index", std::ios::trunc);

	if (!out.is_open())
	{
		std::cerr << "index 파일을 열 수 없습니다.\n";
		return;
	}

	std::unordered_map<std::string, std::string> currentIndex = parseIndex(".minigit\\commits\\" + baseHash + "\\index");
	std::unordered_map<std::string, std::string> targetIndex = parseIndex(".minigit\\commits\\" + targetHash + "\\index");

	std::unordered_set<std::string> allFiles;

	// current + target 파일 목록 합치기
	for (const auto& [file, _] : currentIndex) allFiles.insert(file);
	for (const auto& [file, _] : targetIndex) allFiles.insert(file);

	for (const auto& filename : allFiles)
	{
		if (!fs::exists(filename)) continue;
		std::string content = readFileContent(filename);
		std::string hash = simpleHash(content);
		out << filename << ":" << hash << "\n";
	}

	out.close();
}

void updateIndexFromWorkingDirectory()
{
	std::ofstream out(".minigit\\index", std::ios::trunc);
	if (!out.is_open())
	{
		std::cerr << "index 파일을 열 수 없습니다.\n";
		return;
	}

	std::string currentHash = getCurrentBranchHash();
	std::unordered_map<std::string, std::string> baseIndex = parseIndex(".minigit\\commits\\" + currentHash + "\\index");

	for (const auto& [filename, _] : baseIndex)
	{
		if (!fs::exists(filename)) continue;
		std::string content = readFileContent(filename);
		std::string hash = simpleHash(content);
		out << filename << ":" << hash << "\n";
	}

	out.close();
}

void applyAutoMergeFiles(const std::string& targetHash)
{
	auto targetIndex = parseIndex(".minigit\\commits\\" + targetHash + "\\index");
	for (const auto& [file, _] : targetIndex)
	{
		std::string src = ".minigit\\commits\\" + targetHash + "\\" + file;
		if (!fs::exists(src)) continue;
		fs::copy_file(src, file, fs::copy_options::overwrite_existing);
	}
}