#include "branch/branch_utils.h"
#include "checkout/checkout.h"
#include "commit/commit_utils.h"
#include "commit/commit_graph_utils.h"
#include "merge/merge.h"
#include "merge/merge_conflict.h"
#include "merge/merge_state.h"
#include "merge/merge_utils.h"
#include "utils/utils.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>

namespace fs = std::filesystem;
constexpr const char* MERGE_STATE_PATH = ".minigit\\MERGE_STATE";


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


	// ���� ���°� �����ϴ��� Ȯ�� (�����ϸ� ����)
	if (fs::exists(MERGE_STATE_PATH))
	{
		std::cerr << "�浹 �ذ� �� merge [--abort | --continue] ��ɾ ���� ���Ѿ��մϴ�.\n";
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

	if (currentHash.empty() || targetHash.empty())
	{
		std::cerr << "���� �Ұ�: �귣ġ Ŀ�� ���� ����\n";
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
		// 3-way ���� ���� (�浹 ���ɼ� ����)
		std::cout << "[3-way merge] ������ �����մϴ�...\n";
		mergeCommit(targetBranch);
	}
}

/**
* @brief ���� Ŀ�� ���� (���� �θ�)
* 
* @param branchToMerge ������ ��� �귣ġ �̸�
*/
void mergeCommit(const std::string& branchToMerge)
{
	std::string currentBranchHash = getCurrentBranchHash();
	std::string targetBrachHash = getBranchHash(branchToMerge);

	if (targetBrachHash.empty())
	{
		std::cerr << "������ �귣ġ '" << branchToMerge << "' �� Ŀ�� �̷��� �����ϴ�.\n";
		return;
	}

	auto conflicts = detectConflicts(currentBranchHash, targetBrachHash);
	if (!conflicts.empty())
	{
		std::cout << "[���] �浹 �߻�! �������� �ذ��� �ʿ��մϴ�.\n";

		// ���� ���� ���
		backupCurrentFilesBeforeMerge(currentBranchHash);

		for (const auto& file : conflicts)
		{
			std::string baseA = readFileContent(".minigit\\commits\\" + currentBranchHash + "\\" + file);
			std::string baseB = readFileContent(".minigit\\commits\\" + targetBrachHash + "\\" + file);
			markConflict(file, baseA, baseB);
			std::cout << "- " << file << " <- �浹 ��ŷ �Ϸ�\n";
		}

		// ���� ���� ����
		saveMergeState(currentBranchHash, targetBrachHash);

		std::cout << "�浹�� �������� �ذ��� ��, `minigit merge --continue`�� ������ �Ϸ��ϼ���.\n";
		return; // ���� �ߴ�
	}

	applyAutoMergeFiles(currentBranchHash, targetBrachHash);
	updateIndexAfterAutoMerge(currentBranchHash, targetBrachHash);

	// ���� �޼��� �� ������ �ؽ� ����
	std::string message = "Merge branch '" + branchToMerge + "'";
	mergeCommitFromState(currentBranchHash, targetBrachHash, message);
}

