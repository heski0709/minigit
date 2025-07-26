#pragma once
#include <string>

/// <summary>
/// 파일이 존재하는지 확인하는 함수
/// </summary>
bool fileExists(const std::string& filename);

/// <summary>
/// 파일 전체 내용을 문자열로 읽기
/// </summary>
std::string readFileContent(const std::string& filename);
std::string simpleHash(const std::string& content);
