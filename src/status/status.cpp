#include "status/status.h"
#include "branch/branch_utils.h"
#include "utils/utils.h"

#include <filesystem>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

std::string computeFileHash(const std::string& path)
{
	std::string content = readFileContent(path);
	return simpleHash(content);
}

/**
* @brief .minigit/index ������ �Ľ��Ͽ� map<filename, hash> ���·� ��ȯ
*/
std::unordered_map<std::string, std::string> loadIndex(const std::string& indexPath)
{
	std::unordered_map<std::string, std::string> indexMap;
	std::ifstream file(indexPath);
	std::string line;

	while (std::getline(file, line))
	{
		auto delim = line.find(":");
		if (delim == std::string::npos) continue;

		std::string name = line.substr(0, delim);
		std::string hash = line.substr(delim + 1);
		indexMap[name] = hash;
	}

	return indexMap;
}

/**
* @brief .minigit/commits/<lastHash>/index ������ �Ľ��Ͽ� map<filename, hash> ���·� ��ȯ
*/
std::unordered_map<std::string, std::string> loadLastCommitIndex()
{
	std::string lastHash = getCurrentBranchHash();
	if (lastHash.empty()) return {};
	
	std::string path = ".minigit\\commits\\" + lastHash + "\\index";
	return loadIndex(path);
}

/**
* @brief ���� �۾� ���丮 �������� ���¸� ���
*/
void showStatus()
{
	std::string branch = getCurrentBranchName();
	std::cout << "���� �귣ġ: " << branch << "\n\n";

	auto indexMap = loadIndex(".minigit\\index");
	auto commitMap = loadLastCommitIndex();

	std::unordered_set<std::string> modified;
	std::unordered_set<std::string> staged;
	std::unordered_set<std::string> untracked;

	// ������¡�� ���� (index�� ��ϵǾ� �ִ� ����)
	for (const auto& [file, hash] : indexMap)
		staged.insert(file);

	// ������ ���� Ȯ��
	for (const auto& [file, stagedHash] : indexMap)
	{
		if (!fs::exists(file)) continue;

		std::string nowHash = computeFileHash(file);
		if (nowHash != stagedHash)
			modified.insert(file);
	}

	// ��ü ���� ��� ��ȸ
	for (const auto& entry : fs::directory_iterator("."))
	{
		std::string filename = entry.path().filename().string();
		// .minigit ���� ����
		if (filename == ".minigit") continue;

		// �̹� index�� commit�� ������ ����
		if (indexMap.count(filename) || commitMap.count(filename)) continue;

		untracked.insert(filename);
	}

	// ���
	if (!staged.empty())
	{
		std::cout << "������¡�� ����:\n";
		for (const auto& file : staged)
			std::cout << "	" << file << "\n";
		std::cout << "\n";
	}

	if (!modified.empty())
	{
		std::cout << "������ ����:\n";
		for (const auto& file : modified)
			std::cout << "	" << file << "\n";
		std::cout << "\n";
	}

	if (!untracked.empty())
	{
		std::cout << "�������� ���� ����:\n";
		for (const auto& file : untracked)
			std::cout << "	" << file << "\n";
		std::cout << "\n";
	}
}