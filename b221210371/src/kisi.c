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
    if (!f) return NULL;

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

void kisileriYaslandir(Kisi* bas, double carpan) {
    Kisi* tmp = bas;
    while (tmp) {
        tmp->kalan_omur -= 1.0 * carpan;
        if ((int)tmp->kalan_omur % (int)(8760 * carpan) == 0)
            tmp->yas++;
        tmp = tmp->sonraki;
    }
}

void kisiYazdir(const Kisi* kisi) {
    printf("İsim: %s | Yaş: %d | Kalan Ömür: %.2f | Uzay Aracı: %s\n",
           kisi->isim, kisi->yas, kisi->kalan_omur,
           kisi->bulundugu_arac_adi ? kisi->bulundugu_arac_adi : "YOK");
}
