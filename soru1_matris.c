/*
 * Soru 1: Dinamik 2D Matris - Yalnizca Isaretti Aritmetigi Kullanarak
 * Derleme: gcc -o soru1_matris soru1_matris.c
 * Calistirma: ./soru1_matris
 */

#include <stdio.h>
#include <stdlib.h>

/*
 * special_sum: N x N matrisin ana kosegen ve ikincil kosegen
 * elemanlarinin toplamini hesaplar. Yalnizca isaretci aritmetigi kullanir.
 * Merkez eleman (eger varsa) iki kez sayilmaz.
 *
 * Parametreler:
 *   mat  - Matrisin baslangic adresini gosteren isaretci (1D bellek blogu)
 *   rows - Matrisin satir sayisi
 *   cols - Matrisin sutun sayisi
 *
 * Donus: Kosegen elemanlarinin toplami (int)
 */
int special_sum(int *mat, int rows, int cols) {
    int sum = 0;
    int i;

    for (i = 0; i < rows; i++) {
        /* Ana kosegen elemani: pozisyon (i, i)
         * Bellekteki karsilik: mat + i * cols + i
         * mat[i][j] sozdizimi KULLANILMIYOR */
        sum += *(mat + i * cols + i);

        /* Ikincil kosegen elemani: pozisyon (i, cols-1-i)
         * Bellekteki karsilik: mat + i * cols + (cols - 1 - i) */
        int secondary_col = cols - 1 - i;
        if (secondary_col != i) {
            /* Merkez eleman degil: iki kez saymamak icin kontrol */
            sum += *(mat + i * cols + secondary_col);
        }
        /* Eger secondary_col == i ise, bu eleman ana koşegen elemanıdır
         * (merkez eleman), zaten yukarida eklendi, tekrar eklenmez. */
    }

    return sum;
}

int main(void) {
    int n;          /* Matris boyutu (NxN) */
    int *mat;       /* Matrisi temsil eden tek boyutlu isaretci */
    int i, j;       /* Dongu degiskenleri */
    int result;     /* special_sum sonucu */

    /* Kullanicidan matris boyutunu al */
    printf("Matris boyutunu girin (N): ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        fprintf(stderr, "Gecersiz boyut!\n");
        return 1;
    }

    /* malloc ile N*N boyutunda bellek tahsis et (tek 1D blok) */
    mat = (int *)malloc(n * n * sizeof(int));
    if (mat == NULL) {
        fprintf(stderr, "Bellek tahsisi basarisiz!\n");
        return 1;
    }

    /* Kullanicidan matris elemanlarini oku
     * Eleman (i,j) bellekte: mat + i*n + j konumundadir */
    printf("Matris elemanlarini satir satir girin (%d x %d):\n", n, n);
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            printf("mat[%d][%d] = ", i, j);
            /* Isaretci aritmetigi ile elemana eris */
            if (scanf("%d", (mat + i * n + j)) != 1) {
                fprintf(stderr, "Gecersiz giris!\n");
                free(mat);
                return 1;
            }
        }
    }

    /* Girilen matrisi ekrana yazdir (dogrulama amacli) */
    printf("\nGirilen Matris:\n");
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            /* Isaretci aritmetigi ile okuma: *(mat + i*n + j) */
            printf("%5d", *(mat + i * n + j));
        }
        printf("\n");
    }

    /* special_sum fonksiyonunu cagir ve sonucu yazdir */
    result = special_sum(mat, n, n);
    printf("\nAna + Ikincil Kosegen Elemanlari Toplami = %d\n", result);

    /* Tahsis edilen bellek serbest birakiliyor - bellek sizintisi onlenir */
    free(mat);
    mat = NULL;

    return 0;
}
