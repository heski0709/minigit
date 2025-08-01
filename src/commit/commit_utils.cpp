#include "commit/commit_utils.h"
#include "branch/branch_utils.h"
#include "index/index_utils.h"
#include "utils/utils.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

bool createCommitDirectory(const std::string& hash)
{
	std::string path = ".minigit\\commits\\" + hash;
	return fs::create_directory(path);
}

bool copyFileToCommit(const std::string& src, const std::string& destDir)
{
	fs::path destPath = fs::path(destDir) / src;
	fs::create_directories(destPath.parent_path());

	std::ifstream in(src, std::ios::binary);
	std::ofstream out(destDir + "\\" + src, std::ios::binary);
	if (!in.is_open() || !out.is_open()) return false;

	out << in.rdbuf();
	return true;
}

void writeMeta(const std::string& destDir, const std::vector<std::string>& parentHashes, const std::string& message)
{
	std::ofstream meta(destDir + "\\meta.txt");
	if (!meta.is_open()) return;

	std::string joinedParents;
	for (size_t i = 0; i < parentHashes.size(); ++i)
	{
		joinedParents += parentHashes[i];
		if (i != parentHashes.size() - 1)
			joinedParents += ",";
	}

	auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	char timeStr[100];

	if (ctime_s(timeStr, sizeof(timeStr), &now) != 0)
	{
		meta << "parent: " << joinedParents << "\n";
		meta << "message: " << message << "\n";
		meta << "timestamp: (�ð� �Ľ� ����)\n";
	}
	else
	{
		meta << "parent: " << joinedParents << "\n";
		meta << "message: " << message << "\n";
		meta << "timestamp: " << timeStr;
	}
	meta.close();
}

std::string resolveReferenceType(const std::string& ref) {
	if (fs::exists(".minigit/refs/heads/" + ref)) {
		return "branch";
	}
	else if (fs::exists(".minigit/objects/" + ref)) {
		return "commit";
	}
	else {
		return "invalid";
	}
}

void mergeCommitFromState(const std::string& currentHash, const std::string& targetHash, const std::string& message)
{
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
