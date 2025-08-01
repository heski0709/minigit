#pragma once
#include <string>

/**
 * @brief 병합 상태 파일을 기반으로 병합 커밋을 생성
 *
 * @param currentHash 현재 브랜치의 커밋 해시
 * @param targetHash 병합할 대상 브랜치의 커밋 해시
 */
void mergeCommitFromState(const std::string& currentHash, const std::string& targetHash, const std::string& message);