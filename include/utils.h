#pragma once
#include <string>
#include <vector>

/**
* @brief ������ �����ϴ��� Ȯ���ϴ� �Լ�
* @param filename ��θ� ������ �����̸�
*/
bool fileExists(const std::string& filename);

/**
* @brief ���� ��ü ������ �б�
* @param filename ��θ� ������ �����̸�
*/
std::string readFileContent(const std::string& filename);

/**
* @brief ���� ������ �ؽ÷� ��ȯ�ϴ� �Լ�
*/
std::string improvedHash(const std::vector<std::string>& entries);
std::string simpleHash(const std::string& content);