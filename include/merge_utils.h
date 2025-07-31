#pragma once
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

/**
* @brief 특정 커밋의 index 파일을 파싱하여 (파일명 -> 해시) map으로 반환
*
* @param commitHash 커밋 해시
* @return std::unordered_map<std::string, std::string> 파일명과 대응하는 해시
*/
std::unordered_map<std::string, std::string> parseIndex(const std::string& commitHash);

/**
* @brief 두 커밋의 idnex 파일을 비교하여 충돌 파일을 반환
*
* @param currentHash 현재 브랜치의 커밋 해시
* @param targetHash 병합 대상 브랜치의 커밋 해시
* @return std::vector<std::string> 충돌이 발생한 파일 이름 목록
*/
std::vector<std::string> detectConflicts(
	const std::string& currentHash,
	const std::string& targetHash);

/**
* @brief 파일에 충돌 마커(`<<<<<`, `======`, `>>>>>`)를 삽입
*
* @param filename 충돌파일 이름
* @param contentA 현재 브랜치의 파일 내용
* @param contentB 병합 대상 브랜치의 파일 내용
*/
void markConflict(
	const std::string& filename,
	const std::string& contentA,
	const std::string& contentB
);

/**
 * @brief 특정 커밋의 모든 조상들을 탐색하여 집합으로 반환
 * 
 * @param startHash 조상을 탐색할 커밋 해시
 */
std::unordered_set<std::string> collectAncestors(const std::string& startHash);

/**
 * @brief 두 커밋의 공통 조상을 찾음 (가장 가까운 조상)
 */
std::string findCommonAncestor(const std::string& hashA, const std::string& hashB);

/**
* @brief merge 중 상태를 기록 (충돌 발생 시)
* 
* @param currentHash 현재 브랜치의 커밋 해시
* @param targetHash 병합 대상 브랜치의 커밋 해시
*/
void saveMergeState(const std::string& currentHash, const std::string& targetHash);

/**
* @brief MERGE_STATE 파일 삭제 (abort 또는 완료 시)
*/
void clearMergeState();

/**
* @brief merge 상태 파일이 존재하는 여부 반환
*
* @return 존재하면 true, 없을 경우 false
*/
bool isMergeInProgress();

/**
* @brief merge 상태를 불러와 currentHash, targetHash로 반환
*/
std::pair<std::string, std::string> loadMergeState();

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
* @brief 현재 작업 디렉토리의 파일들을 기준으로 .minigit/index 파일을 갱신
*
* @details 현재 커밋된 파일 목록을 기준으로, 작업 디렉토리의 실제 파일 내용을 해시하여 index 파일에 기록함 (충돌 없는 병합 시 사용)
*
* @param baseHash 기존 커밋 해시 (병합의 베이스가 되는 커밋)
* @param targetHash 병합할 브랜치의 커밋 해시
*/
void updateIndexAfterAutoMerge(const std::string& baseHash, const std::string& targetHash);