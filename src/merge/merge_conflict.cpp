#include "merge/merge_conflict.h"
#include "index/index_utils.h"
#include "utils/utils.h"

#include <fstream>
#include <sstream>
#include <iostream>

std::vector<std::string> detectConflicts(const std::string& currentHash, const std::string& targetHash)
{
	std::vector<std::string> conflicts;

	auto currentIndex = parseIndex(currentHash);
	auto targetIndex = parseIndex(targetHash);

	for (const auto& [filename, hash] : currentIndex)
	{
		if (targetIndex.count(filename) && targetIndex[filename] != hash)
			conflicts.push_back(filename);
	}

	return conflicts;
}

std::string generateConflictMarkedContent(
	const std::string& currentContent,
	const std::string& targetContent,
	const std::string& currentLabel = "HEAD",
	const std::string& targetLabel = "MERGE"
)
{
	std::stringstream ss;
	ss << "<<<<<<< " << currentLabel << "\n";
	ss << currentContent << "\n";
	ss << "=======\n";
	ss << targetContent << "\n";
	ss << ">>>>>>> " << targetLabel << "\n";
	return ss.str();
}

void markConflict(
	const std::string& filename,
	const std::string& currentContent,
	const std::string& targetContent,
	const std::string& currentLabel = "HEAD",
	const std::string& targetLabel = "MERGE"
)
{
	std::string marked = generateConflictMarkedContent(currentContent, targetContent, currentLabel, targetLabel);

	std::ofstream out(filename);
	if (!out.is_open()) {
		std::cerr << "[오류] 충돌 파일 쓰기 실패: " << filename << "\n";
		return;
	}
	out << marked;
	out.close();
}
