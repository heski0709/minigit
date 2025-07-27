#include "commit.h"
#include "branch_utils.h"
#include "commit_utils.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

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
	std::string commitHash = improvedHash(entries);
	std::string commitPath = ".minigit\\commits\\" + commitHash;

	// ���� �귣ġ�� HEAD �ؽÿ� commit �ؽ� ��
	std::string currentBranchHash = getCurrentBranchHash();
	if (currentBranchHash == commitHash)
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
	writeMeta(commitPath, { currentBranchHash }, message);


	// �귣ġ HEAD ������Ʈ
	updateBranchHead(commitHash);
	
	// index �ʱ�ȭ
	std::ofstream clearIndex(".minigit\\index", std::ios::trunc); // truncate
	if (!clearIndex.is_open())
	{
		std::cerr << "index �ʱ�ȭ ����\n";
	}
}