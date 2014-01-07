
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>
#include <future>
#include <functional>
#include "NLPIR.h"

using namespace std;

#define SERVER_PORT 9101
#define LISTEN_QUEUE 10000
#define BUFFER_SIZE 65536

int parse(int socket);
int server_socket;

void at_exit(int sig) {
    if (server_socket > 0) 
        close(server_socket);
    cout << "Server Exited"<<endl;
    exit(0);
}

int main(int argc, char ** argv) {
    signal(SIGINT, at_exit);


    if (!NLPIR_Init(NULL, UTF8_CODE)) {
        cerr<< "ICTCLAS init Error"<<endl;
        exit(1);
    }
    else {
        cout<< "ICTCLAS Inited"<<endl;
    }
    // import user dict
    unsigned int count = NLPIR_ImportUserDict("userdict.txt");
    printf("load user dict count: %d\n", count);
    count = NLPIR_SaveTheUsrDic();
    printf("save user dict count: %d\n", count);



    char buf1[] = "路飞索隆山治";
    unsigned int len = strlen(buf1);
    const char *rst = NLPIR_ParagraphProcess(buf1, 1);
    cout<<rst<<endl;
    if (argc > 1) {
	len = strlen(argv[1]);
        rst = NLPIR_ParagraphProcess(argv[1], 1);
	cout<<rst<<endl;
    }

    char buf2[] = "中华人民共和国";
    len = strlen(buf2);
    rst = NLPIR_ParagraphProcess(buf2, 1);
    cout<<rst<<endl;

    return 0;
}
