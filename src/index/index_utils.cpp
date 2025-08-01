#include "index/index_utils.h"
#include "utils/utils.h"

#include <fstream>

std::string getCurrentIndexSnapshot()
{
	return readFileContent(".minigit\\index");
}

std::unordered_map<std::string, std::string> parseIndex(const std::string& commitHash)
{
	std::unordered_map<std::string, std::string> map;
	std::ifstream index(".minigit\\commits\\" + commitHash + "\\index");
	if (!index.is_open()) return map;

	std::string line;
	while (std::getline(index, line))
	{
		auto delim = line.find(":");
		if (delim == std::string::npos) continue;

		std::string filename = line.substr(0, delim);
		std::string hash = line.substr(delim + 1);
		map[filename] = hash;
	}

	return map;
}
