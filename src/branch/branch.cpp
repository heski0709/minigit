#include "branch/branch.h"
#include "branch/branch_utils.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

/**
* @brief ���� �����ϴ� �귣ġ ����� ����Ѵ�.
* @details	���� �ý��ۿ��� '.minigit/refs/heads/' ���� ���ϵ��� ��ȸ�Ͽ� �귣ġ���� ���.
*			���� �귣ġ�� �տ� '*' ǥ�ø� ����
*/
void listBranches()
{
	std::string current = getCurrentBranchName();
	std::string refDir = ".minigit/refs/heads";

	if (!fs::exists(refDir))
	{
		std::cerr << "�귣ġ ������ �����ϴ�.\n";
		return;
	}

	for (const auto& file : fs::directory_iterator(refDir))
	{
		std::string name = file.path().filename().string();

		// ���� �귣ġ�� ��� * ǥ��
		if (name == current)
			std::cout << "* " << name << "\n";
		else
			std::cout << "  " << name << "\n";
	}
}

/**
* @brief ���� HEAD�� ����Ű�� Ŀ�� �ؽø� ������� ���ο� �귣ġ ����
* @param newBranchName ���ο� �귣ġ �̸�
* @details	�� �귣ġ ������ '.minigit/refs/heads/<�귣ġ��>'�� �����Ǹ�,
*			�� ������ ���� HEAD�� ����Ű�� Ŀ�� �ؽ��̴�.
*/
void createBranch(const std::string& newBranchName)
{
	std::string currentHash;

	// HEAD���� ���� �귣ġ�� �����ϴ� ref ��� �б�
	std::ifstream head(".minigit\\HEAD");
	if (!head.is_open())
	{
		std::cerr << "HEAD ���� ����\n";
		return;
	}

	std::string refPath;
	std::getline(head, refPath);
	head.close();

	// ref���Ͽ��� Ŀ�� �ؽ� �б�
	std::ifstream ref(".minigit\\" + refPath);
	if (ref.is_open())
	{
		std::getline(ref, currentHash);
		ref.close();
	}

	// �̹� �����ϴ� �귣ġ�� ��� ����
	std::string newBranchPath = ".minigit\\refs\\heads\\" + newBranchName;
	if (fs::exists(newBranchPath))
	{
		std::cerr << "�̹� �����ϴ� �귣ġ�Դϴ�.\n";
		return;
	}

	// �귣ġ ���� ���� �� Ŀ�� �ؽ� ���
	std::ofstream out(newBranchPath);
	if (!out.is_open())
	{
		std::cerr << "�귣ġ ���� ����\n";
		return;
	}

	out << currentHash;
	out.close();

	std::cout << "�귣ġ ������: " << newBranchName << "\n";
}

/**
* @brief branch ��ɾ� ó�� �Լ�
* 
* @param argc main �Լ��� argc
* @param argv main �Լ��� argv
*/
void handleBranchCommand(int argc, char* argv[])
{
	if (argc == 2)
		listBranches();
	else if (argc == 3)
	{
		std::string name = argv[2];
		createBranch(name);
	}
	else
	{
		std::cerr << "����:\n";
		std::cerr << "minigit branch		�� ��� ����\n";
		std::cerr << "minigit branch <name>	�� �귣ġ ����\n";
	}
}