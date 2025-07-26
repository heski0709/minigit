#include <iostream>
#include <windows.h>
#include <fstream>
#include "init.h"

bool createDirectory(const std::string& path)
{
	// CreateDirectory: ASCII ���ڿ� ��ο� ���� ���丮 ����
	// �̹� �����ϴ� ��� ERROR_ALREAY_EXISTS ��ȯ
	return (CreateDirectoryA(path.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS);
}

void init()
{
	// .minigit ���丮 ����
	if (!createDirectory(".minigit")) return;

	// .minigit/commits ���丮 ����
	if (!createDirectory(".minigit\\commits")) return;

	// index ���� ���� (������¡ ���� ���)
	std::ofstream index(".minigit\\index");
	if (!index.is_open())
	{
		std::cerr << "index ���� ���� ���� \n";
		return;
	}
	index.close();

	// HEAD ���� ���� (���� Ŀ���� ����Ŵ)
	std::ofstream head(".minigit\\HEAD");
	if (!head.is_open())
	{
		std::cerr << "HEAD ���� ���� ����\n";
		return;
	}

	head.close();

	std::cout << "MiniGit �ʱ�ȭ �Ϸ�\n";
}
