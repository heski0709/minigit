#pragma once
#include <string>
#include <unordered_set>

/**
* @brief �־��� Ŀ���� �ٸ� Ŀ���� ����(ancestor)���� Ȯ��
*
* @param ancestor �������� Ȯ���� Ŀ�� �ؽ�
* @paran descendant Ž���� ��� Ŀ�� �ؽ�
* @return true descendant�� ancestor�� �ڼ��� ���
* @return false ancestor�� ���� ���� ��� (�Ǵ� ��� ����)
*/
bool isAncestor(const std::string& ancestor, const std::string& descendant);


/**
 * @brief Ư�� Ŀ���� ��� ������� Ž���Ͽ� �������� ��ȯ
 *
 * @param startHash ������ Ž���� Ŀ�� �ؽ�
 */
std::unordered_set<std::string> collectAncestors(const std::string& startHash);

/**
 * @brief �� Ŀ���� ���� ������ ã�� (���� ����� ����)
 */
std::string findCommonAncestor(const std::string& hashA, const std::string& hashB);