#pragma once
#include <string>

/**
* @brief index ���Ͽ� ���� �ؽð� �����ϴ��� Ȯ��
* @param hash �ش� ������ �ؽ��ڵ�
*/
bool isAlreadyStaged(const std::string& hash);

/**
* @brief index ���Ͽ� ���ϸ��� �߰��ϴ� �Լ�
* @param filename ���� �̸�
*/
void add(const std::string& filename);