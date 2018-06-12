#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

const int MAX_EVENTS = 2;

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
    addr.sin_port = htons(11999);

    if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("connect error");
        return 1;
    }

    int pipe_fd[2];
    pipe(pipe_fd);

    int epfd = epoll_create(1);
    struct epoll_event ev, events[MAX_EVENTS];
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = sock;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &ev) == -1) {
		perror("error epoll_ctl");
		return 1;
	}
    ev.data.fd = pipe_fd[0];
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, pipe_fd[0], &ev) == -1) {
		perror("error epoll_ctl");
		return 1;
	}

    int work = 1;
    int pid = fork();
    if (!pid) {
        close(pipe_fd[0]);
        printf("Write something. Use \"exit\" to complete\n");
        char wstr[100];
        while(work) {
            fgets(wstr, 100, stdin);
            if (strlen(wstr) == 5 && !strncmp(wstr, "exit", 4)) {
                work = 0;
            } else {
                write(pipe_fd[1], wstr, strlen(wstr) + 1);
            }
        }
    } else if (pid > 0) {
        close(pipe_fd[1]);
        while(work) {
            int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
            int i;
            for (i = 0; i < nfds; ++i) {
                char rstr[100];
                if (events[i].data.fd == sock) {
                    bzero(rstr, 100);
                    if (recv(sock, rstr, 100, 0) <= 0) {
                        work = 0;
                        close(sock);
                    } else {
                        printf("%s", rstr);
                    }
                } else {
                    if (read(events[i].data.fd, rstr, 100) <= 0) {
                        work = 0;
                    } else {
                        send(sock, rstr, 100, 0);
                    }
                }
            }
        }
    } else {
        perror("error fork");
        return 1;
    }
    return 0;
}
