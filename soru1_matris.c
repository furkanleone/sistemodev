

#include <stdio.h>
#include <stdlib.h>

// Bu fonksiyon ana ve yan (ikincil) köşegenleri topluyor.
// Parametre olarak matrisin ilk adresini ve satır/sütun sayısını gönderiyoruz.
int special_sum(int *mat, int rows, int cols) {
    int sum = 0;
    int i;

    for (i = 0; i < rows; i++) {
        // Ana köşegen için elemanlar: (0,0), (1,1), (2,2) vs.
        // Hoca köşeli parantez kullanmayın dediği için formül: mat + (i * cols) + i
        sum += *(mat + i * cols + i);

        // Yan köşegenin sütun indisini buluyoruz (sondan geriye doğru geliyor)
        int secondary_col = cols - 1 - i;
        
        // Eğer matris boyutu tek sayıysa (mesela 3x3), tam ortadaki elemanı 
        // hem ana hem yan köşegende iki kere toplamamak için bir kontrol yapmamız lazım.
        if (secondary_col != i) {
            sum += *(mat + i * cols + secondary_col);
        }
    }

    return sum;
}

int main(void) {
    int n;          // Matrisin boyutu (NxN olacak)
    int *mat;       // Matrisi tutacağımız 1 boyutlu pointer
    int i, j;       // Klasik döngü değişkenleri
    int result;     // Toplamı tutacağımız değişken

    // Kullanıcıdan matrisin boyutunu istiyoruz
    printf("Matris boyutunu girin (N): ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        fprintf(stderr, "Hatalı boyut girdiniz!\n");
        return 1;
    }

    // Matrisi aslında 1 boyutlu dümdüz bir dizi gibi oluşturuyoruz.
    // N*N tane eleman için malloc ile RAM'den yer istiyoruz.
    mat = (int *)malloc(n * n * sizeof(int));
    if (mat == NULL) {
        fprintf(stderr, "RAM'de yer ayrılamadı!\n");
        return 1;
    }

    // Kullanıcıdan elemanları tek tek alıyoruz
    printf("Matris elemanlarını satır satır girin (%d x %d):\n", n, n);
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            printf("mat[%d][%d] = ", i, j);
            
            // scanf içine adres yazarken de pointer aritmetiği kullandım.
            // i*n satırları atlamak için, j ise sütunda ilerlemek için.
            if (scanf("%d", (mat + i * n + j)) != 1) {
                fprintf(stderr, "Geçersiz bir sayı girdiniz!\n");
                free(mat); // Hata verip çıkmadan önce çöp bırakmayalım
                return 1;
            }
        }
    }

    // Girilen matrisi ekrana düzgün bir şekilde yazdırıp kontrol ediyoruz
    printf("\nGirilen Matris:\n");
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            // Değeri okumak için pointer'ın başına * koyup dereference yapıyoruz
            printf("%5d", *(mat + i * n + j));
        }
        printf("\n");
    }

    // Fonksiyonu çağırıp işlemi yaptırıyoruz
    result = special_sum(mat, n, n);
    printf("\nAna + İkincil Köşegen Elemanları Toplamı = %d\n", result);

    // İşimiz bitince belleği işletim sistemine geri veriyoruz (memory leak olmasın diye)
    free(mat);
    mat = NULL; // Ne olur ne olmaz pointer'ı boşa çıkartalım

    return 0;
}