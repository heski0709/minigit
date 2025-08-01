#include "checkout/checkout_utils.h"
#include "index/index_utils.h"

#include <fstream>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

/**
* @brief Ŀ�� ���丮 ���� ������ ���� �۾� ���丮�� ����
* @param srcDir Ŀ�� ���丮 ���
* @param filename ���� ����
* @return ���� ����
*/
bool restoreFile(const std::string& srcDir, const std::string& filename)
{
	std::filesystem::path src = std::filesystem::path(srcDir) / filename;
	std::filesystem::path dst = filename;

	try
	{
		// ������ ����� ���� ���丮 ����
		std::filesystem::path parentPath = dst.parent_path();

		if (!parentPath.empty())
			std::filesystem::create_directories(parentPath);

		std::ifstream in(src, std::ios::binary);
		if (!in.is_open())
		{
			std::cerr << "[���� ����] ���� ���� ���� ����: " << src << "\n";
			return false;
		}

		std::ofstream out(dst, std::ios::binary);
		if (!out.is_open())
		{
			std::cerr << "[���� ����] ���� ���� ���� ����: " << dst << "\n";
			return false;
		}

		out << in.rdbuf();

		if (out.fail())
		{
			std::cerr << "[���� ����] ���� �� ���� �߻�: " << dst << "\n";
			return false;
		}
		return true;
	}
	catch (const std::exception& e)
	{
		std::cerr << "[���� ����] " << filename << ": " << e.what() << "\n";
		return false;
	}
}
