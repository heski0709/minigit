#include <iostream>
#include <string>
#include "init.h"
#include "add.h"
#include "commit.h"
#include "log.h"

int main(int argc, char* argv[])
{
	// ��ɾ� �Ľ�
	if (argc < 2)
	{
		std::cout << R"(
����
minigit init: minigit�� �ʱ�ȭ�մϴ�.
minigit add [���ϸ�]: ������ ������¡�մϴ�.
minigit commit [�޼���]: Ŀ�� �޼��� �ۼ�
minigit log: Ŀ�� ����� ����մϴ�.
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
	else
	{
		std::cerr << "�� �� ���� ��ɾ��Դϴ�: " << command << "\n";
	}

	return 0;
}