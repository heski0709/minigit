#pragma once
#include <string>
#include <vector>

/**
* @brief 커밋 디렉토리 생성
* @param hash 커밋 해시코드
*/
bool createCommitDirectory(const std::string& hash);

/**
* @brief 커밋 디렉토리로 파일 복사
* @param src 원본파일
* @param destDir 복사 파일을 저장할 커밋 디렉토리
*/
bool copyFileToCommit(const std::string& src, const std::string& destDir);

/**
* @brief 커밋 메시지와 시간을 기록하는 함수
* @param destDir 커밋 디렉토리
* @param message 커밋된 메세지
*/
void writeMeta(const std::string& destDir, const std::vector<std::string>& parentHashes, const std::string& message);

/**
 * @brief 입력값이 브랜치명, 태그, 커밋해시 중 어떤 것인지 판별
 * @param ref 사용자가 입력한 레퍼런스 문자열
 * @return 브랜치라면 "branch", 해시라면 "commit", 태그라면 "tag", 아니면 "invalid"
 */
std::string resolveReferenceType(const std::string& ref);
