/*
==============================================================================
 textdraws.inc
 NUSANTARA ROLEPLAY - Clean TextDraw UI (Speedometer, Location HUD, Alerts, Trauma)
 JGRP / Classic SA-MP Style:
 - Speedometer (Panel kanan-bawah saat berkendara: Kecepatan, Bensin, Mesin, Pintu)
 - Street Zone & Compass (Kompas mata angin & nama distrik Los Santos)
 - Alert Banner & Subtitle (Pesan notifikasi ringkas bawah layar)
 - Critical Trauma Overlay (Layar merah saat kondisi sekarat)
 - Mengeliminasi UI Textdraw yang rumit/buggy dan beralih penuh ke sistem Dialog
==============================================================================
*/

#if defined _INC_textdraws
	#endinput
#endif
#define _INC_textdraws

#tryinclude "header.inc"
#tryinclude "modules/header.inc"

// ---------------------------------------------------------------------------
// PALET WARNA BERSIH (JGRP / CLASSIC SA-MP STYLE, BEBAS AI-SLOP)
// ---------------------------------------------------------------------------
#define TD_COLOR_WHITE       0xFFFFFFFF  // Teks putih utama
#define TD_COLOR_YELLOW      0xFFFF00FF  // Aksen kuning
#define TD_COLOR_GREEN       0x2ECC71FF  // Hijau status hidup / sukses
#define TD_COLOR_RED         0xE74C3CFF  // Merah status mati / terkunci
#define TD_COLOR_GRAY        0xBDC3C7FF  // Abu-abu status bebas / netral
#define TD_COLOR_MUTED       0x8A8FA8FF  // Label teks redup
#define TD_COLOR_PANEL       0x00000088  // Latar panel box hitam transparan natural
#define TD_COLOR_ALERT_BG    0x000000AA  // Latar banner notifikasi
#define TD_COLOR_INJURED_BG  0x330000D0  // Latar layar merah sekarat

// ---------------------------------------------------------------------------
// PENALAAN & BATASAN
// ---------------------------------------------------------------------------
#define TD_ALERT_DEFAULT_MS   3000
#define TD_SUBTITLE_DEFAULT_MS 4000
#define TD_SPEEDO_INTERVAL    150
#define TD_BAR_LENGTH         10
#define TD_MAX_TEXT           128
#define TD_VEH_NAME_MAX       32

// ---------------------------------------------------------------------------
// HANDLES PLAYER TEXTDRAW (RINGKAS & CRASH-PROOF)
// ---------------------------------------------------------------------------
// --- Speedometer ---
#define pTD_SpdPanel        0
#define pTD_SpdVehName      1
#define pTD_SpdSpeedLbl     2
#define pTD_SpdSpeedVal     3
#define pTD_SpdFuelLbl      4
#define pTD_SpdFuelBar      5
#define pTD_SpdEngineLbl    6
#define pTD_SpdEngineVal    7
#define pTD_SpdLockLbl      8
#define pTD_SpdLockVal      9
// --- Alert & Subtitle ---
#define pTD_AlertPanel      10
#define pTD_AlertText       11
#define pTD_SubPanel        12
#define pTD_SubText         13
// --- Street Zone & Compass HUD ---
#define pTD_ZoneBox         14
#define pTD_ZoneCompass     15
#define pTD_ZoneName        16
// --- Critical Trauma Overlay ---
#define pTD_InjuredBg       17
#define pTD_InjuredHeader   18
#define pTD_InjuredTimer    19
#define pTD_InjuredInfo     20
// --- Batas Total TextDraw ---
#define pTD_COUNT           21

#define TD_SPD_FIRST       pTD_SpdPanel
#define TD_SPD_LAST        pTD_SpdLockVal
#define TD_ZONE_FIRST      pTD_ZoneBox
#define TD_ZONE_LAST       pTD_ZoneName
#define TD_INJURED_FIRST   pTD_InjuredBg
#define TD_INJURED_LAST    pTD_InjuredInfo

// ---------------------------------------------------------------------------
// STATE MODUL
// ---------------------------------------------------------------------------
new
    PlayerText:gTD_PlayerText[MAX_PLAYERS][pTD_COUNT],
    bool:gTD_Ready[MAX_PLAYERS],
    gTD_SpdTimer[MAX_PLAYERS],
    gTD_SpdVehicle[MAX_PLAYERS];

// Helper kalkulasi progress bar baris string
stock TD_GetPercentBar(dest[], destlen, Float:percent, cells = TD_BAR_LENGTH)
{
    new filled, i, j;
    if (percent < 0.0) percent = 0.0;
    if (percent > 100.0) percent = 100.0;
    if (cells < 1) cells = 1;
    if (cells > 24) cells = 24;

    filled = floatround((percent / 100.0) * cells, floatround_round);
    i = 0;
    if (i < destlen - 1) dest[i++] = '[';
    for (j = 0; j < cells; j++) {
        if (i >= destlen - 2) break;
        dest[i++] = (j < filled) ? '|' : ' ';
    }
    if (i < destlen - 1) dest[i++] = ']';
    dest[i] = '\0';
    return 1;
}

stock Float:TD_GetVehicleSpeedKmh(vehicleid)
{
    new Float:vx, Float:vy, Float:vz;
    if (vehicleid < 1) return 0.0;
    GetVehicleVelocity(vehicleid, vx, vy, vz);
    return floatsqroot(vx * vx + vy * vy + vz * vz) * 180.0;
}

stock TD_GetVehicleParamsSafe(vehicleid, &engine, &doors)
{
    new lights, alarm, bonnet, boot, objective;
    engine = 0; doors = 0;
    if (vehicleid < 1) return 0;
    GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
    return 1;
}

stock TD_GetOwnedVehicleFuel(vehicleid, &Float:fuel)
{
    fuel = 0.0;
    if (vehicleid < 1) return 0;
    for (new i = 0; i < MAX_OWNED_VEHICLES; i++) {
        if (gVehicleData[i][vUsed] && gVehicleData[i][vVehicleHandle] == vehicleid) {
            fuel = gVehicleData[i][vFuel];
            return 1;
        }
    }
    return 0;
}

// ---------------------------------------------------------------------------
// INISIALISASI TEXTDRAW PEMAIN
// ---------------------------------------------------------------------------
stock InitPlayerTextDraws(playerid)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;

    // Inisialisasi Activity Progress Bar
    ProgressBar_Init(playerid);

    // ================= PANEL SPEEDOMETER (kanan-bawah) =================
    gTD_PlayerText[playerid][pTD_SpdPanel] = CreatePlayerTextDraw(playerid, 450.0, 340.0, "_");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_SpdPanel], 0.0, 8.8);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_SpdPanel], 618.0, 0.0);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_SpdPanel], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SpdPanel], TD_COLOR_WHITE);
    PlayerTextDrawUseBox(playerid, gTD_PlayerText[playerid][pTD_SpdPanel], true);
    PlayerTextDrawBoxColour(playerid, gTD_PlayerText[playerid][pTD_SpdPanel], TD_COLOR_PANEL);

    gTD_PlayerText[playerid][pTD_SpdVehName] = CreatePlayerTextDraw(playerid, 612.0, 343.0, "Kendaraan");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_SpdVehName], 0.22, 0.95);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_SpdVehName], TEXT_DRAW_ALIGN_RIGHT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SpdVehName], TD_COLOR_YELLOW);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_SpdVehName], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_SpdVehName], true);

    gTD_PlayerText[playerid][pTD_SpdSpeedLbl] = CreatePlayerTextDraw(playerid, 456.0, 355.0, "KECEPATAN");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_SpdSpeedLbl], 0.18, 0.80);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_SpdSpeedLbl], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SpdSpeedLbl], TD_COLOR_MUTED);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_SpdSpeedLbl], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_SpdSpeedLbl], true);

    gTD_PlayerText[playerid][pTD_SpdSpeedVal] = CreatePlayerTextDraw(playerid, 612.0, 354.0, "0 KM/J");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_SpdSpeedVal], 0.25, 1.0);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_SpdSpeedVal], TEXT_DRAW_ALIGN_RIGHT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SpdSpeedVal], TD_COLOR_WHITE);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_SpdSpeedVal], TEXT_DRAW_FONT_2);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_SpdSpeedVal], true);

    gTD_PlayerText[playerid][pTD_SpdFuelLbl] = CreatePlayerTextDraw(playerid, 456.0, 367.0, "BAHAN BAKAR");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_SpdFuelLbl], 0.18, 0.80);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_SpdFuelLbl], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SpdFuelLbl], TD_COLOR_MUTED);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_SpdFuelLbl], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_SpdFuelLbl], true);

    gTD_PlayerText[playerid][pTD_SpdFuelBar] = CreatePlayerTextDraw(playerid, 612.0, 367.0, "[          ]");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_SpdFuelBar], 0.20, 0.88);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_SpdFuelBar], TEXT_DRAW_ALIGN_RIGHT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SpdFuelBar], TD_COLOR_GREEN);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_SpdFuelBar], TEXT_DRAW_FONT_2);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_SpdFuelBar], true);

    gTD_PlayerText[playerid][pTD_SpdEngineLbl] = CreatePlayerTextDraw(playerid, 456.0, 378.0, "MESIN");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_SpdEngineLbl], 0.18, 0.80);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_SpdEngineLbl], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SpdEngineLbl], TD_COLOR_MUTED);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_SpdEngineLbl], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_SpdEngineLbl], true);

    gTD_PlayerText[playerid][pTD_SpdEngineVal] = CreatePlayerTextDraw(playerid, 612.0, 378.0, "MATI");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_SpdEngineVal], 0.20, 0.88);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_SpdEngineVal], TEXT_DRAW_ALIGN_RIGHT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SpdEngineVal], TD_COLOR_RED);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_SpdEngineVal], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_SpdEngineVal], true);

    gTD_PlayerText[playerid][pTD_SpdLockLbl] = CreatePlayerTextDraw(playerid, 456.0, 389.0, "PINTU");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_SpdLockLbl], 0.18, 0.80);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_SpdLockLbl], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SpdLockLbl], TD_COLOR_MUTED);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_SpdLockLbl], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_SpdLockLbl], true);

    gTD_PlayerText[playerid][pTD_SpdLockVal] = CreatePlayerTextDraw(playerid, 612.0, 389.0, "BEBAS");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_SpdLockVal], 0.20, 0.88);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_SpdLockVal], TEXT_DRAW_ALIGN_RIGHT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SpdLockVal], TD_COLOR_GRAY);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_SpdLockVal], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_SpdLockVal], true);

    // =================== BANNER ALERT (tengah-bawah) ===================
    gTD_PlayerText[playerid][pTD_AlertPanel] = CreatePlayerTextDraw(playerid, 130.0, 418.0, "_");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_AlertPanel], 0.0, 2.6);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_AlertPanel], 510.0, 0.0);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_AlertPanel], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_AlertPanel], TD_COLOR_WHITE);
    PlayerTextDrawUseBox(playerid, gTD_PlayerText[playerid][pTD_AlertPanel], true);
    PlayerTextDrawBoxColour(playerid, gTD_PlayerText[playerid][pTD_AlertPanel], TD_COLOR_ALERT_BG);

    gTD_PlayerText[playerid][pTD_AlertText] = CreatePlayerTextDraw(playerid, 320.0, 424.0, "");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_AlertText], 0.21, 0.92);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_AlertText], TEXT_DRAW_ALIGN_CENTER);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_AlertText], TD_COLOR_WHITE);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_AlertText], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_AlertText], true);

    // ================ BANNER SUBTITLE ================
    gTD_PlayerText[playerid][pTD_SubPanel] = CreatePlayerTextDraw(playerid, 155.0, 392.0, "_");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_SubPanel], 0.0, 2.2);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_SubPanel], 485.0, 0.0);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_SubPanel], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SubPanel], TD_COLOR_WHITE);
    PlayerTextDrawUseBox(playerid, gTD_PlayerText[playerid][pTD_SubPanel], true);
    PlayerTextDrawBoxColour(playerid, gTD_PlayerText[playerid][pTD_SubPanel], TD_COLOR_PANEL);

    gTD_PlayerText[playerid][pTD_SubText] = CreatePlayerTextDraw(playerid, 320.0, 397.0, "");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_SubText], 0.20, 0.90);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_SubText], TEXT_DRAW_ALIGN_CENTER);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SubText], TD_COLOR_WHITE);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_SubText], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_SubText], true);

    // ==================== STREET ZONE & COMPASS HUD ====================
    gTD_PlayerText[playerid][pTD_ZoneBox] = CreatePlayerTextDraw(playerid, 28.0, 432.0, "_");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_ZoneBox], 0.0, 1.4);
    PlayerTextDrawUseBox(playerid, gTD_PlayerText[playerid][pTD_ZoneBox], true);
    PlayerTextDrawBoxColour(playerid, gTD_PlayerText[playerid][pTD_ZoneBox], TD_COLOR_PANEL);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_ZoneBox], 200.0, 13.0);

    gTD_PlayerText[playerid][pTD_ZoneCompass] = CreatePlayerTextDraw(playerid, 32.0, 433.0, "[ N ]");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_ZoneCompass], 0.18, 0.80);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_ZoneCompass], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_ZoneCompass], TD_COLOR_GREEN);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_ZoneCompass], TEXT_DRAW_FONT_2);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_ZoneCompass], true);

    gTD_PlayerText[playerid][pTD_ZoneName] = CreatePlayerTextDraw(playerid, 56.0, 433.0, "Pershing Square, Los Santos");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_ZoneName], 0.17, 0.80);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_ZoneName], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_ZoneName], TD_COLOR_WHITE);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_ZoneName], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_ZoneName], true);

    // ==================== CRITICAL TRAUMA OVERLAY ====================
    gTD_PlayerText[playerid][pTD_InjuredBg] = CreatePlayerTextDraw(playerid, 160.0, 185.0, "_");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_InjuredBg], 0.0, 7.8);
    PlayerTextDrawUseBox(playerid, gTD_PlayerText[playerid][pTD_InjuredBg], true);
    PlayerTextDrawBoxColour(playerid, gTD_PlayerText[playerid][pTD_InjuredBg], TD_COLOR_INJURED_BG);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_InjuredBg], 480.0, 75.0);

    gTD_PlayerText[playerid][pTD_InjuredHeader] = CreatePlayerTextDraw(playerid, 320.0, 192.0, "~r~KONDISI KRITIS (SEKARAT)");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_InjuredHeader], 0.28, 1.15);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_InjuredHeader], TEXT_DRAW_ALIGN_CENTER);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_InjuredHeader], TD_COLOR_RED);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_InjuredHeader], TEXT_DRAW_FONT_2);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_InjuredHeader], true);

    gTD_PlayerText[playerid][pTD_InjuredTimer] = CreatePlayerTextDraw(playerid, 320.0, 212.0, "~w~Waktu Pertolongan Darurat: ~y~90 Detik");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_InjuredTimer], 0.20, 0.90);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_InjuredTimer], TEXT_DRAW_ALIGN_CENTER);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_InjuredTimer], TD_COLOR_WHITE);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_InjuredTimer], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_InjuredTimer], true);

    gTD_PlayerText[playerid][pTD_InjuredInfo] = CreatePlayerTextDraw(playerid, 320.0, 230.0, "~w~Gunakan ~g~/call 911 ~w~untuk memanggil Medis | Warga dapat membantu via ~g~/treat [id]");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_InjuredInfo], 0.17, 0.80);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_InjuredInfo], TEXT_DRAW_ALIGN_CENTER);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_InjuredInfo], TD_COLOR_WHITE);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_InjuredInfo], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_InjuredInfo], true);

    gTD_HUD[playerid] = false;
    gTD_Speedo[playerid] = false;
    gTD_Alert[playerid] = false;
    gTD_Subtitle[playerid] = false;
    gAlertTimer[playerid] = 0;
    gSubtitleTimer[playerid] = 0;
    gTD_SpdTimer[playerid] = 0;
    gTD_SpdVehicle[playerid] = 0;
    gTD_Ready[playerid] = true;
    return 1;
}

stock DestroyPlayerTextDraws(playerid)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;

    ProgressBar_Destroy(playerid);
    HidePlayerAlert(playerid);
    HidePlayerSubtitle(playerid);
    HidePlayerSpeedo(playerid);
    HidePlayerZoneHUD(playerid);
    HidePlayerInjuredHUD(playerid);

    for (new e = 0; e < pTD_COUNT; e++) {
        if (gTD_PlayerText[playerid][e] != INVALID_PLAYER_TEXT_DRAW) {
            PlayerTextDrawDestroy(playerid, gTD_PlayerText[playerid][e]);
            gTD_PlayerText[playerid][e] = INVALID_PLAYER_TEXT_DRAW;
        }
    }
    gTD_Ready[playerid] = false;
    return 1;
}

// ---------------------------------------------------------------------------
// HUD & ZONE DISPLAY
// ---------------------------------------------------------------------------
stock ShowPlayerHUD(playerid)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS || !gTD_Ready[playerid]) return 0;
    ShowPlayerZoneHUD(playerid);
    gTD_HUD[playerid] = true;
    return 1;
}

stock HidePlayerHUD(playerid)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;
    HidePlayerZoneHUD(playerid);
    gTD_HUD[playerid] = false;
    return 1;
}

stock UpdatePlayerHUD(playerid)
{
    #pragma unused playerid
    return 1;
}

// ---------------------------------------------------------------------------
// SPEEDOMETER KENDARAAN
// ---------------------------------------------------------------------------
stock ShowPlayerSpeedo(playerid)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS || !gTD_Ready[playerid]) return 0;
    if (gTD_Speedo[playerid]) return 1;

    for (new e = TD_SPD_FIRST; e <= TD_SPD_LAST; e++)
        PlayerTextDrawShow(playerid, gTD_PlayerText[playerid][e]);

    gTD_Speedo[playerid] = true;
    UpdatePlayerSpeedo(playerid);

    if (gTimerSpeedo == 0 && gTD_SpdTimer[playerid] == 0)
        gTD_SpdTimer[playerid] = SetTimerEx("UpdatePlayerSpeedo", TD_SPEEDO_INTERVAL, true, "i", playerid);
    return 1;
}

stock HidePlayerSpeedo(playerid)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;
    if (!gTD_Speedo[playerid]) return 1;

    for (new e = TD_SPD_FIRST; e <= TD_SPD_LAST; e++)
        PlayerTextDrawHide(playerid, gTD_PlayerText[playerid][e]);

    gTD_Speedo[playerid] = false;
    gTD_SpdVehicle[playerid] = 0;

    if (gTD_SpdTimer[playerid] != 0) {
        KillTimer(gTD_SpdTimer[playerid]);
        gTD_SpdTimer[playerid] = 0;
    }
    return 1;
}

stock UpdatePlayerSpeedo(playerid)
{
    new line[TD_MAX_TEXT], vehname[TD_VEH_NAME_MAX];
    new engine, doors, Float:fuel;

    if (playerid < 0 || playerid >= MAX_PLAYERS || !gTD_Ready[playerid]) return 0;
    if (!gTD_Speedo[playerid]) return 1;

    new vehicleid = GetPlayerVehicleID(playerid);
    if (vehicleid < 1 || GetPlayerState(playerid) != PLAYER_STATE_DRIVER) {
        HidePlayerSpeedo(playerid);
        return 1;
    }

    if (gTD_SpdVehicle[playerid] != vehicleid) {
        gTD_SpdVehicle[playerid] = vehicleid;
        GetVehicleModelName(GetVehicleModel(vehicleid), vehname, sizeof(vehname));
        PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_SpdVehName], vehname);
    }

    format(line, sizeof(line), "%d KM/J", floatround(TD_GetVehicleSpeedKmh(vehicleid), floatround_round));
    PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_SpdSpeedVal], line);

    if (TD_GetOwnedVehicleFuel(vehicleid, fuel)) {
        TD_GetPercentBar(line, sizeof(line), fuel);
        PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_SpdFuelBar], line);
        PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SpdFuelBar], TD_COLOR_GREEN);
    } else {
        PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_SpdFuelBar], "N/A");
        PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SpdFuelBar], TD_COLOR_MUTED);
    }

    TD_GetVehicleParamsSafe(vehicleid, engine, doors);

    if (engine) {
        PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_SpdEngineVal], "MENYALA");
        PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SpdEngineVal], TD_COLOR_GREEN);
    } else {
        PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_SpdEngineVal], "MATI");
        PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SpdEngineVal], TD_COLOR_RED);
    }

    if (doors) {
        PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_SpdLockVal], "TERKUNCI");
        PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SpdLockVal], TD_COLOR_YELLOW);
    } else {
        PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_SpdLockVal], "BEBAS");
        PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SpdLockVal], TD_COLOR_GRAY);
    }
    return 1;
}

forward RP_SpeedoTick();
public RP_SpeedoTick()
{
    for (new playerid = 0; playerid < MAX_PLAYERS; playerid++) {
        if (gTD_Speedo[playerid]) UpdatePlayerSpeedo(playerid);
    }
    return 1;
}

// ---------------------------------------------------------------------------
// TOAST ALERT & SUBTITLE
// ---------------------------------------------------------------------------
stock ShowPlayerAlert(playerid, const message[])
{
    return ShowPlayerAlertEx(playerid, message, TD_ALERT_DEFAULT_MS);
}

stock ShowPlayerAlertEx(playerid, const message[], time_ms = TD_ALERT_DEFAULT_MS)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS || !gTD_Ready[playerid]) return 0;
    if (time_ms < 500) time_ms = 500;

    PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_AlertText], message);
    PlayerTextDrawShow(playerid, gTD_PlayerText[playerid][pTD_AlertPanel]);
    PlayerTextDrawShow(playerid, gTD_PlayerText[playerid][pTD_AlertText]);
    gTD_Alert[playerid] = true;

    if (gAlertTimer[playerid] != 0) KillTimer(gAlertTimer[playerid]);
    gAlertTimer[playerid] = SetTimerEx("RP_HideAlert", time_ms, false, "i", playerid);
    return 1;
}

stock HidePlayerAlert(playerid)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;
    if (gAlertTimer[playerid] != 0) {
        KillTimer(gAlertTimer[playerid]);
        gAlertTimer[playerid] = 0;
    }
    PlayerTextDrawHide(playerid, gTD_PlayerText[playerid][pTD_AlertPanel]);
    PlayerTextDrawHide(playerid, gTD_PlayerText[playerid][pTD_AlertText]);
    gTD_Alert[playerid] = false;
    return 1;
}

forward RP_HideAlert(playerid);
public RP_HideAlert(playerid)
{
    return HidePlayerAlert(playerid);
}

stock ShowPlayerSubtitle(playerid, const message[], time_ms = TD_SUBTITLE_DEFAULT_MS)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS || !gTD_Ready[playerid]) return 0;
    if (time_ms < 500) time_ms = 500;

    PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_SubText], message);
    PlayerTextDrawShow(playerid, gTD_PlayerText[playerid][pTD_SubPanel]);
    PlayerTextDrawShow(playerid, gTD_PlayerText[playerid][pTD_SubText]);
    gTD_Subtitle[playerid] = true;

    if (gSubtitleTimer[playerid] != 0) KillTimer(gSubtitleTimer[playerid]);
    gSubtitleTimer[playerid] = SetTimerEx("RP_HideSubtitle", time_ms, false, "i", playerid);
    return 1;
}

stock HidePlayerSubtitle(playerid)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;
    if (gSubtitleTimer[playerid] != 0) {
        KillTimer(gSubtitleTimer[playerid]);
        gSubtitleTimer[playerid] = 0;
    }
    PlayerTextDrawHide(playerid, gTD_PlayerText[playerid][pTD_SubPanel]);
    PlayerTextDrawHide(playerid, gTD_PlayerText[playerid][pTD_SubText]);
    gTD_Subtitle[playerid] = false;
    return 1;
}

forward RP_HideSubtitle(playerid);
public RP_HideSubtitle(playerid)
{
    return HidePlayerSubtitle(playerid);
}

// ---------------------------------------------------------------------------
// STREET ZONE & COMPASS HUD
// ---------------------------------------------------------------------------
stock GetPlayerZoneName(playerid, zone[], maxlen = sizeof(zone))
{
    new Float:x, Float:y, Float:z;
    #pragma unused z
    if (IsPlayerConnected(playerid)) {
        GetPlayerPos(playerid, x, y, z);
    } else {
        x = gPlayerData[playerid][pLastX];
        y = gPlayerData[playerid][pLastY];
        z = gPlayerData[playerid][pLastZ];
    }

    if (x >= 1370.0 && x <= 1530.0 && y >= -1850.0 && y <= -1550.0) format(zone, maxlen, "Pershing Square, Los Santos");
    else if (x >= 1700.0 && x <= 2100.0 && y >= -1850.0 && y <= -1550.0) format(zone, maxlen, "Idlewood, Los Santos");
    else if (x >= 2100.0 && x <= 2500.0 && y >= -1850.0 && y <= -1550.0) format(zone, maxlen, "Ganton, Los Santos");
    else if (x >= 1350.0 && x <= 1700.0 && y >= -1550.0 && y <= -1250.0) format(zone, maxlen, "Commerce, Los Santos");
    else if (x >= 1000.0 && x <= 1350.0 && y >= -1450.0 && y <= -1200.0) format(zone, maxlen, "Market, Los Santos");
    else if (x >= 1100.0 && x <= 1300.0 && y >= -1400.0 && y <= -1250.0) format(zone, maxlen, "All Saints Hospital, Los Santos");
    else if (x >= 1800.0 && x <= 2200.0 && y >= -1450.0 && y <= -1100.0) format(zone, maxlen, "Jefferson, Los Santos");
    else if (x >= 1600.0 && x <= 1850.0 && y >= -1950.0 && y <= -1750.0) format(zone, maxlen, "Unity Station, Los Santos");
    else if (x >= 1800.0 && x <= 2200.0 && y >= -2200.0 && y <= -1900.0) format(zone, maxlen, "Willowfield, Los Santos");
    else if (x >= 2300.0 && x <= 2800.0 && y >= -2600.0 && y <= -2100.0) format(zone, maxlen, "Ocean Docks, Los Santos");
    else if (x >= 1400.0 && x <= 2200.0 && y >= -2700.0 && y <= -2200.0) format(zone, maxlen, "LS International Airport");
    else if (x >= 200.0 && x <= 1000.0 && y >= -2000.0 && y <= -1400.0) format(zone, maxlen, "Santa Maria Beach, Los Santos");
    else format(zone, maxlen, "Los Santos City Area");
    return 1;
}

stock GetPlayerCompassDirection(playerid, dir[], maxlen = sizeof(dir))
{
    new Float:angle;
    if (IsPlayerConnected(playerid)) GetPlayerFacingAngle(playerid, angle);
    else angle = gPlayerData[playerid][pLastA];

    if (angle >= 337.5 || angle < 22.5) format(dir, maxlen, "N");
    else if (angle >= 22.5 && angle < 67.5) format(dir, maxlen, "NW");
    else if (angle >= 67.5 && angle < 112.5) format(dir, maxlen, "W");
    else if (angle >= 112.5 && angle < 157.5) format(dir, maxlen, "SW");
    else if (angle >= 157.5 && angle < 202.5) format(dir, maxlen, "S");
    else if (angle >= 202.5 && angle < 247.5) format(dir, maxlen, "SE");
    else if (angle >= 247.5 && angle < 292.5) format(dir, maxlen, "E");
    else format(dir, maxlen, "NE");
    return 1;
}

stock ShowPlayerZoneHUD(playerid)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS || !gTD_Ready[playerid]) return 0;
    for (new e = TD_ZONE_FIRST; e <= TD_ZONE_LAST; e++)
        PlayerTextDrawShow(playerid, gTD_PlayerText[playerid][e]);
    UpdatePlayerZoneHUD(playerid);
    return 1;
}

stock HidePlayerZoneHUD(playerid)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;
    for (new e = TD_ZONE_FIRST; e <= TD_ZONE_LAST; e++)
        PlayerTextDrawHide(playerid, gTD_PlayerText[playerid][e]);
    return 1;
}

stock UpdatePlayerZoneHUD(playerid)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS || !gTD_Ready[playerid]) return 0;
    new zone[48], dir[8];
    GetPlayerZoneName(playerid, zone, sizeof(zone));
    GetPlayerCompassDirection(playerid, dir, sizeof(dir));

    new strDir[16];
    format(strDir, sizeof(strDir), "[ %s ]", dir);
    PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_ZoneCompass], strDir);
    PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_ZoneName], zone);
    return 1;
}

// ---------------------------------------------------------------------------
// CRITICAL TRAUMA OVERLAY
// ---------------------------------------------------------------------------
stock ShowPlayerInjuredHUD(playerid, timeLeft = 90)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS || !gTD_Ready[playerid]) return 0;
    new timerStr[64];
    format(timerStr, sizeof(timerStr), "~w~Waktu Pertolongan Darurat: ~y~%d Detik", timeLeft);
    PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_InjuredTimer], timerStr);

    for (new e = TD_INJURED_FIRST; e <= TD_INJURED_LAST; e++)
        PlayerTextDrawShow(playerid, gTD_PlayerText[playerid][e]);
    return 1;
}

stock HidePlayerInjuredHUD(playerid)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;
    for (new e = TD_INJURED_FIRST; e <= TD_INJURED_LAST; e++)
        PlayerTextDrawHide(playerid, gTD_PlayerText[playerid][e]);
    return 1;
}

forward TextdrawCreateAll(playerid);
public TextdrawCreateAll(playerid)
{
    return InitPlayerTextDraws(playerid);
}

forward TextdrawDestroyAll(playerid);
public TextdrawDestroyAll(playerid)
{
    return DestroyPlayerTextDraws(playerid);
}

// ---------------------------------------------------------------------------
// KOMPATIBILITAS HELPER STUBS (DIALIHKAN KE DIALOG JGRP STYLE)
// ---------------------------------------------------------------------------
stock ShowPlayerInventoryUI(playerid)
{
	return ShowPlayerInventoryDialog(playerid);
}

stock HidePlayerInventoryUI(playerid)
{
	#pragma unused playerid
	return 1;
}

stock UpdatePlayerInventoryUI(playerid)
{
	#pragma unused playerid
	return 1;
}

stock ShowPlayerVehDash(playerid)
{
	return ShowPlayerRadialMenu(playerid);
}

stock HidePlayerVehDash(playerid)
{
	#pragma unused playerid
	return 1;
}

stock UpdatePlayerVehDash(playerid)
{
	#pragma unused playerid
	return 1;
}

stock ShowPlayerActMenu(playerid)
{
	return ShowPlayerRadialMenu(playerid);
}

stock HidePlayerActMenu(playerid)
{
	#pragma unused playerid
	return 1;
}

stock ShowPlayerPhone(playerid)
{
	return ShowPhoneMenu(playerid);
}

stock HidePlayerPhone(playerid)
{
	#pragma unused playerid
	return 1;
}

stock UpdatePlayerPhone(playerid)
{
	#pragma unused playerid
	return 1;
}

stock ShowPlayerBankUI(playerid)
{
	return ShowBankDialog(playerid);
}

stock HidePlayerBankUI(playerid)
{
	#pragma unused playerid
	return 1;
}

stock ShowPlayerActionHub(playerid)
{
	return ShowPlayerRadialMenu(playerid);
}

stock ProcessPlayerClickTD(playerid, PlayerText:playertextid)
{
	#pragma unused playerid, playertextid
	return 1;
}
