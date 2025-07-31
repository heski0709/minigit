#include "merge.h"
#include "utils.h"
#include "branch_utils.h"
#include "checkout.h"
#include "commit_utils.h"
#include "merge_utils.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>

namespace fs = std::filesystem;
constexpr const char* MERGE_STATE_PATH = ".minigit\\MERGE_STATE";

/**
* @brief ���� index ������ ������ string���� ����
*/
std::string getCurrentIndexSnapshot()
{
	return readFileContent(".minigit\\index");
}

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

		std::cout << "�浹�� �������� �ذ��� ��, `minigit commit`���� ������ �Ϸ��ϼ���.\n";
		return; // ���� �ߴ�
	}

	updateIndexAfterAutoMerge(currentBranchHash, targetBrachHash);

	// ���� �޼��� �� ������ �ؽ� ����
	std::string message = "Merge branch '" + branchToMerge + "'";
	mergeCommitFromState(currentBranchHash, targetBrachHash, message);
}

/**
 * @brief ���� ���� ������ ������� ���� Ŀ���� ����
 *
 * @param currentHash ���� �귣ġ�� Ŀ�� �ؽ�
 * @param targetHash ������ ��� �귣ġ�� Ŀ�� �ؽ�
 */
void mergeCommitFromState(const std::string& currentHash, const std::string& targetHash, const std::string& message)
{
	// index snapshot Ȯ��
	std::string snapshot = getCurrentIndexSnapshot();
	std::vector<std::string> snapshotLines = splitLines(snapshot);
	std::string commitHash = improvedHash(snapshotLines);
	std::string commitPath = ".minigit\\commits\\" + commitHash;

	if (!createCommitDirectory(commitHash))
	{
		std::cerr << "Ŀ�� ���丮 ���� ����\n";
		return;
	}

	// index ����
	std::ofstream destIndex(commitPath + "\\index");
	if (!destIndex.is_open())
	{
		std::cerr << "index ���� ����\n";
		return;
	}
	destIndex << snapshot;
	destIndex.close();

	// ���� ����
	for (const auto& line : snapshotLines)
	{
		auto delim = line.find(":");
		if (delim == std::string::npos) continue;

		std::string filename = line.substr(0, delim);
		copyFileToCommit(filename, commitPath);
	}

	// ��Ÿ �ۼ�
	writeMeta(commitPath, { currentHash, targetHash }, message);

	// HEAD �귣ġ ������Ʈ
	updateBranchHead(commitHash);

	// index �ʱ�ȭ
	std::ofstream clearIndex(".minigit\\index", std::ios::trunc);
	if (!clearIndex.is_open())
		std::cerr << "index �ʱ�ȭ ����\n";

	std::cout << "���� Ŀ���� �Ϸ�Ǿ����ϴ�.\n";
}
