#include "commit.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <chrono>

/// <summary>
/// HEAD�� ����� Ŀ�� �ؽø� �������� �Լ�
/// </summary>
std::string getCurrentHeadHash()
{
	std::ifstream head(".minigit\\HEAD");
	if (!head.is_open()) return "";

	std::string hash;
	std::getline(head, hash);
	return hash;
}

/// <summary>
/// Ŀ�� ���丮 ����
/// </summary>
bool createCommitDirectory(const std::string& hash)
{
	std::string path = ".minigit\\commits\\" + hash;
	return std::filesystem::create_directory(path);
}

/// <summary>
/// ���� ����
/// </summary>
/// <param name="src">���� ����</param>
/// <param name="destDir">����� ������ ���� ���� ��ġ</param>
bool copyFileToCommit(const std::string& src, const std::string& destDir)
{
	std::ifstream in(src, std::ios::binary);
	std::ofstream out(destDir + "\\" + src, std::ios::binary);
	if (!in.is_open() || !out.is_open()) return false;

	out << in.rdbuf();
	return true;
}

/// <summary>
/// Ŀ�� �޽����� �ð��� ����ϴ� �Լ�
/// </summary>
/// <param name="destDir">����Ǵ� ���� ��ġ</param>
/// <param name="message">Ŀ�� �޼���</param>
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

	std::stringstream snapshot;
	std::string line;

	// �ؽ� ������ ���ڿ� ����
	while (std::getline(index, line))
	{
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

	// index ������ �ٽ� �о� ���� ����
	index.clear();
	index.seekg(0);

	while (std::getline(index, line))
	{
		std::string filename = line.substr(0, line.find(":")); // ���ϸ� ����
		if (!copyFileToCommit(filename, commitPath))
		{
			std::cerr << "���� ���� ����: " << filename << "\n";
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