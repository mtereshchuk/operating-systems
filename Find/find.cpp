#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <string.h>
#include <sys/stat.h>

using namespace std;

char *main_directory;

bool has_inode = 0;
ino_t num_inode;

bool has_name = 0;
char *name;

bool has_size = 0;
char sign;
int size;

bool has_links = 0;
int cnt_links;

bool has_exec = 0;
char *path;

void parse_parameters(int argc, char *argv[]) {
    for (int i = 2; i < argc; ++i) {
        if (!strcmp(argv[i], "-inum")) {
            has_inode = 1;
            num_inode = (ino_t) stoi(string(argv[++i]));
        } else if (!strcmp(argv[i], "-name")) {
            has_name = 1;
            name = argv[++i];
        } else if (!strcmp(argv[i], "-size")) {
            has_size = 1;
            char *par = argv[++i];
            sign = par[0];
            size = stoi(string(par).substr(1));
        } else if (!strcmp(argv[i], "-nlinks")) {
            has_links = 1;
            cnt_links = stoi(string(argv[++i]));
        } else if (!strcmp(argv[i], "-exec")) {
            has_exec = 1;
            path = argv[++i];
        }
    }
}

vector<char *> result_names;

void execute() {
    for (char *name : result_names) {
        pid_t pid = fork();
        if (pid == 0) {
            char p[1000];
            strcpy(p, main_directory);
            strcat(p, "/");
            strcat(p, name);
            if (execl(path, p) == -1) {
                perror("Error run program");
            }
        } else if (pid > 0) {
            pid_t wpid;
            int status;
            do {
                wpid = waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        } else {
            perror("Error fork");
        }
        cout << " is result of" << ' ' << name << '\n';
    }
}

void find(char *directory) {
    //cout << directory << '\n';
    DIR *dir;
    dir = opendir(directory);
    if (!dir) {
        perror("Error open directory");
        exit(1);
    };

    struct dirent *cur;
    bool take;
    struct stat buf;
    char p[1000];
    while ((cur = readdir(dir)) != NULL) {
            strcpy(p, directory);
            strcat(p, "/");
            strcat(p, cur->d_name);
            //cout << s << '\n';
            stat(p, &buf);
            //cout << cur->d_name << ' ' << cur->d_ino << ' ' << buf.st_size 
            //<< ' ' << buf.st_nlink << '\n';
            //cout << cur->d_name << ' ';// ' ' << cur->d_type << '\n'; 
            if (cur->d_type == DT_DIR) {
                if (strcmp(cur->d_name, ".") && strcmp(cur->d_name, "..")) {   
                    char p2[1000];
                    strcpy(p2, main_directory);
                    strcat(p2, "/");
                    strcat(p2, cur->d_name);
                    find(p2);
                }
            } else {
                take = 1;
                if (has_inode) {
                    if (cur->d_ino != num_inode) {
                        take = 0;
                    }
                }
                if (has_name) {
                    if (strcmp(cur->d_name, name)) {
                        take = 0;
                    }
                }  
                if (has_size) {
                    if (sign == '-' && buf.st_size >= size 
                    || sign == '=' && buf.st_size != size 
                    || sign == '+' && buf.st_size <= size) {
                        take = 0; 
                    }
                } 
                if (has_links) {
                    if (buf.st_nlink != cnt_links) {
                        take = 0;
                    }
                } 
                if (take) {
                    result_names.push_back(cur->d_name);
                    if (has_name) {
                        break;
                    }
                }
            }
    };
    closedir(dir);
    if (!strcmp(directory, main_directory)) {
        if (result_names.empty()) {
        cout << "Not found\n";
        } else {
            if (!has_exec) {
                //cout << result_names.size() << '\n';
                for (char *name : result_names) {
                    cout << name << '\n';
                }
            } else {
                execute();        
            }
        }
    }
}

int main(int argc, char *argv[]) {
    //cout << argc << '\n';
    parse_parameters(argc, argv);
    //cout << has_inode << ' ' << has_name << ' ' << has_size << ' ' << has_links << '\n';
    //cout << name << '\n';
    //cout << sign << '\n';
    //cout << size << '\n';
    main_directory = argv[1];
    find(main_directory);
    return 0;
};