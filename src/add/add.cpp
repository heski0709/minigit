#include <iostream>
#include <fstream>
#include <string>
#include "add/add.h"
#include "utils/utils.h"

bool isAlreadyStaged(const std::string& hash)
{
	std::ifstream indexFile(".minigit\\index");
	if (!indexFile.is_open()) return false;

	std::string line;
	while (std::getline(indexFile, line))
	{
		if (line.find(hash) != std::string::npos)
		{
			return true; // 이미 존재
		}
	}

	return false;
}


void add(const std::string& filename)
{
	// 파일이 존재하는지 확인
	if (!fileExists(filename))
	{
		std::cerr << "파일이 존재하지 않습니다 : " << filename << "\n";
		return;
	}

	std::string content = readFileContent(filename);
	std::string hash = simpleHash(content);

	if (isAlreadyStaged(hash))
	{
		std::cout << "이미 스테이지에 존재합니다.\n";
		return;
	}

	// index 파일 열기(append 모드)
	std::ofstream indexFile(".minigit\\index", std::ios::app);
	if (!indexFile.is_open())
	{
		std::cerr << "index 파일을 열 수 없습니다.\n";
		return;
	}

	indexFile << filename << ":" << hash << "\n";
	indexFile.close();

	std::cout << "스테이징 완료: " << filename << " (" << hash << ")\n";
}

