#pragma once
#include <string>

std::string getCurrentBranchName();
std::string getCurrentBranchHash();
std::string getBranchHash(const std::string& branchName);
void updateBranchHead(const std::string hash);