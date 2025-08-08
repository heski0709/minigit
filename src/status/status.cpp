#include "status/status.h"
#include "branch/branch_utils.h"
#include "index/index_utils.h"
#include "utils/utils.h"

#include <filesystem>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

std::string computeFileHash(const std::string& path)
{
	std::string content = readFileContent(path);
	return simpleHash(content);
}

/**
* @brief 현재 작업 디렉토리 기준으로 상태를 출력
*/
void showStatus()
{
	std::string branch = getCurrentBranchName();
	std::cout << "현재 브랜치: " << branch << "\n\n";

	auto indexMap = loadIndex();
	auto commitMap = loadLastCommitIndex();

	std::unordered_set<std::string> modified;
	std::unordered_set<std::string> staged;
	std::unordered_set<std::string> untracked;

	// 스테이징된 파일 (index에 기록되어 있는 파일)
	for (const auto& [file, hash] : indexMap)
		staged.insert(file);

	// 수정된 파일 확인
	for (const auto& [file, stagedHash] : indexMap)
	{
		if (!fs::exists(file)) continue;

		std::string nowHash = computeFileHash(file);
		if (nowHash != stagedHash)
			modified.insert(file);
	}

	// 전체 파일 목록 조회
	for (const auto& entry : fs::directory_iterator("."))
	{
		std::string filename = entry.path().filename().string();
		// .minigit 내부 무시
		if (filename == ".minigit") continue;

		// 이미 index나 commit에 있으면 무시
		if (indexMap.count(filename) || commitMap.count(filename)) continue;

		untracked.insert(filename);
	}

	// 출력
	if (!staged.empty())
	{
		std::cout << "스테이징된 파일:\n";
		for (const auto& file : staged)
			std::cout << "	" << file << "\n";
		std::cout << "\n";
	}

	if (!modified.empty())
	{
		std::cout << "수정된 파일:\n";
		for (const auto& file : modified)
			std::cout << "	" << file << "\n";
		std::cout << "\n";
	}

	if (!untracked.empty())
	{
		std::cout << "추적되지 않은 파일:\n";
		for (const auto& file : untracked)
			std::cout << "	" << file << "\n";
		std::cout << "\n";
	}
}