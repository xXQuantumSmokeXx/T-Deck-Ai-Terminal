#pragma once
#include <TFT_eSPI.h>

void sigilInit(TFT_eSPI &tft);
bool sigilLoop(TFT_eSPI &tft);
void sigilTrackballUp();
void sigilTrackballDown();
void sigilTrackballLeft();
void sigilTrackballRight();
void sigilTouchTap(uint16_t x, uint16_t y);
