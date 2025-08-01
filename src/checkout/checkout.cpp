#include "checkout/checkout.h"
#include "checkout/checkout_utils.h"
#include "branch/branch_utils.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;


/**
* @brief ������ Ŀ�� �ؽø� �������� HEAD ����, �ش� Ŀ���� ���� ���·� �۾� ���丮 ����
* Git�� 'reset --hard <hash>'�� ������ ������� �۵�
* @param hash ������ Ŀ���� �ؽ��ڵ�
*/
void checkoutCommit(const std::string& hash)
{
	std::string commitDir = ".minigit\\commits\\" + hash;

	if (!fs::exists(commitDir) || !fs::is_directory(commitDir))
	{
		std::cerr << "�������� �ʴ� Ŀ�� �ؽ��Դϴ�: " << hash << "\n";
		return;
	}

	std::ifstream index(commitDir + "\\index");
	if (!index.is_open())
	{
		std::cerr << "Ŀ�� index ������ �������� �ʽ��ϴ�.\n";
		return;
	}

	std::string line;
	while (std::getline(index, line))
	{
		// ��: "a.txt:abcd1234" ���� �� ���ϸ� ����
		auto delim = line.find(':');
		if (delim == std::string::npos) continue;

		std::string filename = line.substr(0, delim);

		bool ok = restoreFile(commitDir, filename);
		if (ok)
			std::cout << filename << " ���� �Ϸ�\n";
		else
			std::cout << filename << " ���� ����\n";
	}

	updateBranchHead(hash);

	std::cout << "checkout �Ϸ�. HEAD �� " << hash << "\n";
}

/**
* @brief �귣ġ �̸��� �������� �ش� �귣ġ�� �̵�
* 
* @param branchName �̵��� �귣ġ �̸�
*/
void checkoutBranch(const std::string& branchName)
{
	std::string branchPath = ".minigit\\refs\\heads\\" + branchName;

	// �귣ġ ���� Ȯ��
	if (!fs::exists(branchPath))
	{
		std::cerr << "�ش��ϴ� �귣ġ�� �������� �ʽ��ϴ�.\n";
		return;
	}

	// �ش� �귣ġ HEAD�� Ŀ�� �ؽ� �б�
	std::ifstream branchHEAD(branchPath);
	if (!branchHEAD.is_open())
	{
		std::cerr << "�귣ġ ������ ���� �� �����ϴ�.\n";
		return;
	}

	std::string commitHash;
	std::getline(branchHEAD, commitHash);
	branchHEAD.close();

	// HEAD �����͸� �ش� �귣ġ�� ����
	std::ofstream head(".minigit\\HEAD");
	if (!head.is_open())
	{
		std::cerr << "HEAD ������Ʈ ����\n";
		return;
	}
	head << "refs/heads/" << branchName;

	// �ش� Ŀ�� �������� checkout
	if (!commitHash.empty())
	{
		checkoutCommit(commitHash);
	}

	std::cout << "�귣ġ '" << branchName << "' �� �̵��Ϸ�\n";
}