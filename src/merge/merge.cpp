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
* @brief 지정한 브랜치를 현재 브랜치에 병합한다.
* @details fast-forward 방식만 지원하며, 충돌 처리 및 다중 부모 병합은 지원하지 않음
*
* @param targetBranch 병합 대상 브랜치 이름
*/
void mergeBranch(const std::string& targetBranch)
{
	std::string targetPath = ".minigit\\refs\\heads\\" + targetBranch;

	// 대상 브랜치가 존재하는지 확인
	if (!fs::exists(targetPath))
	{
		std::cerr << "병합 대상 브랜치가 존재하지 않습니다.\n";
		return;
	}


	// 병합 상태가 존재하는지 확인 (존재하면 리턴)
	if (fs::exists(MERGE_STATE_PATH))
	{
		std::cerr << "충돌 해결 후 merge [--abort | --continue] 명령어를 실행 시켜야합니다.\n";
		return;
	}


	// 병합 대상 브랜치의 커밋 해시 읽기
	std::string targetHash;
	std::ifstream in(targetPath);
	std::getline(in, targetHash);
	in.close();

	if (targetHash.empty())
	{
		std::cerr << "병합 대상 브랜치에 커밋이 없습니다.\n";
		return;
	}

	std::string currentHash = getCurrentBranchHash();
	std::string currentBranch = getCurrentBranchName();

	if (currentHash.empty() || targetHash.empty())
	{
		std::cerr << "병합 불가: 브랜치 커밋 정보 없음\n";
		return;
	}

	// Fast-forward 가능한지 확인
	if (isAncestor(currentHash, targetHash))
	{
		std::cout << "Fast-forward 병합을 수행합니다...\n";

		// 커밋 내용 복원 및 HEAD 갱신
		checkoutCommit(targetHash);
		updateBranchHead(targetHash);

		std::cout << "병합 완료: '" << targetBranch << "' -> '" << currentBranch << "'\n";
	}
	else
	{
		// 3-way 병합 수행 (충돌 가능성 포함)
		std::cout << "[3-way merge] 병합을 수행합니다...\n";
		mergeCommit(targetBranch);
	}
}

/**
* @brief 병합 커밋 생성 (다중 부모)
* 
* @param branchToMerge 병합할 대상 브랜치 이름
*/
void mergeCommit(const std::string& branchToMerge)
{
	std::string currentBranchHash = getCurrentBranchHash();
	std::string targetBrachHash = getBranchHash(branchToMerge);

	if (targetBrachHash.empty())
	{
		std::cerr << "병합할 브랜치 '" << branchToMerge << "' 는 커밋 이력이 없습니다.\n";
		return;
	}

	auto conflicts = detectConflicts(currentBranchHash, targetBrachHash);
	if (!conflicts.empty())
	{
		std::cout << "[경고] 충돌 발생! 수동으로 해결이 필요합니다.\n";

		// 현재 파일 백업
		backupCurrentFilesBeforeMerge(currentBranchHash);

		for (const auto& file : conflicts)
		{
			std::string baseA = readFileContent(".minigit\\commits\\" + currentBranchHash + "\\" + file);
			std::string baseB = readFileContent(".minigit\\commits\\" + targetBrachHash + "\\" + file);
			markConflict(file, baseA, baseB);
			std::cout << "- " << file << " <- 충돌 마킹 완료\n";
		}

		// 병합 상태 저장
		saveMergeState(currentBranchHash, targetBrachHash);

		std::cout << "충돌을 수동으로 해결한 후, `minigit merge --continue`로 병합을 완료하세요.\n";
		return; // 병합 중단
	}

	applyAutoMergeFiles(currentBranchHash, targetBrachHash);
	updateIndexAfterAutoMerge(currentBranchHash, targetBrachHash);

	// 병합 메세지 및 스냅샷 해시 생성
	std::string message = "Merge branch '" + branchToMerge + "'";
	mergeCommitFromState(currentBranchHash, targetBrachHash, message);
}

