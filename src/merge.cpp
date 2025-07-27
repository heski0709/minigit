#include "merge.h"
#include "branch_utils.h"
#include "checkout.h"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

/**
* @brief �־��� Ŀ���� �ٸ� Ŀ���� ����(ancestor)���� Ȯ��
* 
* @param ancestor �������� Ȯ���� Ŀ�� �ؽ�
* @paran descendant Ž���� ��� Ŀ�� �ؽ�
* @return true descendant�� ancestor�� �ڼ��� ���
* @return false ancestor�� ���� ���� ��� (�Ǵ� ��� ����)
*/
bool isAncestor(const std::string& ancestor, const std::string& descendant)
{
	std::string current = descendant;
	while (!current.empty())
	{
		if (current == ancestor) return true;

		std::ifstream meta(".minigit\\commits\\" + current + "\\meta.txt");
		if (!meta.is_open()) break;

		std::string line;
		while (std::getline(meta, line))
		{
			// ��Ÿ �������� parent �ؽ� ����
			if (line.rfind("parent: ", 0) == 0)
			{
				current = line.substr(8);
				break;
			}
		}

		meta.close();
	}

	return false;
}

/**
* @brief ������ �귣ġ�� ���� �귣ġ�� �����Ѵ�.
* @details fast-forward ��ĸ� �����ϸ�, �浹 ó�� �� ���� �θ� ������ �������� ����
*
* @param targetBranch ���� ��� �귣ġ �̸�
*/
void mergeBranch(const std::string& targetBranch)
{
	std::string targetPath = ".minigit\\refs\\heads\\" + targetBranch;

	// ��� �귣ġ�� �����ϴ��� Ȯ��
	if (!fs::exists(targetPath))
	{
		std::cerr << "���� ��� �귣ġ�� �������� �ʽ��ϴ�.\n";
		return;
	}

	// ���� ��� �귣ġ�� Ŀ�� �ؽ� �б�
	std::string targetHash;
	std::ifstream in(targetPath);
	std::getline(in, targetHash);
	in.close();

	if (targetHash.empty())
	{
		std::cerr << "���� ��� �귣ġ�� Ŀ���� �����ϴ�.\n";
		return;
	}

	std::string currentHash = getCurrentBranchHash();
	std::string currentBranch = getCurrentBranchName();

	if (currentHash.empty())
	{
		std::cerr << "���� �귣ġ '" << currentBranch << "'�� Ŀ�� �̷��� ���� ������ �� �����ϴ�.\n";
		return;
	}

	// Fast-forward �������� Ȯ��
	if (isAncestor(currentHash, targetHash))
	{
		std::cout << "Fast-forward ������ �����մϴ�...\n";

		// Ŀ�� ���� ���� �� HEAD ����
		checkoutCommit(targetHash);
		updateBranchHead(targetHash);

		std::cout << "���� �Ϸ�: '" << targetBranch << "' -> '" << currentBranch << "'\n";
	}
	else
	{
		std::cerr << "Fast-forward ������ �Ұ����մϴ�.\n";
		std::cerr << "(���� Ŀ�� ����� ���� �������� �ʽ��ϴ�.)\n";
	}
}
