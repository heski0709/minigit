#pragma once
#include <string>
#include <vector>


/**
* @brief �� Ŀ���� idnex ������ ���Ͽ� �浹 ������ ��ȯ
*
* @param currentHash ���� �귣ġ�� Ŀ�� �ؽ�
* @param targetHash ���� ��� �귣ġ�� Ŀ�� �ؽ�
* @return std::vector<std::string> �浹�� �߻��� ���� �̸� ���
*/
std::vector<std::string> detectConflicts(
	const std::string& currentHash,
	const std::string& targetHash);

/**
 * @brief �浹 ������ ��Ŀ �������� ���� ���ڿ��� ��ȯ�մϴ�.
 *
 * @param currentContent ���� �귣ġ�� ���� ����
 * @param targetContent ���� ��� �귣ġ�� ���� ����
 * @param currentLabel ��Ŀ�� ���� �귣ġ �� (�⺻��: "HEAD")
 * @param targetLabel ��Ŀ�� ���� ��� �� (�⺻��: "MERGE")
 * @return �浹 ��Ŀ�� ���Ե� ���� ���ڿ�
 *
 * @details
 * �Ʒ��� ���� �������� ����� �����˴ϴ�:
 * @code
 * <<<<<<< HEAD
 * (currentContent)
 * =======
 * (targetContent)
 * >>>>>>> MERGE
 * @endcode
 */
std::string generateConflictMarkedContent(
    const std::string& currentContent,
    const std::string& targetContent,
    const std::string& currentLabel = "HEAD",
    const std::string& targetLabel = "MERGE"
);

/**
 * @brief �浹 ������ ���Ͽ� ����ϸ�, conflict ��Ŀ�� �����մϴ�.
 *
 * @param filename �浹 ������ ����� ���� ���
 * @param currentContent ���� �귣ġ�� ���� ����
 * @param targetContent ���� ��� �귣ġ�� ���� ����
 * @param currentLabel ��Ŀ�� ���� �귣ġ �� (�⺻��: "HEAD")
 * @param targetLabel ��Ŀ�� ���� ��� �� (�⺻��: "MERGE")
 *
 * @details
 * ���������� generateConflictMarkedContent()�� ����Ͽ� ��Ŀ�� ������ ��,
 * ���Ͽ� ����ϴ�. ������ ������ ���� ��� ���� �޽����� ����մϴ�.
 */
void markConflict(
    const std::string& filename,
    const std::string& currentContent,
    const std::string& targetContent,
    const std::string& currentLabel = "HEAD",
    const std::string& targetLabel = "MERGE"
);