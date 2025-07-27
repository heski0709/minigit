#pragma once
#include <string>
#include <vector>

/**
* @brief Ŀ�� ���丮 ����
* @param hash Ŀ�� �ؽ��ڵ�
*/
bool createCommitDirectory(const std::string& hash);

/**
* @brief Ŀ�� ���丮�� ���� ����
* @param src ��������
* @param destDir ���� ������ ������ Ŀ�� ���丮
*/
bool copyFileToCommit(const std::string& src, const std::string& destDir);

/**
* @brief Ŀ�� �޽����� �ð��� ����ϴ� �Լ�
* @param destDir Ŀ�� ���丮
* @param message Ŀ�Ե� �޼���
*/
void writeMeta(const std::string& destDir, const std::vector<std::string>& parentHashes, const std::string& message);
