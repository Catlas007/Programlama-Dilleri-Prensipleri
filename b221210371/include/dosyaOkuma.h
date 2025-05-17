#ifndef DOSYAOKUMA_H
#define DOSYAOKUMA_H

#include "kisi.h"
#include "uzayaraci.h"
#include "gezegen.h"

Kisi* KisileriOku(const char* yol);
UzayAraci* UzayAraclariniOku(const char* yol);
GezegenNode* GezegenleriOku(const char* yol);

#endif
