#include "commit.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <chrono>

/**
* @brief HEAD에 저장된 커밋 해시를 가져오는 함수
*/
std::string getCurrentHeadHash()
{
	std::ifstream head(".minigit\\HEAD");
	if (!head.is_open()) return "";

	std::string hash;
	std::getline(head, hash);
	return hash;
}

/**
* @brief 커밋 디렉토리 생성
* @param hash 커밋 해시코드
*/
bool createCommitDirectory(const std::string& hash)
{
	std::string path = ".minigit\\commits\\" + hash;
	return std::filesystem::create_directory(path);
}

/**
* @brief 커밋 디렉토리로 파일 복사
* @param src 원본파일
* @param destDir 복사 파일을 저장할 커밋 디렉토리
*/
bool copyFileToCommit(const std::string& src, const std::string& destDir)
{
	std::ifstream in(src, std::ios::binary);
	std::ofstream out(destDir + "\\" + src, std::ios::binary);
	if (!in.is_open() || !out.is_open()) return false;

	out << in.rdbuf();
	return true;
}

/**
* @brief 커밋 메시지와 시간을 기록하는 함수
* @param destDir 커밋 디렉토리
* @param message 커밋된 메세지
*/
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

	std::vector<std::string> entries;
	std::stringstream snapshot;
	std::string line;

	// 해시 생성용 문자열 누적
	while (std::getline(index, line))
	{
		entries.push_back(line);
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