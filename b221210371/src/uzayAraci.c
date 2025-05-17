#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uzayaraci.h"

UzayAraci* uzayAraciOlustur(const char* ad, const char* c, const char* v, Zaman t, int mesafe) {
    UzayAraci* a = (UzayAraci*)malloc(sizeof(UzayAraci));
    a->ad = strdup(ad);
    a->cikis_gezegeni = strdup(c);
    a->varis_gezegeni = strdup(v);
    a->cikis_tarihi = t;
    a->mesafe = mesafe;
    a->kalan_saat = mesafe;
    a->durum = 3;
    a->varis_tarihi = t;

    for (int i = 0; i < mesafe; i++) {
        birSaatIlerle(&a->varis_tarihi);
    }
    
    a->sonraki = NULL;
    return a;
}

UzayAraci* uzayAraclariniYukle(const char* dosya_adi) {
    FILE* f = fopen(dosya_adi, "r");
    if (!f) {
        printf("Araçlar dosyası açılamadı: %s\n", dosya_adi);
        return NULL;
    }

    UzayAraci* bas = NULL;
    UzayAraci* son = NULL;
    char satir[256];

    while (fgets(satir, sizeof(satir), f)) {
        char* ad = strtok(satir, "#");
        char* c = strtok(NULL, "#");
        char* v = strtok(NULL, "#");
        char* tarihStr = strtok(NULL, "#");
        char* mesafeStr = strtok(NULL, "#\n");

        int gun, ay, yil;
        sscanf(tarihStr, "%d.%d.%d", &gun, &ay, &yil);
        Zaman t = { yil, ay, gun, 0 };

        UzayAraci* yeni = uzayAraciOlustur(ad, c, v, t, atoi(mesafeStr));
        
        if (!bas) bas = son = yeni;
        else { son->sonraki = yeni; son = yeni; }
    }

    fclose(f);
    return bas;
}

void uzayAraciListesiniYokEt(UzayAraci* bas) {
    while (bas) {
        UzayAraci* g = bas;
        bas = bas->sonraki;
        free(g->ad);
        free(g->cikis_gezegeni);
        free(g->varis_gezegeni);
        free(g);
    }
}

void uzayAraclariniGuncelle(UzayAraci* bas, const Zaman* gezegenTarih) {
    if (!bas || bas->durum == IMHA) return;
    
    
    if (bas->durum == BEKLEMEDE && gezegenTarih != NULL) {
        if (zamanKiyasla(&bas->cikis_tarihi, gezegenTarih) <= 0) {
            bas->durum = YOLDA;
            bas->kalan_saat = bas->mesafe; 
        }
    }
    
    
    if (bas->durum == YOLDA) {
        bas->kalan_saat--;
        
        
        if (bas->kalan_saat <= 0) {
            bas->durum = HEDEFTE;
            bas->kalan_saat = 0;
        }
    }
    
}

void uzayAraciYazdir(const UzayAraci* a) {
    if (!a) return;
    
    printf("%-5s | %s → %s | ", a->ad, a->cikis_gezegeni, a->varis_gezegeni);
    
    if (a->durum == IMHA) {
        printf("IMHA | Hedefe kalan saat: -- | Hedefe varacağı tarih: --\n");
    } else {
        const char* durumStr = a->durum == YOLDA ? "YOLDA" : (a->durum == HEDEFTE ? "HEDEFTE" : "BEKLEMEDE");
        printf("%s | Hedefe kalan saat: %d | Hedefe varacağı tarih: ", durumStr, a->kalan_saat);
        zamanYazdir(&a->varis_tarihi);
    }
}