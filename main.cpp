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
#include "WorkWithGit.h"

int main(int args, char* argv[]) {
        if (args != 2) {
                std::cout << "Использование:" << argv[0] << " [path to Project folder]" << std::endl;
                return 0;
        }

        // WorkWithGit git(argv[1]);
        // while (git.next_commit()) {
        //         git.get_diffs();
        //         std::cout << "-------------" << std::endl;
        // }

        // std::cout << "Start parse commit" << std::endl;

        std::string path(argv[1]);
        States states(path);

        // std::cout << "Start graphic" << std::endl;

        GraphicEngine engine(&states);
        engine.start();
}
