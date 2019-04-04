#pragma once
#include <string>
#include <vector>

std::string exec(const std::string cmd);

std::string get_comiit_or_tree_text(std::string path, std::string commitHash);

std::vector<std::string> splitStringByDelimiter(std::string s, std::string delimiter);

std::vector<std::string> splitStringByTwoDelimiters(std::string s, std::string delimiter, std::string delimiter2);

std::string readFile(std::string path);
