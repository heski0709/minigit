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
	// 명령어 파싱
	if (argc < 2)
	{
		std::cout << R"(사용법
init: minigit를 초기화합니다.
add [파일명]: 파일을 스테이징합니다.
commit [메세지]: 커밋 메세지 작성
log: 커밋 기록을 출력합니다.
checkout <commit_hash>: 해당 커밋기준으로 파일을 복원합니다.
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
			std::cerr << "add 명령에는 파일명이 필요합니다.\n";
			return 1;
		}
		add(argv[2]);
	}
	else if (command == "commit")
	{
		if (argc < 3)
		{
			std::cerr << "commit 명령에는 메시지가 필요합니다.\n";
			return 1;
		}

		// 메세지를 따옴표로 묶지 않아도 여러 단어 메세지를 받을 수 있도록
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
			std::cerr << "사용법: minigit checkout <commit_hash>\n";
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
		std::cerr << "알 수 없는 명령어입니다: " << command << "\n";
	}

	return 0;
}