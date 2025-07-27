#include <fstream>
#include <filesystem>
#include <sstream>
#include "utils.h"

std::string improvedHash(const std::vector<std::string>& entries)
{
	std::size_t combined = 0;

	for (const auto& entry : entries)
	{
		std::hash<std::string> hasher;
		std::size_t h = hasher(entry);
		combined ^= (h << 1);
	}

	// 16���� ��ȯ
	std::stringstream ss;
	ss << std::hex << combined;
	return ss.str();
}

std::string simpleHash(const std::string& content)
{
	unsigned int hash = 0;
	for (auto c : content)
	{
		hash = (hash * 31) + static_cast<unsigned char>(c); // 31�� �Ҽ� (�浹���� ����)
	}

	// ���� hash�� 16���� ���ڿ��� ��ȯ
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
		if (!line.empty())
			lines.push_back(line);
	}
	return lines;
}