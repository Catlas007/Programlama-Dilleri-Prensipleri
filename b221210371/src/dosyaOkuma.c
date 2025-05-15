#include "dosyaokuma.h"
#include <stdio.h>

Kisi* KisileriOku(const char* yol) {
    return kisileriYukle(yol);
}

UzayAraci* UzayAraclariniOku(const char* yol) {
    return uzayAraclariniYukle(yol);
}

GezegenNode* GezegenleriOku(const char* yol) {
    return gezegenleriYukle(yol);
}
