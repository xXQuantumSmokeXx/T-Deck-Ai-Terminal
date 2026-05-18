#pragma once
#include <TFT_eSPI.h>

void noaaInit(TFT_eSPI &tft);
bool noaaLoop(TFT_eSPI &tft);
void noaaTrackballUp();
void noaaTrackballDown();
void noaaTouchTap(uint16_t x, uint16_t y);