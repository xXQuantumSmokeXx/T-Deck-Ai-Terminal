#pragma once
#include <TFT_eSPI.h>

void shtfInit(TFT_eSPI &tft);
bool shtfLoop(TFT_eSPI &tft);
void shtfTrackballUp();
void shtfTrackballDown();
