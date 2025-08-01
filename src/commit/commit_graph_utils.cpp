#include "commit/commit_graph_utils.h"

#include <fstream>
#include <unordered_set>
#include <queue>

bool isAncestor(const std::string& ancestor, const std::string& descendant)
{
	std::string current = descendant;
	while (!current.empty())
	{
		if (current == ancestor) return true;

		std::ifstream meta(".minigit\\commits\\" + current + "\\meta.txt");
		if (!meta.is_open()) break;

		std::string line;
		while (std::getline(meta, line))
		{
			// ��Ÿ �������� parent �ؽ� ����
			if (line.rfind("parent: ", 0) == 0)
			{
				current = line.substr(8);
				break;
			}
		}

		meta.close();
	}

	return false;
}


std::unordered_set<std::string> collectAncestors(const std::string& startHash)
{
	std::unordered_set<std::string> visited;
	std::deque<std::string> queue = { startHash };

	while (!queue.empty())
	{
		std::string current = queue.front();
		queue.pop_front();

		if (visited.count(current)) continue;
		visited.insert(current);

		std::ifstream meta(".minigit\\commits\\" + current + "\\meta.txt");
		if (!meta.is_open()) continue;

		std::string line;
		while (std::getline(meta, line))
		{
			if (line.rfind("parent: ", 0) == 0)
			{
				std::string parentsStr = line.substr(8);
				size_t pos = 0;
				while ((pos = parentsStr.find(",") != std::string::npos))
				{
					queue.push_back(parentsStr.substr(0, pos));
					parentsStr.erase(0, pos + 1);
				}

				if (!parentsStr.empty()) queue.push_back(parentsStr);
				break;
			}
		}
	}

	return visited;
}

/**
* @brief �� Ŀ���� ���� ������ ã�� (���� ����� ����, LCA ����)
*
* @details hashA�� ��� ������ set���� �����ϰ�,
*          hashB�� ���� Ž���ϸ鼭 ó�� ������ ������ ��ȯ.
*          -> ���� ���� set���� ������ �ʰ�, ���� ����� ���� ��ȯ ���� ����.
 */
std::string findCommonAncestor(const std::string& hashA, const std::string& hashB)
{
	auto ancestorsA = collectAncestors(hashA);
	std::deque<std::string> queue = { hashB };
	std::unordered_set<std::string> visited;

	while (!queue.empty())
	{
		std::string current = queue.front();
		queue.pop_front();

		if (visited.count(current)) continue;
		visited.insert(current);

		// ���� ����� ���� �߰�
		if (ancestorsA.count(current)) return current;

		std::ifstream meta(".minigit\\commits\\" + current + "\\meta.txt");
		if (!meta.is_open()) continue;

		std::string line;
		while (std::getline(meta, line))
		{
			if (line.rfind("parent: ", 0) == 0)
			{
				std::string parentsStr = line.substr(8);
				size_t pos = 0;
				while ((pos = parentsStr.find(",") != std::string::npos))
				{
					queue.push_back(parentsStr.substr(0, pos));
					parentsStr.erase(0, pos + 1);
				}

				if (!parentsStr.empty()) queue.push_back(parentsStr);
				break;
			}
		}
	}

	return "";
}

