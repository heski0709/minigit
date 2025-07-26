#include "commit.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <chrono>

/**
* @brief HEAD�� ����� Ŀ�� �ؽø� �������� �Լ�
*/
std::string getCurrentHeadHash()
{
	std::ifstream head(".minigit\\HEAD");
	if (!head.is_open()) return "";

	std::string hash;
	std::getline(head, hash);
	return hash;
}

/**
* @brief Ŀ�� ���丮 ����
* @param hash Ŀ�� �ؽ��ڵ�
*/
bool createCommitDirectory(const std::string& hash)
{
	std::string path = ".minigit\\commits\\" + hash;
	return std::filesystem::create_directory(path);
}

/**
* @brief Ŀ�� ���丮�� ���� ����
* @param src ��������
* @param destDir ���� ������ ������ Ŀ�� ���丮
*/
bool copyFileToCommit(const std::string& src, const std::string& destDir)
{
	std::ifstream in(src, std::ios::binary);
	std::ofstream out(destDir + "\\" + src, std::ios::binary);
	if (!in.is_open() || !out.is_open()) return false;

	out << in.rdbuf();
	return true;
}

/**
* @brief Ŀ�� �޽����� �ð��� ����ϴ� �Լ�
* @param destDir Ŀ�� ���丮
* @param message Ŀ�Ե� �޼���
*/
void writeMeta(const std::string& destDir, const std::string& message)
{
	std::ofstream meta(destDir + "\\meta.txt");
	if (!meta.is_open()) return;

	std::string parentHash = getCurrentHeadHash();
	auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	char timeStr[100];

	if (ctime_s(timeStr, sizeof(timeStr), &now) != 0)
	{
		meta << "parent: " << parentHash << "\n";
		meta << "message: " << message << "\n";
		meta << "timestamp: (�ð� �Ľ� ����)\n";
	}
	else
	{
		meta << "parent: " << parentHash << "\n";
		meta << "message: " << message << "\n";
		meta << "timestamp: " << timeStr;
	}
	meta.close();
}

void commit(const std::string& message)
{
	std::ifstream index(".minigit\\index");

	// index�� ���ų�, ����ִٸ� Ŀ�� �ߴ�
	if (!index.is_open())
	{
		std::cerr << "index ������ �� �� �����ϴ�.\n";
		return;
	}
	if (index.peek() == std::ifstream::traits_type::eof())
	{
		std::cerr << "������¡�� ������ �����ϴ�. Ŀ���� ���� �� �����ϴ�.\n";
		return;
	}

	std::vector<std::string> entries;
	std::stringstream snapshot;
	std::string line;

	// �ؽ� ������ ���ڿ� ����
	while (std::getline(index, line))
	{
		entries.push_back(line);
		snapshot << line << "\n";
	}
	std::string commitHash = simpleHash(snapshot.str());
	std::string commitPath = ".minigit\\commits\\" + commitHash;

	// ���� HEAD �ؽÿ� commit �ؽ� ��
	std::string currentHeadHash = getCurrentHeadHash();
	if (currentHeadHash == commitHash)
	{
		std::cout << "���� Ŀ�԰� ������ �����Դϴ�. Ŀ���� ����մϴ�.\n";

		std::ofstream clearIndex(".minigit\\index", std::ios::trunc);
		if (!clearIndex.is_open()) {
			std::cerr << "index �ʱ�ȭ ����\n";
		}
		return;
	}

	// Ŀ�� ���丮 ����
	if (!createCommitDirectory(commitHash))
	{
		std::cerr << "Ŀ�� ���丮 ���� ����\n";
		return;
	}

	{
		std::ofstream destIndex(commitPath + "\\index", std::ios::trunc);
		if (!destIndex.is_open())
		{
			std::cerr << "Ŀ�Կ� index ���� ����\n";
			return;
		}
		destIndex << snapshot.str();
		destIndex.close();
	}

	for (auto& e : entries)
	{
		auto delim = e.find(":");
		std::string filename = (delim == std::string::npos ? e : e.substr(0, delim));
		if (!copyFileToCommit(filename, commitPath))
		{
			std::cerr << "���� ���� ����: " << filename << "\n";
			return;
		}
	}
	// �޼��� ���
	writeMeta(commitPath, message);

	// HEAD ������Ʈ
	std::ofstream head(".minigit\\HEAD");
	if (head.is_open())
	{
		head << commitHash;
		head.close();
	}

	// index �ʱ�ȭ
	std::ofstream clearIndex(".minigit\\index", std::ios::trunc); // truncate
	if (!clearIndex.is_open())
	{
		std::cerr << "index �ʱ�ȭ ����\n";
	}
}