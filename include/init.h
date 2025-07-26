#pragma once

/// <summary>
/// 디렉토리 생성함수
/// 성공하면 true, 이미 존재해도 true, 실패하면 false 반환
/// </summary>
bool createDirectory(const std::string& path);

/// <summary>
/// mini-git 저장소 초기화 함수
/// </summary>
void init();
