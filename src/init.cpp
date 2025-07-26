#include <iostream>
#include <windows.h>
#include <fstream>
#include "init.h"

bool createDirectory(const std::string& path)
{
	// CreateDirectory: ASCII 문자열 경로에 대해 디렉토리 생성
	// 이미 존재하는 경우 ERROR_ALREAY_EXISTS 반환
	return (CreateDirectoryA(path.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS);
}

void init()
{
	// .minigit 디렉토리 생성
	if (!createDirectory(".minigit")) return;

	// .minigit/commits 디렉토리 생성
	if (!createDirectory(".minigit\\commits")) return;

	// index 파일 생성 (스테이징 영역 기록)
	std::ofstream index(".minigit\\index");
	if (!index.is_open())
	{
		std::cerr << "index 파일 생성 실패 \n";
		return;
	}
	index.close();

	// HEAD 파일 생성 (현재 커밋을 가리킴)
	std::ofstream head(".minigit\\HEAD");
	if (!head.is_open())
	{
		std::cerr << "HEAD 파일 생성 실패\n";
		return;
	}

	head.close();

	std::cout << "MiniGit 초기화 완료\n";
}
