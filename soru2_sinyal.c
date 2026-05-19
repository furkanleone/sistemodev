#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h> /* nanosleep() icin eklendi */

volatile pid_t child_pid = 0;
volatile int alarm_count = 0;

void child_sigint_handler(int sig) {
    (void)sig; /* Kullanilmayan parametre uyarisi susturuldu */
    signal(SIGINT, child_sigint_handler); 
    printf("Çocuk: SIGINT alındı ancak devam ediliyor...\n");
    fflush(stdout);
}

void child_sigcont_handler(int sig) {
    (void)sig; 
    signal(SIGCONT, child_sigcont_handler);
    printf("Çocuk: İşlem yeniden başlatıldı\n");
    fflush(stdout);
}

void parent_sigalrm_handler(int sig) {
    (void)sig; 
    signal(SIGALRM, parent_sigalrm_handler);
    
    alarm_count++;

    if (child_pid <= 0) return;

    printf("Ebeveyn: Çocuk durduruluyor...\n");
    fflush(stdout);
    kill(child_pid, SIGSTOP);

    sleep(2);

    printf("Ebeveyn: Çocuk devam ediyor...\n");
    fflush(stdout);
    kill(child_pid, SIGCONT);

    if (alarm_count < 2) {
        alarm(3);
    } else {
        printf("Ebeveyn: SIGINT gönderiliyor...\n");
        fflush(stdout);
        kill(child_pid, SIGINT);

        /* POSIX-2008 uyumlu 0.1 saniyelik bekleme (usleep yerine) */
        struct timespec req = {0, 100000000}; 
        nanosleep(&req, NULL);

        kill(child_pid, SIGKILL);
        waitpid(child_pid, NULL, 0);

        printf("Ebeveyn: Çocuk sonlandırıldı.\n");
        fflush(stdout);
        exit(0);
    }
}

int main(void) {
    pid_t pid;
    int counter = 0;

    pid = fork();

    if (pid < 0) {
        perror("fork hatası");
        return 1;
    }

    if (pid == 0) {
        /* ÇOCUK SÜREÇ */
        signal(SIGINT, child_sigint_handler);
        signal(SIGCONT, child_sigcont_handler);

        while (1) {
            printf("Çocuk sayacı: %d\n", counter);
            fflush(stdout);
            counter++;
            sleep(1);
        }
        return 0;
    }

    /* ANA SÜREÇ */
    child_pid = pid;
    signal(SIGALRM, parent_sigalrm_handler);

    alarm(3);

    while (1) {
        pause(); 
    }

    return 0;
}