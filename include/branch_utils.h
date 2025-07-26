#pragma once
#include <string>

std::string getCurrentBranchName();
std::string getCurrentBranchHash();
void updateBranchHead(std::string hash);