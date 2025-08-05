#pragma once
#include <string>
#include <vector>

/**
* @brief merge �� ���¸� ��� (�浹 �߻� ��)
* 
* @param currentHash ���� �귣ġ�� Ŀ�� �ؽ�
* @param targetHash ���� ��� �귣ġ�� Ŀ�� �ؽ�
*/
void saveMergeState(const std::string& currentHash, const std::string& targetHash);

/**
* @brief merge ���¸� �ҷ��� currentHash, targetHash�� ��ȯ
*/
std::pair<std::string, std::string> loadMergeState();

/**
* @brief MERGE_STATE ���� ���� (abort �Ǵ� �Ϸ� ��)
*/
void clearMergeState();


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
* @brief merge �� �浹�� �߻��� ���� ����
*
* @param conflicts �浹�� ���ϸ��
*/
void saveMergeConflicts(const std::vector<std::string>& conflicts);