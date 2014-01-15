#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <future>
#include <functional>
#include <signal.h>
#include <iostream>
#include "ICTCLAS50.h"

using namespace std;

#define SERVER_PORT 9100
#define LISTEN_QUEUE 10000
#define BUFFER_SIZE 65536

#define USER_DICT "../../dict/userdict.txt"

int parse(int socket);
bool LA_import_user_dict(const char *path, eCodeType type=CODE_TYPE_UTF8);
int server_socket;

void at_exit(int sig) {
    if (server_socket > 0) 
        close(server_socket);
    cout << "Server Exited"<<endl;
    exit(0);
}

#define MAX_EVENTS 1000

int main(int argc, char **argv) {
    signal(SIGINT, at_exit);

    struct epoll_event ev, events[100];
    int epollfd, nfds;
    epollfd = epoll_create(MAX_EVENTS);
    if (epollfd == -1) {
        perror("epoll creat");
        exit(EXIT_FAILURE);
    }

    if (!ICTCLAS_Init()) {
        cerr<< "ICTCLAS init Error"<<endl;
        exit(1);
    }
    // ICTCLAS_SetPOSmap(ICT_POS_MAP_SECOND);
    // import user dict
    LA_import_user_dict(USER_DICT, CODE_TYPE_UTF8);

    // server
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)  {
        perror("Socket create error: ");
        exit(2);
    }
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) ) {
        perror("Socket bin error: ");
        exit(2);
    }
    if (listen(server_socket, LISTEN_QUEUE)) {
        perror("Socket listen error: ");
        exit(2);
    }
    ev.events = EPOLLIN;
    ev.data.fd = server_socket;

    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, server_socket, &ev) == -1) {
        perror("epoll_ctl: listen_sock");
        exit(EXIT_FAILURE);
    }

    while(1) {
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_pwait");
            exit(EXIT_FAILURE);
        }

        struct sockaddr_in client_addr;
        int n_socket;
        int flag;
        socklen_t len;
        for (int n = 0; n < nfds; n++) {
            if (events[n].data.fd == server_socket) {
                len = sizeof(client_addr);
                n_socket = accept(server_socket, (struct sockaddr *)&client_addr, &len);
                if (n_socket < 0) {
                    perror("Client connect error");
                    continue;
                }
                flag = 1;
                ioctl(n_socket, FIONBIO, &flag);
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = n_socket;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, n_socket, &ev) == -1) {
                    perror("epoll_ctl: n_socket");
                    exit(EXIT_FAILURE);
                }
            }
            else {
                parse(n_socket);
                // auto instance = async(launch::async, parse, n_socket);
                // instance.get();
            }
        }
    }
    close(server_socket);
    ICTCLAS_Exit();
    return 0;
}


int parse(int n_socket) {
    if (n_socket < 0)
        return 1;
    char buffer[65000];
    ssize_t len = 0;
    len = read(n_socket, buffer, sizeof(buffer));
    if (len >= 4) {
        if (buffer[0] == 'P' && buffer[1] == 'P') {
            char *rst = (char *)malloc(len * 6);
            len = ICTCLAS_ParagraphProcess(buffer + 4, len - 4, rst, CODE_TYPE_UTF8, 1);
            len = write(n_socket, rst, strlen(rst));
        }
	    else if (buffer[0] == 'A' && buffer[1] == 'W') {
            LA_import_user_dict(USER_DICT);
	    }
    }
    close(n_socket);
    return 0;
}



/********************************************
 * param:
 *     path: the dict path
 */
bool LA_import_user_dict(const char *path, eCodeType type) {
    unsigned int count = ICTCLAS_ImportUserDictFile(USER_DICT, type);
    ICTCLAS_SaveTheUsrDic();
    printf("import count: %d\n", count);
    return true;
}
