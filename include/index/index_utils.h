#pragma once
#include <unordered_map>
#include <string>

/**
* @brief 현재 index 파일의 내용을 string으로 읽음
*/
std::string getCurrentIndexSnapshot();

/**
* @brief 특정 커밋의 index 파일을 파싱하여 (파일명 -> 해시) map으로 반환
*
* @param commitHash 커밋 해시
* @return std::unordered_map<std::string, std::string> 파일명과 대응하는 해시
*/
std::unordered_map<std::string, std::string> parseIndex(const std::string& commitHash);
