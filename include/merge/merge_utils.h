#pragma once
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>


/**
* @brief merge ���� ������ �����ϴ� ���� ��ȯ
*
* @return �����ϸ� true, ���� ��� false
*/
bool isMergeInProgress();

/**
* @brief ���� �۾� ���丮�� ���ϵ��� �������� .minigit/index ������ ����
*
* @details ���� Ŀ�Ե� ���� ����� ��������, �۾� ���丮�� ���� ���� ������ �ؽ��Ͽ� index ���Ͽ� ����� (�浹 ���� ���� �� ���)
*
* @param baseHash ���� Ŀ�� �ؽ� (������ ���̽��� �Ǵ� Ŀ��)
* @param targetHash ������ �귣ġ�� Ŀ�� �ؽ�
*/
void updateIndexAfterAutoMerge(const std::string& baseHash, const std::string& targetHash);

void updateIndexFromWorkingDirectory();

void applyAutoMergeFiles(const std::string& targetHash);
