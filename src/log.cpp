#include "log.h"
#include "branch_utils.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

namespace fs = std::filesystem;

struct CommitInfo
{
	std::string hash;
	std::string message;
	std::string timestamp;
	fs::file_time_type time; // 정렬용
};

/**
* @brief 로그를 출력하는 함수
* @param onelineMode true면 한 줄 로그
*/
void showLog(bool onelineMode)
{
	std::string currentHash = getCurrentBranchHash();
	std::string currentBranch = getCurrentBranchName();

	if (currentHash.empty())
	{
		std::cout << "커밋 내역이 없습니다.\n";
		return;
	}

	std::cout << "커밋 내역 (최신 순):\n\n";
	std::string headHash = currentHash;

	while (!currentHash.empty())
	{
		std::string commitDir = ".minigit\\commits\\" + currentHash;
		std::ifstream meta(commitDir + "\\meta.txt");
		if (!meta.is_open())
		{
			std::cerr << "커밋 " << currentHash << " 의 meta.txt를 찾을 수 없습니다.\n";
			break;
		}

		std::string line, parent, message, timestamp;

		while (std::getline(meta, line))
		{
			if (line.rfind("parent: ", 0) == 0)
			{
				parent = line.substr(8);
			}
			else if (line.rfind("message: ", 0) == 0)
			{
				message = line.substr(9);
			}
			else if (line.rfind("timestamp: ", 0) == 0)
			{
				timestamp = line.substr(11);
			}
		}

		if (!onelineMode)
		{
			std::cout << "commit " << currentHash;
			if (currentHash == headHash)
				std::cout << " (HEAD → " << currentBranch << ")";
			std::cout << "\n";
			std::cout << "message: " << message << "\n";
			std::cout << "timestamp: " << timestamp << "\n\n";
		}
		else
		{
			std::string shortHash = currentHash.substr(0, 4);
			std::cout << shortHash;
			if (currentHash == headHash)
				std::cout << " (HEAD → " << currentBranch << ") " << message << "\n";
			else
				std::cout << " " << message << "\n";
		}

		currentHash = parent;
	}
}

/**
* @brief 커밋 그래프 스타일 로그 출력
*/
void showLogWithGraph()
{
	std::string currentHash = getCurrentBranchHash();
	std::string currentBranch = getCurrentBranchName();

	if (currentHash.empty())
	{
		std::cout << "커밋 내역이 없습니다.\n";
		return;
	}

	std::string headHash = currentHash;

	while (!currentHash.empty())
	{
		std::string commitDir = ".minigit\\commits\\" + currentHash;
		std::ifstream meta(commitDir + "\\meta.txt");

		if (!meta.is_open())
		{
			std::cerr << "커밋 " << currentHash << " 의 meta.txt를 찾을 수 없습니다.\n";
			break;
		}

		std::string line, parent, message;

		while (std::getline(meta, line))
		{
			if (line.rfind("parent: ", 0) == 0)
				parent = line.substr(8);
			else if (line.rfind("message: ", 0) == 0)
				message = line.substr(9);
		}

		std::string shortHash = currentHash.substr(0, 4);
		std::cout << "* " << shortHash;

		if (headHash == currentHash)
			std::cout << " (HEAD → " << currentBranch << ")";

		std::cout << " " << message << "\n";

		currentHash = parent;
	}
}
