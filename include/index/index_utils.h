#pragma once
#include <unordered_set>
#include <unordered_map>
#include <string>

/**
* @brief path 파일의 내용을 string으로 읽음
*/
std::string readIndexSnapshot(const std::string& path = ".minigit/index");

/**
 * @brief 주어진 커밋 해시 또는 인덱스 경로를 파싱하여 파일 해시 맵을 반환합니다.
 *
 * @param path index 파일의 경로 (예: ".minigit/index" 또는 ".minigit/commits/<hash>/index")
 * @return 파일명과 해시 값을 매핑한 std::unordered_map
 */
std::unordered_map<std::string, std::string> parseIndex(const std::string& path);

/**
 * @brief .minigit/index 파일 로드
 */
std::unordered_map<std::string, std::string> loadIndex();

/**
 * @brief 현재 브랜치가 가리키는 커밋의 index 파일 로드
 */
std::unordered_map<std::string, std::string> loadLastCommitIndex();

void writeIndexFromWorkingDirectory(
	const std::unordered_set<std::string>& files,
	const std::string& outputPath);