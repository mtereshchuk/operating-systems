#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

char **parse(char *s, ssize_t size) {
    char **args = malloc(size * sizeof(char *));
    int i = 0;
    char *cur_arg = strtok(s, " \t\n");
    while (cur_arg != NULL) {
        args[i++] = cur_arg;
        cur_arg = strtok(NULL, " \t\n");
    } 
    args[i] = NULL;
    return args;
}

void run_program(char **args) {
    if (!strcmp(args[0], "exit")) {
        exit(EXIT_SUCCESS);
    }
    pid_t pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("Error run program");
        }
    } else if (pid > 0) {
        pid_t wpid;
        int status;
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        printf("%d\n", WEXITSTATUS(status));
    } else {
        perror("Error fork");
    }
}

int main(int argc, char **argv) {
    char *s;
    size_t s_size;
    ssize_t size;
    while (1) {
        printf("> ");    
        size = getline(&s, &s_size, stdin);
        char **args = parse(s, size);
        run_program(args);
        free(args);
    } 
    return 0;
}