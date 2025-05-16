#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kisi.h"

Kisi* kisiOlustur(const char* isim, int yas, double kalan_omur, const char* arac_adi) {
    Kisi* yeni = (Kisi*)malloc(sizeof(Kisi));
    yeni->isim = strdup(isim);
    yeni->yas = yas;
    yeni->kalan_omur = kalan_omur;
    yeni->bulundugu_arac_adi = strdup(arac_adi);
    yeni->sonraki = NULL;
    return yeni;
}

void kisiYokEt(Kisi* kisi) {
    if (kisi) {
        free(kisi->isim);
        free(kisi->bulundugu_arac_adi);
        free(kisi);
    }
}

Kisi* kisileriYukle(const char* dosya_adi) {
    FILE* f = fopen(dosya_adi, "r");
    if (!f) {
        printf("Kişiler dosyası açılamadı: %s\n", dosya_adi);
        return NULL;
    }

    Kisi* bas = NULL;
    Kisi* son = NULL;
    char satir[256];

    while (fgets(satir, sizeof(satir), f)) {
        char* isim = strtok(satir, "#");
        char* yasStr = strtok(NULL, "#");
        char* omurStr = strtok(NULL, "#");
        char* arac = strtok(NULL, "#\n");

        if (isim && yasStr && omurStr && arac) {
            Kisi* yeni = kisiOlustur(isim, atoi(yasStr), atof(omurStr), arac);
            if (!bas)
                bas = son = yeni;
            else {
                son->sonraki = yeni;
                son = yeni;
            }
        }
    }

    fclose(f);
    return bas;
}

void kisiListesiniYokEt(Kisi* bas) {
    while (bas) {
        Kisi* gecici = bas;
        bas = bas->sonraki;
        kisiYokEt(gecici);
    }
}

// void kisileriYaslandir(Kisi* bas, double carpan) {
//     Kisi* tmp = bas;
//     while (tmp) {
//         if (tmp->kalan_omur > 0) {
//             double onceki_omur = tmp->kalan_omur;
            
//             // Yaşlanma çarpanı: düşük çarpan = daha yavaş yaşlanma
//             // Çarpan 0.1 ise 10 kat daha az yaşlanır
//             tmp->kalan_omur -= carpan;
            
//             // 1 yıl = 8760 saat (365 gün × 24 saat)
//             if ((int)(onceki_omur / 8760) > (int)(tmp->kalan_omur / 8760)) {
//                 tmp->yas++;
//             }
            
//             // Ömür sıfır veya altına düştüyse ölmüş sayılır
//             if (tmp->kalan_omur <= 0) {
//                 tmp->kalan_omur = 0;
//             }
//         }
//         tmp = tmp->sonraki;
//     }
// }

void kisileriYaslandir(Kisi* bas, double carpan) {
    Kisi* tmp = bas;
    while (tmp) {
        if (tmp->kalan_omur > 0) {
            double onceki_omur = tmp->kalan_omur;
            
            // Yaşlanma çarpanı doğru şekilde uygulanıyor
            // Düşük çarpan = daha yavaş yaşlanma
            tmp->kalan_omur -= carpan;
            
            // 1 yıl = 8760 saat (365 gün × 24 saat)
            if ((int)(onceki_omur / 8760) > (int)(tmp->kalan_omur / 8760)) {
                tmp->yas++;
            }
            
            // Ömür sıfır veya altına düştüyse ölmüş sayılır
            if (tmp->kalan_omur <= 0) {
                tmp->kalan_omur = 0;
            }
        }
        tmp = tmp->sonraki;
    }
}

void kisiYazdir(const Kisi* kisi) {
    if (!kisi) return;
    
    printf("%s | Yaş: %d | Kalan Ömür: %.2f saat | Araç: %s", 
           kisi->isim, kisi->yas, kisi->kalan_omur,
           kisi->bulundugu_arac_adi ? kisi->bulundugu_arac_adi : "YOK");
           
    if (kisi->kalan_omur <= 0) {
        printf(" | ÖLMÜŞ");
    }
    
    printf("\n");
}