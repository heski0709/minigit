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
	fs::file_time_type time; // ���Ŀ�
};

void showLog(bool onelineMode)
{
	std::string currentHash = getCurrentBranchHash();

	if (currentHash.empty())
	{
		std::cout << "Ŀ�� ������ �����ϴ�.\n";
		return;
	}

	std::cout << "Ŀ�� ���� (�ֽ� ��):\n\n";
	std::string headHash = currentHash;

	while (!currentHash.empty())
	{
		std::string commitDir = ".minigit\\commits\\" + currentHash;
		std::ifstream meta(commitDir + "\\meta.txt");
		if (!meta.is_open())
		{
			std::cerr << "Ŀ�� " << currentHash << " �� meta.txt�� ã�� �� �����ϴ�.\n";
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
			if (currentHash == headHash)
				std::cout << "HEAD �� " << currentHash << "\n";
			std::cout << "commit " << currentHash << "\n";
			std::cout << "message: " << message << "\n";
			std::cout << "timestamp: " << timestamp << "\n\n";
		}
		else
		{
			std::string shortHash = currentHash.substr(0, 4);
			if (currentHash == headHash)
				std::cout << "HEAD �� " << shortHash << " " << message << "\n";
			else
				std::cout << shortHash << " " << message << "\n";
		}

		currentHash = parent;
	}
}