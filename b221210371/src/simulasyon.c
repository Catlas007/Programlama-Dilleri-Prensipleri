#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h> // system("cls") için
#include "simulasyon.h"
#include "zaman.h"

static void ekranTemizle() {
    system("cls"); // Linux ise: system("clear");
}

static void gezegenZamanlariniIlerle(GezegenNode* gezegenler) {
    while (gezegenler) {
        birSaatIlerle(&gezegenler->gezegen->tarih);
        gezegenler = gezegenler->sonraki;
    }
}

static double kisiIcinYaslanmaCarpani(const Kisi* kisi, GezegenNode* gezegenler, UzayAraci* araclar) {
    if (!kisi->bulundugu_arac_adi || strlen(kisi->bulundugu_arac_adi) == 0)
        return 1.0;

    UzayAraci* arac = araclar;
    while (arac) {
        if (strcmp(arac->ad, kisi->bulundugu_arac_adi) == 0) {
            if (arac->durum == YOLDA) return 1.0;
            if (arac->durum == HEDEFTE) {
                Gezegen* g = gezegenBul(gezegenler, arac->varis_gezegeni);
                return g ? g->yaslanma_katsayisi() : 1.0;
            }
            if (arac->durum == IMHA) return 1.0;
        }
        arac = arac->sonraki;
    }

    return 1.0;
}

static void kisileriYaslandirGuncelle(Kisi* kisiler, GezegenNode* gezegenler, UzayAraci* araclar) {
    Kisi* onceki = NULL;
    Kisi* simdiki = kisiler;

    while (simdiki) {
        double carpan = kisiIcinYaslanmaCarpani(simdiki, gezegenler, araclar);
        simdiki->kalan_omur -= carpan;

        if ((int)simdiki->kalan_omur % (int)(8760 * carpan) == 0)
            simdiki->yas++;

        if (simdiki->kalan_omur <= 0) {
            // Kişi ölmüş → listeden çıkar
            if (onceki) {
                onceki->sonraki = simdiki->sonraki;
            } else {
                kisiler = simdiki->sonraki;
            }
            Kisi* silinecek = simdiki;
            simdiki = simdiki->sonraki;
            kisiYokEt(silinecek);
        } else {
            onceki = simdiki;
            simdiki = simdiki->sonraki;
        }
    }
}

static void uzayAraclariniGuncelle(UzayAraci* araclar, GezegenNode* gezegenler, Kisi* kisiler) {
    UzayAraci* a = araclar;
    while (a) {
        if (a->durum == YOLDA) {
            Gezegen* cikis = gezegenBul(gezegenler, a->cikis_gezegeni);
            if (zamanKiyasla(&cikis->tarih, &a->cikis_tarihi) >= 0) {
                a->kalan_saat--;
                if (a->kalan_saat <= 0) {
                    a->durum = HEDEFTE;
                    Gezegen* varis = gezegenBul(gezegenler, a->varis_gezegeni);
                    Zaman varis_t = varis->tarih;
                    zamanKopyala(&a->varis_tarihi, &varis_t);
                }
            }
        }

        // Araçtaki herkes ölmüş mü?
        int yasayan_var = 0;
        Kisi* k = kisiler;
        while (k) {
            if (k->bulundugu_arac_adi && strcmp(k->bulundugu_arac_adi, a->ad) == 0) {
                yasayan_var = 1;
                break;
            }
            k = k->sonraki;
        }

        if (!yasayan_var && a->durum != HEDEFTE) {
            a->durum = IMHA;
        }

        a = a->sonraki;
    }
}

static void ekraniYazdir(GezegenNode* gezegenler, UzayAraci* araclar) {
    printf("----- GEZEGENLER -----\n");
    GezegenNode* g = gezegenler;
    while (g) {
        printf("%-10s | ", g->gezegen->ad);
        zamanYazdir(&g->gezegen->tarih);
        g = g->sonraki;
    }

    printf("\n\n----- UZAY ARAÇLARI -----\n");
    UzayAraci* a = araclar;
    while (a) {
        printf("%-5s | %s → %s | ", a->ad, a->cikis_gezegeni, a->varis_gezegeni);
        if (a->durum == IMHA)
            printf("IMHA | Kalan: -- | Varış: --\n");
        else {
            const char* durumStr = a->durum == YOLDA ? "YOLDA" : "HEDEFTE";
            printf("%s | Kalan: %d | Varış: ", durumStr, a->kalan_saat);
            zamanYazdir(&a->varis_tarihi);
        }
        a = a->sonraki;
    }
    printf("\n");
}

void simulasyonuBaslat(Kisi* kisiler, UzayAraci* araclar, GezegenNode* gezegenler) {
    while (1) {
        ekranTemizle();
        gezegenZamanlariniIlerle(gezegenler);
        kisileriYaslandirGuncelle(kisiler, gezegenler, araclar);
        uzayAraclariniGuncelle(araclar, gezegenler, kisiler);
        ekraniYazdir(gezegenler, araclar);

        // Simülasyonu durdurma şartı: tüm araçlar hedefte veya imha
        int bitti = 1;
        UzayAraci* a = araclar;
        while (a) {
            if (a->durum == YOLDA) {
                bitti = 0;
                break;
            }
            a = a->sonraki;
        }

        if (bitti) break;
    }

    printf("\nTüm uzay araçları varış noktasına ulaştı veya imha oldu. Simülasyon tamamlandı.\n");
}
