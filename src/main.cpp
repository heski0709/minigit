#include <iostream>
#include <string>
#include "init.h"
#include "add.h"
#include "commit.h"
#include "log.h"
#include "checkout.h"
#include "branch.h"
#include "branch_utils.h"
#include "status.h"
#include "merge.h"
#include "merge_utils.h"

int main(int argc, char* argv[])
{
	// ��ɾ� �Ľ�
	if (argc < 2)
	{
		std::cout << R"(����
init: minigit�� �ʱ�ȭ�մϴ�.
add <filename>: ������ ������¡�մϴ�.
commit <message>: Ŀ�� �޼��� �ۼ�
log [--online]: Ŀ�� ����� ����մϴ�.
checkout <branch_name | commit_hash>: �ش� �귣ġ �Ǵ� Ŀ�� �������� ������ �����մϴ�.
branch [name]: �귣ġ ����Ʈ ���, �귣ġ ����
status: ���� �귣ġ�� ���¸� ����մϴ�.
merge <branch>: �ش� �귣ġ�� �����մϴ�. �浹�� �߻��� ��� �Ʒ� ��ɾ �Է����ּ���.
merge [--abort | --continue]:
	--abort		�����ϱ� ������ �ǵ����ϴ�.
	--continue	�浹�� �߻��� ��, ������ �����մϴ�.  
)";
		return 0;
	}

	std::string command = argv[1];

	if (command == "init")
	{
		init();
	}
	else if (command == "add")
	{
		if (argc < 3)
		{
			std::cerr << "add ��ɿ��� ���ϸ��� �ʿ��մϴ�.\n";
			return 1;
		}
		add(argv[2]);
	}
	else if (command == "commit")
	{
		if (argc < 3)
		{
			std::cerr << "commit ��ɿ��� �޽����� �ʿ��մϴ�.\n";
			return 1;
		}

		// �޼����� ����ǥ�� ���� �ʾƵ� ���� �ܾ� �޼����� ���� �� �ֵ���
		std::string msg;
		for (int i = 2; i < argc; ++i)
		{
			if (i > 2) msg += " ";
			msg += argv[i];
		}
		commit(msg);
	}
	else if (command == "log")
	{
		if (argc == 2)
			showLog(false);
		else if (argc == 3 && std::string(argv[2]) == "--oneline")
			showLog(true);
		else if (argc == 3 && std::string(argv[2]) == "--graph")
			showLogWithGraph();
	}
	else if (command == "checkout")
	{
		if (argc < 3)
		{
			std::cerr << "����: minigit checkout <commit_hash | branch_name>\n";
			return 1;
		}
		std::string target = argv[2];

		// FIXME: �ؽ����� �귣ġ���� �Ǵ��ϴ� �ڵ� �߰� ����
		if (target.find_first_not_of("0123456789abcdef") == std::string::npos)
		{
			checkoutCommit(target);
		}
		else
		{
			checkoutBranch(target);
		}
	}
	else if (command == "branch")
	{
		handleBranchCommand(argc, argv);
	}
	else if (command == "status")
	{
		showStatus();
	}
	else if (command == "merge")
	{
		if (argc < 3)
		{
			std::cerr << R"(����:
minigit merge <branch>: ���� �õ�
minigit merge --abort: ���� �ߴ�
minigit merge --continue: ���� �Ϸ�
)";
			return 1;
		}

		std::string option = argv[2];

		if (option == "--abort")
		{
			if (!isMergeInProgress())
			{
				std::cerr << "[����] ���� ���� ���� �ƴմϴ�.\n";
				return 1;
			}

			auto [current, _] = loadMergeState();
			if (current.empty())
			{
				std::cerr << "[����] ���� ���¸� ������ �� �����ϴ�.\n";
				return 1;
			}

			std::cout << "���� �ߴ�: ���� Ŀ�� ���·� �ǵ����ϴ�.\n";
			restoreFilesFromBackup();
			updateBranchHead(current);
			clearMergeState();
			clearBackup();
		}
		else if (option == "--continue")
		{
			if (!isMergeInProgress())
			{
				std::cerr << "[����] ���� ���� �ƴմϴ�.\n";
				return 1;
			}

			auto [current, target] = loadMergeState();
			if (current.empty() || target.empty())
			{
				std::cerr << "[����] ���� ���� ������ �ҿ����մϴ�.\n";
				return 1;
			}

			std::cout << "���� Ŀ���� �����մϴ�...\n";
			mergeCommitFromState(current, target, "Merge continue");
			clearMergeState();
			clearBackup();
		}
		else
		{
			mergeBranch(option);
		}
	}
	else
	{
		std::cerr << "�� �� ���� ��ɾ��Դϴ�: " << command << "\n";
	}

	return 0;
}