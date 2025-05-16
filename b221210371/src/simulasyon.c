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


// Varış tarihi hesaplama fonksiyonunu düzelt
static void hesaplaVarisTarihiHedefGezegeneGore(UzayAraci* arac, GezegenNode* gezegenler) {
    if (!arac || arac->durum == IMHA) return;
    
    // Hedef gezegeni bul
    Gezegen* varis_gezegeni = gezegenBul(gezegenler, arac->varis_gezegeni);
    if (!varis_gezegeni) return;
    
    // Çıkış gezegenini bul
    Gezegen* cikis_gezegeni = gezegenBul(gezegenler, arac->cikis_gezegeni);
    if (!cikis_gezegeni) return;
    
    // Çıkış tarihinden başla
    arac->varis_tarihi = arac->cikis_tarihi;
    
    // Seyahat süresini hedef gezegenin zaman sistemiyle hesapla
    int kalan_saat = arac->mesafe;
    
    while (kalan_saat > 0) {
        // Her iterasyonda 1 saat geç
        birSaatIlerle(&arac->varis_tarihi);
        kalan_saat--;
    }
    
    // Varış tarihini hedef gezegenin gün uzunluğuna göre ayarla
    // Bu kısım hedef gezegenin tarih sistemine göre düzenleme gerektirebilir
}

// Utility function to get the planet a vehicle is currently on
static Gezegen* aracinBulunduguGezegeniGetir(UzayAraci* arac, GezegenNode* gezegenler) {
    // If vehicle is in transit or destroyed, it's not on any planet
    if (arac->durum == YOLDA || arac->durum == IMHA)
        return NULL;
        
    // If at destination, return destination planet; otherwise return departure planet
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
        return true; // Vehicle destroyed
    }
    
    return false; // Vehicle still active
}

// Update all vehicle statuses
static void aracDurumlariniGuncelle(UzayAraci* araclar, Kisi* kisiler, GezegenNode* gezegenler) {
    UzayAraci* arac = araclar;
    
    while (arac) {
        // Check passengers - destroy vehicle if all are dead
        bool imha_oldu = aracYolculariKontrolEt(arac, kisiler);
        
        // Skip to next vehicle if this one is destroyed
        if (imha_oldu || arac->durum == IMHA) {
            arac = arac->sonraki;
            continue;
        }
        
        // Update vehicle based on current status
        if (arac->durum == YOLDA) {
            // If in transit, normal time progression
            uzayAraclariniGuncelle(arac, NULL);
            
            // After each update, recalculate arrival time based on destination planet
            // This ensures the arrival time accounts for the destination planet's time flow
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
            } else { // Not yet departed
                bulundugu_gezegen = gezegenBul(gezegenler, arac->cikis_gezegeni);
            }
            
            if (bulundugu_gezegen) {
                uzayAraclariniGuncelle(arac, &bulundugu_gezegen->tarih);
                
                // When a vehicle starts its journey, calculate its arrival time 
                // based on the destination planet's time progression
                if (arac->durum == YOLDA) {
                    hesaplaVarisTarihiHedefGezegeneGore(arac, gezegenler);
                }
            }
        }
        
        arac = arac->sonraki;
    }
}

// Age passengers based on their current planet/vehicle
static void kisileriGuncelleYaslandir(Kisi* kisiler, UzayAraci* araclar, GezegenNode* gezegenler) {
    Kisi* kisi = kisiler;
    
    while (kisi) {
        // Skip aging process if person is already dead
        if (kisi->kalan_omur <= 0) {
            kisi = kisi->sonraki;
            continue;
        }
        
        // Find the vehicle the person is on
        UzayAraci* arac = araclar;
        UzayAraci* kisinin_araci = NULL;
        
        while (arac) {
            if (strcmp(arac->ad, kisi->bulundugu_arac_adi) == 0) {
                kisinin_araci = arac;
                break;
            }
            arac = arac->sonraki;
        }
        
        // If vehicle not found or destroyed, person is considered dead
        if (!kisinin_araci || kisinin_araci->durum == IMHA) {
            kisi->kalan_omur = 0; // Person is dead
            kisi = kisi->sonraki;
            continue;
        }
        
        double yaslanma_carpani = 1.0; // Default aging factor
        
        // If vehicle is on a planet (not departed or arrived), use that planet's aging factor
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
        
        // Age the person - FIXED: Always apply at least 1 hour of aging
        double onceki_omur = kisi->kalan_omur;
        kisi->kalan_omur -= yaslanma_carpani;
        
        // Age increase logic: 1 year = 8760 hours (365 days × 24 hours)
        if ((int)(onceki_omur / 8760) > (int)(kisi->kalan_omur / 8760)) {
            kisi->yas++;
        }
        
        kisi = kisi->sonraki;
    }
}


static void gezegenleriGuncelle(GezegenNode* gezegenler) {
    GezegenNode* current = gezegenler;
    
    while (current) {
        Gezegen* g = current->gezegen;
        
        // Gezegenin kendi gün uzunluğunu kullan
        birSaatIlerleGezegen(&g->tarih, g->gun_saat);
        
        current = current->sonraki;
    }
}


static void durumYazdir(Kisi* kisiler, UzayAraci* araclar, GezegenNode* gezegenler) {
    system("cls"); // Windows için (Linux'ta "clear" kullan)
    
    // GEZEGENLER - hizalanmış tablo
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
        
        // Nüfus sayımı: sadece yaşayan kişiler dahil
        while (kisi) {
            if (kisi->kalan_omur > 0) {
                UzayAraci* a = araclar;
                while (a) {
                    if (strcmp(kisi->bulundugu_arac_adi, a->ad) == 0) {
                        // Araç bu gezegendeyse nüfusa dahil et
                        if ((a->durum == HEDEFTE && strcmp(a->varis_gezegeni, g->ad) == 0) ||
                            (a->durum == BEKLEMEDE && strcmp(a->cikis_gezegeni, g->ad) == 0)) {
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
    printf("\n\n\n");

    // UZAY ARAÇLARI
    printf("Uzay Araclari:\n");
    printf("Arac Adi    Durum      Cikis    Varis    Kalan Saat    Varacagi Tarih\n");
    UzayAraci* a = araclar;
    while (a) {
        char* durumStr;
        switch(a->durum) {
            case YOLDA: durumStr = "Yolda"; break;
            case HEDEFTE: durumStr = "Hedefe"; break;
            case IMHA: durumStr = "IMHA"; break;
            default: durumStr = "Bekliyor"; break;
        }

        printf("%-11s %-10s %-8s %-8s ", a->ad, durumStr, a->cikis_gezegeni, a->varis_gezegeni);

        if (a->durum == IMHA) {
            printf("--               --\n");
        } else if (a->durum == HEDEFTE) {
            printf("0               %02d.%02d.%04d\n",
                   a->varis_tarihi.gun, a->varis_tarihi.ay, a->varis_tarihi.yil);
        } else if (a->durum == YOLDA) {
            printf("%-16d%02d.%02d.%04d\n", a->kalan_saat,
                   a->varis_tarihi.gun, a->varis_tarihi.ay, a->varis_tarihi.yil);
        } else {
            // Hesaplanmış varış tarihini göster ama henüz başlamamış
            printf("%-16s%02d.%02d.%04d\n", "Bekliyor",
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

// Check if all vehicles have reached their destinations or been destroyed
static bool tumAraclarHedefteVeyaImha(UzayAraci* araclar) {
    UzayAraci* arac = araclar;
    
    while (arac) {
        if (arac->durum != HEDEFTE && arac->durum != IMHA) {
            return false; // At least one vehicle is still in transit or waiting
        }
        arac = arac->sonraki;
    }
    
    return true; // All vehicles have reached destination or been destroyed
}


void simulasyonuBaslat(Kisi* kisiler, UzayAraci* araclar, GezegenNode* gezegenler) {
    // Başlangıç durumlarını ayarla
    UzayAraci* arac = araclar;
    while (arac) {
        // Çıkış gezegenini bul
        Gezegen* cikis_gezegeni = gezegenBul(gezegenler, arac->cikis_gezegeni);
        if (cikis_gezegeni) {
            // Eğer çıkış tarihi geçmişse YOLDA yap
            if (zamanKiyasla(&arac->cikis_tarihi, &cikis_gezegeni->tarih) <= 0) {
                arac->durum = YOLDA;
                // Kalan saati hesapla
                arac->kalan_saat = arac->mesafe;
            } else {
                arac->durum = BEKLEMEDE;
            }
            // Varış tarihini hesapla (hedef gezegenin zaman sistemi göz önünde bulundurarak)
            hesaplaVarisTarihiHedefGezegeneGore(arac, gezegenler);
        }
        arac = arac->sonraki;
    }
    
    int saat_sayaci = 0;
    
    // Ana simülasyon döngüsü
    while (!tumAraclarHedefteVeyaImha(araclar)) {
        // 1. Gezegen zamanlarını güncelle (her gezegen kendi gün uzunluğuyla)
        gezegenleriGuncelle(gezegenler);
        
        // 2. Araç durumlarını güncelle
        aracDurumlariniGuncelle(araclar, kisiler, gezegenler);
        
        // 3. Kişileri yaşlandır (bulundukları gezegene göre)
        kisileriGuncelleYaslandir(kisiler, araclar, gezegenler);
        
        // 4. Durumu ekrana yazdır
        durumYazdir(kisiler, araclar, gezegenler);
        
        // 5. Simülasyon hızı için kısa bekleme (isteğe bağlı)
        // usleep(100000); // 0.1 saniye - Linux için
        // Sleep(100); // 0.1 saniye - Windows için
        
        saat_sayaci++;
        
        // Güvenlik için maksimum iterasyon sayısı
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