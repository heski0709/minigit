#include "merge/merge_conflict.h"
#include "index/index_utils.h"
#include "utils/utils.h"

#include <fstream>

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

void markConflict(const std::string& filename, const std::string& contentA, const std::string& contentB)
{
	std::ofstream out(filename);
	if (!out.is_open()) return;

	std::vector<std::string> linesA = splitLines(contentA);
	std::vector<std::string> linesB = splitLines(contentB);

	out << "<<<<< HEAD\n";
	for (const auto& line : linesA) out << line << "\n";

	out << "=====\n";
	for (const auto& line : linesB) out << line << "\n";

	out << ">>>>> MERGE\n";
	out.close();
}
