

#ifndef ZAMAN_H
#define ZAMAN_H

typedef struct {
    int yil;
    int ay;
    int gun;
    int saat;
} Zaman;

// Fonksiyon bildirimleri - Zaman struct'ından sonra
int gunSayisi(int ay, int yil);
void birSaatIlerle(Zaman* z);
void birSaatIlerleGezegen(Zaman* z, int gun_saat); // Yeni eklenen fonksiyon
int zamanKiyasla(const Zaman* z1, const Zaman* z2);
void zamanKopyala(Zaman* hedef, const Zaman* kaynak);
void zamanYazdir(const Zaman* z); // debug için

#endif