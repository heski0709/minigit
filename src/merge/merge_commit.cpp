#include "merge/merge_commit.h"
#include "merge/merge_conflict.h"
#include "merge/merge_utils.h"
#include "index/index_utils.h"
#include "commit/commit_utils.h"
#include "branch/branch_utils.h"
#include "utils/utils.h"

#include <fstream>
#include <iostream>

void mergeCommitFromState(const std::string& currentHash, const std::string& targetHash, const std::string& message)
{
	auto conflicts = detectConflicts(currentHash, targetHash);
	applyAutoMergeFiles(currentHash, targetHash, conflicts);
	updateIndexAfterAutoMerge(currentHash, targetHash, conflicts);
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
