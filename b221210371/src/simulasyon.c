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

// Varış tarihi hesaplama - HEDEF GEZEGENİN ZAMAN DİLİMİNE GÖRE
static void hesaplaVarisTarihiHedefGezegeneGore(UzayAraci* arac, GezegenNode* gezegenler) {
    if (!arac || arac->durum == IMHA) return;
    
    // Varış tarihi çıkış tarihinden başlar
    arac->varis_tarihi = arac->cikis_tarihi;
    
    // Seyahat süresi STANDART SAAT olarak hesaplanır (24 saatlik günler)
    int kalan_saat = arac->mesafe;
    
    while (kalan_saat > 0) {
        // Standart 24 saatlik zaman ilerletme (hedef gezegenin gün uzunluğu değil!)
        birSaatIlerle(&arac->varis_tarihi);
        kalan_saat--;
    }
    
    // NOT: Varış tarihini hedef gezegenin zaman sistemiyle göstermek için
    // hedef gezegenin o andaki tarihiyle senkronize etmek gerekebilir
    // Ama seyahat süresi her zaman standart saatlerle hesaplanmalı
}

// Utility function to get the planet a vehicle is currently on
static Gezegen* aracinBulunduguGezegeniGetir(UzayAraci* arac, GezegenNode* gezegenler) {
    if (arac->durum == YOLDA || arac->durum == IMHA)
        return NULL;
        
    const char* gezegen_adi = (arac->durum == HEDEFTE) ? arac->varis_gezegeni : arac->cikis_gezegeni;
    return gezegenBul(gezegenler, gezegen_adi);
}

// Check passengers on a vehicle - if all are dead, mark vehicle as DESTROYED
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
        return true;
    }
    
    return false;
}

// Update all vehicle statuses
static void aracDurumlariniGuncelle(UzayAraci* araclar, Kisi* kisiler, GezegenNode* gezegenler) {
    UzayAraci* arac = araclar;
    
    while (arac) {
        if (arac->durum == IMHA) {
            arac = arac->sonraki;
            continue;
        }
        
        // Update vehicle based on current status
        if (arac->durum == YOLDA) {
            uzayAraclariniGuncelle(arac, NULL);
            
            if (arac->durum == HEDEFTE) {
                // When arriving, set the arrival time to the destination planet's current time
                Gezegen* varis_gezegeni = gezegenBul(gezegenler, arac->varis_gezegeni);
                if (varis_gezegeni) {
                    arac->varis_tarihi = varis_gezegeni->tarih;
                }
            }
        } else {
            Gezegen* bulundugu_gezegen = NULL;
            
            if (arac->durum == HEDEFTE) {
                bulundugu_gezegen = gezegenBul(gezegenler, arac->varis_gezegeni);
            } else {
                bulundugu_gezegen = gezegenBul(gezegenler, arac->cikis_gezegeni);
            }
            
            if (bulundugu_gezegen) {
                uzayAraclariniGuncelle(arac, &bulundugu_gezegen->tarih);
                
                if (arac->durum == YOLDA) {
                    hesaplaVarisTarihiHedefGezegeneGore(arac, gezegenler);
                }
            }
        }
        
        arac = arac->sonraki;
    }
}

// Age passengers - KRİTİK: Hedefe varan araçlardaki kişiler de yaşlanır
static void kisileriGuncelleYaslandir(Kisi* kisiler, UzayAraci* araclar, GezegenNode* gezegenler) {
    Kisi* kisi = kisiler;
    
    while (kisi) {
        if (kisi->kalan_omur <= 0) {
            kisi = kisi->sonraki;
            continue;
        }
        
        // Find the vehicle the person is on
        UzayAraci* kisinin_araci = NULL;
        UzayAraci* arac = araclar;
        
        while (arac) {
            if (strcmp(arac->ad, kisi->bulundugu_arac_adi) == 0) {
                kisinin_araci = arac;
                break;
            }
            arac = arac->sonraki;
        }
        
        if (!kisinin_araci || kisinin_araci->durum == IMHA) {
            kisi->kalan_omur = 0;
            kisi = kisi->sonraki;
            continue;
        }
        
        double yaslanma_carpani = 1.0; // Default aging (space/transit)
        
        // HEDEFTE araçlar da yaşlanma gerektirir
        if (kisinin_araci->durum == HEDEFTE) {
            Gezegen* hedef_gezegen = gezegenBul(gezegenler, kisinin_araci->varis_gezegeni);
            if (hedef_gezegen) {
                yaslanma_carpani = hedef_gezegen->yaslanma_katsayisi();
            }
        } else if (kisinin_araci->durum == BEKLEMEDE) {
            Gezegen* cikis_gezegen = gezegenBul(gezegenler, kisinin_araci->cikis_gezegeni);
            if (cikis_gezegen) {
                yaslanma_carpani = cikis_gezegen->yaslanma_katsayisi();
            }
        }
        // YOLDA ise normal yaşlanma (1.0)
        
        kisiYaslandir(kisi, yaslanma_carpani);
        kisi = kisi->sonraki;
    }
}

static void gezegenleriGuncelle(GezegenNode* gezegenler) {
    GezegenNode* current = gezegenler;
    
    while (current) {
        Gezegen* g = current->gezegen;
        birSaatIlerleGezegen(&g->tarih, g->gun_saat);
        current = current->sonraki;
    }
}

static void durumYazdir(Kisi* kisiler, UzayAraci* araclar, GezegenNode* gezegenler) {
    system("cls");
    
    // GEZEGENLER
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
        // Sadece yaşayan kişileri say
        if (kisi->kalan_omur > 0) {
            UzayAraci* a = araclar;
            while (a) {
                if (strcmp(kisi->bulundugu_arac_adi, a->ad) == 0) {
                    // İMHA araçlardaki kişiler hiçbir gezegene dahil edilmez
                    if (a->durum != IMHA) {
                        // HEDEFTE olan araçlar hedef gezegene dahil
                        if (a->durum == HEDEFTE && strcmp(a->varis_gezegeni, g->ad) == 0) {
                            nufus++;
                        }
                        // BEKLEMEDE olan araçlar çıkış gezegene dahil
                        else if (a->durum == BEKLEMEDE && strcmp(a->cikis_gezegeni, g->ad) == 0) {
                            nufus++;
                        }
                        // YOLDA olan araçlar hiçbir gezegene dahil değil
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

    printf("\n\n\n");

    // UZAY ARAÇLARI
    printf("Uzay Araclari:\n");
    printf("Arac Adi    Durum      Cikis    Varis    Kalan Saat     Varacagi Tarih\n");
    UzayAraci* a = araclar;
    while (a) {
        char* durumStr;
        switch(a->durum) {
            case YOLDA: durumStr = "Yolda"; break;
            case HEDEFTE: durumStr = "Vardi"; break;
            case IMHA: durumStr = "IMHA"; break;
            default: durumStr = "Bekliyor"; break;
        }

        printf("%-11s %-10s %-8s %-8s ", a->ad, durumStr, a->cikis_gezegeni, a->varis_gezegeni);

        if (a->durum == IMHA) {
            printf("--                --\n");
        } else if (a->durum == HEDEFTE) {
            printf("0                 %02d.%02d.%04d\n",
                   a->varis_tarihi.gun, a->varis_tarihi.ay, a->varis_tarihi.yil);
        } else if (a->durum == YOLDA) {
            printf("%-17d %02d.%02d.%04d\n", a->kalan_saat,
                   a->varis_tarihi.gun, a->varis_tarihi.ay, a->varis_tarihi.yil);
        } else {
            printf("%-17d %02d.%02d.%04d\n", a->mesafe,
                   a->varis_tarihi.gun, a->varis_tarihi.ay, a->varis_tarihi.yil);
        }

        a = a->sonraki;
    }
    printf("\n");
}

// Helper functions
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

static bool tumAraclarHedefteVeyaImha(UzayAraci* araclar) {
    UzayAraci* arac = araclar;
    
    while (arac) {
        if (arac->durum != HEDEFTE && arac->durum != IMHA) {
            return false;
        }
        arac = arac->sonraki;
    }
    
    return true;
}

void simulasyonuBaslat(Kisi* kisiler, UzayAraci* araclar, GezegenNode* gezegenler) {
    UzayAraci* arac = araclar;
    while (arac) {
        Gezegen* cikis_gezegeni = gezegenBul(gezegenler, arac->cikis_gezegeni);
        if (cikis_gezegeni) {
            // Tüm gezegenler aynı tarihte başladığı için kontrol et
            int tarih_karsilastirma = zamanKiyasla(&arac->cikis_tarihi, &cikis_gezegeni->tarih);
            
            if (tarih_karsilastirma <= 0) {
                // Çıkış tarihi geçmiş veya şu an, seyahate başla
                arac->durum = YOLDA;
                arac->kalan_saat = arac->mesafe;
            } else {
                // Henüz çıkış zamanı gelmedi, bekle
                arac->durum = BEKLEMEDE;
                arac->kalan_saat = arac->mesafe;
            }
            
            // Varış tarihini standart zaman ile hesapla
            hesaplaVarisTarihiHedefGezegeneGore(arac, gezegenler);
        }
        arac = arac->sonraki;
    }
    
    int saat_sayaci = 0;
    
    // Ana simülasyon döngüsü
    while (!tumAraclarHedefteVeyaImha(araclar)) {
    // 1. Gezegen zamanlarını güncelle
    gezegenleriGuncelle(gezegenler);
    
    // 2. Araç durumlarını güncelle (iteratif olarak)
    UzayAraci* arac_iter = araclar;
    while (arac_iter) {
        if (arac_iter->durum != IMHA) {
            if (arac_iter->durum == YOLDA) {
                uzayAraclariniGuncelle(arac_iter, NULL);
                
                if (arac_iter->durum == HEDEFTE) {
                    // Hedefe vardığında tarihini hedef gezegenin tarihine ayarla
                    Gezegen* varis_gezegeni = gezegenBul(gezegenler, arac_iter->varis_gezegeni);
                    if (varis_gezegeni) {
                        arac_iter->varis_tarihi = varis_gezegeni->tarih;
                    }
                }
            } else {
                Gezegen* bulundugu_gezegen = NULL;
                
                if (arac_iter->durum == HEDEFTE) {
                    bulundugu_gezegen = gezegenBul(gezegenler, arac_iter->varis_gezegeni);
                } else {
                    bulundugu_gezegen = gezegenBul(gezegenler, arac_iter->cikis_gezegeni);
                }
                
                if (bulundugu_gezegen) {
                    uzayAraclariniGuncelle(arac_iter, &bulundugu_gezegen->tarih);
                    
                    if (arac_iter->durum == YOLDA) {
                        hesaplaVarisTarihiHedefGezegeneGore(arac_iter, gezegenler);
                    }
                }
            }
        }
        arac_iter = arac_iter->sonraki;
    }
    
    // 3. Kişileri yaşlandır
    kisileriGuncelleYaslandir(kisiler, araclar, gezegenler);
    
    // 4. İMHA kontrolü yap
    UzayAraci* arac_check = araclar;
    while (arac_check) {
        aracYolculariKontrolEt(arac_check, kisiler);
        arac_check = arac_check->sonraki;
    }
    
    // 5. Durumu ekrana yazdır
    durumYazdir(kisiler, araclar, gezegenler);
    
    saat_sayaci++;
    
    if (saat_sayaci > 100000) {
        printf("Simülasyon çok uzun sürdü, durduruldu.\n");
        break;
    }
}
    
    // Son durumu göster
    printf("\n\nSimülasyon tamamlandı. Geçen saat: %d\n", saat_sayaci);
    durumYazdir(kisiler, araclar, gezegenler);
    
    // Belleği temizle
    kisiListesiniYokEt(kisiler);
    uzayAraciListesiniYokEt(araclar);
    gezegenListesiniYokEt(gezegenler);
}