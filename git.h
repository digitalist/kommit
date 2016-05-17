//
// Created by user on 4/19/16.
//
#pragma once
#include <iostream>
#include <git2.h>
#include <git2/diff.h>

namespace commonUtils {
    using  std::cerr;
    namespace v1 {
        namespace git {

            /******************************** LIBGIT2 HELPERS */
            struct formats{
                enum {
                    DEFAULT   = 0,
                    LONG      = 1,
                    SHORT     = 2,
                    PORCELAIN = 3,
                };
            };


            typedef struct { /* … */ } match_data;


// ADDING
            enum print_options {
                SKIP = 1,
                VERBOSE = 2,
                UPDATE = 4,
            };
            struct print_payload {
                enum print_options options;
                git_repository *repo;
            };



// ADDING END

            void check_lg2(int check, std::string s, void * param){
                if (check != 0){
                    std::cout << s;
                    exit(check);
                }
            }

            static std::string  get_branch(git_repository *repo, int format)
            {
                int error = 0;
                const char *branch = nullptr;
                git_reference *head = nullptr;

                error = git_repository_head(&head, repo);

                if (error == GIT_EUNBORNBRANCH || error == GIT_ENOTFOUND)
                    branch = nullptr;
                else if (!error) {
                    branch = git_reference_shorthand(head);
                } else
                    check_lg2(error, "failed to get current branch", nullptr);

                /*if (format == formats::LONG)
                    printf("# On branch %s\n",
                           branch ? branch : "Not currently on any branch.");
                else
                    printf("## %s\n", branch ? branch : "HEAD (no branch)");
*/
                git_reference_free(head);

                return branch;

            }

            static std::string  get_branch(std::string path, int format) {
                git_repository *repo = nullptr;

/* Open repository in given directory (or fail if not a repository) */
                auto result = git_repository_open_ext(
                        &repo, path.c_str(), GIT_REPOSITORY_OPEN_NO_SEARCH, NULL);
                if(!result){
                    return get_branch(repo, format);
                }else{
                    return "";
                }

            }



/******************************** LIBGIT2 HELPERS END */

            static int commit(std::string path, std::string message) {
                git_libgit2_init();
                int result;
                git_signature *author = NULL;

                git_repository *repo = NULL;

/* Open repository in given directory (or fail if not a repository) */
                result = git_repository_open_ext(
                        &repo, path.c_str(), GIT_REPOSITORY_OPEN_NO_SEARCH, NULL);

                git_signature_default(&author, repo);
                git_diff *diff = NULL;
                result = git_diff_index_to_workdir(&diff, repo, NULL, NULL);
                if (result != 0) cerr << giterr_last()->message << "\n";

                // Get the git index.
                git_index *index = NULL;
                result = git_repository_index(&index, repo);
                if (result != 0) cerr << giterr_last()->message << "\n";

// TRYING TO DELETE DELETED START
                size_t count = git_index_entrycount(index);
                for (size_t i=0; i<count; i++) {
                    /* const git_index_entry *entry = */ git_index_get_byindex(index, i);
                    auto delta = git_diff_get_delta(diff, i); //i syncs with index and diff
                    if (delta != NULL){ //if we have a delta of deleted files?
                        std::cout << "file: " << delta->new_file.path<<"\n";
                        git_index_remove_bypath(index,  delta->new_file.path); //delete before commit
                    }
                }
// TRYING TO DELETE DELETED END
                // Add all files to the git index.
                result = git_index_add_all(index, NULL, 0, NULL, NULL);
                if (result != 0) cerr << giterr_last()->message << "\n";


                // Write the index to disk.
                result = git_index_write(index);
                if (result != 0) cerr << giterr_last()->message << "\n";

                // Create a tree from the index.
                git_oid tree_oid;
                result = git_index_write_tree(&tree_oid, index);
                if (result != 0) cerr << giterr_last()->message << "\n";

                git_tree *tree = NULL;
                result = git_tree_lookup(&tree, repo, &tree_oid);
                if (result != 0) cerr << giterr_last()->message << "\n";

                /* Get HEAD as a commit object to use as the parent of the commit */
                git_oid parent_id;
                git_commit *parent;
                git_reference_name_to_id(&parent_id, repo, "HEAD");
                git_commit_lookup(&parent, repo, &parent_id);
                git_oid commit_oid;
                result = git_commit_create_v(
                        &commit_oid, /* out id */
                        repo,
                        //NULL, // don't update the HEAD
                        "HEAD", // update the HEAD
                        author,
                        author,
                        NULL, /* use default message encoding */
                        message.c_str(),
                        tree,
                        1, parent);

                // Now we can take a look at the commit SHA we've generated.
                //git_oid_fmt(out, &commit_id);


                git_libgit2_shutdown();
                return result;
            }


        }
    }

}