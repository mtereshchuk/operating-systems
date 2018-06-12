#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

char *dict[] = {"good", "bad", "little", "much", "many", "far", "old"};

int check(char *str) {
    int i;
    for (i = 0; i < 7; ++i) {
        if (strlen(dict[i]) == strlen(str) - 1 && !strncmp(dict[i], str, strlen(str) - 1)) {
            return 1;
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        perror("incorrect number of arguments");
        return 1;
    }

    int listener;
    if ((listener = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, argv[1], &addr.sin_addr) <= 0) {
        perror("inet_pton error");
        return 1;
    }
    addr.sin_port = htons(12139);
 
    if (bind(listener, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("bind error");
        return 1;
    }
 
    listen(listener, 10);

    int sock;
    char str[100];
    while(1) {
        sock = accept(listener, (struct sockaddr*) NULL, NULL);
        while(1) {
            if (read(sock, str, 100) <= 0 || strlen(str) == 5 && (!strncmp(str, "echo", 4) || !strncmp(str, "dict", 4))) {
                break;
            } else {
                write(sock, "Choose mode \"echo\" or \"dict\". Use \"exit\" to complete\n", 54);
            }   
        }
        if (strlen(str) == 5 && !strncmp(str, "echo", 4)) {
            write(sock, "mode echo\n", 11);
            while(1) {
                if (read(sock, str, 100) <= 0) {
                    break;
                } else {
                    write(sock, str, strlen(str) + 1);
                }   
            }
        } else if (strlen(str) == 5 && !strncmp(str, "dict", 4)) {
            write(sock, "mode dict\n", 11);
            while(1) {
                if (read(sock, str, 100) <= 0) {
                    break;
                } else {
                    if (check(str)) {
                        write(sock, "in dict\n", 8);
                    } else {
                        write(sock, "not in dict\n", 12);
                    }
                }   
            }
        }
        //printf("close\n");
        close(sock);
    }
    return 0;
}