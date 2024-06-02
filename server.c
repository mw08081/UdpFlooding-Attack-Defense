#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 2
#define BUFFER_SIZE 1024

// 클라이언트 정보 구조체
struct Client {
    struct sockaddr_in addr; // 클라이언트의 주소 정보
};

int main() {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    char buffer[BUFFER_SIZE];

    // 화이트 테이블 초기화
    struct Client white_table[MAX_CLIENTS];
    int num_clients = 0;

    // 소켓 생성
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // 서버 주소 설정
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(1234);

    // 소켓에 주소 바인딩
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // 메시지 수신 및 처리
    while (1) {
        len = sizeof(cliaddr);
        ssize_t n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0';
        
        
        // 클라이언트의 정보를 화이트 테이블에 저장
        int i;
        int isNewClient = 1; // 새로운 클라이언트 여부
        for (i = 0; i < num_clients; i++) {
            if (memcmp(&cliaddr, &white_table[i].addr, sizeof(struct sockaddr_in)) == 0) {
                isNewClient = 0;
                break;
            }
        }
        if (isNewClient && num_clients < MAX_CLIENTS) {
            white_table[num_clients].addr = cliaddr;
            num_clients++;

            // 새로운 멤버의 IP 주소와 포트 번호 출력
            printf("New member added to white table: %s:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
        }

        // 클라이언트 정보 출력
        printf("Received message from %s:%d - %s\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), buffer);

        // 클라이언트로부터 수신한 메시지를 화이트 테이블에 있는 모든 클라이언트에게 전송
        for (i = 0; i < num_clients; i++) {
            sendto(sockfd, (const char *)buffer, strlen(buffer), MSG_CONFIRM, (const struct sockaddr *)&white_table[i].addr, sizeof(struct sockaddr_in));
        }
    }

    close(sockfd); // close() 함수 호출을 여기서 추가합니다.
    return 0;
}