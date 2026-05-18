#include "waze.h"
#include "../ui/theme.h"
#include "../ui/widgets.h"
#include "../config/nvs_config.h"
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TinyGPS++.h>
#include <time.h>

// ── Constants ─────────────────────────────────────────────────────────────────
#define KB_ADDR         0x55
#define GPS_RX_PIN      44          // L76K TXD → ESP32 GPIO44 (Serial1 RX)
#define GPS_TX_PIN      43          // L76K RXD ← ESP32 GPIO43 (Serial1 TX)
#define GPS_BAUD        9600
#define GPS_TIMEOUT_MS  60000       // 60 s cold-start; any keypress skips
#define WAZE_RADIUS     "2"
#define WAZE_RADIUS_UNIT "KM"
#define WAZE_MAX_ITEMS  25
#define WAZE_VISIBLE    10          // rows visible at once  (CONTENT_H/WAZE_ROW_H)
#define WAZE_ROW_H      18
#define WAZE_CACHE_TTL  900         // 15 minutes

// ── Mode ──────────────────────────────────────────────────────────────────────
enum WazeMode { WAZE_HAZARD, WAZE_POLICE, WAZE_ROAD };

static WazeMode s_mode = WAZE_HAZARD;

// Per-mode config helpers
static const char* modeTitle() {
    switch (s_mode) {
        case WAZE_POLICE: return "< HOME | POLICE";
        case WAZE_ROAD:   return "< HOME | ROAD";
        default:          return "< HOME | HAZARDS";
    }
}
static const char* modeAlertTypes() {
    switch (s_mode) {
        case WAZE_POLICE: return "POLICE";
        case WAZE_ROAD:   return "ROAD_CLOSED,JAM";
        default:          return "ACCIDENT,HAZARD";
    }
}
static int modeMaxAlerts() { return s_mode == WAZE_ROAD ? 10 : 20; }
static int modeMaxJams()    { return s_mode == WAZE_ROAD ? 10 : 0;  }
static const char* modeClearMsg() {
    switch (s_mode) {
        case WAZE_POLICE: return "No police reported nearby";
        case WAZE_ROAD:   return "Roads are open";
        default:          return "No hazards nearby";
    }
}
static const char* modeCachePrefix() {
    switch (s_mode) {
        case WAZE_POLICE: return "waze_p";
        case WAZE_ROAD:   return "waze_r";
        default:          return "waze_h";
    }
}
static uint16_t colorFromBadge(const char *badge) {
    if (strcmp(badge, "[POL]") == 0) return COL_AMBER;
    if (strcmp(badge, "[ACC]") == 0) return COL_RED;
    if (strcmp(badge, "[RD!]") == 0) return COL_RED;
    if (strcmp(badge, "[JAM]") == 0) return COL_WHITE;
    return COL_GOLD;
}

// ── State ─────────────────────────────────────────────────────────────────────
struct WazeItem {
    char     badge[8];   // "[POL]", "[HAZ]", "[ACC]", "[JAM]", "[RD!]"
    char     detail[52];
    uint16_t color;
};

static TFT_eSPI   *s_tft        = nullptr;
static WazeItem    s_items[WAZE_MAX_ITEMS];
static int         s_itemCount  = 0;
static int         s_scrollOff  = 0;
static float       s_lat        = 0.0f;
static float       s_lon        = 0.0f;
static bool        s_gpsValid   = false;
static bool        s_gpsCached  = false;
static bool        s_dataCached = false;
static char        s_errMsg[52] = "";
static TinyGPSPlus s_gps;

// ── Helpers ───────────────────────────────────────────────────────────────────
static char readKeyboard() {
    char key = 0;
    Wire.requestFrom((uint8_t)KB_ADDR, (uint8_t)1);
    while (Wire.available()) key = Wire.read();
    return key;
}

// Strips leading type prefix ("HAZARD_", "JAM_", etc.) and replaces _ with space.
static String formatSubtype(const char *sub) {
    if (!sub || !*sub || strcmp(sub, "NO_SUBTYPE") == 0) return "";
    String s = sub;
    if      (s.startsWith("HAZARD_"))       s = s.substring(7);
    else if (s.startsWith("JAM_"))          s = s.substring(4);
    else if (s.startsWith("POLICE_"))       s = s.substring(7);
    else if (s.startsWith("ACCIDENT_"))     s = s.substring(9);
    else if (s.startsWith("ROAD_CLOSED_")) s = s.substring(12);
    s.replace("_", " ");
    return s;
}

static uint16_t typeColor(const char *type) {
    if (strcmp(type, "POLICE") == 0)      return COL_AMBER;
    if (strcmp(type, "ACCIDENT") == 0)    return COL_RED;
    if (strcmp(type, "ROAD_CLOSED") == 0) return COL_RED;
    if (strcmp(type, "JAM") == 0)         return COL_WHITE;
    return COL_GOLD;  // HAZARD
}

static const char* typeBadge(const char *type) {
    if (strcmp(type, "POLICE") == 0)      return "[POL]";
    if (strcmp(type, "ACCIDENT") == 0)    return "[ACC]";
    if (strcmp(type, "HAZARD") == 0)      return "[HAZ]";
    if (strcmp(type, "JAM") == 0)         return "[JAM]";
    if (strcmp(type, "ROAD_CLOSED") == 0) return "[RD!]";
    return "[???]";
}

// ── GPS — hardware acquisition (Serial1, GPIO44/43, L76K GNSS) ───────────────
static bool tryHardwareGPS() {
    s_tft->fillRect(0, CONTENT_Y + 30, SCREEN_W, SCREEN_H - CONTENT_Y - 30, COL_BG);
    s_tft->setTextFont(FONT_SMALL);
    s_tft->setTextColor(COL_GREY_MID, COL_BG);
    s_tft->drawCentreString("Starting GPS (L76K)...", SCREEN_W / 2, CONTENT_Y + 34, FONT_SMALL);

    // BOARD_POWERON (GPIO10) is already HIGH from setup() — GPS module is powered.
    Serial1.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    delay(100);

    // L76K init: stop output, configure dual constellation, re-enable NMEA
    Serial1.println("$PCAS03,0,0,0,0,0,0,0,0,0,0,,,0,0*02");  delay(100);
    Serial1.println("$PCAS04,5*1C");                             delay(100);
    Serial1.println("$PCAS03,1,1,1,1,1,1,1,1,1,1,,,0,0*02");  delay(100);
    Serial1.println("$PCAS11,3*1E");                             delay(100);

    uint32_t start = millis(), lastDraw = 0;

    while (millis() - start < GPS_TIMEOUT_MS) {
        while (Serial1.available()) {
            if (s_gps.encode(Serial1.read()) && s_gps.location.isValid()) {
                s_lat = (float)s_gps.location.lat();
                s_lon = (float)s_gps.location.lng();
                Serial1.end();
                nvsPutString("waze_lat", String(s_lat, 5));
                nvsPutString("waze_lon", String(s_lon, 5));
                s_gpsValid = true; s_gpsCached = false;
                return true;
            }
        }
        uint32_t now = millis();
        if (now - lastDraw >= 500) {
            lastDraw = now;
            int rem = (int)((GPS_TIMEOUT_MS - (now - start)) / 1000);
            s_tft->fillRect(0, CONTENT_Y + 36, SCREEN_W, 14, COL_BG);
            char buf[52];
            snprintf(buf, sizeof(buf), "Acquiring... %ds  (any key = skip)", rem);
            s_tft->setTextColor(COL_GREY_MID, COL_BG);
            s_tft->drawCentreString(buf, SCREEN_W / 2, CONTENT_Y + 38, FONT_SMALL);
        }
        if (readKeyboard() != 0) break;
        delay(50);
    }
    Serial1.end();
    return false;
}

// ── GPS — NVS cache check (used on init) ─────────────────────────────────────
static bool loadCachedGPS() {
    String latStr = nvsGetString("waze_lat");
    String lonStr = nvsGetString("waze_lon");
    if (latStr.length() == 0 || lonStr.length() == 0) return false;
    float la = latStr.toFloat();
    float lo = lonStr.toFloat();
    if (la == 0.0f && lo == 0.0f) return false;
    s_lat = la; s_lon = lo;
    s_gpsValid = true; s_gpsCached = true;
    return true;
}


// ── JSON parse ────────────────────────────────────────────────────────────────
static void parseWazeDoc(JsonDocument &doc) {
    s_itemCount = 0;

    // Alerts array
    JsonArray alerts = doc["data"]["alerts"].as<JsonArray>();
    for (JsonObject a : alerts) {
        if (s_itemCount >= WAZE_MAX_ITEMS) break;
        WazeItem &item = s_items[s_itemCount++];

        const char *type = a["type"] | "UNK";
        const char *sub  = a["subtype"] | "";
        const char *desc = a["description"] | "";

        strlcpy(item.badge, typeBadge(type), sizeof(item.badge));
        item.color = typeColor(type);

        // Prefer description when it looks useful; else format the subtype
        if (desc && strlen(desc) > 3 && strcmp(desc, "null") != 0) {
            strlcpy(item.detail, desc, sizeof(item.detail));
        } else {
            String fs = formatSubtype(sub);
            if (fs.length() == 0) fs = type;
            strlcpy(item.detail, fs.c_str(), sizeof(item.detail));
        }
    }

    // Jams array (only used by WAZE_ROAD mode, but we parse either way)
    JsonArray jams = doc["data"]["jams"].as<JsonArray>();
    for (JsonObject j : jams) {
        if (s_itemCount >= WAZE_MAX_ITEMS) break;
        WazeItem &item = s_items[s_itemCount++];

        strlcpy(item.badge, "[JAM]", sizeof(item.badge));
        item.color = COL_WHITE;

        float speed  = j["speed_kmh"] | 0.0f;
        int   delay  = j["delay_seconds"] | 0;
        float length = j["length"] | 0.0f;  // metres

        if (delay > 60) {
            snprintf(item.detail, sizeof(item.detail), "+%dmin delay  %.0f kmh", delay / 60, speed);
        } else if (length > 0.0f) {
            snprintf(item.detail, sizeof(item.detail), "%.1f km long  %.0f kmh", length / 1000.0f, speed);
        } else {
            snprintf(item.detail, sizeof(item.detail), "%.0f kmh", speed);
        }
    }
}

// ── HTTP fetch ────────────────────────────────────────────────────────────────
static bool fetchWaze() {
    String key = nvsGetString("waze_key");
    if (key.isEmpty()) {
        strlcpy(s_errMsg, "NO API KEY - put key in waze.txt on SD", sizeof(s_errMsg));
        return false;
    }
    if (!s_gpsValid) {
        strlcpy(s_errMsg, "NO LOCATION - press L to enter coords", sizeof(s_errMsg));
        return false;
    }

    char url[320];
    snprintf(url, sizeof(url),
        "https://api.openwebninja.com/waze/alerts-and-jams"
        "?center=%.5f,%.5f&radius=%s&radius_units=%s"
        "&alert_types=%s&max_alerts=%d&max_jams=%d",
        s_lat, s_lon, WAZE_RADIUS, WAZE_RADIUS_UNIT,
        modeAlertTypes(), modeMaxAlerts(), modeMaxJams());

    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;
    http.begin(client, url);
    http.addHeader("x-api-key", key);
    http.setTimeout(12000);

    int code = http.GET();
    if (code != 200) {
        snprintf(s_errMsg, sizeof(s_errMsg), "API error %d", code);
        http.end();
        return false;
    }

    String json = http.getString();
    http.end();

    JsonDocument doc;
    if (deserializeJson(doc, json)) {
        strlcpy(s_errMsg, "JSON parse error", sizeof(s_errMsg));
        return false;
    }

    strlcpy(s_errMsg, "", sizeof(s_errMsg));
    parseWazeDoc(doc);
    return true;
}

// ── Blocking line input (for manual lat/lon) ──────────────────────────────────
static String wReadLine(const char *prompt) {
    String buf;
    s_tft->fillScreen(COL_BG);
    s_tft->setTextFont(FONT_SMALL);
    s_tft->setTextColor(g_themeColor, COL_BG);
    s_tft->drawString(prompt, 4, 12);
    auto redraw = [&]() {
        s_tft->fillRect(0, 28, SCREEN_W, 14, COL_BG);
        s_tft->setTextColor(COL_WHITE, COL_BG);
        s_tft->drawString(buf + "_", 4, 30);
    };
    redraw();
    while (true) {
        char k = readKeyboard();
        if (k == 0) { delay(20); continue; }
        if (k == '\r' || k == '\n') break;
        if ((k == 8 || k == 127) && buf.length() > 0) buf.remove(buf.length() - 1);
        else if (isprint((unsigned char)k) && buf.length() < 40) buf += k;
        redraw();
        delay(20);
    }
    return buf;
}

static void doSetLocation() {
    String latStr = wReadLine("Latitude  (e.g. 40.7128):");
    latStr.trim();
    if (latStr.isEmpty()) return;
    String lonStr = wReadLine("Longitude (e.g. -74.0060):");
    lonStr.trim();
    if (lonStr.isEmpty()) return;
    s_lat = latStr.toFloat();
    s_lon = lonStr.toFloat();
    s_gpsValid  = true;
    s_gpsCached = false;
    nvsPutString("waze_lat", latStr);
    nvsPutString("waze_lon", lonStr);
}

// ── Screen draw ───────────────────────────────────────────────────────────────
static void drawWazeScreen() {
    s_tft->fillScreen(COL_BG);

    // Topbar
    char timeBuf[12] = "";
    struct tm ti;
    if (getLocalTime(&ti, 0)) {
        int h = ti.tm_hour;
        const char *ap = h >= 12 ? "PM" : "AM";
        if (h > 12) h -= 12; else if (h == 0) h = 12;
        snprintf(timeBuf, sizeof(timeBuf), "%d:%02d %s", h, ti.tm_min, ap);
    }
    drawTopbar(*s_tft, modeTitle(), timeBuf, g_themeColor);

    // Status bar — GPS coords left, count right
    s_tft->fillRect(0, TOPBAR_H, SCREEN_W, STATUSBAR_H, COL_BG);
    s_tft->setTextFont(FONT_SMALL);
    if (s_gpsValid) {
        char gpsBuf[40];
        snprintf(gpsBuf, sizeof(gpsBuf), "%.4f, %.4f%s",
                 s_lat, s_lon, s_gpsCached ? " (cached)" : " (GPS)");
        s_tft->setTextColor(s_gpsCached ? COL_GREY_MID : g_themeColor, COL_BG);
        s_tft->drawString(gpsBuf, 4, TOPBAR_H + 3);
    } else {
        s_tft->setTextColor(COL_AMBER, COL_BG);
        s_tft->drawString("NO LOCATION", 4, TOPBAR_H + 3);
    }
    if (s_itemCount > 0) {
        char cntBuf[16];
        if (s_dataCached)
            snprintf(cntBuf, sizeof(cntBuf), "~%d FOUND", s_itemCount);
        else
            snprintf(cntBuf, sizeof(cntBuf), "%d FOUND", s_itemCount);
        int cw = s_tft->textWidth(cntBuf);
        s_tft->setTextColor(s_dataCached ? COL_AMBER : COL_RED, COL_BG);
        s_tft->drawString(cntBuf, SCREEN_W - cw - 4, TOPBAR_H + 3);
    }
    s_tft->drawFastHLine(0, TOPBAR_H + STATUSBAR_H - 1, SCREEN_W, g_themeColor);

    int y = CONTENT_Y + 2;

    // ── Empty / error states ──────────────────────────────────────────────────
    if (s_itemCount == 0) {
        s_tft->setTextFont(FONT_MED);
        if (strlen(s_errMsg) > 0) {
            s_tft->setTextColor(COL_AMBER, COL_BG);
            s_tft->drawCentreString("ERROR", SCREEN_W / 2, y + 18, FONT_MED);
            s_tft->setTextFont(FONT_SMALL);
            s_tft->setTextColor(COL_GREY_MID, COL_BG);
            s_tft->drawCentreString(s_errMsg, SCREEN_W / 2, y + 40, FONT_SMALL);
        } else if (s_gpsValid) {
            s_tft->setTextColor(g_themeColor, COL_BG);
            s_tft->drawCentreString("ALL CLEAR", SCREEN_W / 2, y + 22, FONT_MED);
            s_tft->setTextFont(FONT_SMALL);
            s_tft->setTextColor(COL_GREY_MID, COL_BG);
            s_tft->drawCentreString(modeClearMsg(), SCREEN_W / 2, y + 44, FONT_SMALL);
        } else {
            s_tft->setTextColor(COL_AMBER, COL_BG);
            s_tft->drawCentreString("NO LOCATION", SCREEN_W / 2, y + 22, FONT_MED);
            s_tft->setTextFont(FONT_SMALL);
            s_tft->setTextColor(COL_GREY_MID, COL_BG);
            s_tft->drawCentreString("L=enter coords  R=retry GPS", SCREEN_W / 2, y + 44, FONT_SMALL);
        }
        int ya = SCREEN_H - BOTTOMBAR_H;
        s_tft->drawFastHLine(0, ya, SCREEN_W, g_themeColor);
        s_tft->setTextFont(FONT_SMALL);
        s_tft->setTextColor(g_themeColor, COL_BG);
        s_tft->drawCentreString("Q=home  R=refresh  L=location  G=GPS", SCREEN_W / 2, ya + 3, FONT_SMALL);
        return;
    }

    // ── Alert list ────────────────────────────────────────────────────────────
    int visible = min(s_itemCount - s_scrollOff, WAZE_VISIBLE);
    s_tft->setTextFont(FONT_SMALL);

    for (int i = 0; i < visible; i++) {
        int idx = s_scrollOff + i;
        if (idx >= s_itemCount) break;
        const WazeItem &item = s_items[idx];
        int ry = y + i * WAZE_ROW_H;

        s_tft->setTextColor(item.color, COL_BG);
        s_tft->drawString(item.badge, 4, ry + 1);
        int bw = s_tft->textWidth(item.badge) + 6;

        int maxPx = SCREEN_W - bw - 4;
        String det = item.detail;
        while (det.length() > 3 && s_tft->textWidth(det) > maxPx)
            det.remove(det.length() - 1);
        if (det.length() < strlen(item.detail) && det.length() > 2)
            det = det.substring(0, det.length() - 2) + "..";
        s_tft->setTextColor(COL_WHITE, COL_BG);
        s_tft->drawString(det, bw + 2, ry + 1);

        if (i < visible - 1)
            s_tft->drawFastHLine(4, ry + WAZE_ROW_H - 1, SCREEN_W - 8, COL_GREY_DIM);
    }

    // Scroll indicator
    if (s_itemCount > WAZE_VISIBLE) {
        char sbuf[20];
        snprintf(sbuf, sizeof(sbuf), "%d-%d/%d",
                 s_scrollOff + 1, min(s_scrollOff + WAZE_VISIBLE, s_itemCount), s_itemCount);
        int sw = s_tft->textWidth(sbuf);
        s_tft->setTextColor(COL_GREY_MID, COL_BG);
        s_tft->drawString(sbuf, SCREEN_W - sw - 4, y);
    }

    // Bottom bar
    int ya = SCREEN_H - BOTTOMBAR_H;
    s_tft->drawFastHLine(0, ya, SCREEN_W, g_themeColor);
    s_tft->setTextFont(FONT_SMALL);
    s_tft->setTextColor(g_themeColor, COL_BG);
    s_tft->drawCentreString("Q=home  R=refresh  L=location  G=GPS", SCREEN_W / 2, ya + 3, FONT_SMALL);
}

static void showLoadingScreen() {
    s_tft->fillScreen(COL_BG);
    drawTopbar(*s_tft, modeTitle(), "", g_themeColor);
    s_tft->setTextFont(FONT_MED);
    s_tft->setTextColor(g_themeColor, COL_BG);

    const char *heading = (s_mode == WAZE_POLICE) ? "POLICE ALERTS"
                        : (s_mode == WAZE_ROAD)   ? "ROAD & JAMS"
                        :                           "HAZARD ALERTS";
    s_tft->drawCentreString(heading, SCREEN_W / 2, CONTENT_Y + 14, FONT_MED);
    s_tft->setTextFont(FONT_SMALL);
    s_tft->setTextColor(COL_GREY_MID, COL_BG);
    s_tft->drawCentreString("Checking location...", SCREEN_W / 2, CONTENT_Y + 36, FONT_SMALL);
}

static void showFetching() {
    s_tft->fillRect(0, CONTENT_Y + 30, SCREEN_W, 20, COL_BG);
    s_tft->setTextFont(FONT_SMALL);
    s_tft->setTextColor(COL_GREY_MID, COL_BG);
    s_tft->drawCentreString("Fetching Waze data...", SCREEN_W / 2, CONTENT_Y + 34, FONT_SMALL);
}

// ── Cache ─────────────────────────────────────────────────────────────────────
static void saveWazeCache() {
    const char *pfx = modeCachePrefix();
    // Pack items: badge\x01detail\x02...
    String packed;
    for (int i = 0; i < s_itemCount; i++) {
        if (i > 0) packed += '\x02';
        packed += s_items[i].badge;  packed += '\x01';
        packed += s_items[i].detail;
    }
    char key[16];
    snprintf(key, sizeof(key), "%s_d", pfx);
    nvsPutString(key, packed);
    snprintf(key, sizeof(key), "%s_n", pfx);
    nvsPutInt(key, s_itemCount);
    snprintf(key, sizeof(key), "%s_at", pfx);
    nvsPutInt(key, (int)time(nullptr));
}

static bool loadWazeCache() {
    const char *pfx = modeCachePrefix();
    char key[16];
    snprintf(key, sizeof(key), "%s_at", pfx);
    int32_t ts = nvsGetInt(key);
    if (ts == 0) return false;
    if ((int32_t)time(nullptr) - ts > WAZE_CACHE_TTL) return false;

    snprintf(key, sizeof(key), "%s_n", pfx);
    int n = nvsGetInt(key);
    if (n <= 0 || n > WAZE_MAX_ITEMS) return false;

    snprintf(key, sizeof(key), "%s_d", pfx);
    String packed = nvsGetString(key);
    if (packed.length() == 0) return false;

    s_itemCount = 0;
    int pos = 0;
    for (int i = 0; i < n && pos < (int)packed.length(); i++) {
        int end = packed.indexOf('\x02', pos);
        if (end < 0) end = packed.length();
        String chunk = packed.substring(pos, end);
        int f1 = chunk.indexOf('\x01');
        if (f1 >= 0) {
            WazeItem &item = s_items[s_itemCount++];
            strlcpy(item.badge,  chunk.substring(0, f1).c_str(),  sizeof(item.badge));
            strlcpy(item.detail, chunk.substring(f1+1).c_str(),   sizeof(item.detail));
            item.color = colorFromBadge(item.badge);
        }
        pos = end + 1;
    }

    s_dataCached = true;
    return true;
}

// ── Shared init ───────────────────────────────────────────────────────────────
static void wazeInitCommon(TFT_eSPI &tft) {
    s_tft        = &tft;
    s_itemCount  = 0;
    s_scrollOff  = 0;
    s_gpsValid   = false;
    s_gpsCached  = false;
    s_dataCached = false;
    strlcpy(s_errMsg, "", sizeof(s_errMsg));

    // Restore GPS from NVS (fast, no network)
    loadCachedGPS();

    // Use cached data if still fresh — no loading screen
    if (loadWazeCache()) {
        drawWazeScreen();
        return;
    }

    showLoadingScreen();

    if (s_gpsValid && WiFi.isConnected()) {
        showFetching();
        if (fetchWaze()) saveWazeCache();
    } else if (!s_gpsValid) {
        strlcpy(s_errMsg, "No location — press L to enter coords, G for GPS", sizeof(s_errMsg));
    }

    drawWazeScreen();
}

// ── Shared loop ───────────────────────────────────────────────────────────────
static bool wazeLoopImpl(TFT_eSPI &tft) {
    (void)tft;
    char key = readKeyboard();
    if (key == 0) { delay(20); return true; }

    if (key == 'q' || key == 'Q' || key == 27 || key == 17 || key == 8) return false;

    if (key == 'r' || key == 'R') {
        // Invalidate cache for current mode, force fresh fetch
        char invKey[16];
        snprintf(invKey, sizeof(invKey), "%s_at", modeCachePrefix());
        nvsPutInt(invKey, 0);
        s_dataCached = false;
        s_itemCount  = 0;
        s_scrollOff  = 0;
        strlcpy(s_errMsg, "", sizeof(s_errMsg));
        if (!s_gpsValid) loadCachedGPS();
        if (s_gpsValid && WiFi.isConnected()) {
            showFetching();
            if (fetchWaze()) saveWazeCache();
        }
        drawWazeScreen();
    }

    if (key == 'l' || key == 'L') {
        doSetLocation();
        s_dataCached = false;
        s_itemCount  = 0;
        s_scrollOff  = 0;
        if (s_gpsValid && WiFi.isConnected()) {
            showFetching();
            if (fetchWaze()) saveWazeCache();
        }
        drawWazeScreen();
    }

    if (key == 'g' || key == 'G') {
        s_dataCached = false;
        s_itemCount  = 0;
        s_scrollOff  = 0;
        strlcpy(s_errMsg, "", sizeof(s_errMsg));
        if (tryHardwareGPS()) {
            if (WiFi.isConnected()) {
                showFetching();
                if (fetchWaze()) saveWazeCache();
            }
        } else {
            strlcpy(s_errMsg, "GPS timeout — press L to enter coords manually", sizeof(s_errMsg));
        }
        drawWazeScreen();
    }

    delay(20);
    return true;
}

// ── Public API ────────────────────────────────────────────────────────────────
void wazeHazardInit(TFT_eSPI &tft) { s_mode = WAZE_HAZARD; wazeInitCommon(tft); }
bool wazeHazardLoop(TFT_eSPI &tft) { return wazeLoopImpl(tft); }

void wazePoliceInit(TFT_eSPI &tft) { s_mode = WAZE_POLICE; wazeInitCommon(tft); }
bool wazePoliceLoop(TFT_eSPI &tft) { return wazeLoopImpl(tft); }

void wazeRoadInit(TFT_eSPI &tft)   { s_mode = WAZE_ROAD;   wazeInitCommon(tft); }
bool wazeRoadLoop(TFT_eSPI &tft)   { return wazeLoopImpl(tft); }

void wazeTrackballUp() {
    if (s_scrollOff > 0) { s_scrollOff--; drawWazeScreen(); }
}
void wazeTrackballDown() {
    int maxOff = max(0, s_itemCount - WAZE_VISIBLE);
    if (s_scrollOff < maxOff) { s_scrollOff++; drawWazeScreen(); }
}
