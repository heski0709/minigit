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
 */
std::unordered_set<std::string> collectAncestors(const std::string& startHash);

/**
 * @brief 두 커밋의 공통 조상을 찾음 (가장 가까운 조상)
 */
std::string findCommonAncestor(const std::string& hashA, const std::string& hashB);