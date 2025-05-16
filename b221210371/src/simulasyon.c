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

// Function to calculate arrival date based on destination planet's time progression
static void hesaplaVarisTarihiHedefGezegeneGore(UzayAraci* arac, GezegenNode* gezegenler) {
    if (!arac || arac->durum == IMHA) return;
    
    // Find destination planet
    Gezegen* varis_gezegeni = gezegenBul(gezegenler, arac->varis_gezegeni);
    if (!varis_gezegeni) return;
    
    // Start with departure time
    arac->varis_tarihi = arac->cikis_tarihi;
    
    // Calculate arrival time by advancing destination planet time by travel hours
    // Each planet-hour corresponds to different real hours based on planet's day length
    int total_hours_to_add = arac->mesafe;
    
    // Add the travel time in standard hours to the arrival date
    for (int i = 0; i < total_hours_to_add; i++) {
        birSaatIlerle(&arac->varis_tarihi);
    }
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

// Update time on all planets
static void gezegenleriGuncelle(GezegenNode* gezegenler) {
    GezegenNode* current = gezegenler;
    
    while (current) {
        // Advance one hour on each planet
        birSaatIlerle(&current->gezegen->tarih);
        current = current->sonraki;
    }
}

// Display simulation status
static void durumYazdir(Kisi* kisiler, UzayAraci* araclar, GezegenNode* gezegenler) {
    system("cls"); // Clear screen for Windows (use "clear" for Linux)
    
    // PLANETS - aligned table
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
            if (kisi->kalan_omur > 0) {  // Only count living people
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


    // Empty lines
    printf("\n\n\n");

    // SPACECRAFT SECTION
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
            printf("%-12d %02d.%02d.%04d %02d:00\n", a->kalan_saat,
                   a->varis_tarihi.gun, a->varis_tarihi.ay, a->varis_tarihi.yil, a->varis_tarihi.saat);
        } else {
            printf("Bekliyor     %02d.%02d.%04d %02d:00\n",
                   a->cikis_tarihi.gun, a->cikis_tarihi.ay, a->cikis_tarihi.yil, a->cikis_tarihi.saat);
        }

        a = a->sonraki;
    }

    // PASSENGER INFORMATION
    printf("\nToplam Yolcu: %d | Yasayan: %d | Olu: %d\n",
           kisiSayisi(kisiler), yasayanKisiSayisi(kisiler),
           kisiSayisi(kisiler) - yasayanKisiSayisi(kisiler));
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

// Main simulation function
void simulasyonuBaslat(Kisi* kisiler, UzayAraci* araclar, GezegenNode* gezegenler) {
    // Initialize vehicle statuses and calculate proper arrival times
    UzayAraci* arac = araclar;
    while (arac) {
        // Find the planet this vehicle departs from
        Gezegen* cikis_gezegeni = gezegenBul(gezegenler, arac->cikis_gezegeni);
        if (cikis_gezegeni) {
            // If departure date is in the past compared to planet time, set to YOLDA
            if (zamanKiyasla(&arac->cikis_tarihi, &cikis_gezegeni->tarih) <= 0) {
                arac->durum = YOLDA;
                // Calculate arrival time based on destination planet
                hesaplaVarisTarihiHedefGezegeneGore(arac, gezegenler);
                // Set remaining hours
                arac->kalan_saat = arac->mesafe;
            } else {
                // Otherwise it's waiting to depart
                arac->durum = 3; // BEKLEMEDE - waiting to depart
                // Still calculate potential arrival time for display
                hesaplaVarisTarihiHedefGezegeneGore(arac, gezegenler);
            }
        }
        arac = arac->sonraki;
    }
    
    // Simulation hour counter
    long saat_sayaci = 0;
    
    while (!tumAraclarHedefteVeyaImha(araclar)) {
        // Update planet time
        gezegenleriGuncelle(gezegenler);
        
        // Update vehicle statuses
        aracDurumlariniGuncelle(araclar, kisiler, gezegenler);
        
        // Age passengers
        kisileriGuncelleYaslandir(kisiler, araclar, gezegenler);
        
        // Display status
        durumYazdir(kisiler, araclar, gezegenler);
        
        // Increment hour counter
        saat_sayaci++;
    }
    
    // Simulation complete, display final status
    printf("\n\nSimülasyon tamamlandı. Toplam geçen saat: %ld\n", saat_sayaci);
    durumYazdir(kisiler, araclar, gezegenler);
    
    // Free memory
    kisiListesiniYokEt(kisiler);
    uzayAraciListesiniYokEt(araclar);
    gezegenListesiniYokEt(gezegenler);
}