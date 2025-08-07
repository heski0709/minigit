#include "merge/merge_commit.h"
#include "merge/merge_conflict.h"
#include "merge/merge_utils.h"
#include "index/index_utils.h"
#include "commit/commit_utils.h"
#include "branch/branch_utils.h"
#include "utils/utils.h"

#include <fstream>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <filesystem>

namespace fs = std::filesystem;

void mergeCommitFromState(const std::string& currentHash, const std::string& targetHash, const std::string& message)
{
	auto conflicts = loadMergeConflicts();
	std::unordered_set<std::string> conflictFiles;

	if (!conflicts.empty()) 
	{
		for (auto& [filename, hash] : conflicts) conflictFiles.insert(filename);

		// 충돌 파일이 아직 그대로 있다면 경고
		for (const auto& [file, _] : conflicts)
		{
			std::ifstream f(file);
			std::string content((std::istreambuf_iterator<char>(f)), {});
			if (content.find("<<<<<") != std::string::npos)
			{
				std::cerr << "[오류] 아직 충돌 파일이 해결되지 않았습니다: " << file << "\n";
				return;
			}
		}
	}

	// 자동병합 파일 처리
	applyAutoMergeFiles(currentHash, targetHash, conflictFiles);
	updateIndexAfterAutoMerge(currentHash, targetHash, conflictFiles);

	// 충돌이 해결되었을 경우 index 파일 업데이트
	if (!conflicts.empty())
	{
		std::string outputPath = ".minigit\\index";
		std::ofstream out(outputPath, std::ios::app);

		if (!out.is_open())
		{
			std::cerr << "[오류] index 파일을 열 수 없습니다: " << outputPath << "\n";
			return;
		}

		for (const auto& [filename, _] : conflicts)
		{
			if (!fs::exists(filename)) continue;
			std::string content = readFileContent(filename);
			std::string hash = simpleHash(content);
			out << filename << ":" << hash << "\n";
		}
	}

	// index snapshot 확보
	std::string snapshot = readIndexSnapshot();
	std::vector<std::string> snapshotLines = splitLines(snapshot);
	std::string commitHash = improvedHash(snapshotLines);
	std::string commitPath = ".minigit\\commits\\" + commitHash;

	if (!createCommitDirectory(commitHash))
	{
		std::cerr << "커밋 디렉토리 생성 실패\n";
		return;
	}

	// index 복사
	std::ofstream destIndex(commitPath + "\\index");
	if (!destIndex.is_open())
	{
		std::cerr << "index 복사 실패\n";
		return;
	}
	destIndex << snapshot;
	destIndex.close();

	// 파일 복사
	for (const auto& line : snapshotLines)
	{
		auto delim = line.find(":");
		if (delim == std::string::npos) continue;

		std::string filename = line.substr(0, delim);
		copyFileToCommit(filename, commitPath);
	}

	// 메타 작성
	writeMeta(commitPath, { currentHash, targetHash }, message);

	// HEAD 브랜치 업데이트
	updateBranchHead(commitHash);

	// index 초기화
	std::ofstream clearIndex(".minigit\\index", std::ios::trunc);
	if (!clearIndex.is_open())
		std::cerr << "index 초기화 실패\n";

	std::cout << "병합 커밋이 완료되었습니다.\n";
}
