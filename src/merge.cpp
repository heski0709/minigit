#include "merge.h"
#include "branch_utils.h"
#include "checkout.h"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

/**
* @brief 주어진 커밋이 다른 커밋의 조상(ancestor)인지 확인
* 
* @param ancestor 조상인지 확인할 커밋 해시
* @paran descendant 탐색할 대상 커밋 해시
* @return true descendant가 ancestor의 자손일 경우
* @return false ancestor와 관련 없는 경우 (또는 경로 없음)
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
			// 메타 정보에서 parent 해시 추출
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

	if (currentHash.empty())
	{
		std::cerr << "현재 브랜치 '" << currentBranch << "'에 커밋 이력이 없어 병합할 수 없습니다.\n";
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
		std::cerr << "Fast-forward 병합이 불가능합니다.\n";
		std::cerr << "(병합 커밋 기능은 아직 지원되지 않습니다.)\n";
	}
}
