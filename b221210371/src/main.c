#include "dosyaokuma.h"
#include "simulasyon.h"

int main() {
    Kisi* kisiler = KisileriOku("veri/Kisiler.txt");
    UzayAraci* araclar = UzayAraclariniOku("veri/Araclar.txt");
    GezegenNode* gezegenler = GezegenleriOku("veri/Gezegenler.txt");

    simulasyonuBaslat(kisiler, araclar, gezegenler);
    return 0;
}
