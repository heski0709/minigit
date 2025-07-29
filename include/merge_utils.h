#pragma once
#include <string>
#include <vector>

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