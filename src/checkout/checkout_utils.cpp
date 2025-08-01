#include "checkout/checkout_utils.h"
#include "index/index_utils.h"

#include <fstream>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

/**
* @brief 커밋 디렉토리 내의 파일을 현재 작업 디렉토리로 복원
* @param srcDir 커밋 디렉토리 경로
* @param filename 복원 파일
* @return 성공 여부
*/
bool restoreFile(const std::string& srcDir, const std::string& filename)
{
	std::filesystem::path src = std::filesystem::path(srcDir) / filename;
	std::filesystem::path dst = filename;

	try
	{
		// 복원할 경로의 상위 디렉토리 생성
		std::filesystem::path parentPath = dst.parent_path();

		if (!parentPath.empty())
			std::filesystem::create_directories(parentPath);

		std::ifstream in(src, std::ios::binary);
		if (!in.is_open())
		{
			std::cerr << "[복원 실패] 원본 파일 열기 실패: " << src << "\n";
			return false;
		}

		std::ofstream out(dst, std::ios::binary);
		if (!out.is_open())
		{
			std::cerr << "[복원 실패] 원본 파일 열기 실패: " << dst << "\n";
			return false;
		}

		out << in.rdbuf();

		if (out.fail())
		{
			std::cerr << "[복원 실패] 복사 중 오류 발생: " << dst << "\n";
			return false;
		}
		return true;
	}
	catch (const std::exception& e)
	{
		std::cerr << "[복원 예외] " << filename << ": " << e.what() << "\n";
		return false;
	}
}
