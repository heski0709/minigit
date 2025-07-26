#pragma once
#include <string>

/**
* @brief 파일이 존재하는지 확인하는 함수
* @param filename 경로를 포함한 파일이름
*/
bool fileExists(const std::string& filename);

/**
* @brief 파일 전체 내용을 읽기
* @param filename 경로를 포함한 파일이름
*/
std::string readFileContent(const std::string& filename);

/**
* @brief 파일 내용을 해시로 변환하는 함수
*/
std::string simpleHash(const std::string& content);
