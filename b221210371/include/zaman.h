#ifndef ZAMAN_H
#define ZAMAN_H

typedef struct {
    int yil;
    int ay;
    int gun;
    int saat;
} Zaman;

int gunSayisi(int ay, int yil);
void birSaatIlerle(Zaman* z);
int zamanKiyasla(const Zaman* z1, const Zaman* z2);
void zamanKopyala(Zaman* hedef, const Zaman* kaynak);
void zamanYazdir(const Zaman* z); // debug için

#endif
