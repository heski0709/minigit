#include <iostream>
#include <fstream>
#include <string>
#include "add/add.h"
#include "utils/utils.h"

bool isAlreadyStaged(const std::string& hash)
{
	std::ifstream indexFile(".minigit\\index");
	if (!indexFile.is_open()) return false;

	std::string line;
	while (std::getline(indexFile, line))
	{
		if (line.find(hash) != std::string::npos)
		{
			return true; // �̹� ����
		}
	}

	return false;
}


void add(const std::string& filename)
{
	// ������ �����ϴ��� Ȯ��
	if (!fileExists(filename))
	{
		std::cerr << "������ �������� �ʽ��ϴ� : " << filename << "\n";
		return;
	}

	std::string content = readFileContent(filename);
	std::string hash = simpleHash(content);

	if (isAlreadyStaged(hash))
	{
		std::cout << "�̹� ���������� �����մϴ�.\n";
		return;
	}

	// index ���� ����(append ���)
	std::ofstream indexFile(".minigit\\index", std::ios::app);
	if (!indexFile.is_open())
	{
		std::cerr << "index ������ �� �� �����ϴ�.\n";
		return;
	}

	indexFile << filename << ":" << hash << "\n";
	indexFile.close();

	std::cout << "������¡ �Ϸ�: " << filename << " (" << hash << ")\n";
}

