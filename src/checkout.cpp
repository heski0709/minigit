#include "checkout.h"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

/// <summary>
/// Ŀ�� ���丮 ���� ������ ���� �۾� ���丮�� ����
/// </summary>
/// <param name="srcDir">Ŀ�� ���丮 ���</param>
/// <param name="filename">������ ����</param>
/// <returns>���� ����</returns>
bool restoreFile(const std::string& srcDir, const std::string& filename)
{
	std::ifstream in(srcDir + "\\" + filename, std::ios::binary);
	std::ofstream out(filename, std::ios::binary);
	if (!in.is_open() || !out.is_open()) return false;
	out << in.rdbuf();
	return true;
}

/// <summary>
/// ������ Ŀ�� �ؽø� �������� HEAD�� ����, �ش� Ŀ���� ���� ���·� �۾� ���丮 ����
/// Git�� 'reset --hard <�ؽ�>'�� ������ ������� �۵���.
/// </summary>
/// <param name="hash">������ Ŀ���� �ؽ� ���ڿ�</param>
void checkoutCommit(const std::string& hash)
{
	std::string commitDir = ".minigit\\commits\\" + hash;

	if (!fs::exists(commitDir) || !fs::is_directory(commitDir))
	{
		std::cerr << "�������� �ʴ� Ŀ�� �ؽ��Դϴ�: " << hash << "\n";
		return;
	}

	std::ifstream index(commitDir + "\\index");
	if (!index.is_open())
	{
		std::cerr << "Ŀ�� index ������ �������� �ʽ��ϴ�.\n";
		return;
	}

	std::string line;
	while (std::getline(index, line))
	{
		// ��: "a.txt:abcd1234" ���� �� ���ϸ� ����
		auto delim = line.find(':');
		if (delim == std::string::npos) continue;

		std::string filename = line.substr(0, delim);

		bool ok = restoreFile(commitDir, filename);
		if (ok)
			std::cout << filename << " ���� �Ϸ�\n";
		else
			std::cout << filename << " ���� ����\n";
	}

	// HEAD ������ �̵�
	std::ofstream head(".minigit\\HEAD");
	if (!head.is_open())
	{
		std::cerr << "HEAD ������Ʈ ����\n";
		return;
	}
	head << hash;

	std::cout << "checkout �Ϸ�. HEAD �� " << hash << "\n";
}