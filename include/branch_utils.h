#pragma once
#include <string>

std::string getCurrentBranchName();
std::string getCurrentBranchHash();
void updateBranchHead(const std::string hash);