#pragma once
#include <string>

void mergeBranch(const std::string& targetBranch);
void mergeCommit(const std::string& branchToMerge);
void mergeCommitFromState(const std::string& currentHash, const std::string& targetHash, const std::string& message);