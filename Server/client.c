#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        perror("incorrect number of arguments");
        return 1;
    }

    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
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
 
    if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("connect error");
        return 1;
    }

    char wstr[100], rstr[100];
    printf("Choose mode \"echo\" or \"dict\". Use \"exit\" to complete\n");
    while(1) {
        fgets(wstr, 100, stdin);
        if (strlen(wstr) == 5 && !strncmp(wstr, "exit", 4)) {
            break;
        }
        write(sock, wstr, strlen(wstr) + 1);
        bzero(rstr, 100);
        if (read(sock, rstr, 100) <= 0) {
            break;
        }
        printf("%s", rstr);
    }
    close(sock);
    return 0;
}