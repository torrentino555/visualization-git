#include "WorkWithGit.h"

int diff_file_cb(const git_diff_delta* delta, float progress, void *payload);

WorkWithGit::WorkWithGit(std::string path_to_rep, Tree* tree) : tree(tree) {
    int error;
    if (git_libgit2_init() < 0) {
        std::cout << "git_libgit2 exit with error" << std::endl;
    }
    error = git_repository_open(&repo, path_to_rep.c_str());

    if (error < 0) {
        const git_error *e = giterr_last();
        printf("Error %d/%d: %s\n", error, e->klass, e->message);
        std::cout << "Path: " << path_to_rep << std::endl;
        exit(error);
    }

    git_revwalk_new(&walker, repo);

    // перебор коммитов в порядке времени
    git_revwalk_sorting(walker, GIT_SORT_TIME | GIT_SORT_REVERSE);

    // установка корня из HEAD репозитория
    git_revwalk_push_head(walker);
}

// true если есть следующий коммит
bool WorkWithGit::next_commit() {
    if (old_tree != nullptr) {
        git_tree_free(old_tree);
    }

    git_commit* commit;

    git_oid oid;
    if (!git_revwalk_next(&oid, walker)) {
        git_commit_lookup(&commit, repo, &oid);

        old_tree = new_tree;
        git_commit_tree(&new_tree, commit);
        
        git_commit_free(commit);
        return true;
    }

    return false;
}

void WorkWithGit::get_diffs() {
    if (old_tree == nullptr) {
        // sha для пустой директории git, не костыль
        const char* sha = "4b825dc642cb6eb9a060e54bf8d69288fbee4904";
        git_oid oid;
        git_oid_fromstr(&oid, sha);
        git_tree_lookup(&old_tree, repo, &oid);
    }

    if (old_tree != nullptr && new_tree != nullptr) {
        git_diff* diff;
        git_diff_tree_to_tree(&diff, repo, old_tree, new_tree, nullptr);
        git_diff_foreach(diff, diff_file_cb, nullptr, nullptr, nullptr, tree);
    }
}

// обработка различий в коммитах
int diff_file_cb(const git_diff_delta* delta, float progress, void *payload) {
    Tree *tree = (Tree*)payload;
    const git_diff_file* file;
    switch (delta->status)
    {
        case GIT_DELTA_ADDED:
            file = &delta->new_file;
            tree->addFile(file->path);
            break;

        case GIT_DELTA_DELETED:
            file = &delta->old_file;
            tree->removeFile(file->path);
            break;

        case GIT_DELTA_MODIFIED:
            file = &delta->new_file;
            tree->modifyFile(file->path);
            break;
    
        default:
            break;
    }

    return 0;
}

WorkWithGit::~WorkWithGit() {
    if (old_tree != nullptr) {
        git_tree_free(old_tree);
    }

    if (new_tree != nullptr) {
        git_tree_free(new_tree);
    }

    git_revwalk_free(walker);
    git_repository_free(repo);
    git_libgit2_shutdown();
}

