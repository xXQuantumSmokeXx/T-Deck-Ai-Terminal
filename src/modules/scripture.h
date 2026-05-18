#pragma once
#include <TFT_eSPI.h>

void scriptureInit(TFT_eSPI &tft);
bool scriptureLoop(TFT_eSPI &tft);
void scriptureTrackballUp();
void scriptureTrackballDown();
void scriptureTouchTap(uint16_t x, uint16_t y);
bool scriptureWantsHome();
