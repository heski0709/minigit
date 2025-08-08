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
	if (!createDirectory(".minigit"))
	{
		std::cerr << "이미 초기화된 저장소입니다.\n";
		return;
	}

	// .minigit 하위 디렉토리 생성
	createDirectory(".minigit\\commits");
	createDirectory(".minigit\\refs");
	createDirectory(".minigit\\refs\\heads");

	// index 파일 생성 (스테이징 영역 기록)
	std::ofstream index(".minigit\\index");
	if (!index.is_open())
	{
		std::cerr << "index 파일 생성 실패 \n";
		return;
	}
	index.close();

	// main 브랜치 생성
	std::ofstream mainBranch(".minigit\\refs\\heads\\main");
	if (!mainBranch.is_open())
	{
		std::cerr << "main 브랜치 생성 실패\n";
		return;
	}

	// HEAD 파일 생성 (main 브랜치를 가리킴)
	std::ofstream head(".minigit\\HEAD");
	if (!head.is_open())
	{
		std::cerr << "HEAD 파일 생성 실패\n";
		return;
	}

	head << "refs/heads/main";
	head.close();

	std::cout << "MiniGit 초기화 완료\n";
}
