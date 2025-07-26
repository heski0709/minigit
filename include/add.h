#pragma once
#include <string>

/// <summary>
/// index 파일에 이미 같은 해시가 존재하는지 확인
/// </summary>
bool isAlreadyStaged(const std::string& hash);

/// <summary>
/// index 파일에 파일명을 추가하는 함수
/// </summary>
void add(const std::string& filename);