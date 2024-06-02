#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define TEMP_WHITE_TABLE_SIZE 1000000

// 클라이언트 정보 구조체
struct Client {
    struct sockaddr_in addr; // 클라이언트의 주소 정보
};

struct Client white_table[MAX_CLIENTS];
struct Client temp_white_table[TEMP_WHITE_TABLE_SIZE];
int num_clients = 0;
int num_temp_clients = 0;
int packet_count = 0;

void reset_temp_white_table() {
    num_temp_clients = 0;
    memset(temp_white_table, 0, sizeof(temp_white_table));
}

int find_client(struct Client table[], int size, struct sockaddr_in *cliaddr) {
    for (int i = 0; i < size; i++) {
        if (memcmp(&table[i].addr, cliaddr, sizeof(struct sockaddr_in)) == 0) {
            return i;
        }
    }
    return -1;
}

void add_to_white_table(struct sockaddr_in *cliaddr) {
    if (num_clients < MAX_CLIENTS) {
        white_table[num_clients].addr = *cliaddr;
        num_clients++;
        printf("New member added to white table: %s:%d\n", inet_ntoa(cliaddr->sin_addr), ntohs(cliaddr->sin_port));
    }
}

void process_packet(int sockfd, char *buffer, struct sockaddr_in *cliaddr) {
    // 패킷 카운트 증가
    packet_count++;
    if (packet_count >= 50) {
        reset_temp_white_table();
        packet_count = 0;
    }

    // 화이트 테이블에 있는지 확인
    int index = find_client(white_table, num_clients, cliaddr);
    if (index < 0) {
        // 화이트 테이블에 없는 경우
        index = find_client(temp_white_table, num_temp_clients, cliaddr);
        printf("%d %d \n", index, num_temp_clients);
        if (index < 0 && num_temp_clients < TEMP_WHITE_TABLE_SIZE) {
            // temp white table에도 없는 경우
            temp_white_table[num_temp_clients].addr = *cliaddr;
            num_temp_clients++;
            printf("Added to temp white table: %s:%d\n", inet_ntoa(cliaddr->sin_addr), ntohs(cliaddr->sin_port));
        } else if (index >= 0) {
            // temp white table에 있는 경우
            add_to_white_table(cliaddr);
        }

        return;
    }

    // 클라이언트 정보 출력
    printf("Received message from %s:%d - %s\n", inet_ntoa(cliaddr->sin_addr), ntohs(cliaddr->sin_port), buffer);

    // 클라이언트로부터 수신한 메시지를 화이트 테이블에 있는 모든 클라이언트에게 전송
    for (int i = 0; i < num_clients; i++) {
        sendto(sockfd, buffer, strlen(buffer), MSG_CONFIRM, (const struct sockaddr *)&white_table[i].addr, sizeof(struct sockaddr_in));
    }

    
}

int main() {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    char buffer[BUFFER_SIZE];

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
        ssize_t n = recvfrom(sockfd, buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0';
        process_packet(sockfd, buffer, &cliaddr);
    }

    close(sockfd);
    return 0;
}
