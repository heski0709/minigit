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