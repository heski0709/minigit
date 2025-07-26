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
	if (!createDirectory(".minigit"))
	{
		std::cerr << "�̹� �ʱ�ȭ�� ������Դϴ�.\n";
		return;
	}

	// .minigit ���� ���丮 ����
	createDirectory(".minigit\\commits");
	createDirectory(".minigit\\refs");
	createDirectory(".minigit\\refs\heads");

	// index ���� ���� (������¡ ���� ���)
	std::ofstream index(".minigit\\index");
	if (!index.is_open())
	{
		std::cerr << "index ���� ���� ���� \n";
		return;
	}
	index.close();

	// main �귣ġ ����
	std::ofstream mainBranch(".minigit\\refs\\heads\\main");
	if (!mainBranch.is_open())
	{
		std::cerr << "main �귣ġ ���� ����\n";
		return;
	}

	// HEAD ���� ���� (main �귣ġ�� ����Ŵ)
	std::ofstream head(".minigit\\HEAD");
	if (!head.is_open())
	{
		std::cerr << "HEAD ���� ���� ����\n";
		return;
	}

	head << "refs/heads/main";
	head.close();

	std::cout << "MiniGit �ʱ�ȭ �Ϸ�\n";
}
