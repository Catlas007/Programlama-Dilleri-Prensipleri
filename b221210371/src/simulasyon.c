#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "simulasyon.h"
#include "gezegen.h"
#include "kisi.h"
#include "uzayaraci.h"

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
    
    printf("--------------- GEZEGENLER ---------------\n");
    GezegenNode* gezegen_node = gezegenler;
    while (gezegen_node) {
        Gezegen* g = gezegen_node->gezegen;
        int nufus = 0;
        
        // Gezegendeki nüfusu hesapla
        Kisi* kisi = kisiler;
        while (kisi) {
            if (kisi->kalan_omur > 0) { // Sadece yaşayan kişileri say
                UzayAraci* arac = araclar;
                while (arac) {
                    if (strcmp(kisi->bulundugu_arac_adi, arac->ad) == 0) {
                        // Kişi bu araçta
                        if ((arac->durum == HEDEFTE && strcmp(arac->varis_gezegeni, g->ad) == 0) ||
                            (arac->durum != YOLDA && arac->durum != HEDEFTE && strcmp(arac->cikis_gezegeni, g->ad) == 0)) {
                            nufus++;
                        }
                        break;
                    }
                    arac = arac->sonraki;
                }
            }
            kisi = kisi->sonraki;
        }
        
        printf("%-5s | Tür: %d | Gün: %d Saat | Tarih: %02d.%02d.%04d %02d:00 | Nüfus: %d\n", 
               g->ad, g->tur, g->gun_saat, 
               g->tarih.gun, g->tarih.ay, g->tarih.yil, g->tarih.saat,
               nufus);
               
        gezegen_node = gezegen_node->sonraki;
    }
    
    printf("\n--------------- UZAY ARAÇLARI ---------------\n");
    UzayAraci* arac = araclar;
    while (arac) {
        printf("%-5s | %s → %s | ", arac->ad, arac->cikis_gezegeni, arac->varis_gezegeni);
        
        if (arac->durum == IMHA) {
            printf("IMHA | Hedefe kalan saat: -- | Hedefe varacağı tarih: --\n");
        } else {
            const char* durumStr = arac->durum == YOLDA ? "YOLDA" : (arac->durum == HEDEFTE ? "HEDEFTE" : "BEKLEMEDE");
            
            if (arac->durum == HEDEFTE) {
                printf("%s | Hedefe kalan saat: 0 | Varış tarihi: %02d.%02d.%04d %02d:00\n",
                       durumStr, arac->varis_tarihi.gun, arac->varis_tarihi.ay, 
                       arac->varis_tarihi.yil, arac->varis_tarihi.saat);
            } else if (arac->durum == YOLDA) {
                Gezegen* varis_gezegeni = gezegenBul(gezegenler, arac->varis_gezegeni);
                if (varis_gezegeni) {
                    // Varış zamanını hesapla
                    Zaman varis_zamani = arac->cikis_tarihi;
                    for (int i = 0; i < arac->mesafe - arac->kalan_saat; i++) {
                        birSaatIlerle(&varis_zamani);
                    }
                    
                    printf("%s | Hedefe kalan saat: %d | Tahmini varış: %02d.%02d.%04d %02d:00\n",
                           durumStr, arac->kalan_saat, varis_zamani.gun, varis_zamani.ay, 
                           varis_zamani.yil, varis_zamani.saat);
                } else {
                    printf("%s | Hedefe kalan saat: %d | Varış gezegeni bulunamadı\n", 
                           durumStr, arac->kalan_saat);
                }
            } else {
                printf("%s | Kalkış bekliyor | Kalkış tarihi: %02d.%02d.%04d %02d:00\n",
                       durumStr, arac->cikis_tarihi.gun, arac->cikis_tarihi.ay, 
                       arac->cikis_tarihi.yil, arac->cikis_tarihi.saat);
            }
        }
        
        arac = arac->sonraki;
    }
    
    printf("\n--------------- YOLCULAR ---------------\n");
    printf("Toplam Yolcu: %d | Yaşayan: %d | Ölü: %d\n", 
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