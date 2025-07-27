#include "commit.h"
#include "branch_utils.h"
#include "commit_utils.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

void commit(const std::string& message)
{
	std::ifstream index(".minigit\\index");

	// index가 없거나, 비어있다면 커밋 중단
	if (!index.is_open())
	{
		std::cerr << "index 파일을 열 수 없습니다.\n";
		return;
	}
	if (index.peek() == std::ifstream::traits_type::eof())
	{
		std::cerr << "스테이징된 파일이 없습니다. 커밋을 만들 수 없습니다.\n";
		return;
	}

	std::vector<std::string> entries;
	std::stringstream snapshot;
	std::string line;

	// 해시 생성용 문자열 누적
	while (std::getline(index, line))
	{
		entries.push_back(line);
		snapshot << line << "\n";
	}
	std::string commitHash = improvedHash(entries);
	std::string commitPath = ".minigit\\commits\\" + commitHash;

	// 현재 브랜치의 HEAD 해시와 commit 해시 비교
	std::string currentBranchHash = getCurrentBranchHash();
	if (currentBranchHash == commitHash)
	{
		std::cout << "이전 커밋과 동일한 상태입니다. 커밋을 취소합니다.\n";

		std::ofstream clearIndex(".minigit\\index", std::ios::trunc);
		if (!clearIndex.is_open()) {
			std::cerr << "index 초기화 실패\n";
		}
		return;
	}

	// 커밋 디렉토리 생성
	if (!createCommitDirectory(commitHash))
	{
		std::cerr << "커밋 디렉토리 생성 실패\n";
		return;
	}

	{
		std::ofstream destIndex(commitPath + "\\index", std::ios::trunc);
		if (!destIndex.is_open())
		{
			std::cerr << "커밋용 index 복사 실패\n";
			return;
		}
		destIndex << snapshot.str();
		destIndex.close();
	}

	for (auto& e : entries)
	{
		auto delim = e.find(":");
		std::string filename = (delim == std::string::npos ? e : e.substr(0, delim));
		if (!copyFileToCommit(filename, commitPath))
		{
			std::cerr << "파일 복사 실패: " << filename << "\n";
			return;
		}
	}
	// 메세지 기록
	writeMeta(commitPath, { currentBranchHash }, message);


	// 브랜치 HEAD 업데이트
	updateBranchHead(commitHash);
	
	// index 초기화
	std::ofstream clearIndex(".minigit\\index", std::ios::trunc); // truncate
	if (!clearIndex.is_open())
	{
		std::cerr << "index 초기화 실패\n";
	}
}