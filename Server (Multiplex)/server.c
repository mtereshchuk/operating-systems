#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>

const int MAX_CONN = 10, MAX_EVENTS = 20;

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
    fcntl(listener, F_SETFL, fcntl(listener, F_GETFD, 0) | O_NONBLOCK);

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0) {
        perror("inet_pton error");
        return 1;
    }
    serv_addr.sin_port = htons(11999);
 
    if (bind(listener, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind error");
        return 1;
    }
 
    listen(listener, MAX_CONN);
    
    int epfd = epoll_create(1);
    struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
	ev.data.fd = listener;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, listener, &ev) == -1) {
		perror("error epoll_ctl");
		return 1;
	}

    struct epoll_event events[MAX_EVENTS];
    while(1) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        int i;
        for (i = 0; i < nfds; ++i) {
            char str[100];
            if (events[i].data.fd == listener) {
                struct sockaddr_in cli_addr;
                int socklen;
				int sock = accept(listener, (struct sockaddr *) &cli_addr, &socklen);
                
				inet_ntop(AF_INET, (char *) &(cli_addr.sin_addr), str, sizeof(cli_addr));
                //printf("connect\n");

				fcntl(sock, F_SETFL, fcntl(sock, F_GETFD, 0) | O_NONBLOCK);

                struct epoll_event ev;
	            ev.events = EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP;
	            ev.data.fd = sock;
	            if (epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &ev) == -1) {
		            perror("error epoll_ctl");
		            return 1;
	            }
			} else if (events[i].events & EPOLLIN) {
				while(1) {
					if (read(events[i].data.fd, str, 100) <= 0) {
						break;
					} else {
                        //printf("received %s", str);
						write(events[i].data.fd, str, strlen(str) + 1);
					}
				}
			}
            if (events[i].events & (EPOLLRDHUP | EPOLLHUP)) {
                //printf("disconnect\n");
				epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
				close(events[i].data.fd);
			}
        }
    }
    return 0;
}