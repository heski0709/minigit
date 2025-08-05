#pragma once
#include <string>
#include <vector>

/**
* @brief merge 중 상태를 기록 (충돌 발생 시)
* 
* @param currentHash 현재 브랜치의 커밋 해시
* @param targetHash 병합 대상 브랜치의 커밋 해시
*/
void saveMergeState(const std::string& currentHash, const std::string& targetHash);

/**
* @brief merge 상태를 불러와 currentHash, targetHash로 반환
*/
std::pair<std::string, std::string> loadMergeState();

/**
* @brief MERGE_STATE 파일 삭제 (abort 또는 완료 시)
*/
void clearMergeState();


/**
* @brief conflict 발생 시 현재 브랜치의 파일 백업
*/
void backupCurrentFilesBeforeMerge(const std::string& currentHash);

/**
* @brief merge --abort 실행 시 백업해놓은 파일 복원
*/
void restoreFilesFromBackup();

/**
* @brief .minigit/_merge_backup/ 폴더 삭제
*/
void clearBackup();

/**
* @brief merge 중 충돌이 발생한 파일 저장
*
* @param conflicts 충돌한 파일목록
*/
void saveMergeConflicts(const std::vector<std::string>& conflicts);