#include "branch_utils.h"
#include <fstream>

/**
 * @brief 현재 HEAD가 가리키는 브랜치명을 반환한다.
 *
 * @return std::string 브랜치명 (예: "main"), 실패 시 빈 문자열
 */
std::string getCurrentBranchName()
{
	std::ifstream head(".minigit\\HEAD");
	if (!head.is_open()) return "";

	std::string ref;
	std::getline(head, ref);
	head.close();

	const std::string prefix = "refs/heads/";
	if (ref.rfind(prefix, 0) == 0)
	{
		return ref.substr(prefix.length());
	}

	return "";
}


/**
* @brief 브랜치 HEAD에 저장된 커밋 해시를 가져오는 함수
*/
std::string getCurrentBranchHash()
{
	std::string currentBranchName = getCurrentBranchName();
	std::ifstream head(".minigit\\refs\\heads\\" + currentBranchName);
	if (!head.is_open()) return "";

	std::string hash;
	std::getline(head, hash);
	return hash;
}

void updateBranchHead(const std::string hash)
{
	std::string currentBranchName = getCurrentBranchName();
	std::ofstream head(".minigit\\refs\\heads\\" + currentBranchName);
	if (head.is_open())
	{
		head << hash;
		head.close();
	}
}
