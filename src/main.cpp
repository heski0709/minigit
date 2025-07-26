#include <iostream>
#include <string>
#include "init.h"
#include "add.h"
#include "commit.h"
#include "log.h"
#include "checkout.h"
#include "branch.h"

int main(int argc, char* argv[])
{
	// ��ɾ� �Ľ�
	if (argc < 2)
	{
		std::cout << R"(����
init: minigit�� �ʱ�ȭ�մϴ�.
add [���ϸ�]: ������ ������¡�մϴ�.
commit [�޼���]: Ŀ�� �޼��� �ۼ�
log: Ŀ�� ����� ����մϴ�.
checkout <commit_hash>: �ش� Ŀ�Ա������� ������ �����մϴ�.
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
		if (argc >= 3 && std::string(argv[2]) == "--oneline")
		{
			showLog(true);
		}
		else
		{
			showLog(false);
		}
	}
	else if (command == "checkout")
	{
		if (argc < 3)
			std::cerr << "����: minigit checkout <commit_hash>\n";
		std::string target = argv[2];

		if (target.size() == 40 && target.find_first_not_of("0123456789abcdef") == std::string::npos) 
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
	else
	{
		std::cerr << "�� �� ���� ��ɾ��Դϴ�: " << command << "\n";
	}

	return 0;
}