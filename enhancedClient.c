#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h> // unistd.h 헤더 파일 추가
#include <sys/time.h> // 시간 관련 헤더 파일 추가
#include <time.h> // time.h 헤더 파일 추가

#define SERVER_IP "192.168.126.139" // 서버의 IP 주소
#define SERVER_PORT 1234 // 서버의 포트 번호
#define CLIENT_PORT 4321 // 클라이언트의 포트 번호
#define BUFFER_SIZE 1024

// 현재 시간을 문자열로 변환하는 함수
void get_current_time(char *time_str) {
    struct timeval tv;
    struct tm *tm_info;

    gettimeofday(&tv, NULL);
    tm_info = localtime(&tv.tv_sec);

    strftime(time_str, 20, "%Y-%m-%d %H:%M:%S", tm_info);
}

int main() {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr, other_cliaddr;
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    socklen_t len;
    int interval = 2; // 2초 간격으로 반복

    // 소켓 생성
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        return EXIT_FAILURE;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // 서버 주소 설정
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &servaddr.sin_addr) <= 0) {
        perror("invalid address");
        close(sockfd);
        return EXIT_FAILURE;
    }

    // 클라이언트 주소 설정
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_addr.s_addr = INADDR_ANY;
    cliaddr.sin_port = htons(CLIENT_PORT);

    // 소켓에 주소 바인딩
    if (bind(sockfd, (const struct sockaddr *)&cliaddr, sizeof(cliaddr)) < 0) {
        perror("bind failed");
        close(sockfd);
        return EXIT_FAILURE;
    }

    while (1) {
        // 현재 시간과 "hello world" 메시지를 함께 생성
        char current_time[20];
        get_current_time(current_time);
        sprintf(message, "c2 's udp packet (%s)", current_time);

        // 메시지 전송
        sendto(sockfd, message, strlen(message), MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr));

        // 서버로부터 메시지 수신
        len = sizeof(other_cliaddr);
        ssize_t n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_DONTWAIT, (struct sockaddr *)&other_cliaddr, &len);
        while (n < 0) {
            printf("recvfrom failed - %s\n", message);
            // 1초 기다린 후 다시 시도
            sleep(1);

            sendto(sockfd, message, strlen(message), MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr));
            n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_DONTWAIT, (struct sockaddr *)&other_cliaddr, &len);
        } 
        buffer[n] = '\0';
        printf("Message from server: %s\n", buffer);

        // 3초 대기
        sleep(interval);
    }

    close(sockfd);
    return 0;
}
