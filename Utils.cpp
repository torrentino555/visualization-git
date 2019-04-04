#include "Utils.h"
#include <array>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <stdlib.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

std::string exec(const std::string cmd) {
  std::array<char, 128> buffer;
  std::string result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"),
                                                pclose);
  if (!pipe) {
    throw std::runtime_error("popen() failed!");
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }
  return result;
}

std::string get_comiit_or_tree_text(std::string path, std::string commitHash) {
  return exec("cd " + std::string(path) + " && git cat-file -p " + commitHash);
}

std::vector<std::string> splitStringByDelimiter(std::string s,
                                                std::string delimiter) {
  std::vector<std::string> lines;
  int i;
  while ((i = s.find(delimiter)) != -1) {
    lines.push_back(s.substr(0, i));

    if ((unsigned)i + 1 < s.length()) {
      s = s.substr(i + 1);
    } else {
      break;
    }
  }

  if (s.length() > 0) {
    lines.push_back(s);
  }

  return lines;
}

std::vector<std::string> splitStringByTwoDelimiters(std::string s,
                                                    std::string delimiter,
                                                    std::string delimiter2) {
  std::vector<std::string> lines;
  int i;
  while ((i = s.find(delimiter)) != -1 || (i = s.find(delimiter2)) != -1) {
    lines.push_back(s.substr(0, i));

    if ((unsigned)i + 1 < s.length()) {
      s = s.substr(i + 1);
    } else {
      break;
    }
  }

  if (s.length() > 0) {
    lines.push_back(s);
  }

  return lines;
}

std::string readFile(std::string path) {
  std::string text;
  std::ifstream file;
  file.exceptions(std::ifstream::badbit);
  try {
    file.open(path);
    std::stringstream stream;
    stream << file.rdbuf();
    text = stream.str();
    file.close();
  } catch (std::ifstream::failure e) {
    std::cout << "Error read file:" << path << std::endl;
  }
  
  return text;
}
