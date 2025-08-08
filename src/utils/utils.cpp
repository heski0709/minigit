#include "utils/utils.h"

#include <fstream>
#include <filesystem>
#include <sstream>

std::string improvedHash(const std::vector<std::string>& entries)
{
	std::size_t combined = 0;

	for (const auto& entry : entries)
	{
		std::hash<std::string> hasher;
		std::size_t h = hasher(entry);
		combined ^= (h << 1);
	}

	// 16진수 변환
	std::stringstream ss;
	ss << std::hex << combined;
	return ss.str();
}

std::string simpleHash(const std::string& content)
{
	unsigned int hash = 0;
	for (auto c : content)
	{
		hash = (hash * 31) + static_cast<unsigned char>(c); // 31은 소수 (충돌방지 목적)
	}

	// 숫자 hash를 16진수 문자열로 변환
	char buf[70];
	sprintf_s(buf, "%08x", hash);
	return std::string(buf);
}

bool fileExists(const std::string& filename)
{
	return std::filesystem::exists(filename);
}

std::string readFileContent(const std::string& filename)
{
	std::ifstream file(filename);
	if (!file.is_open()) return "";

	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();
	return content;
}


std::vector<std::string> splitLines(const std::string& content)
{
	std::vector<std::string> lines;
	std::stringstream ss(content);
	std::string line;
	while (std::getline(ss, line))
	{
		lines.push_back(line);
	}

	std::sort(lines.begin(), lines.end());
	return lines;
}