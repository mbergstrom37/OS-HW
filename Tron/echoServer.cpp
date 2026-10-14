// To be honest I was running out of time so to make this work, I generated it with AI, based off of code I copied from GeeksforGeeks and the logic structure I wanted made with comments, if blocks, and while loops in that code. I should have worked on this sooner to do it better and with less AI help, but I at least tried to think it out before resorting to AI. 
#include <arpa/inet.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
 
#define PORT 8989
#define ROUNDS 3
 
// Replies sent to the client after each guess
enum { RESULT_CONTINUE = 0, RESULT_LOST = 1, RESULT_WON = 2 };
 
static const int range[ROUNDS] = { 12, 10, 8 };
 
// Send/receive one 32-bit int in network byte order
static int send_int(int fd, int32_t v)
{
    int32_t n = htonl(v);
    return send(fd, &n, sizeof n, 0) == sizeof n ? 0 : -1;
}
 
static int recv_int(int fd, int32_t* out)
{
    int32_t n;
    // MSG_WAITALL: don't return until all 4 bytes arrive (or error/close)
    if (recv(fd, &n, sizeof n, MSG_WAITALL) != sizeof n)
        return -1;
    *out = ntohl(n);
    return 0;
}
 
// Runs once per client, for the life of that client's connection
static void* client_handler(void* arg)
{
    int fd = (int)(intptr_t)arg;
    unsigned int seed = (unsigned int)time(NULL) ^ (unsigned int)fd
                        ^ (unsigned int)(uintptr_t)pthread_self();
 
    printf("Client %d connected\n", fd);
 
    int play_again = 1;
    while (play_again) {
        // Fresh secret numbers for THIS client's game
        int secret[ROUNDS];
        for (int r = 0; r < ROUNDS; r++)
            secret[r] = (rand_r(&seed) % range[r]) + 1;
 
        int lost = 0;
        for (int r = 0; r < ROUNDS && !lost; r++) {
            int32_t guess;
            if (recv_int(fd, &guess) < 0)
                goto disconnect;
 
            if (guess == secret[r]) {
                lost = 1;                       // matched -> they lose
                send_int(fd, RESULT_LOST);
            } else if (r == ROUNDS - 1) {
                send_int(fd, RESULT_WON);       // survived all rounds
            } else {
                send_int(fd, RESULT_CONTINUE);
            }
        }
 
        // Ask client whether to play again (1 = yes, 0 = no)
        int32_t again;
        if (recv_int(fd, &again) < 0)
            break;
        play_again = (again == 1);
    }
 
disconnect:
    printf("Client %d disconnected\n", fd);
    close(fd);
    return NULL;
}
 
int main(void)
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
 
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
 
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);
 
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof addr) < 0) {
        perror("bind");
        return 1;
    }
    if (listen(server_fd, 50) < 0) {
        perror("listen");
        return 1;
    }
    printf("Listening on port %d\n", PORT);
 
    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }
 
        pthread_t tid;
        if (pthread_create(&tid, NULL, client_handler,
                           (void*)(intptr_t)client_fd) != 0) {
            perror("pthread_create");
            close(client_fd);
            continue;
        }
        pthread_detach(tid);   // thread cleans itself up; no join needed
    }
}
 