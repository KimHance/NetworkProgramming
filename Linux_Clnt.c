//===리눅스 클라이언트
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE 100
#define NAME_SIZE 20

void* send_msg(void* arg);
void* recv_msg(void* arg);
void error_handling(char* msg);

int main(int argc, char* argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    char name[NAME_SIZE] = "[DEFAULT]";
    int n;

    if (argc != 4) {
        printf("Usage : %s <IP> <port> <name>\n", argv[0]);
        exit(1);
    }

    sprintf(name, "[%s]", argv[3]);         //name변수에 argv[3]이 저장됨
    sock = socket(PF_INET, SOCK_STREAM, 0);

    //memset함수는 메모리의 시작점부터 연속된 범위를 어떤 값으로 모두 지정하고 싶을 때 사용
    //serv_addr이 시작주소, 메모리에 0을 채운다. sizeof(serv_addr)크기만큼 메모리 채움
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    //서버 연결 설정(connect 에러처리)
    if (connect(sock, (struct sockaddr*) & serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error");

    // 게임
    write(sock, name, strlen(name) + 1);
    printf("============================================== \n");
    printf("현봉지의 배스킨라빈스 31게임을 시작하겠습니다. \n");
    printf("============================================== \n");
    read(sock, &n, sizeof(int));

    while (1) {
        if (n > 0) {
            recv_msg(&sock);
            send_msg(&sock);
        }
        else {
            send_msg(&sock);
            recv_msg(&sock);
        }
    }

    close(sock);
    return 0;
}

void* send_msg(void* arg)   // send thread main
{
    int n;
    int sock = *((int*)arg);
    char msg[BUF_SIZE];

    while (1) {
        printf("숫자를 입력하세요 :");
        fgets(msg, BUF_SIZE, stdin);            //msg를 입력받는다.

        if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
        {
            //q나 Q를 누르면 소켓 종료
            close(sock);
            exit(0);
        }

        n = atoi(msg);

        if ((n > 3) || (n < 1)) {
            //클라이언트 입력 숫자가 1보다 작거나 3보다 크면 예외처리
            printf("잘못 입력하셨습니다. 1~3 숫자를 입력해주세요(종료:q나 Q)\n");
            continue;
        }
        break;
    }
    //클라이언트가 입력한 숫자를 보낸다.
    write(sock, &n, sizeof(int));

    return NULL;
}

void* recv_msg(void* arg)   //read thread main
{
    int number;
    int sock = *((int*)arg);
    int str_len;
    str_len = read(sock, &number, sizeof(int));
    if (str_len == -1) {
        return (void*)-1;
    }
    if (number == -1) {
        //클라이언트가 number -1을 주면 패배
        printf("패배!!!!!!\n");
        exit(1);
    }
    else if (number == -2) {
        //클라이언트가 number -2를 주면 승리
        printf("승리!!!!!!\n");
        exit(1);
    }
    printf("Accumulated number: %d\n", number);
    return NULL;
}

void error_handling(char* msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}
