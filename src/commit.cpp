#include "commit.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <chrono>

/// <summary>
/// HEAD에 저장된 커밋 해시를 가져오는 함수
/// </summary>
std::string getCurrentHeadHash()
{
	std::ifstream head(".minigit\\HEAD");
	if (!head.is_open()) return "";

	std::string hash;
	std::getline(head, hash);
	return hash;
}

/// <summary>
/// 커밋 디렉토리 생성
/// </summary>
bool createCommitDirectory(const std::string& hash)
{
	std::string path = ".minigit\\commits\\" + hash;
	return std::filesystem::create_directory(path);
}

/// <summary>
/// 파일 복사
/// </summary>
/// <param name="src">원본 파일</param>
/// <param name="destDir">복사된 파일을 놓을 폴더 위치</param>
bool copyFileToCommit(const std::string& src, const std::string& destDir)
{
	std::ifstream in(src, std::ios::binary);
	std::ofstream out(destDir + "\\" + src, std::ios::binary);
	if (!in.is_open() || !out.is_open()) return false;

	out << in.rdbuf();
	return true;
}

/// <summary>
/// 커밋 메시지와 시간을 기록하는 함수
/// </summary>
/// <param name="destDir">저장되는 폴더 위치</param>
/// <param name="message">커밋 메세지</param>
void writeMeta(const std::string& destDir, const std::string& message)
{
	std::ofstream meta(destDir + "\\meta.txt");
	if (!meta.is_open()) return;

	std::string parentHash = getCurrentHeadHash();
	auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	char timeStr[100];

	if (ctime_s(timeStr, sizeof(timeStr), &now) != 0)
	{
		meta << "parent: " << parentHash << "\n";
		meta << "message: " << message << "\n";
		meta << "timestamp: (시간 파싱 실패)\n";
	}
	else
	{
		meta << "parent: " << parentHash << "\n";
		meta << "message: " << message << "\n";
		meta << "timestamp: " << timeStr;
	}
	meta.close();
}

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

	std::stringstream snapshot;
	std::string line;

	// 해시 생성용 문자열 누적
	while (std::getline(index, line))
	{
		snapshot << line << "\n";
	}
	std::string commitHash = simpleHash(snapshot.str());
	std::string commitPath = ".minigit\\commits\\" + commitHash;

	// 현재 HEAD 해시와 commit 해시 비교
	std::string currentHeadHash = getCurrentHeadHash();
	if (currentHeadHash == commitHash)
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

	// index 파일을 다시 읽어 파일 복사
	index.clear();
	index.seekg(0);

	while (std::getline(index, line))
	{
		std::string filename = line.substr(0, line.find(":")); // 파일명 추출
		if (!copyFileToCommit(filename, commitPath))
		{
			std::cerr << "파일 복사 실패: " << filename << "\n";
		}
	}

	// 메세지 기록
	writeMeta(commitPath, message);

	// HEAD 업데이트
	std::ofstream head(".minigit\\HEAD");
	if (head.is_open())
	{
		head << commitHash;
		head.close();
	}

	// index 초기화
	std::ofstream clearIndex(".minigit\\index", std::ios::trunc); // truncate
	if (!clearIndex.is_open())
	{
		std::cerr << "index 초기화 실패\n";
	}
}