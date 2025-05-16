#include <stdio.h>
#include "zaman.h"

int gunSayisi(int ay, int yil) {
    if (ay == 2)
        return ((yil % 4 == 0 && yil % 100 != 0) || (yil % 400 == 0)) ? 29 : 28;
    else if (ay == 4 || ay == 6 || ay == 9 || ay == 11)
        return 30;
    else
        return 31;
}

void birSaatIlerle(Zaman* z) {
    z->saat++;
    if (z->saat >= 24) {
        z->saat = 0;
        z->gun++;
        if (z->gun > gunSayisi(z->ay, z->yil)) {
            z->gun = 1;
            z->ay++;
            if (z->ay > 12) {
                z->ay = 1;
                z->yil++;
            }
        }
    }
}

// Yeni fonksiyon: Gezegen-spesifik zaman güncelleme
void birSaatIlerleGezegen(Zaman* z, int gun_saat) {
    z->saat++;
    if (z->saat >= gun_saat) {  // Gezegenin gün uzunluğunu kullan
        z->saat = 0;
        z->gun++;
        if (z->gun > gunSayisi(z->ay, z->yil)) {
            z->gun = 1;
            z->ay++;
            if (z->ay > 12) {
                z->ay = 1;
                z->yil++;
            }
        }
    }
}

int zamanKiyasla(const Zaman* z1, const Zaman* z2) {
    if (z1->yil != z2->yil)
        return z1->yil - z2->yil;
    if (z1->ay != z2->ay)
        return z1->ay - z2->ay;
    if (z1->gun != z2->gun)
        return z1->gun - z2->gun;
    return z1->saat - z2->saat;
}

void zamanKopyala(Zaman* hedef, const Zaman* kaynak) {
    hedef->yil = kaynak->yil;
    hedef->ay = kaynak->ay;
    hedef->gun = kaynak->gun;
    hedef->saat = kaynak->saat;
}

void zamanYazdir(const Zaman* z) {
    printf("%02d.%02d.%04d - %02d:00\n", z->gun, z->ay, z->yil, z->saat);
}