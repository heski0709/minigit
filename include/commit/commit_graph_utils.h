#pragma once
#include <string>
#include <unordered_set>

/**
* @brief 주어진 커밋이 다른 커밋의 조상(ancestor)인지 확인
*
* @param ancestor 조상인지 확인할 커밋 해시
* @paran descendant 탐색할 대상 커밋 해시
* @return true descendant가 ancestor의 자손일 경우
* @return false ancestor와 관련 없는 경우 (또는 경로 없음)
*/
bool isAncestor(const std::string& ancestor, const std::string& descendant);


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