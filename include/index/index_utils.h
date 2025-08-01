#pragma once
#include <unordered_map>
#include <string>

/**
* @brief ���� index ������ ������ string���� ����
*/
std::string getCurrentIndexSnapshot();

/**
 * @brief �־��� Ŀ�� �ؽ� �Ǵ� �ε��� ��θ� �Ľ��Ͽ� ���� �ؽ� ���� ��ȯ�մϴ�.
 *
 * @param path index ������ ��� (��: ".minigit/index" �Ǵ� ".minigit/commits/<hash>/index")
 * @return ���ϸ�� �ؽ� ���� ������ std::unordered_map
 */
std::unordered_map<std::string, std::string> parseIndex(const std::string& path);

/**
 * @brief .minigit/index ���� �ε�
 */
std::unordered_map<std::string, std::string> loadIndex();

/**
 * @brief ���� �귣ġ�� ����Ű�� Ŀ���� index ���� �ε�
 */
std::unordered_map<std::string, std::string> loadLastCommitIndex();