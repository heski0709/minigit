#pragma once
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

/**
* @brief Ư�� Ŀ���� index ������ �Ľ��Ͽ� (���ϸ� -> �ؽ�) map���� ��ȯ
*
* @param commitHash Ŀ�� �ؽ�
* @return std::unordered_map<std::string, std::string> ���ϸ�� �����ϴ� �ؽ�
*/
std::unordered_map<std::string, std::string> parseIndex(const std::string& commitHash);

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
* @brief ���Ͽ� �浹 ��Ŀ(`<<<<<`, `======`, `>>>>>`)�� ����
*
* @param filename �浹���� �̸�
* @param contentA ���� �귣ġ�� ���� ����
* @param contentB ���� ��� �귣ġ�� ���� ����
*/
void markConflict(
	const std::string& filename,
	const std::string& contentA,
	const std::string& contentB
);

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

/**
* @brief merge �� ���¸� ��� (�浹 �߻� ��)
* 
* @param currentHash ���� �귣ġ�� Ŀ�� �ؽ�
* @param targetHash ���� ��� �귣ġ�� Ŀ�� �ؽ�
*/
void saveMergeState(const std::string& currentHash, const std::string& targetHash);

/**
* @brief MERGE_STATE ���� ���� (abort �Ǵ� �Ϸ� ��)
*/
void clearMergeState();

/**
* @brief merge ���� ������ �����ϴ� ���� ��ȯ
*
* @return �����ϸ� true, ���� ��� false
*/
bool isMergeInProgress();

/**
* @brief merge ���¸� �ҷ��� currentHash, targetHash�� ��ȯ
*/
std::pair<std::string, std::string> loadMergeState();

/**
* @brief conflict �߻� �� ���� �귣ġ�� ���� ���
*/
void backupCurrentFilesBeforeMerge(const std::string& currentHash);

/**
* @brief merge --abort ���� �� ����س��� ���� ����
*/
void restoreFilesFromBackup();

/**
* @brief .minigit/_merge_backup/ ���� ����
*/
void clearBackup();

/**
* @brief ���� �۾� ���丮�� ���ϵ��� �������� .minigit/index ������ ����
*
* @details ���� Ŀ�Ե� ���� ����� ��������, �۾� ���丮�� ���� ���� ������ �ؽ��Ͽ� index ���Ͽ� ����� (�浹 ���� ���� �� ���)
*
* @param baseHash ���� Ŀ�� �ؽ� (������ ���̽��� �Ǵ� Ŀ��)
* @param targetHash ������ �귣ġ�� Ŀ�� �ؽ�
*/
void updateIndexAfterAutoMerge(const std::string& baseHash, const std::string& targetHash);