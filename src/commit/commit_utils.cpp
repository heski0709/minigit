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
