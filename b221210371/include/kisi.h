#ifndef KISI_H
#define KISI_H

typedef struct Kisi {
    char* isim;
    int yas;
    double kalan_omur;
    char* bulundugu_arac_adi;
    struct Kisi* sonraki;
} Kisi;


Kisi* kisiOlustur(const char* isim, int yas, double kalan_omur, const char* arac_adi);
void kisiYokEt(Kisi* kisi);
Kisi* kisileriYukle(const char* dosya_adi);
void kisiListesiniYokEt(Kisi* bas);
void kisileriYaslandir(Kisi* bas, double carpan);
void kisiYaslandir(Kisi* kisi, double carpan);
void kisiYazdir(const Kisi* kisi);

#endif