#include <iostream>
#include <string>
#include "init.h"
#include "add/add.h"
#include "commit/commit.h"
#include "commit/commit_utils.h"
#include "log/log.h"
#include "checkout/checkout.h"
#include "branch/branch.h"
#include "branch/branch_utils.h"
#include "status/status.h"
#include "merge/merge.h"
#include "merge/merge_commit.h"
#include "merge/merge_state.h"
#include "merge/merge_conflict.h"
#include "merge/merge_utils.h"

int main(int argc, char* argv[])
{
	// 명령어 파싱
	if (argc < 2)
	{
		std::cout << R"(사용법
init: minigit를 초기화합니다.
add <filename>: 파일을 스테이징합니다.
commit <message>: 커밋 메세지 작성
log [--online]: 커밋 기록을 출력합니다.
checkout <branch_name | commit_hash>: 해당 브랜치 또는 커밋 기준으로 파일을 복원합니다.
branch [name]: 브랜치 리스트 출력, 브랜치 생성
status: 현재 브랜치의 상태를 출력합니다.
merge <branch>: 해당 브랜치와 병합합니다. 충돌이 발생할 경우 아래 명령어를 입력해주세요.
merge [--abort | --continue]:
	--abort		병합하기 전으로 되돌립니다.
	--continue	충돌이 발생한 후, 병합을 진행합니다.  
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
			std::cerr << "사용법: minigit checkout <commit_hash | branch_name>\n";
			return 1;
		}

		std::string input = argv[2];
		std::string refType = resolveReferenceType(input);

		if (refType == "branch")
			checkoutBranch(input);
		else if (refType == "commit")
			checkoutCommit(input);
		else
			std::cerr << "에러: '" << input << "'은(는) 유효한 브랜치, 커밋해시가 아닙니다.\n";
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
			std::cerr << R"(사용법:
minigit merge <branch>: 병합 시도
minigit merge --abort: 병합 중단
minigit merge --continue: 병합 완료
)";
			return 1;
		}

		std::string option = argv[2];

		if (option == "--abort")
		{
			if (!isMergeInProgress())
			{
				std::cerr << "[오류] 현재 병합 중이 아닙니다.\n";
				return 1;
			}

			auto [current, _] = loadMergeState();
			if (current.empty())
			{
				std::cerr << "[오류] 병합 상태를 복원할 수 없습니다.\n";
				return 1;
			}

			std::cout << "병합 중단: 이전 커밋 상태로 되돌립니다.\n";
			restoreFilesFromBackup();
			updateBranchHead(current);
			clearMergeState();
			clearBackup();
		}
		else if (option == "--continue")
		{
			if (!isMergeInProgress())
			{
				std::cerr << "[오류] 병합 중이 아닙니다.\n";
				return 1;
			}

			auto [current, target] = loadMergeState();
			if (current.empty() || target.empty())
			{
				std::cerr << "[오류] 병합 상태 정보가 불완전합니다.\n";
				return 1;
			}

			std::cout << "병합 커밋을 생성합니다...\n";
			mergeCommitFromState(current, target, "Merge continue");
			clearMergeState();
			clearMergeConflicts();
			clearBackup();
		}
		else
		{
			mergeBranch(option);
		}
	}
	else
	{
		std::cerr << "알 수 없는 명령어입니다: " << command << "\n";
	}

	return 0;
}