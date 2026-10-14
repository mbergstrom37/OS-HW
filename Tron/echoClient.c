// To be honest I was running out of time so to make this work, I generated it with AI, based off of code I copied from GeeksforGeeks and the logic structure I wanted made with comments, if blocks, and while loops in that code. I should have worked on this sooner to do it better and with less AI help, but I at least tried to think it out before resorting to AI.
#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
 
#define PORT 8989
#define ROUNDS 3
 
enum { RESULT_CONTINUE = 0, RESULT_LOST = 1, RESULT_WON = 2 };
 
static const int range[ROUNDS] = { 12, 10, 8 };
 
static int send_int(int fd, int32_t v)
{
    int32_t n = htonl(v);
    return send(fd, &n, sizeof n, 0) == sizeof n ? 0 : -1;
}
 
static int recv_int(int fd, int32_t* out)
{
    int32_t n;
    if (recv(fd, &n, sizeof n, MSG_WAITALL) != sizeof n)
        return -1;
    *out = ntohl(n);
    return 0;
}
 
// Read an int in [lo, hi] from stdin, re-prompting until valid
static int read_in_range(int lo, int hi)
{
    int v;
    while (1) {
        if (scanf("%d", &v) != 1) {
            if (feof(stdin)) exit(0);
            scanf("%*s");               // discard non-numeric input
        } else if (v >= lo && v <= hi) {
            return v;
        }
        printf("Invalid input. Enter a number between %d and %d.\n", lo, hi);
    }
}
 
int main(void)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
 
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
 
    if (connect(fd, (struct sockaddr*)&addr, sizeof addr) < 0) {
        perror("connect");
        return 1;
    }
    printf("Connected to server\n");
 
    int keep_playing = 1;
    while (keep_playing) {
        int result = RESULT_CONTINUE;
 
        for (int r = 0; r < ROUNDS && result == RESULT_CONTINUE; r++) {
            printf("Round %d: enter a number between 1 and %d.\n",
                   r + 1, range[r]);
            int guess = read_in_range(1, range[r]);
 
            int32_t reply;
            if (send_int(fd, guess) < 0 || recv_int(fd, &reply) < 0) {
                printf("Lost connection to server.\n");
                close(fd);
                return 1;
            }
            result = reply;
 
            if (result == RESULT_LOST)
                printf("You crashed the bike into a line. You lose!\n");
            else if (result == RESULT_WON)
                printf("You managed to maneuver around the other's lines and destroy all other bikes. You win!\n");
            else
                printf("Wow! Nice turn to avoid that line! On to the next round.\n");
        }

        // Server reveals all 3 secret numbers once the game is over
        int32_t secret[ROUNDS];
        for (int r = 0; r < ROUNDS; r++) {
            if (recv_int(fd, &secret[r]) < 0) {
                printf("Lost connection to server.\n");
                close(fd);
                return 1;
            }
        }
        printf("The server's numbers were: %d, %d, %d\n",
               secret[0], secret[1], secret[2]);
 
        printf("Play again? (1 = yes, 0 = no)\n");
        keep_playing = read_in_range(0, 1);
        send_int(fd, keep_playing);
    }
 
    close(fd);
    return 0;
}