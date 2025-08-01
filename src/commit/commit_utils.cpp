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
		meta << "timestamp: (시간 파싱 실패)\n";
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
