#pragma once
#include <string>

/**
 * @brief ���� ���� ������ ������� ���� Ŀ���� ����
 *
 * @param currentHash ���� �귣ġ�� Ŀ�� �ؽ�
 * @param targetHash ������ ��� �귣ġ�� Ŀ�� �ؽ�
 */
void mergeCommitFromState(const std::string& currentHash, const std::string& targetHash, const std::string& message);