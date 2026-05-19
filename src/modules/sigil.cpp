#include "sigil.h"
#include "sigil_cards.h"
#include "../ui/theme.h"
#include "../ui/widgets.h"
#include <Arduino.h>
#include <Wire.h>
#include <SD.h>
#include <time.h>

#define KB_ADDR         0x55
#define BOARD_SDCARD_CS 39
#define SIG_Y           (TOPBAR_H + 4)   // content start — skip status bar row
#define TEXT_COLS       38               // chars per wrapped interpretation line
#define MAX_LINES       22               // wrap buffer depth
#define IS_BACK(k)      ((k)=='q'||(k)=='\b'||(k)==0x7F)

// ── State ─────────────────────────────────────────────────────────────────────
typedef enum { SS_MENU, SS_SINGLE, SS_SPREAD } SigilState;

static TFT_eSPI*  s_tft        = nullptr;
static SigilState s_state      = SS_MENU;
static int        s_menuCursor = 0;

static int  s_deck[78];
static int  s_drawn[5];
static bool s_reversed[5];
static int  s_spreadSize = 1;
static int  s_scroll     = 0;
static int  s_activeCard = 0;
static bool s_saved      = false;

static char s_lines[MAX_LINES][TEXT_COLS + 1];
static int  s_lineCount = 0;

static const char* THREE_LBL[3] = { "PAST", "PRESENT", "FUTURE" };
static const char* FIVE_LBL[5]  = { "SIT.", "OBST.", "ADVICE", "FOUND.", "OUTCO." };
static const char* MENU_LBL[3]  = { "Single Card", "Three Card", "Celtic Cross (5)" };

// ── Helpers ───────────────────────────────────────────────────────────────────
static char readKeyboard() {
    char key = 0;
    Wire.requestFrom((uint8_t)KB_ADDR, (uint8_t)1);
    while (Wire.available()) key = Wire.read();
    return key;
}

static void shuffleDeck() {
    for (int i = 0; i < 78; i++) s_deck[i] = i;
    for (int i = 77; i > 0; i--) {
        int j = rand() % (i + 1);
        int t = s_deck[i]; s_deck[i] = s_deck[j]; s_deck[j] = t;
    }
}

static void drawCards(int n) {
    s_spreadSize = n;
    for (int i = 0; i < n; i++) {
        s_drawn[i]    = s_deck[i];
        s_reversed[i] = (rand() % 10) < 3;
    }
    s_activeCard = 0;
    s_scroll     = 0;
    s_saved      = false;
}

static void wrapText(const char* text) {
    s_lineCount = 0;
    if (!text || !*text) return;
    const char* p = text;
    while (*p && s_lineCount < MAX_LINES) {
        while (*p == ' ') p++;
        if (!*p) break;
        int rem = (int)strlen(p);
        if (rem <= TEXT_COLS) {
            strlcpy(s_lines[s_lineCount++], p, TEXT_COLS + 1);
            break;
        }
        int brk = TEXT_COLS;
        while (brk > 0 && p[brk] != ' ') brk--;
        if (brk == 0) brk = TEXT_COLS;
        memcpy(s_lines[s_lineCount], p, brk);
        s_lines[s_lineCount][brk] = '\0';
        s_lineCount++;
        p += brk;
    }
}

static void drawSymbol(int y0) {
    const TarotCard& card = TAROT_CARDS[s_drawn[s_activeCard]];
    char buf[200];
    strlcpy(buf, card.symbol, sizeof(buf));
    s_tft->setTextFont(FONT_MED);
    s_tft->setTextColor(g_themeColor, COL_BG);
    char* line = strtok(buf, "\n");
    int y = y0;
    while (line && y < SCREEN_H - 30) {
        s_tft->drawCentreString(line, SCREEN_W / 2, y, FONT_MED);
        y += 16;
        line = strtok(nullptr, "\n");
    }
}

// ── Draw: menu ────────────────────────────────────────────────────────────────
static void drawMenu() {
    s_tft->fillScreen(COL_BG);
    drawTopbar(*s_tft, ">> ORACLE", "", g_themeColor);

    s_tft->setTextFont(FONT_MED);
    s_tft->setTextColor(COL_WHITE, COL_BG);
    s_tft->drawCentreString("ORACLE READING", SCREEN_W / 2, SIG_Y, FONT_MED);
    s_tft->drawFastHLine(4, SIG_Y + 22, SCREEN_W - 8, g_themeColor);

    for (int i = 0; i < 3; i++) {
        int iy = SIG_Y + 38 + i * 38;
        bool sel = (i == s_menuCursor);
        if (sel) drawCornerBrackets(*s_tft, 8, iy - 4, SCREEN_W - 16, 22, g_themeColor, 4);
        s_tft->setTextFont(FONT_MED);
        s_tft->setTextColor(sel ? COL_WHITE : g_themeColor, COL_BG);
        s_tft->drawCentreString(MENU_LBL[i], SCREEN_W / 2, iy, FONT_MED);
    }

    drawMenuBar(*s_tft, "W/S=move  ENTER=draw  Q=exit");
}

// ── Draw: full card view (single or active spread card) ───────────────────────
static void drawCardFull() {
    s_tft->fillScreen(COL_BG);
    const TarotCard& card = TAROT_CARDS[s_drawn[s_activeCard]];
    bool rev = s_reversed[s_activeCard];

    char top[32];
    snprintf(top, sizeof(top), ">> %s", card.suit);
    drawTopbar(*s_tft, top, "", g_themeColor);

    int y = SIG_Y;

    // Card name (FONT_LARGE, ~26px)
    s_tft->setTextFont(FONT_LARGE);
    s_tft->setTextColor(COL_WHITE, COL_BG);
    s_tft->drawCentreString(card.name, SCREEN_W / 2, y, FONT_LARGE);
    y += 30;

    // Reversed indicator (FONT_SMALL)
    s_tft->setTextFont(FONT_SMALL);
    if (rev) {
        s_tft->setTextColor(COL_AMBER, COL_BG);
        s_tft->drawCentreString("~ reversed ~", SCREEN_W / 2, y, FONT_SMALL);
    }
    y += 12;

    // ASCII symbol (6 lines x 16px = 96px at FONT_MED)
    drawSymbol(y);
    y += 98;

    // Keywords (FONT_SMALL)
    s_tft->setTextFont(FONT_SMALL);
    s_tft->setTextColor(g_themeColor, COL_BG);
    s_tft->drawCentreString(card.keywords, SCREEN_W / 2, y, FONT_SMALL);
    y += 12;

    s_tft->drawFastHLine(4, y, SCREEN_W - 8, g_themeColor);
    y += 5;

    // Scrollable interpretation
    wrapText(rev ? card.reversed : card.upright);
    int visLines = (SCREEN_H - BOTTOMBAR_H - y) / 10;
    s_tft->setTextFont(FONT_SMALL);
    s_tft->setTextColor(COL_WHITE, COL_BG);
    for (int i = 0; i < visLines && s_scroll + i < s_lineCount; i++) {
        s_tft->drawString(s_lines[s_scroll + i], 4, y + i * 10);
    }
    if (s_lineCount > visLines) {
        char ind[8];
        snprintf(ind, sizeof(ind), "%d/%d", s_scroll + 1, s_lineCount);
        s_tft->setTextColor(g_themeColor, COL_BG);
        s_tft->drawString(ind, SCREEN_W - (int)s_tft->textWidth(ind) - 4, y);
    }

    if (s_spreadSize == 1)
        drawMenuBar(*s_tft, s_saved ? "W/S=scroll  [SAVED]  Q=exit" : "W/S=scroll  R=save  Q=exit");
    else
        drawMenuBar(*s_tft, "TBALL=card  W/S=scroll  Q=back");
}

// ── Draw: spread row + active card interpretation ─────────────────────────────
static void drawSpreadView() {
    s_tft->fillScreen(COL_BG);
    drawTopbar(*s_tft, ">> ORACLE", "", g_themeColor);

    const char** lbl = (s_spreadSize == 3) ? THREE_LBL : FIVE_LBL;
    int colW     = SCREEN_W / s_spreadSize;
    bool bigTabs = (s_spreadSize <= 3);
    int labelH   = bigTabs ? 20 : 12;   // FONT_MED row vs FONT_SMALL row
    int tabH     = labelH + 12 + 14;    // label + name + reversed space + gap

    int y = SIG_Y;

    for (int i = 0; i < s_spreadSize; i++) {
        int cx  = i * colW + colW / 2;
        int lx  = i * colW;
        bool act = (i == s_activeCard);

        // Column divider (left edge of each column except the first)
        if (i > 0) s_tft->drawFastVLine(lx, y - 2, tabH + 2, g_themeColor);

        // Position label — FONT_MED for 3-card, FONT_SMALL for 5-card
        uint8_t lblFont = bigTabs ? FONT_MED : FONT_SMALL;
        s_tft->setTextFont(lblFont);
        s_tft->setTextColor(act ? COL_WHITE : g_themeColor, COL_BG);
        s_tft->drawCentreString(lbl[i], cx, y, lblFont);

        // Card name (abbreviated to fit column width)
        int maxChars = colW / 6 - 1;
        if (maxChars < 4) maxChars = 4;
        char abbr[20];
        strlcpy(abbr, TAROT_CARDS[s_drawn[i]].name, sizeof(abbr));
        if ((int)strlen(abbr) > maxChars) { abbr[maxChars - 1] = '.'; abbr[maxChars] = '\0'; }
        s_tft->setTextFont(FONT_SMALL);
        s_tft->setTextColor(act ? COL_WHITE : g_themeColor, COL_BG);
        s_tft->drawCentreString(abbr, cx, y + labelH, FONT_SMALL);

        // Reversed indicator
        if (s_reversed[i]) {
            s_tft->setTextColor(COL_AMBER, COL_BG);
            s_tft->drawCentreString("~r~", cx, y + labelH + 12, FONT_SMALL);
        }

        // Active tab: thick underline
        if (act) {
            int ul = y + tabH - 2;
            s_tft->drawFastHLine(lx + 2, ul,     colW - 4, g_themeColor);
            s_tft->drawFastHLine(lx + 2, ul + 1, colW - 4, g_themeColor);
        }
    }
    y += tabH + 2;

    s_tft->drawFastHLine(4, y, SCREEN_W - 8, g_themeColor);
    y += 5;

    // Active card summary (FONT_MED name + FONT_SMALL keywords)
    const TarotCard& card = TAROT_CARDS[s_drawn[s_activeCard]];
    bool rev = s_reversed[s_activeCard];

    s_tft->setTextFont(FONT_MED);
    s_tft->setTextColor(COL_WHITE, COL_BG);
    int nw = (int)s_tft->textWidth(card.name);
    s_tft->drawString(card.name, 4, y);
    if (rev) {
        s_tft->setTextFont(FONT_SMALL);
        s_tft->setTextColor(COL_AMBER, COL_BG);
        s_tft->drawString(" ~rev~", 4 + nw, y + 4);
    }
    y += 20;

    s_tft->setTextFont(FONT_SMALL);
    s_tft->setTextColor(g_themeColor, COL_BG);
    s_tft->drawString(card.keywords, 4, y);
    y += 12;
    s_tft->drawFastHLine(4, y, SCREEN_W - 8, g_themeColor);
    y += 4;

    // Scrollable interpretation
    wrapText(rev ? card.reversed : card.upright);
    int visLines = (SCREEN_H - BOTTOMBAR_H - y) / 10;
    s_tft->setTextFont(FONT_SMALL);
    s_tft->setTextColor(COL_WHITE, COL_BG);
    for (int i = 0; i < visLines && s_scroll + i < s_lineCount; i++) {
        s_tft->drawString(s_lines[s_scroll + i], 4, y + i * 10);
    }

    const char* hint = s_saved ? "TBALL=card/scroll  [SAVED]  Q=exit"
                                : "TBALL=card/scroll  R=save  Q=exit";
    drawMenuBar(*s_tft, hint);
}

// ── Save reading to SD ────────────────────────────────────────────────────────
static void saveReading() {
    if (!SD.begin(BOARD_SDCARD_CS)) return;
    SD.mkdir("/oracle");
    SD.mkdir("/oracle/readings");

    char fname[64];
    struct tm ti;
    if (getLocalTime(&ti, 0))
        snprintf(fname, sizeof(fname), "/oracle/readings/%04d-%02d-%02d_%02d-%02d.txt",
                 ti.tm_year + 1900, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min);
    else
        strlcpy(fname, "/oracle/readings/reading.txt", sizeof(fname));

    File f = SD.open(fname, FILE_WRITE);
    if (!f) { SD.end(); return; }

    const char*  sname = s_spreadSize == 1 ? "Single Card" :
                         s_spreadSize == 3 ? "Three Card"  : "Celtic Cross";
    const char** lbl   = s_spreadSize == 3 ? THREE_LBL : FIVE_LBL;

    f.println("===============================");
    if (getLocalTime(&ti, 0)) {
        char ds[40];
        snprintf(ds, sizeof(ds), "ORACLE READING -- %04d-%02d-%02d %02d:%02d",
                 ti.tm_year + 1900, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min);
        f.println(ds);
    } else {
        f.println("ORACLE READING");
    }
    f.print("Spread: "); f.println(sname);
    f.println("===============================");
    f.println();

    for (int i = 0; i < s_spreadSize; i++) {
        const TarotCard& c = TAROT_CARDS[s_drawn[i]];
        if (s_spreadSize > 1) { f.print(lbl[i]); f.print(": "); }
        f.print(c.name);
        f.println(s_reversed[i] ? " (Reversed)" : " (Upright)");
        f.print("Keywords: "); f.println(c.keywords);
        f.println(s_reversed[i] ? c.reversed : c.upright);
        f.println();
    }
    f.println("===============================");
    f.close();
    SD.end();
    s_saved = true;
}

// ── Trackball ─────────────────────────────────────────────────────────────────
void sigilTrackballUp() {
    if (!s_tft) return;
    if (s_state == SS_MENU) {
        if (s_menuCursor > 0) { s_menuCursor--; drawMenu(); }
    } else if (s_state == SS_SINGLE) {
        if (s_scroll > 0) { s_scroll--; drawCardFull(); }
    } else if (s_state == SS_SPREAD) {
        if (s_scroll > 0) { s_scroll--; drawSpreadView(); }
    }
}

void sigilTrackballDown() {
    if (!s_tft) return;
    if (s_state == SS_MENU) {
        if (s_menuCursor < 2) { s_menuCursor++; drawMenu(); }
    } else if (s_state == SS_SINGLE) {
        if (s_scroll + 1 < s_lineCount) { s_scroll++; drawCardFull(); }
    } else if (s_state == SS_SPREAD) {
        if (s_scroll + 1 < s_lineCount) { s_scroll++; drawSpreadView(); }
    }
}

void sigilTrackballLeft() {
    if (!s_tft) return;
    if (s_state == SS_MENU) {
        if (s_menuCursor > 0) { s_menuCursor--; drawMenu(); }
    } else if (s_state == SS_SPREAD) {
        if (s_activeCard > 0) { s_activeCard--; s_scroll = 0; drawSpreadView(); }
    }
}

void sigilTrackballRight() {
    if (!s_tft) return;
    if (s_state == SS_MENU) {
        if (s_menuCursor < 2) { s_menuCursor++; drawMenu(); }
    } else if (s_state == SS_SPREAD) {
        if (s_activeCard < s_spreadSize - 1) { s_activeCard++; s_scroll = 0; drawSpreadView(); }
    }
}

// ── Public API ────────────────────────────────────────────────────────────────
void sigilInit(TFT_eSPI& tft) {
    s_tft        = &tft;
    s_state      = SS_MENU;
    s_menuCursor = 0;
    s_scroll     = 0;
    s_saved      = false;
    srand((unsigned int)time(nullptr));
    shuffleDeck();
    drawMenu();
}

bool sigilLoop(TFT_eSPI& tft) {
    (void)tft;
    char key = readKeyboard();
    if (!key) return true;

    switch (s_state) {
    case SS_MENU:
        if      (key == 'w' || key == 'i') { if (s_menuCursor > 0) { s_menuCursor--; drawMenu(); } }
        else if (key == 's' || key == 'k') { if (s_menuCursor < 2) { s_menuCursor++; drawMenu(); } }
        else if (key == '\r' || key == '\n') {
            static const int sizes[3] = { 1, 3, 5 };
            drawCards(sizes[s_menuCursor]);
            if (s_spreadSize == 1) { s_state = SS_SINGLE; drawCardFull(); }
            else                   { s_state = SS_SPREAD; drawSpreadView(); }
        }
        else if (IS_BACK(key)) return false;
        break;

    case SS_SINGLE:
        if      (key == 'w' || key == 'i') { if (s_scroll > 0)                       { s_scroll--; drawCardFull(); } }
        else if (key == 's' || key == 'k') { if (s_scroll + 1 < s_lineCount)          { s_scroll++; drawCardFull(); } }
        else if (key == 'r')               { saveReading(); drawCardFull(); }
        else if (IS_BACK(key))             { s_state = SS_MENU; s_scroll = 0; drawMenu(); }
        break;

    case SS_SPREAD:
        if      (key == 'w' || key == 'i') { if (s_scroll > 0)              { s_scroll--; drawSpreadView(); } }
        else if (key == 's' || key == 'k') { if (s_scroll + 1 < s_lineCount) { s_scroll++; drawSpreadView(); } }
        else if (key == 'r')               { saveReading(); drawSpreadView(); }
        else if (IS_BACK(key))             { s_state = SS_MENU; s_scroll = 0; drawMenu(); }
        break;
    }
    return true;
}

// ── Touch tap handler ─────────────────────────────────────────────────────────
void sigilTouchTap(uint16_t x, uint16_t y) {
    if (!s_tft) return;

    // Bottom bar → back to menu
    if (y >= (uint16_t)(SCREEN_H - BOTTOMBAR_H)) {
        if (s_state != SS_MENU) { s_state = SS_MENU; s_scroll = 0; drawMenu(); }
        return;
    }

    switch (s_state) {
    case SS_MENU: {
        // Tap a menu item row to launch it
        for (int i = 0; i < 3; i++) {
            int iy = SIG_Y + 38 + i * 38;
            if (y >= (uint16_t)(iy - 6) && y < (uint16_t)(iy + 20)) {
                s_menuCursor = i;
                static const int sizes[3] = { 1, 3, 5 };
                drawCards(sizes[i]);
                if (s_spreadSize == 1) { s_state = SS_SINGLE; drawCardFull(); }
                else                   { s_state = SS_SPREAD; drawSpreadView(); }
                return;
            }
        }
        break;
    }
    case SS_SINGLE:
        // Left half = scroll up, right half = scroll down
        if (x < (uint16_t)(SCREEN_W / 2)) {
            if (s_scroll > 0) { s_scroll--; drawCardFull(); }
        } else {
            if (s_scroll + 1 < s_lineCount) { s_scroll++; drawCardFull(); }
        }
        break;
    case SS_SPREAD: {
        // Tap a specific tab column → jump directly to that card
        bool bigTabs = (s_spreadSize <= 3);
        int  labelH  = bigTabs ? 20 : 12;
        int  tabH    = labelH + 12 + 14;
        int  col     = (int)x / (SCREEN_W / s_spreadSize);
        if (col >= s_spreadSize) col = s_spreadSize - 1;

        if (y >= (uint16_t)(SIG_Y - 2) && y < (uint16_t)(SIG_Y + tabH + 2)) {
            // Tap in tab bar → jump to that position
            if (col != s_activeCard) { s_activeCard = col; s_scroll = 0; drawSpreadView(); }
        } else {
            // Tap in content area: left/right edges change card, centre scrolls
            if (x < (uint16_t)(SCREEN_W / 3)) {
                if (s_activeCard > 0) { s_activeCard--; s_scroll = 0; drawSpreadView(); }
            } else if (x > (uint16_t)(SCREEN_W * 2 / 3)) {
                if (s_activeCard < s_spreadSize - 1) { s_activeCard++; s_scroll = 0; drawSpreadView(); }
            } else {
                if (s_scroll + 1 < s_lineCount) { s_scroll++; drawSpreadView(); }
            }
        }
        break;
    }
    }
}
