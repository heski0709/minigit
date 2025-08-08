#pragma once
#include <string>

/**
* @brief index 파일에 같은 해시가 존재하는지 확인
* @param hash 해당 파일의 해시코드
*/
bool isAlreadyStaged(const std::string& hash);

/**
* @brief index 파일에 파일명을 추가하는 함수
* @param filename 파일 이름
*/
void add(const std::string& filename);