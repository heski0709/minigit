#include "branch_utils.h"
#include <fstream>

/**
 * @brief ���� HEAD�� ����Ű�� �귣ġ���� ��ȯ�Ѵ�.
 *
 * @return std::string �귣ġ�� (��: "main"), ���� �� �� ���ڿ�
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
* @brief �귣ġ HEAD�� ����� Ŀ�� �ؽø� �������� �Լ�
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
