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
 * @brief 충돌 내용을 마커 형식으로 감싸 문자열로 반환합니다.
 *
 * @param currentContent 현재 브랜치의 파일 내용
 * @param targetContent 병합 대상 브랜치의 파일 내용
 * @param currentLabel 마커의 현재 브랜치 라벨 (기본값: "HEAD")
 * @param targetLabel 마커의 병합 대상 라벨 (기본값: "MERGE")
 * @return 충돌 마커가 삽입된 최종 문자열
 *
 * @details
 * 아래와 같은 형식으로 결과가 생성됩니다:
 * @code
 * <<<<<<< HEAD
 * (currentContent)
 * =======
 * (targetContent)
 * >>>>>>> MERGE
 * @endcode
 */
std::string generateConflictMarkedContent(
    const std::string& currentContent,
    const std::string& targetContent,
    const std::string& currentLabel = "HEAD",
    const std::string& targetLabel = "MERGE"
);

/**
 * @brief 충돌 내용을 파일에 기록하며, conflict 마커를 삽입합니다.
 *
 * @param filename 충돌 내용을 기록할 파일 경로
 * @param currentContent 현재 브랜치의 파일 내용
 * @param targetContent 병합 대상 브랜치의 파일 내용
 * @param currentLabel 마커의 현재 브랜치 라벨 (기본값: "HEAD")
 * @param targetLabel 마커의 병합 대상 라벨 (기본값: "MERGE")
 *
 * @details
 * 내부적으로 generateConflictMarkedContent()를 사용하여 마커를 생성한 뒤,
 * 파일에 덮어씁니다. 파일이 열리지 않을 경우 에러 메시지를 출력합니다.
 */
void markConflict(
    const std::string& filename,
    const std::string& currentContent,
    const std::string& targetContent,
    const std::string& currentLabel = "HEAD",
    const std::string& targetLabel = "MERGE"
);