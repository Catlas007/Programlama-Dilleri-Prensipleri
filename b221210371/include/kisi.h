#ifndef KISI_H
#define KISI_H

typedef struct Kisi {
    char* isim;
    int yas;
    double kalan_omur;
    char* bulundugu_arac_adi;
    struct Kisi* sonraki;
} Kisi;

// Bellek işlemleri
Kisi* kisiOlustur(const char* isim, int yas, double kalan_omur, const char* arac_adi);
void kisiYokEt(Kisi* kisi);

// Liste işlemleri
Kisi* kisileriYukle(const char* dosya_adi);
void kisiListesiniYokEt(Kisi* bas);

// Yaşlandırma
void kisileriYaslandir(Kisi* bas, double carpan);

// Yardımcı
void kisiYazdir(const Kisi* kisi);

#endif