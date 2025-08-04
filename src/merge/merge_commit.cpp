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

void mergeCommitFromState(const std::string& currentHash, const std::string& targetHash, const std::string& message)
{
	auto conflicts = detectConflicts(currentHash, targetHash);
	std::unordered_set<std::string> conflictFiles(conflicts.begin(), conflicts.end());

	// �ڵ����� ���� ó��
	applyAutoMergeFiles(currentHash, targetHash, conflictFiles);
	updateIndexAfterAutoMerge(currentHash, targetHash, conflictFiles);

	// index snapshot Ȯ��
	std::string snapshot = readIndexSnapshot();
	std::vector<std::string> snapshotLines = splitLines(snapshot);
	std::string commitHash = improvedHash(snapshotLines);
	std::string commitPath = ".minigit\\commits\\" + commitHash;

	if (!createCommitDirectory(commitHash))
	{
		std::cerr << "Ŀ�� ���丮 ���� ����\n";
		return;
	}

	// index ����
	std::ofstream destIndex(commitPath + "\\index");
	if (!destIndex.is_open())
	{
		std::cerr << "index ���� ����\n";
		return;
	}
	destIndex << snapshot;
	destIndex.close();

	// ���� ����
	for (const auto& line : snapshotLines)
	{
		auto delim = line.find(":");
		if (delim == std::string::npos) continue;

		std::string filename = line.substr(0, delim);
		copyFileToCommit(filename, commitPath);
	}

	// ��Ÿ �ۼ�
	writeMeta(commitPath, { currentHash, targetHash }, message);

	// HEAD �귣ġ ������Ʈ
	updateBranchHead(commitHash);

	// index �ʱ�ȭ
	std::ofstream clearIndex(".minigit\\index", std::ios::trunc);
	if (!clearIndex.is_open())
		std::cerr << "index �ʱ�ȭ ����\n";

	std::cout << "���� Ŀ���� �Ϸ�Ǿ����ϴ�.\n";
}
