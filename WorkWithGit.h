#ifndef _WORK_WITH_GIT_H_
#define _WORK_WITH_GIT_H_
#pragma once
#include <git2.h>
#include <string>
#include <iostream>
#include <vector>
#include "Tree.h"

class Tree;

class WorkWithGit {
private:
    Tree* tree = nullptr;
    git_tree* old_tree = nullptr;
    git_tree* new_tree = nullptr;
    git_revwalk* walker = nullptr;
    git_repository* repo = nullptr;
public:
    WorkWithGit(std::string, Tree*);
    bool next_commit();
    void get_diffs();

    ~WorkWithGit();
};

#endif