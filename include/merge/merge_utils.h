#pragma once
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>


/**
* @brief merge 상태 파일이 존재하는 여부 반환
*
* @return 존재하면 true, 없을 경우 false
*/
bool isMergeInProgress();

/**
* @brief 현재 작업 디렉토리의 파일들을 기준으로 .minigit/index 파일을 갱신
*
* @details 현재 커밋된 파일 목록을 기준으로, 작업 디렉토리의 실제 파일 내용을 해시하여 index 파일에 기록함 (충돌 없는 병합 시 사용)
*
* @param baseHash 기존 커밋 해시 (병합의 베이스가 되는 커밋)
* @param targetHash 병합할 브랜치의 커밋 해시
*/
void updateIndexAfterAutoMerge(const std::string& baseHash, const std::string& targetHash);

void updateIndexFromWorkingDirectory();

void applyAutoMergeFiles(const std::string& targetHash);
