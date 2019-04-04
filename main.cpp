#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <set>
#include <array>
#include "States.h"
#include "Tree.h"
#include "Utils.h"
#include "GraphicEngine.h"

int main(int args, char* argv[]) {
        if (args != 2) {
                std::cout << "Использование:" << argv[0] << " [path to Project folder]" << std::endl;
                return 0;
        }

        std::array<char, 128> fullPath;
        realpath(argv[1], fullPath.data());
        std::string path_to_git = std::string(fullPath.data()) + "/.git/";
        std::ifstream file_head(path_to_git + "HEAD");

        if (!file_head) {
                std::cout << "Неправильный путь, проект не содержит системы git или папка .git повреждена" << std::endl;
                return 0;
        }

        std::string pathToLastCommitInCurrentBranch;
        getline(file_head, pathToLastCommitInCurrentBranch);

        if (pathToLastCommitInCurrentBranch.find("ref: ") != 0) {
                std::cout << "Ожидалось: ref: [ref to commit]\nРеальность: " << pathToLastCommitInCurrentBranch << std::endl;
                return 0;
        }

        // убираем начало строки "ref :"
        pathToLastCommitInCurrentBranch = pathToLastCommitInCurrentBranch.substr(5);

        std::ifstream file_master(path_to_git + pathToLastCommitInCurrentBranch);

        if (!file_master) {
                std::cout << "Папка .git повреждена" << std::endl;
                return 0;
        }

        // работа с коммитами

        std::string last_commit;
        getline(file_master, last_commit);

        std::stack<std::string> trees;
        std::stack<std::string> commits;
        std::set<std::string> parentsSet;

        commits.push(last_commit);
        parentsSet.insert(last_commit);

        // while (!commits.empty()) {
          std::string commitHash = commits.top();
          commits.pop();

          std::string commit_text = get_comiit_or_tree_text(fullPath.data(), commitHash);

          // std::cout << commit_text;

          std::vector<std::string> lines = splitStringByDelimiter(commit_text, "\n");

          for (size_t i = 0; i < lines.size(); i++) {
            if (lines[i].find("tree") == 0) {
              trees.push(lines[i].substr(5));
            } else if (lines[i].find("parent") == 0) {
              std::string parentCommit = lines[i].substr(7);
              if (parentsSet.find(parentCommit) == parentsSet.end()) {
                commits.push(parentCommit);
                parentsSet.insert(parentCommit);
              }
            }
          }
        // }

        std::cout << "Start parse commit" << std::endl;

        States s(fullPath.data(), trees.top());

        std::cout << "Start graphic" << std::endl;

        GraphicEngine engine(&s);
        engine.start();
}
