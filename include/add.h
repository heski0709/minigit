#pragma once
#include <string>

/// <summary>
/// index ���Ͽ� �̹� ���� �ؽð� �����ϴ��� Ȯ��
/// </summary>
bool isAlreadyStaged(const std::string& hash);

/// <summary>
/// index ���Ͽ� ���ϸ��� �߰��ϴ� �Լ�
/// </summary>
void add(const std::string& filename);