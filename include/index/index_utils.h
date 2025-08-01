#pragma once
#include <unordered_map>
#include <string>

/**
* @brief ���� index ������ ������ string���� ����
*/
std::string getCurrentIndexSnapshot();

/**
* @brief Ư�� Ŀ���� index ������ �Ľ��Ͽ� (���ϸ� -> �ؽ�) map���� ��ȯ
*
* @param commitHash Ŀ�� �ؽ�
* @return std::unordered_map<std::string, std::string> ���ϸ�� �����ϴ� �ؽ�
*/
std::unordered_map<std::string, std::string> parseIndex(const std::string& commitHash);
