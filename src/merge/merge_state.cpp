#include "merge/merge_state.h"
#include "merge/merge_utils.h"
#include "branch/branch_utils.h"
#include "checkout/checkout_utils.h"
#include "index/index_utils.h"
#include "utils/utils.h"

#include <fstream>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem; 

void saveMergeState(const std::string& currentHash, const std::string& targetHash)
{
	std::ofstream out(".minigit\\MERGE_STATE");
	if (!out.is_open()) return;

	out << currentHash << "\n" << targetHash << "\n";
	out.close();
}

std::pair<std::string, std::string> loadMergeState()
{
	std::ifstream in(".minigit/MERGE_STATE");
	if (!in.is_open()) return { "", "" };

	std::string current, target;
	std::getline(in, current);
	std::getline(in, target);
	return { current, target };
}

void clearMergeState()
{
	fs::remove(".minigit\\MERGE_STATE");
}


void backupCurrentFilesBeforeMerge(const std::string& currentHash)
{
	auto index = parseIndex(".minigit\\commits\\" + currentHash + "\\index");
	backupIndexFiles(index, ".minigit\\_merge_backup");
}

void restoreFilesFromBackup()
{
	std::string currentBranchHash = getCurrentBranchHash();
	auto index = parseIndex(".minigit\\commits\\" + currentBranchHash + "\\index");

	for (const auto& [filename, _] : index)
	{
		std::string backupPath = ".minigit\\_merge_backup";
		restoreFile(backupPath, filename);
	}
}

void clearBackup()
{
	fs::path backupPath(".minigit\\_merge_backup");

	if (!fs::exists(backupPath))
	{
		// ��� ������ ���ʿ� ���ٸ� ������ �Ѿ
		return;
	}

	try
	{
		std::uintmax_t count = fs::remove_all(backupPath);
		if (count == 0)
		{
			std::cerr << "[���] ��� ������ ���������� ������ �׸��� �����ϴ�.\n";
		}
		else
		{
			std::cout << "[info] ��� ���� ���� �Ϸ� (" << count << "�� �׸� ���ŵ�)\n";
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "[����] ��� ���� �� ���� �߻�: " << e.what() << "\n";
	}
}

void saveMergeConflicts(const std::vector<std::string>& conflicts)
{
	std::ofstream out(".minigit\\MERGE_CONFLICTS");
	if (!out.is_open()) return;

	for (const auto& file : conflicts)
	{
		std::string hash = simpleHash(file);
		out << file << ":" << hash << "\n";
	}
	out.close();
}
