// Soru 2: Fork ve Sinyal İşleme (Signal Handling) Ödevi
// Bu programda ebeveyn ve çocuk süreçlerin sinyallerle haberleşmesini yapıyoruz.

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h> // nanosleep() fonksiyonu için şart

// Ebeveyn süreç sinyal gönderirken çocuğun PID'ini bilsin diye global tanımladım
volatile pid_t child_pid = 0;
// Kaçıncı alarmda olduğumuzu saymak için sayaç
volatile int alarm_count = 0;

// ÇOCUK SÜRECİN SİNYAL HANDLER'LARI
void child_sigint_handler(int sig) {
    (void)sig; // Hoca Makefile'a -Wextra koymuş, "sig kullanılmadı" uyarısını susturmak için.
    
    // Sinyal bir kez gelince handler sıfırlanmasın diye kendini tekrar kurduruyorum
    signal(SIGINT, child_sigint_handler); 
    printf("Çocuk: SIGINT alındı ancak devam ediliyor...\n");
    fflush(stdout); // Terminale anında yazdırsın diye buffer'ı boşaltıyoruz
}

void child_sigcont_handler(int sig) {
    (void)sig; 
    signal(SIGCONT, child_sigcont_handler);
    printf("Çocuk: İşlem yeniden başlatıldı\n");
    fflush(stdout);
}

// EBEVEYN SÜRECİN SİNYAL HANDLER'I (Bütün işi yöneten yer)
void parent_sigalrm_handler(int sig) {
    (void)sig; 
    signal(SIGALRM, parent_sigalrm_handler); // Alarm clock hatası yememek için alarmı tekrar kur
    
    alarm_count++;

    if (child_pid <= 0) return; // Çocuk henüz doğmadıysa bir şey yapma

    // 1. ADIM: Çocuğu durdur
    printf("Ebeveyn: Çocuk durduruluyor...\n");
    fflush(stdout);
    kill(child_pid, SIGSTOP); // SIGSTOP sinyali çocuğu dondurur

    // 2. ADIM: Ebeveyn olarak 2 saniye kestiriyoruz (çocuk o sırada donuk kalıyor)
    sleep(2);

    // 3. ADIM: Çocuğu kaldığı yerden devam ettir
    printf("Ebeveyn: Çocuk devam ediyor...\n");
    fflush(stdout);
    kill(child_pid, SIGCONT); // SIGCONT çocuğu uykudan uyandırır

    // 10 SANİYE KONTROLÜ
    // Her alarm döngüsü: 3 saniye alarm + 2 saniye uyku = toplam 5 saniye sürüyor.
    // Yani 2. alarm bittiğinde tam 10 saniye geçmiş oluyor.
    if (alarm_count < 2) {
        alarm(3); // 10 saniye dolmadıysa sonraki 3 saniyelik alarmı kur
    } else {
        // 10 saniye doldu, artık bitirme vakti!
        printf("Ebeveyn: SIGINT gönderiliyor...\n");
        fflush(stdout);
        kill(child_pid, SIGINT); // Çocuğa önce uyarı amaçlı SIGINT gönderiyoruz

        // Çocuk sinyali yakalayıp mesajı basabilsin ama döngüye girip yeni sayı 
        // sayamasın diye arada 0.1 saniye (çok kısa) bekletiyoruz.
        // POSIX-2008 standatlarında usleep yasak olduğu için nanosleep kullandım.
        struct timespec req = {0, 100000000}; // 100 milyon nanosaniye = 0.1 saniye
        nanosleep(&req, NULL);

        // Çocuğun fişini kesin olarak çekiyoruz
        kill(child_pid, SIGKILL);
        waitpid(child_pid, NULL, 0); // İşletim sisteminde zombi (zombie) süreç kalmasın diye temizlik

        printf("Ebeveyn: Çocuk sonlandırıldı.\n");
        fflush(stdout);
        exit(0); // Ebeveyni de kapatıp programı bitiriyoruz
    }
}

int main(void) {
    pid_t pid;
    int counter = 0; // Çocuğun sayacağı sayı

    // fork() ile süreci ikiye bölüyoruz
    pid = fork();

    if (pid < 0) {
        perror("fork hatası");
        return 1;
    }

    if (pid == 0) {
        /* ---------- ÇOCUK SÜREÇ ---------- */
        // Çocuk süreç kendi yakalayacağı sinyalleri burada tanımlıyor
        signal(SIGINT, child_sigint_handler);
        signal(SIGCONT, child_sigcont_handler);

        // Sonsuz döngüde her saniye sayacı arttırıp yazdırıyor
        while (1) {
            printf("Çocuk sayacı: %d\n", counter);
            fflush(stdout);
            counter++;
            sleep(1); // 1 saniye uyu
        }
        return 0;
    }

    /* ---------- EBEVEYN SÜREÇ ---------- */
    child_pid = pid; // Çocuğun ID'sini globale kaydet ki handler içerisinden kill yapabilelim
    signal(SIGALRM, parent_sigalrm_handler); // Alarm çalınca ne yapacağını söyle

    alarm(3); // İlk fitili ateşliyoruz, 3 saniye sonra alarm çalacak

    // Ebeveyn süreç burada boş boş durup sinyal bekliyor.
    // Buraya waitpid koyunca ilk alarmda bozuluyordu, o yüzden pause döngüsü en mantıklısı oldu.
    while (1) {
        pause(); 
    }

    return 0;
}