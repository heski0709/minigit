#include "branch/branch.h"
#include "branch/branch_utils.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

/**
* @brief 현재 존재하는 브랜치 목록을 출력한다.
* @details	파일 시스템에서 '.minigit/refs/heads/' 하위 파일들을 조회하여 브랜치명을 출력.
*			현재 브랜치는 앞에 '*' 표시를 붙임
*/
void listBranches()
{
	std::string current = getCurrentBranchName();
	std::string refDir = ".minigit/refs/heads";

	if (!fs::exists(refDir))
	{
		std::cerr << "브랜치 정보가 없습니다.\n";
		return;
	}

	for (const auto& file : fs::directory_iterator(refDir))
	{
		std::string name = file.path().filename().string();

		// 현재 브랜치일 경우 * 표시
		if (name == current)
			std::cout << "* " << name << "\n";
		else
			std::cout << "  " << name << "\n";
	}
}

/**
* @brief 현재 HEAD가 가리키는 커밋 해시를 기반으로 새로운 브랜치 생성
* @param newBranchName 새로운 브랜치 이름
* @details	새 브랜치 파일은 '.minigit/refs/heads/<브랜치명>'에 생성되며,
*			그 내용은 현재 HEAD가 가리키는 커밋 해시이다.
*/
void createBranch(const std::string& newBranchName)
{
	std::string currentHash;

	// HEAD에서 현재 브랜치가 참조하는 ref 경로 읽기
	std::ifstream head(".minigit\\HEAD");
	if (!head.is_open())
	{
		std::cerr << "HEAD 열기 실패\n";
		return;
	}

	std::string refPath;
	std::getline(head, refPath);
	head.close();

	// ref파일에서 커밋 해시 읽기
	std::ifstream ref(".minigit\\" + refPath);
	if (ref.is_open())
	{
		std::getline(ref, currentHash);
		ref.close();
	}

	// 이미 존재하는 브랜치일 경우 리턴
	std::string newBranchPath = ".minigit\\refs\\heads\\" + newBranchName;
	if (fs::exists(newBranchPath))
	{
		std::cerr << "이미 존재하는 브랜치입니다.\n";
		return;
	}

	// 브랜치 파일 생성 및 커밋 해시 기록
	std::ofstream out(newBranchPath);
	if (!out.is_open())
	{
		std::cerr << "브랜치 생성 실패\n";
		return;
	}

	out << currentHash;
	out.close();

	std::cout << "브랜치 생성됨: " << newBranchName << "\n";
}

/**
* @brief branch 명령어 처리 함수
* 
* @param argc main 함수의 argc
* @param argv main 함수의 argv
*/
void handleBranchCommand(int argc, char* argv[])
{
	if (argc == 2)
		listBranches();
	else if (argc == 3)
	{
		std::string name = argv[2];
		createBranch(name);
	}
	else
	{
		std::cerr << "사용법:\n";
		std::cerr << "minigit branch		← 목록 보기\n";
		std::cerr << "minigit branch <name>	← 브랜치 생성\n";
	}
}