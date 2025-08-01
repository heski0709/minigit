#include "checkout/checkout.h"
#include "checkout/checkout_utils.h"
#include "branch/branch_utils.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;


/**
* @brief 지정된 커밋 해시를 기준으로 HEAD 갱신, 해당 커밋의 파일 상태로 작업 디렉토리 복원
* Git의 'reset --hard <hash>'와 유사한 방식으로 작동
* @param hash 복원할 커밋의 해시코드
*/
void checkoutCommit(const std::string& hash)
{
	std::string commitDir = ".minigit\\commits\\" + hash;

	if (!fs::exists(commitDir) || !fs::is_directory(commitDir))
	{
		std::cerr << "존재하지 않는 커밋 해시입니다: " << hash << "\n";
		return;
	}

	std::ifstream index(commitDir + "\\index");
	if (!index.is_open())
	{
		std::cerr << "커밋 index 파일이 존재하지 않습니다.\n";
		return;
	}

	std::string line;
	while (std::getline(index, line))
	{
		// 예: "a.txt:abcd1234" 형식 → 파일명만 추출
		auto delim = line.find(':');
		if (delim == std::string::npos) continue;

		std::string filename = line.substr(0, delim);

		bool ok = restoreFile(commitDir, filename);
		if (ok)
			std::cout << filename << " 복원 완료\n";
		else
			std::cout << filename << " 복원 실패\n";
	}

	updateBranchHead(hash);

	std::cout << "checkout 완료. HEAD → " << hash << "\n";
}

/**
* @brief 브랜치 이름을 기준으로 해당 브랜치로 이동
* 
* @param branchName 이동할 브랜치 이름
*/
void checkoutBranch(const std::string& branchName)
{
	std::string branchPath = ".minigit\\refs\\heads\\" + branchName;

	// 브랜치 존재 확인
	if (!fs::exists(branchPath))
	{
		std::cerr << "해당하는 브랜치가 존재하지 않습니다.\n";
		return;
	}

	// 해당 브랜치 HEAD의 커밋 해시 읽기
	std::ifstream branchHEAD(branchPath);
	if (!branchHEAD.is_open())
	{
		std::cerr << "브랜치 정보를 읽을 수 없습니다.\n";
		return;
	}

	std::string commitHash;
	std::getline(branchHEAD, commitHash);
	branchHEAD.close();

	// HEAD 포인터를 해당 브랜치로 설정
	std::ofstream head(".minigit\\HEAD");
	if (!head.is_open())
	{
		std::cerr << "HEAD 업데이트 실패\n";
		return;
	}
	head << "refs/heads/" << branchName;

	// 해당 커밋 기준으로 checkout
	if (!commitHash.empty())
	{
		checkoutCommit(commitHash);
	}

	std::cout << "브랜치 '" << branchName << "' 로 이동완료\n";
}