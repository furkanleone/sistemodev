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

/* Cocuk surec SIGINT aldiginda programi hemen bitirmez;
 * istenen bilgilendirme mesajini yazdirip calismaya devam eder. */
void child_sigint_handler(int sig) {
    (void)sig; /* Kullanilmayan parametre uyarisi susturuldu */
    signal(SIGINT, child_sigint_handler); 
    printf("Çocuk: SIGINT alındı ancak devam ediliyor...\n");
    fflush(stdout);
}

/* Cocuk surec SIGCONT ile devam ettirildiginde bu handler calisir
 * ve surecin tekrar calismaya basladigini bildirir. */
void child_sigcont_handler(int sig) {
    (void)sig; 
    signal(SIGCONT, child_sigcont_handler);
    printf("Çocuk: İşlem devam etti\n");
    fflush(stdout);
}

/* Ana surecin alarm handler'i her tetiklemede cocuk sureci once
 * SIGSTOP ile durdurur, 2 saniye bekler ve SIGCONT ile devam ettirir.
 * Ikinci alarmdan sonra SIGINT gonderip cocuk sureci sonlandirir. */
void parent_sigalrm_handler(int sig) {
    (void)sig; 
    signal(SIGALRM, parent_sigalrm_handler);
    
    alarm_count++;

    if (child_pid <= 0) return;

    printf("Ebeveyn: Çocuk durduruluyor...\n");
    fflush(stdout);
    /* SIGSTOP islenemez; bu nedenle cocuk sureci dogrudan durdurur. */
    kill(child_pid, SIGSTOP);

    /* Odev isterine uygun olarak cocuk surec 2 saniye durdurulmus kalir. */
    sleep(2);

    if (alarm_count < 2) {
        printf("Ebeveyn: Çocuk devam ediyor...\n");
        fflush(stdout);
        /* Cocuk surecin calismasina devam etmesi icin SIGCONT gonderilir. */
        kill(child_pid, SIGCONT);
        alarm(3);
    } else {
        printf("Ebeveyn: Çocuk devam ediyor...\n");
        fflush(stdout);
        /* Son dongude de SIGCONT gonderilir, ardindan bitirme asamasina gecilir. */
        kill(child_pid, SIGCONT);

        printf("Ebeveyn: SIGINT gönderiliyor...\n");
        fflush(stdout);
        /* Cocuk surecin SIGINT handler'inin calistigini gostermek icin SIGINT gonderilir. */
        kill(child_pid, SIGINT);

        /* POSIX-2008 uyumlu 0.1 saniyelik bekleme (usleep yerine) */
        struct timespec req = {0, 100000000}; 
        nanosleep(&req, NULL);

        /* Cocuk surec temiz bicimde beklenmeden once kesin olarak sonlandirilir. */
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
        /* Cocuk surec kendisine gelen SIGINT ve SIGCONT sinyallerini yakalar. */
        signal(SIGINT, child_sigint_handler);
        signal(SIGCONT, child_sigcont_handler);

        /* Cocuk surec sonsuz dongude her saniye sayac degerini yazdirir. */
        while (1) {
            printf("Çocuk sayacı: %d\n", counter);
            fflush(stdout);
            counter++;
            sleep(1);
        }
        return 0;
    }

    /* ANA SÜREÇ */
    /* Ana surec cocugun PID degerini saklar ve SIGALRM icin handler kurar. */
    child_pid = pid;
    signal(SIGALRM, parent_sigalrm_handler);

    /* Ilk alarm 3 saniye sonra tetiklenir. Handler sonraki alarmi tekrar kurar. */
    alarm(3);

    while (1) {
        pause(); 
    }

    return 0;
}
