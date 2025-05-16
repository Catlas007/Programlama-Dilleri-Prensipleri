#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "simulasyon.h"
#include "gezegen.h"
#include "kisi.h"
#include "uzayaraci.h"

static int kisiSayisi(Kisi* kisiler);
static int yasayanKisiSayisi(Kisi* kisiler);

// Gezegenler ve araçlar arasındaki ilişkileri yöneten yardımcı fonksiyonlar
static Gezegen* aracinBulunduguGezegeniGetir(UzayAraci* arac, GezegenNode* gezegenler) {
    // Eğer araç yolda ya da imha olduysa gezegeni yoktur
    if (arac->durum == YOLDA || arac->durum == IMHA)
        return NULL;
        
    // Araç hedefteyse varış gezegeni, değilse çıkış gezegeni
    const char* gezegen_adi = (arac->durum == HEDEFTE) ? arac->varis_gezegeni : arac->cikis_gezegeni;
    return gezegenBul(gezegenler, gezegen_adi);
}

// Bir aracın içindeki yolcuları kontrol eder, tümü ölmüşse aracı IMHA olarak işaretler
static bool aracYolculariKontrolEt(UzayAraci* arac, Kisi* kisiler) {
    if (arac->durum == IMHA)
        return false;
        
    bool yasayan_var = false;
    Kisi* kisi = kisiler;
    
    while (kisi) {
        if (strcmp(kisi->bulundugu_arac_adi, arac->ad) == 0 && kisi->kalan_omur > 0) {
            yasayan_var = true;
            break;
        }
        kisi = kisi->sonraki;
    }
    
    if (!yasayan_var) {
        arac->durum = IMHA;
        return true; // Araç imha edildi
    }
    
    return false; // Araç hala aktif
}

// Tüm araçların kontrol edilip, durumlarının güncellenmesi
static void aracDurumlariniGuncelle(UzayAraci* araclar, Kisi* kisiler, GezegenNode* gezegenler) {
    UzayAraci* arac = araclar;
    
    while (arac) {
        // Araçtaki yolcuları kontrol et, hepsi ölmüşse aracı imha et
        aracYolculariKontrolEt(arac, kisiler);
        
        // Araç hangi gezegendeyse o gezegenin zamanına göre güncelle
        if (arac->durum != IMHA) {
            Gezegen* bulundugu_gezegen = NULL;
            
            if (arac->durum == YOLDA) {
                // Araç yoldaysa, bulunduğu gezegen yok, normal zaman akışıyla ilerler
                uzayAraclariniGuncelle(arac, NULL);
            } else {
                if (arac->durum == HEDEFTE) {
                    bulundugu_gezegen = gezegenBul(gezegenler, arac->varis_gezegeni);
                } else { // Henüz hareket etmemiş
                    bulundugu_gezegen = gezegenBul(gezegenler, arac->cikis_gezegeni);
                }
                
                if (bulundugu_gezegen) {
                    uzayAraclariniGuncelle(arac, &bulundugu_gezegen->tarih);
                }
            }
        }
        
        arac = arac->sonraki;
    }
}

// Kişilerin yaşlanması - bulundukları gezegene/araca göre
static void kisileriGuncelleYaslandir(Kisi* kisiler, UzayAraci* araclar, GezegenNode* gezegenler) {
    Kisi* kisi = kisiler;
    
    while (kisi) {
        if (kisi->kalan_omur <= 0) {
            // Kişi zaten ölmüş, yaşlandırma işlemi yapmaya gerek yok
            kisi = kisi->sonraki;
            continue;
        }
        
        // Kişinin bulunduğu aracı bul
        UzayAraci* arac = araclar;
        UzayAraci* kisinin_araci = NULL;
        
        while (arac) {
            if (strcmp(arac->ad, kisi->bulundugu_arac_adi) == 0) {
                kisinin_araci = arac;
                break;
            }
            arac = arac->sonraki;
        }
        
        // Eğer aracı bulunamazsa ya da imha edilmişse işlem yapma
        if (!kisinin_araci || kisinin_araci->durum == IMHA) {
            kisi = kisi->sonraki;
            continue;
        }
        
        double yaslanma_carpani = 1.0; // Varsayılan yaşlanma faktörü
        
        // Eğer araç bir gezegendeyse (hareket etmemiş ya da varmış) o gezegenin yaşlanma faktörünü kullan
        if (kisinin_araci->durum != YOLDA) {
            Gezegen* bulundugu_gezegen = NULL;
            
            if (kisinin_araci->durum == HEDEFTE) {
                bulundugu_gezegen = gezegenBul(gezegenler, kisinin_araci->varis_gezegeni);
            } else {
                bulundugu_gezegen = gezegenBul(gezegenler, kisinin_araci->cikis_gezegeni);
            }
            
            if (bulundugu_gezegen) {
                yaslanma_carpani = bulundugu_gezegen->yaslanma_katsayisi();
            }
        }
        
        // Kişiyi yaşlandır
        kisi->kalan_omur -= yaslanma_carpani;
        
        kisi = kisi->sonraki;
    }
}

// Gezegenlerdeki zamanı günceller
static void gezegenleriGuncelle(GezegenNode* gezegenler) {
    GezegenNode* current = gezegenler;
    
    while (current) {
        // Her gezegende bir saat ilerle
        birSaatIlerle(&current->gezegen->tarih);
        current = current->sonraki;
    }
}

// Simülasyon durumunu ekrana yazdırır
static void durumYazdir(Kisi* kisiler, UzayAraci* araclar, GezegenNode* gezegenler) {
    system("cls"); // Windows için ekranı temizle (Linux için "clear" kullanılabilir)
    
        kisiSayisi(kisiler) - yasayanKisiSayisi(kisiler);

        system("cls"); // veya "clear" (Linux)
    // GEZEGENLER BÖLÜMÜ
    // printf("Gezegenler:   ");
    // GezegenNode* node = gezegenler;
    // while (node) {
    //     printf("%-12s", node->gezegen->ad);
    //     node = node->sonraki;
    // }
    // printf("\n");

    // printf("Tarih:        ");
    // node = gezegenler;
    // while (node) {
    //     Zaman t = node->gezegen->tarih;
    //     printf("%02d.%02d.%04d   ", t.gun, t.ay, t.yil);
    //     node = node->sonraki;
    // }
    // printf("\n");

    // printf("Nufus:        ");
    // node = gezegenler;
    // while (node) {
    //     int nufus = 0;
    //     Gezegen* g = node->gezegen;
    //     Kisi* kisi = kisiler;
    //     while (kisi) {
    //         if (kisi->kalan_omur > 0) {
    //             UzayAraci* a = araclar;
    //             while (a) {
    //                 if (strcmp(kisi->bulundugu_arac_adi, a->ad) == 0) {
    //                     if ((a->durum == HEDEFTE && strcmp(a->varis_gezegeni, g->ad) == 0) ||
    //                         (a->durum != YOLDA && a->durum != HEDEFTE && strcmp(a->cikis_gezegeni, g->ad) == 0)) {
    //                         nufus++;
    //                     }
    //                     break;
    //                 }
    //                 a = a->sonraki;
    //             }
    //         }
    //         kisi = kisi->sonraki;
    //     }
    //     printf("%-12d", nufus);
    //     node = node->sonraki;
    // }

        // GEZEGENLER - hizalı tablo
    printf("%-12s", "Gezegenler:");
    GezegenNode* node = gezegenler;
    while (node) {
        printf("%-15s", node->gezegen->ad);
        node = node->sonraki;
    }
    printf("\n");

    printf("%-12s", "Tarih:");
    node = gezegenler;
    while (node) {
        Zaman t = node->gezegen->tarih;
        printf("%02d.%02d.%04d     ", t.gun, t.ay, t.yil);
        node = node->sonraki;
    }
    printf("\n");

    printf("%-12s", "Nufus:");
    node = gezegenler;
    while (node) {
        int nufus = 0;
        Gezegen* g = node->gezegen;
        Kisi* kisi = kisiler;
        while (kisi) {
            if (kisi->kalan_omur > 0) {
                UzayAraci* a = araclar;
                while (a) {
                    if (strcmp(kisi->bulundugu_arac_adi, a->ad) == 0) {
                        if ((a->durum == HEDEFTE && strcmp(a->varis_gezegeni, g->ad) == 0) ||
                            (a->durum != YOLDA && a->durum != HEDEFTE && strcmp(a->cikis_gezegeni, g->ad) == 0)) {
                            nufus++;
                        }
                        break;
                    }
                    a = a->sonraki;
                }
            }
            kisi = kisi->sonraki;
        }
        printf("%-15d", nufus);
        node = node->sonraki;
    }
    printf("\n");


    // Boş satırlar
    printf("\n\n\n");

    // UZAY ARAÇLARI BÖLÜMÜ
    printf("Uzay Araclari:\n");
    printf("Ad       Durum      Cikis    Varis    Kalan Saat   Hedef Tarih\n");
    UzayAraci* a = araclar;
    while (a) {
        char* durumStr;
        if (a->durum == YOLDA) durumStr = "YOLDA";
        else if (a->durum == HEDEFTE) durumStr = "HEDEFTE";
        else if (a->durum == IMHA) durumStr = "IMHA";
        else durumStr = "BEKLEME";

        printf("%-8s %-10s %-8s %-8s ", a->ad, durumStr, a->cikis_gezegeni, a->varis_gezegeni);

        if (a->durum == IMHA) {
            printf("--           --\n");
        } else if (a->durum == HEDEFTE) {
            printf("0            %02d.%02d.%04d %02d:00\n",
                   a->varis_tarihi.gun, a->varis_tarihi.ay, a->varis_tarihi.yil, a->varis_tarihi.saat);
        } else if (a->durum == YOLDA) {
            Zaman varis_zamani = a->cikis_tarihi;
            for (int i = 0; i < a->mesafe - a->kalan_saat; i++) {
                birSaatIlerle(&varis_zamani);
            }
            printf("%-12d %02d.%02d.%04d %02d:00\n", a->kalan_saat,
                   varis_zamani.gun, varis_zamani.ay, varis_zamani.yil, varis_zamani.saat);
        } else {
            printf("Bekliyor     %02d.%02d.%04d %02d:00\n",
                   a->cikis_tarihi.gun, a->cikis_tarihi.ay, a->cikis_tarihi.yil, a->cikis_tarihi.saat);
        }

        a = a->sonraki;
    }

    // YOLCULAR BİLGİSİ
    printf("\nToplam Yolcu: %d | Yasayan: %d | Olu: %d\n",
           kisiSayisi(kisiler), yasayanKisiSayisi(kisiler),
           kisiSayisi(kisiler) - yasayanKisiSayisi(kisiler));

}

// Yardımcı fonksiyonlar
static int kisiSayisi(Kisi* kisiler) {
    int sayac = 0;
    while (kisiler) {
        sayac++;
        kisiler = kisiler->sonraki;
    }
    return sayac;
}

static int yasayanKisiSayisi(Kisi* kisiler) {
    int sayac = 0;
    while (kisiler) {
        if (kisiler->kalan_omur > 0) {
            sayac++;
        }
        kisiler = kisiler->sonraki;
    }
    return sayac;
}

// Tüm araçların hedeflerine ulaşıp ulaşmadığını kontrol et
static bool tumAraclarHedefteVeyaImha(UzayAraci* araclar) {
    UzayAraci* arac = araclar;
    
    while (arac) {
        if (arac->durum != HEDEFTE && arac->durum != IMHA) {
            return false; // En az bir araç hala yolda veya beklemede
        }
        arac = arac->sonraki;
    }
    
    return true; // Tüm araçlar hedefte veya imha edilmiş
}

// Ana simülasyon fonksiyonu
void simulasyonuBaslat(Kisi* kisiler, UzayAraci* araclar, GezegenNode* gezegenler) {
    // Simulasyon saat sayaci
    long saat_sayaci = 0;
    
    while (!tumAraclarHedefteVeyaImha(araclar)) {
        // Gezegenlerdeki saati ilerlet
        gezegenleriGuncelle(gezegenler);
        
        // Araçların durumlarını güncelle
        aracDurumlariniGuncelle(araclar, kisiler, gezegenler);
        
        // Kişileri yaşlandır
        kisileriGuncelleYaslandir(kisiler, araclar, gezegenler);
        
        // Durumu ekrana yazdır
        durumYazdir(kisiler, araclar, gezegenler);
        
        // Saat sayacını arttır
        saat_sayaci++;
    }
    
    // Simülasyon tamamlandı, son durumu yazdır
    printf("\n\nSimülasyon tamamlandı. Toplam geçen saat: %ld\n", saat_sayaci);
    durumYazdir(kisiler, araclar, gezegenler);
    
    // Belleği temizle
    kisiListesiniYokEt(kisiler);
    uzayAraciListesiniYokEt(araclar);
    gezegenListesiniYokEt(gezegenler);
}