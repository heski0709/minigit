#pragma once
#include <string>

/// <summary>
/// ������ �����ϴ��� Ȯ���ϴ� �Լ�
/// </summary>
bool fileExists(const std::string& filename);

/// <summary>
/// ���� ��ü ������ ���ڿ��� �б�
/// </summary>
std::string readFileContent(const std::string& filename);
std::string simpleHash(const std::string& content);
