#include "index/index_utils.h"
#include "branch/branch_utils.h"
#include "utils/utils.h"

#include <fstream>

std::string getCurrentIndexSnapshot()
{
	return readFileContent(".minigit\\index");
}

std::unordered_map<std::string, std::string> parseIndex(const std::string& path)
{
	std::unordered_map<std::string, std::string> indexMap;
	std::ifstream file(path);
	std::string line;

	if (!file.is_open()) return indexMap;

	while (std::getline(file, line))
	{
		auto delim = line.find(":");
		if (delim == std::string::npos) continue;

		std::string name = line.substr(0, delim);
		std::string hash = line.substr(delim + 1);
		indexMap[name] = hash;
	}

	return indexMap;
}

std::unordered_map<std::string, std::string> loadIndex()
{
	return parseIndex(".minigit\\index");
}

std::unordered_map<std::string, std::string> loadLastCommitIndex()
{
	std::string lastHash = getCurrentBranchHash();
	if (lastHash.empty()) return {};
	
	std::string path = ".minigit\\commits\\" + lastHash + "\\index";
	return parseIndex(path);
}