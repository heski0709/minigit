#include "checkout.h"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

/**
* @brief 커밋 디렉토리 내의 파일을 현재 작업 디렉토리로 복원
* @param srcDir 커밋 디렉토리 경로
* @param filename 복원 파일
*/
bool restoreFile(const std::string& srcDir, const std::string& filename)
{
	std::ifstream in(srcDir + "\\" + filename, std::ios::binary);
	std::ofstream out(filename, std::ios::binary);
	if (!in.is_open() || !out.is_open()) return false;
	out << in.rdbuf();
	return true;
}

/**
* @brief 지정된 커밋 해시를 기준으로 HEAD 갱신, 해당 커밋의 파일 상태로 작업 디렉토리 복원
* Git의 'reset --hard <hash>'와 유사한 방식으로 작동
* @param hash 복원할 커밋의 해시코드
*/
void checkoutCommit(const std::string& hash)
{
	std::string commitDir = ".minigit\\commits\\" + hash;

	if (!fs::exists(commitDir) || !fs::is_directory(commitDir))
	{
		std::cerr << "존재하지 않는 커밋 해시입니다: " << hash << "\n";
		return;
	}

	std::ifstream index(commitDir + "\\index");
	if (!index.is_open())
	{
		std::cerr << "커밋 index 파일이 존재하지 않습니다.\n";
		return;
	}

	std::string line;
	while (std::getline(index, line))
	{
		// 예: "a.txt:abcd1234" 형식 → 파일명만 추출
		auto delim = line.find(':');
		if (delim == std::string::npos) continue;

		std::string filename = line.substr(0, delim);

		bool ok = restoreFile(commitDir, filename);
		if (ok)
			std::cout << filename << " 복원 완료\n";
		else
			std::cout << filename << " 복원 실패\n";
	}

	// HEAD 포인터 이동
	std::ofstream head(".minigit\\HEAD");
	if (!head.is_open())
	{
		std::cerr << "HEAD 업데이트 실패\n";
		return;
	}
	head << hash;

	std::cout << "checkout 완료. HEAD → " << hash << "\n";
}