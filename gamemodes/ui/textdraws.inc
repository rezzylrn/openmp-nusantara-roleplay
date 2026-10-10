/*
==============================================================================
 textdraws.inc
 NUSANTARA ROLEPLAY - Modern TextDraw UI (HUD, Speedometer, Alert Banner)
==============================================================================
 YANG DIHASILKAN
   HUD         : panel kanan-atas - branding server, nama/ID, uang, bank,
                 bar lapar, bar haus, level
   Speedometer : panel kanan-bawah - nama kendaraan, kecepatan (KM/J),
                 bar bahan bakar, status mesin (MENYALA/MATI),
                 status pintu (TERKUNCI/BEBAS)
   Alert       : banner toast di bawah layar, auto-hide
   Subtitle    : banner kedua untuk pesan roleplay singkat (opsional)

 KOORDINASI DENGAN header.inc
   - Data pemain  : gPlayerData[playerid][pMoney / pBank / pLevel /
                     pHunger / pThirst]           (E_PLAYER_DATA)
   - Data kendaraan: gVehicleData[][vUsed / vFuel] (E_VEHICLE_DATA)
   - Nama model   : stock header.inc
                     GetVehicleModelName(modelid, dest[], len)
                     -> dipanggil langsung, tidak diduplikasi di sini
   - Mesin + pintu: dibaca via GetVehicleParamsEx sehingga semua kendaraan
                     (pribadi, job, faksi, dealer) ikut terbaca benar.
                     vehicles.inc yang men-set param tersebut, jadi tetap sinkron.
   - Bahan bakar  : hanya kendaraan pribadi yang punya vFuel; selain itu
                     dibuat "N/A" (tidak mengarang angka).
   - State bersama dengan header.inc yang DIPAKAI di sini:
       gTD_HUD[], gTD_Speedo[], gTD_Alert[], gTD_Subtitle[]  (penanda tampil)
       gAlertTimer[], gSubtitleTimer[]                        (timer auto-hide)
     Catatan: pTD_HUD/pTD_Speedo/pTD_Alert milik header hanya 1 PlayerText
     per pemain, tidak cukup untuk panel multi-baris, jadi modul ini memakai
     array sendiri gTD_PlayerText[][] dan membiarkan pTD_* header tidak
     dipakai (tidak berbahaya).
   - Callback publik yang di-forward header.inc dan DIIIMPLEMENTASI di sini:
       RP_SpeedoTick()         - tick speedometer global
       RP_HideAlert(playerid)  - auto-hide alert
       RP_HideSubtitle(playerid)- auto-hide subtitle
     Modul lain jangan mengimplementasikan ketiganya.

 CARA PAKAI DARI roleplay.pwn
   OnPlayerConnect      -> TextdrawCreateAll(playerid)
   OnPlayerDisconnect   -> TextdrawDestroyAll(playerid)
   Setelah login/spawn  -> ShowPlayerHUD(playerid)
   Saat masuk kendaraan -> ShowPlayerSpeedo(playerid)   (auto-hide sudah jalan)
   Saat uang/bank/lapar/haus/level berubah -> UpdatePlayerHUD(playerid)
   Notifikasi           -> ShowPlayerAlert(playerid, "...")
                           ShowPlayerAlertEx(playerid, "...", 5000)

   Timer global (disarankan di OnGameModeInit roleplay.pwn):
     gTimerSpeedo = SetTimer("RP_SpeedoTick", 150, true);
   Modul ini juga memasang timer per-pemain sebagai cadangan bila
   gTimerSpeedo == 0, jadi tetap berfungsi walau timer global belum dipasang.
==============================================================================
*/
#if defined _INC_textdraws
	#endinput
#endif
#define _INC_textdraws

#include "inv_textdraw.inc"

// Ambil header bersama.  pawncc mencari relatif ke berkas yang me-include
// lebih dulu, lalu ke -i, jadi kedua ejaan dicoba agar modul ini tetap bisa
// dikompilasi sendiri maupun dari gamemode.
#tryinclude "header.inc"
#tryinclude "modules/header.inc"

// ---------------------------------------------------------------------------
// PALET (diawali TD_ agar tidak bentrok dengan COLOR_* milik header.inc)
// ---------------------------------------------------------------------------
#define TD_COLOR_WHITE       0xFFFFFFFF
#define TD_COLOR_PANEL       0x0D0D1AFF  // latar panel gelap
#define TD_COLOR_ACCENT      0xFFC107FF  // emas - branding
#define TD_COLOR_CASH        0x4CD964FF  // hijau - uang cash
#define TD_COLOR_BANK        0x4DA6FFFF  // biru  - saldo bank
#define TD_COLOR_HUNGER      0xFFA726FF  // oranye- lapar
#define TD_COLOR_THIRST      0x26C6DAFF  // cyan  - haus
#define TD_COLOR_LEVEL       0xAB7CFFFF  // ungu  - level
#define TD_COLOR_MUTED       0x8A8FA8FF  // teks label redup
#define TD_COLOR_ENGINE_ON   0x00E676FF  // hijau - mesin menyala
#define TD_COLOR_ENGINE_OFF  0xFF3B30FF  // merah- mesin mati
#define TD_COLOR_LOCK_ON     0xFFCC00FF  // kuning- terkunci
#define TD_COLOR_LOCK_OFF    0x9AA0A6FF  // abu   - bebas
#define TD_COLOR_ALERT_BG    0x0B0B17E6  // latar banner alert
#define TD_COLOR_SUB_BG      0x101024E6  // latar banner subtitle
#define TD_COLOR_PHONE_BG    0x141720F0  // latar smartphone
#define TD_COLOR_INV_BG      0x10131DEE  // latar inventaris modal

// ---------------------------------------------------------------------------
// PENALAAN
// ---------------------------------------------------------------------------
#define TD_ALERT_DEFAULT_MS   3000      // durasi alert bawaan
#define TD_SUBTITLE_DEFAULT_MS 4000     // durasi subtitle bawaan
#define TD_SPEEDO_INTERVAL    150       // ms antar refresh speedometer
#define TD_BAR_LENGTH         10        // jumlah sel progress bar
#define TD_MAX_TEXT           128       // panjang maksimal pesan alert
#define TD_VEH_NAME_MAX       32

// ---------------------------------------------------------------------------
// HANDLES PLAYER TEXTDRAW (milik modul ini)
//   Index memakai #define (tanpa tag), bukan enum.  Sebab: men-index array
//   PlayerText: dengan konstanta bere-tag enum akan melepas tag PlayerText
//   dari elemennya sehingga muncul warning 213 di pemakaian berikutnya.
//   Dengan index tanpa tag, tag PlayerText tetap utuh dan kompilasi bersih.
// ---------------------------------------------------------------------------
// --- HUD ---
#define pTD_HudPanel        0   // latar box
#define pTD_HudBranding     1   // "NUSANTARA ROLEPLAY"
#define pTD_HudNameId       2   // "Nama (ID 5)"
#define pTD_HudCashLbl      3
#define pTD_HudCashVal      4
#define pTD_HudBankLbl      5
#define pTD_HudBankVal      6
#define pTD_HudHungerLbl    7
#define pTD_HudHungerBar    8
#define pTD_HudThirstLbl    9
#define pTD_HudThirstBar    10
#define pTD_HudLevelLbl     11
#define pTD_HudLevelVal     12
// --- Speedometer ---
#define pTD_SpdPanel        13
#define pTD_SpdVehName      14
#define pTD_SpdSpeedLbl     15
#define pTD_SpdSpeedVal     16
#define pTD_SpdFuelLbl      17
#define pTD_SpdFuelBar      18
#define pTD_SpdEngineLbl    19
#define pTD_SpdEngineVal    20
#define pTD_SpdLockLbl      21
#define pTD_SpdLockVal      22
// --- Alert + Subtitle ---
#define pTD_AlertPanel      23
#define pTD_AlertText       24
#define pTD_SubPanel        25
#define pTD_SubText         26
// --- Phone TextDraws (Smartphone UI) ---
#define pTD_PhoneBody       27
#define pTD_PhoneHeader     28
#define pTD_PhoneTitle      29
#define pTD_PhoneInfo       30
#define pTD_PhoneApp1       31
#define pTD_PhoneApp2       32
#define pTD_PhoneApp3       33
#define pTD_PhoneApp4       34
#define pTD_PhoneApp5       35
#define pTD_PhoneApp6       36
// --- Vehicle Control Dashboard ---
#define pTD_VehDashBg       37
#define pTD_VehDashTitle    38
#define pTD_VehDashEngine   39
#define pTD_VehDashLock     40
#define pTD_VehDashLights   41
#define pTD_VehDashHood     42
#define pTD_VehDashTrunk    43
#define pTD_VehDashClose    44
// --- ATM / Banking UI ---
#define pTD_BankBg          45
#define pTD_BankTitle       46
#define pTD_BankBalance     47
#define pTD_BankBtnWithdraw 48
#define pTD_BankBtnDeposit  49
#define pTD_BankBtnTransfer 50
#define pTD_BankBtnClose    51
// --- Quick Action Menu ---
#define pTD_ActBg           52
#define pTD_ActTitle        53
#define pTD_ActBtnInv       54
#define pTD_ActBtnPhone     55
#define pTD_ActBtnID        56
#define pTD_ActBtnVeh       57
#define pTD_ActBtnStats     58
#define pTD_ActBtnHelp      59
#define pTD_ActBtnClose     60
// --- penanda batas (harus terakhir) ---
#define pTD_COUNT           61

// Index pertama/terakhir tiap panel, untuk loop show/hide.
#define TD_HUD_FIRST       pTD_HudPanel
#define TD_HUD_LAST        pTD_HudLevelVal
#define TD_SPD_FIRST       pTD_SpdPanel
#define TD_SPD_LAST        pTD_SpdLockVal
#define TD_PHONE_FIRST     pTD_PhoneBody
#define TD_PHONE_LAST      pTD_PhoneApp6
#define TD_VEHDASH_FIRST   pTD_VehDashBg
#define TD_VEHDASH_LAST    pTD_VehDashClose
#define TD_BANK_FIRST      pTD_BankBg
#define TD_BANK_LAST       pTD_BankBtnClose
#define TD_ACT_FIRST       pTD_ActBg
#define TD_ACT_LAST        pTD_ActBtnClose

// ---------------------------------------------------------------------------
// STATE MODUL
//   Penanda tampil & timer auto-hide memakai milik header.inc (gTD_*,
//   gAlertTimer, gSubtitleTimer) supaya modul lain ikut melihat keadaan yang
//   sama.  Handle textdraw dan timer speedometer tetap milik modul ini.
// ---------------------------------------------------------------------------
new
    PlayerText:gTD_PlayerText[MAX_PLAYERS][pTD_COUNT],
    bool:gTD_Ready[MAX_PLAYERS],
    gTD_SpdTimer[MAX_PLAYERS],      // cadangan bila gTimerSpeedo == 0
    gTD_SpdVehicle[MAX_PLAYERS];

// ---------------------------------------------------------------------------
// HELPER PRIBADI
// ---------------------------------------------------------------------------
// Pawn tidak bisa mengembalikan array, jadi helper string menulis ke buffer
// milik pemanggil (kebiasaan standar SA-MP/open.mp).

// Isi dest[] dengan progress bar, contoh "[|||||     ]" untuk 50 %.
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
    for (j = 0; j < cells; j++)
    {
        if (i >= destlen - 2) break;
        dest[i++] = (j < filled) ? '|' : ' ';
    }
    if (i < destlen - 1) dest[i++] = ']';
    dest[i] = '\0';
    return 1;
}

// Kecepatan KM/J dari vektor kecepatan kendaraan.
stock Float:TD_GetVehicleSpeedKmh(vehicleid)
{
    new Float:vx, Float:vy, Float:vz;

    if (vehicleid < 1) return 0.0;
    GetVehicleVelocity(vehicleid, vx, vy, vz);
    // sqrt(x^2+y^2+z^2) ber_satuan unit/frame; *180 menghasilkan km/jam.
    return floatsqroot(vx * vx + vy * vy + vz * vz) * 180.0;
}

// Mesin + status pintu langsung dari keadaan game (berlaku untuk semua kendaraan).
stock TD_GetVehicleParamsSafe(vehicleid, &engine, &doors)
{
    new lights, alarm, bonnet, boot, objective;

    engine = 0;
    doors = 0;
    if (vehicleid < 1) return 0;
    GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
    return 1;
}

// Bahan bakar kendaraan pribadi.  return 0 bila kendaraan ini bukan milik
// pemain (mis. kendaraan faksi/job) sehingga pemanggil bisa menampilkan N/A.
stock TD_GetOwnedVehicleFuel(vehicleid, &Float:fuel)
{
    fuel = 0.0;
    if (vehicleid < 1) return 0;

    for (new i = 0; i < MAX_OWNED_VEHICLES; i++)
    {
        if (gVehicleData[i][vUsed] && gVehicleData[i][vVehicleHandle] == vehicleid)
        {
            fuel = gVehicleData[i][vFuel];
            return 1;
        }
    }
    return 0;
}

// ---------------------------------------------------------------------------
// LIFECYCLE: BUAT / HAPUS
// ---------------------------------------------------------------------------
// Membuat seluruh player-textdraw untuk satu pemain.  Panggil sekali saat
// terhubung (TextdrawCreateAll melakukan ini).
stock InitPlayerTextDraws(playerid)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;

    // ==================== PANEL HUD (kanan-atas) ====================
    // Standar LEFT-aligned box: x=440 (kiri), TextSize=620 (kanan).
    gTD_PlayerText[playerid][pTD_HudPanel] = CreatePlayerTextDraw(playerid, 440.0, 34.0, "_");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_HudPanel], 0.0, 11.6);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_HudPanel], 620.0, 0.0);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_HudPanel], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_HudPanel], TD_COLOR_WHITE);
    PlayerTextDrawUseBox(playerid, gTD_PlayerText[playerid][pTD_HudPanel], true);
    PlayerTextDrawBoxColour(playerid, gTD_PlayerText[playerid][pTD_HudPanel], TD_COLOR_PANEL);
    PlayerTextDrawSetShadow(playerid, gTD_PlayerText[playerid][pTD_HudPanel], 0);
    PlayerTextDrawSetOutline(playerid, gTD_PlayerText[playerid][pTD_HudPanel], 0);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_HudPanel], TEXT_DRAW_FONT_0);

    gTD_PlayerText[playerid][pTD_HudBranding] = CreatePlayerTextDraw(playerid, 614.0, 38.0, "NUSANTARA ROLEPLAY");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_HudBranding], 0.245, 1.05);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_HudBranding], TEXT_DRAW_ALIGN_RIGHT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_HudBranding], TD_COLOR_ACCENT);
    PlayerTextDrawSetShadow(playerid, gTD_PlayerText[playerid][pTD_HudBranding], 0);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_HudBranding], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_HudBranding], true);

    gTD_PlayerText[playerid][pTD_HudNameId] = CreatePlayerTextDraw(playerid, 614.0, 50.0, "Pengguna (ID 0)");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_HudNameId], 0.195, 0.85);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_HudNameId], TEXT_DRAW_ALIGN_RIGHT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_HudNameId], TD_COLOR_MUTED);
    PlayerTextDrawSetShadow(playerid, gTD_PlayerText[playerid][pTD_HudNameId], 0);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_HudNameId], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_HudNameId], true);

    // --- Uang ---
    gTD_PlayerText[playerid][pTD_HudCashLbl] = CreatePlayerTextDraw(playerid, 446.0, 63.0, "UANG");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_HudCashLbl], 0.185, 0.82);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_HudCashLbl], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_HudCashLbl], TD_COLOR_MUTED);
    PlayerTextDrawSetShadow(playerid, gTD_PlayerText[playerid][pTD_HudCashLbl], 0);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_HudCashLbl], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_HudCashLbl], true);

    gTD_PlayerText[playerid][pTD_HudCashVal] = CreatePlayerTextDraw(playerid, 614.0, 63.0, "$0");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_HudCashVal], 0.215, 0.9);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_HudCashVal], TEXT_DRAW_ALIGN_RIGHT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_HudCashVal], TD_COLOR_CASH);
    PlayerTextDrawSetShadow(playerid, gTD_PlayerText[playerid][pTD_HudCashVal], 0);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_HudCashVal], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_HudCashVal], true);

    // --- Bank ---
    gTD_PlayerText[playerid][pTD_HudBankLbl] = CreatePlayerTextDraw(playerid, 446.0, 74.0, "BANK");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_HudBankLbl], 0.185, 0.82);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_HudBankLbl], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_HudBankLbl], TD_COLOR_MUTED);
    PlayerTextDrawSetShadow(playerid, gTD_PlayerText[playerid][pTD_HudBankLbl], 0);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_HudBankLbl], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_HudBankLbl], true);

    gTD_PlayerText[playerid][pTD_HudBankVal] = CreatePlayerTextDraw(playerid, 614.0, 74.0, "$0");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_HudBankVal], 0.215, 0.9);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_HudBankVal], TEXT_DRAW_ALIGN_RIGHT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_HudBankVal], TD_COLOR_BANK);
    PlayerTextDrawSetShadow(playerid, gTD_PlayerText[playerid][pTD_HudBankVal], 0);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_HudBankVal], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_HudBankVal], true);

    // --- Lapar ---
    gTD_PlayerText[playerid][pTD_HudHungerLbl] = CreatePlayerTextDraw(playerid, 446.0, 85.0, "LAPAR");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_HudHungerLbl], 0.185, 0.82);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_HudHungerLbl], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_HudHungerLbl], TD_COLOR_MUTED);
    PlayerTextDrawSetShadow(playerid, gTD_PlayerText[playerid][pTD_HudHungerLbl], 0);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_HudHungerLbl], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_HudHungerLbl], true);

    gTD_PlayerText[playerid][pTD_HudHungerBar] = CreatePlayerTextDraw(playerid, 614.0, 85.0, "[          ]");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_HudHungerBar], 0.215, 0.9);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_HudHungerBar], TEXT_DRAW_ALIGN_RIGHT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_HudHungerBar], TD_COLOR_HUNGER);
    PlayerTextDrawSetShadow(playerid, gTD_PlayerText[playerid][pTD_HudHungerBar], 0);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_HudHungerBar], TEXT_DRAW_FONT_2);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_HudHungerBar], true);

    // --- Haus ---
    gTD_PlayerText[playerid][pTD_HudThirstLbl] = CreatePlayerTextDraw(playerid, 446.0, 96.0, "HAUS");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_HudThirstLbl], 0.185, 0.82);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_HudThirstLbl], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_HudThirstLbl], TD_COLOR_MUTED);
    PlayerTextDrawSetShadow(playerid, gTD_PlayerText[playerid][pTD_HudThirstLbl], 0);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_HudThirstLbl], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_HudThirstLbl], true);

    gTD_PlayerText[playerid][pTD_HudThirstBar] = CreatePlayerTextDraw(playerid, 614.0, 96.0, "[          ]");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_HudThirstBar], 0.215, 0.9);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_HudThirstBar], TEXT_DRAW_ALIGN_RIGHT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_HudThirstBar], TD_COLOR_THIRST);
    PlayerTextDrawSetShadow(playerid, gTD_PlayerText[playerid][pTD_HudThirstBar], 0);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_HudThirstBar], TEXT_DRAW_FONT_2);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_HudThirstBar], true);

    // --- Level ---
    gTD_PlayerText[playerid][pTD_HudLevelLbl] = CreatePlayerTextDraw(playerid, 446.0, 107.0, "LEVEL");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_HudLevelLbl], 0.185, 0.82);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_HudLevelLbl], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_HudLevelLbl], TD_COLOR_MUTED);
    PlayerTextDrawSetShadow(playerid, gTD_PlayerText[playerid][pTD_HudLevelLbl], 0);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_HudLevelLbl], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_HudLevelLbl], true);

    gTD_PlayerText[playerid][pTD_HudLevelVal] = CreatePlayerTextDraw(playerid, 614.0, 107.0, "1");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_HudLevelVal], 0.215, 0.9);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_HudLevelVal], TEXT_DRAW_ALIGN_RIGHT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_HudLevelVal], TD_COLOR_LEVEL);
    PlayerTextDrawSetShadow(playerid, gTD_PlayerText[playerid][pTD_HudLevelVal], 0);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_HudLevelVal], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_HudLevelVal], true);

    // ================= PANEL SPEEDOMETER (kanan-bawah) =================
    // Standar LEFT-aligned box: x=440 (kiri), TextSize=620 (kanan).
    gTD_PlayerText[playerid][pTD_SpdPanel] = CreatePlayerTextDraw(playerid, 440.0, 330.0, "_");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_SpdPanel], 0.0, 9.4);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_SpdPanel], 620.0, 0.0);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_SpdPanel], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SpdPanel], TD_COLOR_WHITE);
    PlayerTextDrawUseBox(playerid, gTD_PlayerText[playerid][pTD_SpdPanel], true);
    PlayerTextDrawBoxColour(playerid, gTD_PlayerText[playerid][pTD_SpdPanel], TD_COLOR_PANEL);
    PlayerTextDrawSetShadow(playerid, gTD_PlayerText[playerid][pTD_SpdPanel], 0);
    PlayerTextDrawSetOutline(playerid, gTD_PlayerText[playerid][pTD_SpdPanel], 0);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_SpdPanel], TEXT_DRAW_FONT_0);

    gTD_PlayerText[playerid][pTD_SpdVehName] = CreatePlayerTextDraw(playerid, 614.0, 333.0, "Kendaraan");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_SpdVehName], 0.225, 1.0);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_SpdVehName], TEXT_DRAW_ALIGN_RIGHT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SpdVehName], TD_COLOR_ACCENT);
    PlayerTextDrawSetShadow(playerid, gTD_PlayerText[playerid][pTD_SpdVehName], 0);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_SpdVehName], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_SpdVehName], true);

    gTD_PlayerText[playerid][pTD_SpdSpeedLbl] = CreatePlayerTextDraw(playerid, 446.0, 346.0, "KECEPATAN");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_SpdSpeedLbl], 0.185, 0.82);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_SpdSpeedLbl], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SpdSpeedLbl], TD_COLOR_MUTED);
    PlayerTextDrawSetShadow(playerid, gTD_PlayerText[playerid][pTD_SpdSpeedLbl], 0);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_SpdSpeedLbl], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_SpdSpeedLbl], true);

    gTD_PlayerText[playerid][pTD_SpdSpeedVal] = CreatePlayerTextDraw(playerid, 614.0, 345.0, "0 KM/J");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_SpdSpeedVal], 0.26, 1.05);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_SpdSpeedVal], TEXT_DRAW_ALIGN_RIGHT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SpdSpeedVal], TD_COLOR_WHITE);
    PlayerTextDrawSetShadow(playerid, gTD_PlayerText[playerid][pTD_SpdSpeedVal], 0);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_SpdSpeedVal], TEXT_DRAW_FONT_2);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_SpdSpeedVal], true);

    gTD_PlayerText[playerid][pTD_SpdFuelLbl] = CreatePlayerTextDraw(playerid, 446.0, 358.0, "BAHAN BAKAR");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_SpdFuelLbl], 0.185, 0.82);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_SpdFuelLbl], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SpdFuelLbl], TD_COLOR_MUTED);
    PlayerTextDrawSetShadow(playerid, gTD_PlayerText[playerid][pTD_SpdFuelLbl], 0);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_SpdFuelLbl], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_SpdFuelLbl], true);

    gTD_PlayerText[playerid][pTD_SpdFuelBar] = CreatePlayerTextDraw(playerid, 614.0, 358.0, "[          ]");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_SpdFuelBar], 0.215, 0.9);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_SpdFuelBar], TEXT_DRAW_ALIGN_RIGHT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SpdFuelBar], TD_COLOR_HUNGER);
    PlayerTextDrawSetShadow(playerid, gTD_PlayerText[playerid][pTD_SpdFuelBar], 0);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_SpdFuelBar], TEXT_DRAW_FONT_2);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_SpdFuelBar], true);

    gTD_PlayerText[playerid][pTD_SpdEngineLbl] = CreatePlayerTextDraw(playerid, 446.0, 369.0, "MESIN");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_SpdEngineLbl], 0.185, 0.82);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_SpdEngineLbl], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SpdEngineLbl], TD_COLOR_MUTED);
    PlayerTextDrawSetShadow(playerid, gTD_PlayerText[playerid][pTD_SpdEngineLbl], 0);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_SpdEngineLbl], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_SpdEngineLbl], true);

    gTD_PlayerText[playerid][pTD_SpdEngineVal] = CreatePlayerTextDraw(playerid, 614.0, 369.0, "MATI");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_SpdEngineVal], 0.215, 0.9);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_SpdEngineVal], TEXT_DRAW_ALIGN_RIGHT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SpdEngineVal], TD_COLOR_ENGINE_OFF);
    PlayerTextDrawSetShadow(playerid, gTD_PlayerText[playerid][pTD_SpdEngineVal], 0);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_SpdEngineVal], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_SpdEngineVal], true);

    gTD_PlayerText[playerid][pTD_SpdLockLbl] = CreatePlayerTextDraw(playerid, 446.0, 380.0, "PINTU");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_SpdLockLbl], 0.185, 0.82);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_SpdLockLbl], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SpdLockLbl], TD_COLOR_MUTED);
    PlayerTextDrawSetShadow(playerid, gTD_PlayerText[playerid][pTD_SpdLockLbl], 0);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_SpdLockLbl], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_SpdLockLbl], true);

    gTD_PlayerText[playerid][pTD_SpdLockVal] = CreatePlayerTextDraw(playerid, 614.0, 380.0, "BEBAS");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_SpdLockVal], 0.215, 0.9);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_SpdLockVal], TEXT_DRAW_ALIGN_RIGHT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SpdLockVal], TD_COLOR_LOCK_OFF);
    PlayerTextDrawSetShadow(playerid, gTD_PlayerText[playerid][pTD_SpdLockVal], 0);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_SpdLockVal], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_SpdLockVal], true);

    // =================== BANNER ALERT (tengah-bawah) ===================
    // Standar LEFT-aligned box: x=130 (kiri), TextSize=510 (kanan, lebar 380).
    gTD_PlayerText[playerid][pTD_AlertPanel] = CreatePlayerTextDraw(playerid, 130.0, 418.0, "_");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_AlertPanel], 0.0, 2.6);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_AlertPanel], 510.0, 0.0);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_AlertPanel], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_AlertPanel], TD_COLOR_WHITE);
    PlayerTextDrawUseBox(playerid, gTD_PlayerText[playerid][pTD_AlertPanel], true);
    PlayerTextDrawBoxColour(playerid, gTD_PlayerText[playerid][pTD_AlertPanel], TD_COLOR_ALERT_BG);
    PlayerTextDrawSetShadow(playerid, gTD_PlayerText[playerid][pTD_AlertPanel], 0);
    PlayerTextDrawSetOutline(playerid, gTD_PlayerText[playerid][pTD_AlertPanel], 0);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_AlertPanel], TEXT_DRAW_FONT_0);

    gTD_PlayerText[playerid][pTD_AlertText] = CreatePlayerTextDraw(playerid, 320.0, 424.0, "");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_AlertText], 0.215, 0.95);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_AlertText], TEXT_DRAW_ALIGN_CENTER);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_AlertText], TD_COLOR_WHITE);
    PlayerTextDrawSetShadow(playerid, gTD_PlayerText[playerid][pTD_AlertText], 0);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_AlertText], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_AlertText], true);

    // ================ BANNER SUBTITLE (di atas alert) ================
    // Standar LEFT-aligned box: x=155 (kiri), TextSize=485 (kanan, lebar 330).
    gTD_PlayerText[playerid][pTD_SubPanel] = CreatePlayerTextDraw(playerid, 155.0, 392.0, "_");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_SubPanel], 0.0, 2.2);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_SubPanel], 485.0, 0.0);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_SubPanel], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SubPanel], TD_COLOR_WHITE);
    PlayerTextDrawUseBox(playerid, gTD_PlayerText[playerid][pTD_SubPanel], true);
    PlayerTextDrawBoxColour(playerid, gTD_PlayerText[playerid][pTD_SubPanel], TD_COLOR_SUB_BG);
    PlayerTextDrawSetShadow(playerid, gTD_PlayerText[playerid][pTD_SubPanel], 0);
    PlayerTextDrawSetOutline(playerid, gTD_PlayerText[playerid][pTD_SubPanel], 0);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_SubPanel], TEXT_DRAW_FONT_0);

    gTD_PlayerText[playerid][pTD_SubText] = CreatePlayerTextDraw(playerid, 320.0, 397.0, "");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_SubText], 0.2, 0.9);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_SubText], TEXT_DRAW_ALIGN_CENTER);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SubText], TD_COLOR_WHITE);
    PlayerTextDrawSetShadow(playerid, gTD_PlayerText[playerid][pTD_SubText], 0);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_SubText], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_SubText], true);

    // ================== SMARTPHONE UI (kanan layar) ==================
    // Standar LEFT-aligned box: x=497 (kiri), TextSize=623 (kanan, lebar 126).
    gTD_PlayerText[playerid][pTD_PhoneBody] = CreatePlayerTextDraw(playerid, 497.0, 180.0, "_");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_PhoneBody], 0.0, 16.5);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_PhoneBody], 623.0, 0.0);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_PhoneBody], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_PhoneBody], TD_COLOR_WHITE);
    PlayerTextDrawUseBox(playerid, gTD_PlayerText[playerid][pTD_PhoneBody], true);
    PlayerTextDrawBoxColour(playerid, gTD_PlayerText[playerid][pTD_PhoneBody], TD_COLOR_PHONE_BG);
    PlayerTextDrawSetShadow(playerid, gTD_PlayerText[playerid][pTD_PhoneBody], 0);

    gTD_PlayerText[playerid][pTD_PhoneHeader] = CreatePlayerTextDraw(playerid, 560.0, 184.0, "~w~12:45   ~g~5G   ~w~[||||]");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_PhoneHeader], 0.20, 0.85);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_PhoneHeader], TEXT_DRAW_ALIGN_CENTER);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_PhoneHeader], TD_COLOR_WHITE);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_PhoneHeader], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_PhoneHeader], true);

    gTD_PlayerText[playerid][pTD_PhoneTitle] = CreatePlayerTextDraw(playerid, 560.0, 198.0, "~b~~h~SMARTPHONE OS");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_PhoneTitle], 0.24, 1.0);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_PhoneTitle], TEXT_DRAW_ALIGN_CENTER);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_PhoneTitle], TD_COLOR_WHITE);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_PhoneTitle], TEXT_DRAW_FONT_2);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_PhoneTitle], true);

    gTD_PlayerText[playerid][pTD_PhoneInfo] = CreatePlayerTextDraw(playerid, 560.0, 212.0, "~w~No: ~y~554-123");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_PhoneInfo], 0.20, 0.85);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_PhoneInfo], TEXT_DRAW_ALIGN_CENTER);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_PhoneInfo], TD_COLOR_WHITE);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_PhoneInfo], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_PhoneInfo], true);

    gTD_PlayerText[playerid][pTD_PhoneApp1] = CreatePlayerTextDraw(playerid, 502.0, 230.0, "~w~1. Panggilan (/call)");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_PhoneApp1], 0.18, 0.80);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_PhoneApp1], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_PhoneApp1], TD_COLOR_WHITE);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_PhoneApp1], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_PhoneApp1], true);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_PhoneApp1], 615.0, 242.0);
    PlayerTextDrawSetSelectable(playerid, gTD_PlayerText[playerid][pTD_PhoneApp1], true);

    gTD_PlayerText[playerid][pTD_PhoneApp2] = CreatePlayerTextDraw(playerid, 502.0, 244.0, "~w~2. Pesan SMS (/sms)");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_PhoneApp2], 0.18, 0.80);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_PhoneApp2], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_PhoneApp2], TD_COLOR_WHITE);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_PhoneApp2], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_PhoneApp2], true);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_PhoneApp2], 615.0, 256.0);
    PlayerTextDrawSetSelectable(playerid, gTD_PlayerText[playerid][pTD_PhoneApp2], true);

    gTD_PlayerText[playerid][pTD_PhoneApp3] = CreatePlayerTextDraw(playerid, 502.0, 258.0, "~w~3. GPS Peta (/gps)");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_PhoneApp3], 0.18, 0.80);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_PhoneApp3], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_PhoneApp3], TD_COLOR_WHITE);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_PhoneApp3], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_PhoneApp3], true);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_PhoneApp3], 615.0, 270.0);
    PlayerTextDrawSetSelectable(playerid, gTD_PlayerText[playerid][pTD_PhoneApp3], true);

    gTD_PlayerText[playerid][pTD_PhoneApp4] = CreatePlayerTextDraw(playerid, 502.0, 272.0, "~w~4. Info Pulsa & Saldo");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_PhoneApp4], 0.18, 0.80);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_PhoneApp4], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_PhoneApp4], TD_COLOR_WHITE);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_PhoneApp4], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_PhoneApp4], true);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_PhoneApp4], 615.0, 284.0);
    PlayerTextDrawSetSelectable(playerid, gTD_PlayerText[playerid][pTD_PhoneApp4], true);

    gTD_PlayerText[playerid][pTD_PhoneApp5] = CreatePlayerTextDraw(playerid, 502.0, 286.0, "~w~5. Mode: ~g~Dering");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_PhoneApp5], 0.18, 0.80);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_PhoneApp5], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_PhoneApp5], TD_COLOR_WHITE);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_PhoneApp5], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_PhoneApp5], true);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_PhoneApp5], 615.0, 298.0);
    PlayerTextDrawSetSelectable(playerid, gTD_PlayerText[playerid][pTD_PhoneApp5], true);

    gTD_PlayerText[playerid][pTD_PhoneApp6] = CreatePlayerTextDraw(playerid, 502.0, 304.0, "~r~6. Tutup Ponsel (/p)");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_PhoneApp6], 0.18, 0.80);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_PhoneApp6], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_PhoneApp6], TD_COLOR_WHITE);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_PhoneApp6], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_PhoneApp6], true);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_PhoneApp6], 615.0, 316.0);
    PlayerTextDrawSetSelectable(playerid, gTD_PlayerText[playerid][pTD_PhoneApp6], true);

    // Inisialisasi TextDraw Inventaris Modern 25 Slot
    InvTextDraw_Init(playerid);

    // ================= DASHBOARD KENDARAAN (tengah layar) =================
    // Standar LEFT-aligned box: x=170 (kiri), TextSize=470 (kanan, lebar 300).
    gTD_PlayerText[playerid][pTD_VehDashBg] = CreatePlayerTextDraw(playerid, 170.0, 175.0, "_");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_VehDashBg], 0.0, 18.0);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_VehDashBg], 470.0, 0.0);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_VehDashBg], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_VehDashBg], TD_COLOR_WHITE);
    PlayerTextDrawUseBox(playerid, gTD_PlayerText[playerid][pTD_VehDashBg], true);
    PlayerTextDrawBoxColour(playerid, gTD_PlayerText[playerid][pTD_VehDashBg], TD_COLOR_PANEL);
    PlayerTextDrawSetShadow(playerid, gTD_PlayerText[playerid][pTD_VehDashBg], 0);

    gTD_PlayerText[playerid][pTD_VehDashTitle] = CreatePlayerTextDraw(playerid, 320.0, 180.0, "~b~~h~DASHBOARD KONTROL MOBIL");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_VehDashTitle], 0.24, 1.05);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_VehDashTitle], TEXT_DRAW_ALIGN_CENTER);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_VehDashTitle], TD_COLOR_WHITE);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_VehDashTitle], TEXT_DRAW_FONT_2);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_VehDashTitle], true);

    gTD_PlayerText[playerid][pTD_VehDashEngine] = CreatePlayerTextDraw(playerid, 180.0, 204.0, "1. [ MESIN: HIDUP / MATI ]");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_VehDashEngine], 0.19, 0.85);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_VehDashEngine], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_VehDashEngine], TD_COLOR_WHITE);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_VehDashEngine], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_VehDashEngine], true);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_VehDashEngine], 450.0, 216.0);
    PlayerTextDrawSetSelectable(playerid, gTD_PlayerText[playerid][pTD_VehDashEngine], true);

    gTD_PlayerText[playerid][pTD_VehDashLock] = CreatePlayerTextDraw(playerid, 180.0, 222.0, "2. [ KUNCI PINTU: KUNCI / BUKA ]");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_VehDashLock], 0.19, 0.85);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_VehDashLock], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_VehDashLock], TD_COLOR_WHITE);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_VehDashLock], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_VehDashLock], true);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_VehDashLock], 450.0, 234.0);
    PlayerTextDrawSetSelectable(playerid, gTD_PlayerText[playerid][pTD_VehDashLock], true);

    gTD_PlayerText[playerid][pTD_VehDashLights] = CreatePlayerTextDraw(playerid, 180.0, 240.0, "3. [ LAMPU UTAMA: NYALA / MATI ]");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_VehDashLights], 0.19, 0.85);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_VehDashLights], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_VehDashLights], TD_COLOR_WHITE);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_VehDashLights], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_VehDashLights], true);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_VehDashLights], 450.0, 252.0);
    PlayerTextDrawSetSelectable(playerid, gTD_PlayerText[playerid][pTD_VehDashLights], true);

    gTD_PlayerText[playerid][pTD_VehDashHood] = CreatePlayerTextDraw(playerid, 180.0, 258.0, "4. [ KAP MESIN DEPAN: BUKA / TUTUP ]");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_VehDashHood], 0.19, 0.85);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_VehDashHood], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_VehDashHood], TD_COLOR_WHITE);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_VehDashHood], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_VehDashHood], true);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_VehDashHood], 450.0, 270.0);
    PlayerTextDrawSetSelectable(playerid, gTD_PlayerText[playerid][pTD_VehDashHood], true);

    gTD_PlayerText[playerid][pTD_VehDashTrunk] = CreatePlayerTextDraw(playerid, 180.0, 276.0, "5. [ BAGASI BELAKANG: BUKA / TUTUP ]");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_VehDashTrunk], 0.19, 0.85);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_VehDashTrunk], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_VehDashTrunk], TD_COLOR_WHITE);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_VehDashTrunk], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_VehDashTrunk], true);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_VehDashTrunk], 450.0, 288.0);
    PlayerTextDrawSetSelectable(playerid, gTD_PlayerText[playerid][pTD_VehDashTrunk], true);

    gTD_PlayerText[playerid][pTD_VehDashClose] = CreatePlayerTextDraw(playerid, 245.0, 302.0, "~r~[ X. TUTUP PANEL ]");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_VehDashClose], 0.20, 0.90);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_VehDashClose], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_VehDashClose], TD_COLOR_WHITE);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_VehDashClose], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_VehDashClose], true);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_VehDashClose], 395.0, 316.0);
    PlayerTextDrawSetSelectable(playerid, gTD_PlayerText[playerid][pTD_VehDashClose], true);

    // ================= ATM / BANK UI (tengah layar) =================
    // Standar LEFT-aligned box: x=160 (kiri), TextSize=480 (kanan, lebar 320).
    gTD_PlayerText[playerid][pTD_BankBg] = CreatePlayerTextDraw(playerid, 160.0, 160.0, "_");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_BankBg], 0.0, 18.0);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_BankBg], 480.0, 0.0);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_BankBg], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_BankBg], TD_COLOR_WHITE);
    PlayerTextDrawUseBox(playerid, gTD_PlayerText[playerid][pTD_BankBg], true);
    PlayerTextDrawBoxColour(playerid, gTD_PlayerText[playerid][pTD_BankBg], TD_COLOR_PANEL);
    PlayerTextDrawSetShadow(playerid, gTD_PlayerText[playerid][pTD_BankBg], 0);

    gTD_PlayerText[playerid][pTD_BankTitle] = CreatePlayerTextDraw(playerid, 320.0, 166.0, "~b~~h~BANK NUSANTARA ~w~(ATM)");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_BankTitle], 0.26, 1.15);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_BankTitle], TEXT_DRAW_ALIGN_CENTER);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_BankTitle], TD_COLOR_WHITE);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_BankTitle], TEXT_DRAW_FONT_2);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_BankTitle], true);

    gTD_PlayerText[playerid][pTD_BankBalance] = CreatePlayerTextDraw(playerid, 320.0, 186.0, "~g~Saldo Bank: $1000 ~w~| ~y~Tunai: $500");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_BankBalance], 0.20, 0.88);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_BankBalance], TEXT_DRAW_ALIGN_CENTER);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_BankBalance], TD_COLOR_WHITE);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_BankBalance], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_BankBalance], true);

    gTD_PlayerText[playerid][pTD_BankBtnWithdraw] = CreatePlayerTextDraw(playerid, 175.0, 212.0, "1. [ TARIK TUNAI / WITHDRAW ]");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_BankBtnWithdraw], 0.20, 0.90);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_BankBtnWithdraw], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_BankBtnWithdraw], TD_COLOR_CASH);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_BankBtnWithdraw], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_BankBtnWithdraw], true);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_BankBtnWithdraw], 465.0, 226.0);
    PlayerTextDrawSetSelectable(playerid, gTD_PlayerText[playerid][pTD_BankBtnWithdraw], true);

    gTD_PlayerText[playerid][pTD_BankBtnDeposit] = CreatePlayerTextDraw(playerid, 175.0, 234.0, "2. [ SETOR TUNAI / DEPOSIT ]");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_BankBtnDeposit], 0.20, 0.90);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_BankBtnDeposit], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_BankBtnDeposit], TD_COLOR_BANK);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_BankBtnDeposit], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_BankBtnDeposit], true);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_BankBtnDeposit], 465.0, 248.0);
    PlayerTextDrawSetSelectable(playerid, gTD_PlayerText[playerid][pTD_BankBtnDeposit], true);

    gTD_PlayerText[playerid][pTD_BankBtnTransfer] = CreatePlayerTextDraw(playerid, 175.0, 256.0, "3. [ TRANSFER DANA ANTAR WARGA ]");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_BankBtnTransfer], 0.20, 0.90);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_BankBtnTransfer], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_BankBtnTransfer], TD_COLOR_ACCENT);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_BankBtnTransfer], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_BankBtnTransfer], true);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_BankBtnTransfer], 465.0, 270.0);
    PlayerTextDrawSetSelectable(playerid, gTD_PlayerText[playerid][pTD_BankBtnTransfer], true);

    gTD_PlayerText[playerid][pTD_BankBtnClose] = CreatePlayerTextDraw(playerid, 230.0, 290.0, "~r~[ X. KELUARKAN KARTU ]");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_BankBtnClose], 0.20, 0.90);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_BankBtnClose], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_BankBtnClose], TD_COLOR_WHITE);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_BankBtnClose], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_BankBtnClose], true);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_BankBtnClose], 410.0, 304.0);
    PlayerTextDrawSetSelectable(playerid, gTD_PlayerText[playerid][pTD_BankBtnClose], true);

    // ================= MENU INTERAKSI CEPAT (tengah layar) =================
    // Standar LEFT-aligned box: x=170 (kiri), TextSize=470 (kanan, lebar 300).
    gTD_PlayerText[playerid][pTD_ActBg] = CreatePlayerTextDraw(playerid, 170.0, 140.0, "_");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_ActBg], 0.0, 21.0);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_ActBg], 470.0, 0.0);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_ActBg], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_ActBg], TD_COLOR_WHITE);
    PlayerTextDrawUseBox(playerid, gTD_PlayerText[playerid][pTD_ActBg], true);
    PlayerTextDrawBoxColour(playerid, gTD_PlayerText[playerid][pTD_ActBg], TD_COLOR_PANEL);
    PlayerTextDrawSetShadow(playerid, gTD_PlayerText[playerid][pTD_ActBg], 0);

    gTD_PlayerText[playerid][pTD_ActTitle] = CreatePlayerTextDraw(playerid, 320.0, 146.0, "~b~~h~MENU INTERAKSI CEPAT");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_ActTitle], 0.26, 1.15);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_ActTitle], TEXT_DRAW_ALIGN_CENTER);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_ActTitle], TD_COLOR_WHITE);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_ActTitle], TEXT_DRAW_FONT_2);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_ActTitle], true);

    gTD_PlayerText[playerid][pTD_ActBtnInv] = CreatePlayerTextDraw(playerid, 180.0, 172.0, "1. [ INVENTARIS BARANG ]");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_ActBtnInv], 0.19, 0.85);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_ActBtnInv], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_ActBtnInv], TD_COLOR_WHITE);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_ActBtnInv], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_ActBtnInv], true);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_ActBtnInv], 450.0, 184.0);
    PlayerTextDrawSetSelectable(playerid, gTD_PlayerText[playerid][pTD_ActBtnInv], true);

    gTD_PlayerText[playerid][pTD_ActBtnPhone] = CreatePlayerTextDraw(playerid, 180.0, 190.0, "2. [ SMARTPHONE / HP ]");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_ActBtnPhone], 0.19, 0.85);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_ActBtnPhone], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_ActBtnPhone], TD_COLOR_WHITE);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_ActBtnPhone], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_ActBtnPhone], true);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_ActBtnPhone], 450.0, 202.0);
    PlayerTextDrawSetSelectable(playerid, gTD_PlayerText[playerid][pTD_ActBtnPhone], true);

    gTD_PlayerText[playerid][pTD_ActBtnID] = CreatePlayerTextDraw(playerid, 180.0, 208.0, "3. [ KARTU IDENTITAS KTP ]");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_ActBtnID], 0.19, 0.85);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_ActBtnID], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_ActBtnID], TD_COLOR_WHITE);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_ActBtnID], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_ActBtnID], true);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_ActBtnID], 450.0, 220.0);
    PlayerTextDrawSetSelectable(playerid, gTD_PlayerText[playerid][pTD_ActBtnID], true);

    gTD_PlayerText[playerid][pTD_ActBtnVeh] = CreatePlayerTextDraw(playerid, 180.0, 226.0, "4. [ KONTROL KENDARAAN ]");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_ActBtnVeh], 0.19, 0.85);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_ActBtnVeh], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_ActBtnVeh], TD_COLOR_WHITE);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_ActBtnVeh], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_ActBtnVeh], true);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_ActBtnVeh], 450.0, 238.0);
    PlayerTextDrawSetSelectable(playerid, gTD_PlayerText[playerid][pTD_ActBtnVeh], true);

    gTD_PlayerText[playerid][pTD_ActBtnStats] = CreatePlayerTextDraw(playerid, 180.0, 244.0, "5. [ STATISTIK KARAKTER ]");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_ActBtnStats], 0.19, 0.85);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_ActBtnStats], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_ActBtnStats], TD_COLOR_WHITE);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_ActBtnStats], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_ActBtnStats], true);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_ActBtnStats], 450.0, 256.0);
    PlayerTextDrawSetSelectable(playerid, gTD_PlayerText[playerid][pTD_ActBtnStats], true);

    gTD_PlayerText[playerid][pTD_ActBtnHelp] = CreatePlayerTextDraw(playerid, 180.0, 262.0, "6. [ PANDUAN / BANTUAN ]");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_ActBtnHelp], 0.19, 0.85);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_ActBtnHelp], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_ActBtnHelp], TD_COLOR_WHITE);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_ActBtnHelp], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_ActBtnHelp], true);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_ActBtnHelp], 450.0, 274.0);
    PlayerTextDrawSetSelectable(playerid, gTD_PlayerText[playerid][pTD_ActBtnHelp], true);

    gTD_PlayerText[playerid][pTD_ActBtnClose] = CreatePlayerTextDraw(playerid, 250.0, 292.0, "~r~[ X. TUTUP MENU ]");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_ActBtnClose], 0.20, 0.90);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_ActBtnClose], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_ActBtnClose], TD_COLOR_WHITE);
    PlayerTextDrawFont(playerid, gTD_PlayerText[playerid][pTD_ActBtnClose], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gTD_PlayerText[playerid][pTD_ActBtnClose], true);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_ActBtnClose], 390.0, 306.0);
    PlayerTextDrawSetSelectable(playerid, gTD_PlayerText[playerid][pTD_ActBtnClose], true);

    // Setel ulang keadaan berjalan.
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

// Menghancurkan seluruh player-textdraw dan mematikan timer modul ini.
stock DestroyPlayerTextDraws(playerid)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;

    InvTextDraw_Destroy(playerid);
    HidePlayerAlert(playerid);
    HidePlayerSubtitle(playerid);
    HidePlayerSpeedo(playerid);
    HidePlayerHUD(playerid);

    for (new e = pTD_HudPanel; e < pTD_COUNT; e++)
    {
        if (gTD_PlayerText[playerid][e] != INVALID_PLAYER_TEXT_DRAW)
        {
            PlayerTextDrawDestroy(playerid, gTD_PlayerText[playerid][e]);
            gTD_PlayerText[playerid][e] = INVALID_PLAYER_TEXT_DRAW;
        }
    }
    gTD_Ready[playerid] = false;
    return 1;
}

// ---------------------------------------------------------------------------
// HUD
// ---------------------------------------------------------------------------
stock ShowPlayerHUD(playerid)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS || !gTD_Ready[playerid]) return 0;
    if (gTD_HUD[playerid]) return 1;

    for (new e = TD_HUD_FIRST; e <= TD_HUD_LAST; e++)
        PlayerTextDrawShow(playerid, gTD_PlayerText[playerid][e]);

    gTD_HUD[playerid] = true;
    UpdatePlayerHUD(playerid);
    return 1;
}

stock HidePlayerHUD(playerid)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;
    if (!gTD_HUD[playerid]) return 1;

    for (new e = TD_HUD_FIRST; e <= TD_HUD_LAST; e++)
        PlayerTextDrawHide(playerid, gTD_PlayerText[playerid][e]);

    gTD_HUD[playerid] = false;
    return 1;
}

// Menyegarkan seluruh nilai dinamis HUD dari gPlayerData.
stock UpdatePlayerHUD(playerid)
{
    new name[MAX_PLAYER_NAME], line[TD_MAX_TEXT];

    if (playerid < 0 || playerid >= MAX_PLAYERS || !gTD_Ready[playerid]) return 0;
    if (!gTD_HUD[playerid]) return 1;

    GetPlayerName(playerid, name, sizeof(name));
    format(line, sizeof(line), "%s (ID %d)", name, playerid);
    PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_HudNameId], line);

    format(line, sizeof(line), "$%d", gPlayerData[playerid][pMoney]);
    PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_HudCashVal], line);

    format(line, sizeof(line), "$%d", gPlayerData[playerid][pBank]);
    PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_HudBankVal], line);

    TD_GetPercentBar(line, sizeof(line), gPlayerData[playerid][pHunger]);
    PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_HudHungerBar], line);

    TD_GetPercentBar(line, sizeof(line), gPlayerData[playerid][pThirst]);
    PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_HudThirstBar], line);

    format(line, sizeof(line), "%d", gPlayerData[playerid][pLevel]);
    PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_HudLevelVal], line);
    return 1;
}

// ---------------------------------------------------------------------------
// SPEEDOMETER
// ---------------------------------------------------------------------------
stock ShowPlayerSpeedo(playerid)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS || !gTD_Ready[playerid]) return 0;
    if (gTD_Speedo[playerid]) return 1;

    for (new e = TD_SPD_FIRST; e <= TD_SPD_LAST; e++)
        PlayerTextDrawShow(playerid, gTD_PlayerText[playerid][e]);

    gTD_Speedo[playerid] = true;
    UpdatePlayerSpeedo(playerid);

    // Pasang timer cadangan hanya bila timer global belum berjalan, agar
    // refresh tidak dobel kalau roleplay.pwn sudah memasang RP_SpeedoTick.
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

    if (gTD_SpdTimer[playerid] != 0)
    {
        KillTimer(gTD_SpdTimer[playerid]);
        gTD_SpdTimer[playerid] = 0;
    }
    return 1;
}

// Dipanggil oleh timer (global RP_SpeedoTick maupun cadangan per pemain).
stock UpdatePlayerSpeedo(playerid)
{
    new line[TD_MAX_TEXT], vehname[TD_VEH_NAME_MAX];
    new engine, doors, Float:fuel;

    if (playerid < 0 || playerid >= MAX_PLAYERS || !gTD_Ready[playerid]) return 0;
    if (!gTD_Speedo[playerid]) return 1;

    new vehicleid = GetPlayerVehicleID(playerid);
    if (vehicleid < 1 || GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
    {
        // Turun dari kursi sopir (atau kendaraan hilang) - panel diturunkan.
        HidePlayerSpeedo(playerid);
        return 1;
    }

    // Nama kendaraan: hanya ditulis ulang saat kendaraannya benar-benar berganti.
    if (gTD_SpdVehicle[playerid] != vehicleid)
    {
        gTD_SpdVehicle[playerid] = vehicleid;
        GetVehicleModelName(GetVehicleModel(vehicleid), vehname, sizeof(vehname));
        PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_SpdVehName], vehname);
    }

    // Kecepatan (KM/J).
    format(line, sizeof(line), "%d KM/J", floatround(TD_GetVehicleSpeedKmh(vehicleid), floatround_round));
    PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_SpdSpeedVal], line);

    // Bahan bakar - kendaraan pribadi saja, selain itu jujur bilang N/A.
    if (TD_GetOwnedVehicleFuel(vehicleid, fuel))
    {
        TD_GetPercentBar(line, sizeof(line), fuel);
        PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_SpdFuelBar], line);
        PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SpdFuelBar], TD_COLOR_HUNGER);
    }
    else
    {
        PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_SpdFuelBar], "N/A");
        PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SpdFuelBar], TD_COLOR_MUTED);
    }

    // Mesin + pintu dari keadaan game terkini.
    TD_GetVehicleParamsSafe(vehicleid, engine, doors);

    if (engine)
    {
        PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_SpdEngineVal], "MENYALA");
        PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SpdEngineVal], TD_COLOR_ENGINE_ON);
    }
    else
    {
        PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_SpdEngineVal], "MATI");
        PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SpdEngineVal], TD_COLOR_ENGINE_OFF);
    }

    if (doors)
    {
        PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_SpdLockVal], "TERKUNCI");
        PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SpdLockVal], TD_COLOR_LOCK_ON);
    }
    else
    {
        PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_SpdLockVal], "BEBAS");
        PlayerTextDrawColour(playerid, gTD_PlayerText[playerid][pTD_SpdLockVal], TD_COLOR_LOCK_OFF);
    }
    return 1;
}

// Tick global: segarkan speedometer semua pemain yang sedang menampilkan.
forward RP_SpeedoTick();
public RP_SpeedoTick()
{
    for (new playerid = 0; playerid < MAX_PLAYERS; playerid++)
    {
        if (gTD_Speedo[playerid])
            UpdatePlayerSpeedo(playerid);
    }
    return 1;
}

// ---------------------------------------------------------------------------
// ALERT (banner notifikasi)
// ---------------------------------------------------------------------------
// Menampilkan pesan selama time_ms, lalu menyembunyikan sendiri.
stock ShowPlayerAlert(playerid, const message[])
{
    return ShowPlayerAlertEx(playerid, message, TD_ALERT_DEFAULT_MS);
}

stock ShowPlayerAlertEx(playerid, const message[], time_ms = TD_ALERT_DEFAULT_MS)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS || !gTD_Ready[playerid]) return 0;
    if (time_ms < 500) time_ms = 500;

    // Teks dulu supaya banner yang sedang tampil langsung berganti, bukan kedip.
    PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_AlertText], message);
    PlayerTextDrawShow(playerid, gTD_PlayerText[playerid][pTD_AlertPanel]);
    PlayerTextDrawShow(playerid, gTD_PlayerText[playerid][pTD_AlertText]);
    gTD_Alert[playerid] = true;

    // Pasang ulang timer auto-hide.
    if (gAlertTimer[playerid] != 0) KillTimer(gAlertTimer[playerid]);
    gAlertTimer[playerid] = SetTimerEx("RP_HideAlert", time_ms, false, "i", playerid);
    return 1;
}

stock HidePlayerAlert(playerid)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;

    if (gAlertTimer[playerid] != 0)
    {
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

// ---------------------------------------------------------------------------
// SUBTITLE (pesan roleplay singkat, di atas alert)
// ---------------------------------------------------------------------------
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

    if (gSubtitleTimer[playerid] != 0)
    {
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
// TITIK MASUK (dipanggil dari roleplay.pwn)
// ---------------------------------------------------------------------------
// OnPlayerConnect: buat semua textdraw.
forward TextdrawCreateAll(playerid);
public TextdrawCreateAll(playerid)
{
    return InitPlayerTextDraws(playerid);
}

// OnPlayerDisconnect: matikan timer dan hancurkan semua textdraw.
forward TextdrawDestroyAll(playerid);
public TextdrawDestroyAll(playerid)
{
    return DestroyPlayerTextDraws(playerid);
}

// ---------------------------------------------------------------------------
// SMARTPHONE & INVENTARIS UI KONTROL
// ---------------------------------------------------------------------------
stock ShowPlayerPhone(playerid)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;
    UpdatePlayerPhone(playerid);
    for (new e = TD_PHONE_FIRST; e <= TD_PHONE_LAST; e++) {
        PlayerTextDrawShow(playerid, gTD_PlayerText[playerid][e]);
    }
    gPhoneOpen[playerid] = true;
    SelectTextDraw(playerid, 0x33CCFFFF);
    PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
    return 1;
}

stock HidePlayerPhone(playerid)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;
    for (new e = TD_PHONE_FIRST; e <= TD_PHONE_LAST; e++) {
        PlayerTextDrawHide(playerid, gTD_PlayerText[playerid][e]);
    }
    gPhoneOpen[playerid] = false;
    CancelSelectTextDraw(playerid);
    return 1;
}

stock UpdatePlayerPhone(playerid)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;
    new infoBuf[64];
    format(infoBuf, sizeof(infoBuf), "~w~No: ~y~%d", gPlayerData[playerid][pPhone]);
    PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_PhoneInfo], infoBuf);

    new modeBuf[48];
    format(modeBuf, sizeof(modeBuf), "~w~5. Mode: %s", gPlayerData[playerid][pPhoneStatus] ? "~g~Dering" : "~r~Hening");
    PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_PhoneApp5], modeBuf);
    return 1;
}

stock ShowPlayerInventoryUI(playerid)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;
    gSelectedInvSlot[playerid] = 0;
    InvTextDraw_Show(playerid);
    gInventoryOpen[playerid] = true;
    SelectTextDraw(playerid, 0x33CCFFFF);
    PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
    return 1;
}

stock HidePlayerInventoryUI(playerid)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;
    InvTextDraw_Hide(playerid);
    gInventoryOpen[playerid] = false;
    CancelSelectTextDraw(playerid);
    return 1;
}

stock UpdatePlayerInventoryUI(playerid)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;
    InvTextDraw_Update(playerid);
    return 1;
}

stock ShowPlayerVehDash(playerid)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;
    UpdatePlayerVehDash(playerid);
    for (new e = TD_VEHDASH_FIRST; e <= TD_VEHDASH_LAST; e++) {
        PlayerTextDrawShow(playerid, gTD_PlayerText[playerid][e]);
    }
    gVehDashOpen[playerid] = true;
    SelectTextDraw(playerid, 0x33CCFFFF);
    PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
    return 1;
}

stock HidePlayerVehDash(playerid)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;
    for (new e = TD_VEHDASH_FIRST; e <= TD_VEHDASH_LAST; e++) {
        PlayerTextDrawHide(playerid, gTD_PlayerText[playerid][e]);
    }
    gVehDashOpen[playerid] = false;
    CancelSelectTextDraw(playerid);
    return 1;
}

stock UpdatePlayerVehDash(playerid)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;
    new veh = GetPlayerVehicleID(playerid);
    new engine, lights, alarm, doors, bonnet, boot, objective;
    if (veh > 0) GetVehicleParamsEx(veh, engine, lights, alarm, doors, bonnet, boot, objective);

    new str[64];
    format(str, sizeof(str), "1. [ MESIN: %s~w~ ]", (engine == 1) ? "~g~HIDUP" : "~r~MATI");
    PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_VehDashEngine], str);

    format(str, sizeof(str), "2. [ KUNCI: %s~w~ ]", (doors == 1) ? "~y~TERKUNCI" : "~w~BEBAS");
    PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_VehDashLock], str);

    format(str, sizeof(str), "3. [ LAMPU: %s~w~ ]", (lights == 1) ? "~g~NYALA" : "~r~PADAM");
    PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_VehDashLights], str);

    format(str, sizeof(str), "4. [ KAP MESIN: %s~w~ ]", (bonnet == 1) ? "~g~BUKA" : "~w~TUTUP");
    PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_VehDashHood], str);

    format(str, sizeof(str), "5. [ BAGASI: %s~w~ ]", (boot == 1) ? "~g~BUKA" : "~w~TUTUP");
    PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_VehDashTrunk], str);
    return 1;
}

stock ShowPlayerBankUI(playerid)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;
    new balBuf[80];
    format(balBuf, sizeof(balBuf), "~g~Saldo Bank: $%d ~w~| ~y~Tunai: $%d",
        gPlayerData[playerid][pBank], gPlayerData[playerid][pMoney]);
    PlayerTextDrawSetString(playerid, gTD_PlayerText[playerid][pTD_BankBalance], balBuf);

    for (new e = TD_BANK_FIRST; e <= TD_BANK_LAST; e++) {
        PlayerTextDrawShow(playerid, gTD_PlayerText[playerid][e]);
    }
    gBankUIOpen[playerid] = true;
    SelectTextDraw(playerid, 0x33CCFFFF);
    PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
    return 1;
}

stock HidePlayerBankUI(playerid)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;
    for (new e = TD_BANK_FIRST; e <= TD_BANK_LAST; e++) {
        PlayerTextDrawHide(playerid, gTD_PlayerText[playerid][e]);
    }
    gBankUIOpen[playerid] = false;
    CancelSelectTextDraw(playerid);
    return 1;
}

stock ShowPlayerActMenu(playerid)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;
    for (new e = TD_ACT_FIRST; e <= TD_ACT_LAST; e++) {
        PlayerTextDrawShow(playerid, gTD_PlayerText[playerid][e]);
    }
    gActMenuOpen[playerid] = true;
    SelectTextDraw(playerid, 0x33CCFFFF);
    PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
    return 1;
}

stock HidePlayerActMenu(playerid)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;
    for (new e = TD_ACT_FIRST; e <= TD_ACT_LAST; e++) {
        PlayerTextDrawHide(playerid, gTD_PlayerText[playerid][e]);
    }
    gActMenuOpen[playerid] = false;
    CancelSelectTextDraw(playerid);
    return 1;
}

// ---------------------------------------------------------------------------
// MASTER CLICK ROUTER
// ---------------------------------------------------------------------------
stock ProcessPlayerClickTD(playerid, PlayerText:playertextid)
{
    if (playertextid == PlayerText:INVALID_TEXT_DRAW) {
        // Debounce: abaikan penutupan jika ada klik valid di 200ms terakhir
        if (GetTickCount() - gLastValidClickTick[playerid] < 200) {
            return 1;
        }
        if (gPhoneOpen[playerid]) HidePlayerPhone(playerid);
        if (gInventoryOpen[playerid]) HidePlayerInventoryUI(playerid);
        if (gVehDashOpen[playerid]) HidePlayerVehDash(playerid);
        if (gBankUIOpen[playerid]) HidePlayerBankUI(playerid);
        if (gActMenuOpen[playerid]) HidePlayerActMenu(playerid);
        return 1;
    }

    // Catat timestamp klik valid
    gLastValidClickTick[playerid] = GetTickCount();

    // 1. SMARTPHONE APPS
    if (gPhoneOpen[playerid]) {
        if (playertextid == gTD_PlayerText[playerid][pTD_PhoneApp1]) {
            HidePlayerPhone(playerid);
            ShowPlayerDialog(playerid, DIALOG_PHONE_CALL, DIALOG_STYLE_INPUT,
                "PANGGILAN SUARA", "Masukkan nomor telepon tujuan:", "Panggil", "Batal");
            return 1;
        }
        if (playertextid == gTD_PlayerText[playerid][pTD_PhoneApp2]) {
            HidePlayerPhone(playerid);
            ShowPlayerDialog(playerid, DIALOG_PHONE_SMS, DIALOG_STYLE_INPUT,
                "KIRIM PESAN SMS", "Format: [Nomor] [Pesan]", "Kirim", "Batal");
            return 1;
        }
        if (playertextid == gTD_PlayerText[playerid][pTD_PhoneApp3]) {
            HidePlayerPhone(playerid);
            ShowGPSMenu(playerid);
            return 1;
        }
        if (playertextid == gTD_PlayerText[playerid][pTD_PhoneApp4]) {
            ShowPlayerAlertEx(playerid, "Pulsa Ponsel: Aktif & Tidak Terbatas", 4000);
            return 1;
        }
        if (playertextid == gTD_PlayerText[playerid][pTD_PhoneApp5]) {
            gPlayerData[playerid][pPhoneStatus] = !gPlayerData[playerid][pPhoneStatus];
            UpdatePlayerPhone(playerid);
            ShowPlayerAlertEx(playerid, gPlayerData[playerid][pPhoneStatus] ? "Mode Dering Aktif" : "Mode Hening Aktif", 2500);
            return 1;
        }
        if (playertextid == gTD_PlayerText[playerid][pTD_PhoneApp6]) {
            HidePlayerPhone(playerid);
            return 1;
        }
    }

    // 2. INVENTORY SLOTS (25 SLOTS) & ACTIONS
    if (gInventoryOpen[playerid]) {
        for (new s = 0; s < 25; s++) {
            if (playertextid == INDEXTD[playerid][s]) {
                gSelectedInvSlot[playerid] = s;
                InvTextDraw_Update(playerid);
                PlayerPlaySound(playerid, 1083, 0.0, 0.0, 0.0);
                return 1;
            }
        }

        // Amount info / split (INVINFO 1)
        if (playertextid == INVINFO[playerid][1]) {
            new slot = gSelectedInvSlot[playerid];
            if (gPlayerItems[playerid][slot][itType] != ITEM_NONE) {
                new prompt[140];
                format(prompt, sizeof(prompt), "{FFFFFF}Jumlah item saat ini: {FFFF00}%d buah{FFFFFF}.\nSlot: #%d",
                    gPlayerItems[playerid][slot][itAmount], slot + 1);
                ShowPlayerDialog(playerid, DIALOG_HELP, DIALOG_STYLE_MSGBOX, "JUMLAH ITEM", prompt, "Tutup", "");
            }
            return 1;
        }

        // Use (INVINFO 2)
        if (playertextid == INVINFO[playerid][2]) {
            UsePlayerItemBySlot(playerid, gSelectedInvSlot[playerid]);
            InvTextDraw_Update(playerid);
            return 1;
        }

        // Give (INVINFO 3)
        if (playertextid == INVINFO[playerid][3]) {
            HidePlayerInventoryUI(playerid);
            ShowPlayerDialog(playerid, DIALOG_GIVE_PLAYER, DIALOG_STYLE_INPUT,
                "BERIKAN BARANG", "Masukkan ID/Nama Pemain dan Jumlah:\nContoh: 2 1 (Memberikan 1 barang ke ID 2)",
                "Kirim", "Batal");
            return 1;
        }

        // Drop (INVINFO 4)
        if (playertextid == INVINFO[playerid][4]) {
            new slot = gSelectedInvSlot[playerid];
            if (gPlayerItems[playerid][slot][itType] != ITEM_NONE) {
                TakePlayerItem(playerid, gPlayerItems[playerid][slot][itType], 1);
                SendInfo(playerid, "Barang telah dibuang dari kantong.");
                InvTextDraw_Update(playerid);
            }
            return 1;
        }

        // Close (INVINFO 5)
        if (playertextid == INVINFO[playerid][5]) {
            HidePlayerInventoryUI(playerid);
            return 1;
        }
    }

    // 3. VEHICLE CONTROL DASHBOARD
    if (gVehDashOpen[playerid]) {
        new veh = GetPlayerVehicleID(playerid);
        if (playertextid == gTD_PlayerText[playerid][pTD_VehDashEngine]) {
            ProcessEngineToggle(playerid);
            UpdatePlayerVehDash(playerid);
            return 1;
        }
        if (playertextid == gTD_PlayerText[playerid][pTD_VehDashLock]) {
            if (veh > 0) {
                new engine, lights, alarm, doors, bonnet, boot, objective;
                GetVehicleParamsEx(veh, engine, lights, alarm, doors, bonnet, boot, objective);
                SetVehicleParamsEx(veh, engine, lights, alarm, (doors == 1) ? 0 : 1, bonnet, boot, objective);
                UpdatePlayerVehDash(playerid);
                ShowPlayerAlertEx(playerid, (doors == 1) ? "Pintu Kendaraan Dibuka" : "Pintu Kendaraan Dikunci", 2500);
            }
            return 1;
        }
        if (playertextid == gTD_PlayerText[playerid][pTD_VehDashLights]) {
            if (veh > 0) {
                new engine, lights, alarm, doors, bonnet, boot, objective;
                GetVehicleParamsEx(veh, engine, lights, alarm, doors, bonnet, boot, objective);
                SetVehicleParamsEx(veh, engine, (lights == 1) ? 0 : 1, alarm, doors, bonnet, boot, objective);
                UpdatePlayerVehDash(playerid);
            }
            return 1;
        }
        if (playertextid == gTD_PlayerText[playerid][pTD_VehDashHood]) {
            if (veh > 0) {
                new engine, lights, alarm, doors, bonnet, boot, objective;
                GetVehicleParamsEx(veh, engine, lights, alarm, doors, bonnet, boot, objective);
                SetVehicleParamsEx(veh, engine, lights, alarm, doors, (bonnet == 1) ? 0 : 1, boot, objective);
                UpdatePlayerVehDash(playerid);
            }
            return 1;
        }
        if (playertextid == gTD_PlayerText[playerid][pTD_VehDashTrunk]) {
            if (veh > 0) {
                new engine, lights, alarm, doors, bonnet, boot, objective;
                GetVehicleParamsEx(veh, engine, lights, alarm, doors, bonnet, boot, objective);
                SetVehicleParamsEx(veh, engine, lights, alarm, doors, bonnet, (boot == 1) ? 0 : 1, objective);
                UpdatePlayerVehDash(playerid);
            }
            return 1;
        }
        if (playertextid == gTD_PlayerText[playerid][pTD_VehDashClose]) {
            HidePlayerVehDash(playerid);
            return 1;
        }
    }

    // 4. BANK / ATM INTERACTION
    if (gBankUIOpen[playerid]) {
        if (playertextid == gTD_PlayerText[playerid][pTD_BankBtnWithdraw]) {
            HidePlayerBankUI(playerid);
            ShowPlayerDialog(playerid, DIALOG_ATM_WITHDRAW, DIALOG_STYLE_INPUT,
                "TARIK TUNAI ATM", "Masukkan nominal uang yang ingin ditarik:", "Tarik", "Batal");
            return 1;
        }
        if (playertextid == gTD_PlayerText[playerid][pTD_BankBtnDeposit]) {
            HidePlayerBankUI(playerid);
            ShowPlayerDialog(playerid, DIALOG_ATM_DEPOSIT, DIALOG_STYLE_INPUT,
                "SETOR TUNAI ATM", "Masukkan nominal uang yang ingin disetor:", "Setor", "Batal");
            return 1;
        }
        if (playertextid == gTD_PlayerText[playerid][pTD_BankBtnTransfer]) {
            HidePlayerBankUI(playerid);
            ShowPlayerDialog(playerid, DIALOG_BANK_TRANSFER, DIALOG_STYLE_INPUT,
                "TRANSFER DANA", "Format: [ID Pemain] [Jumlah Uang]", "Kirim", "Batal");
            return 1;
        }
        if (playertextid == gTD_PlayerText[playerid][pTD_BankBtnClose]) {
            HidePlayerBankUI(playerid);
            return 1;
        }
    }

    // 5. QUICK ACTION MENU
    if (gActMenuOpen[playerid]) {
        if (playertextid == gTD_PlayerText[playerid][pTD_ActBtnInv]) {
            HidePlayerActMenu(playerid);
            ShowPlayerInventoryUI(playerid);
            return 1;
        }
        if (playertextid == gTD_PlayerText[playerid][pTD_ActBtnPhone]) {
            HidePlayerActMenu(playerid);
            ShowPlayerPhone(playerid);
            return 1;
        }
        if (playertextid == gTD_PlayerText[playerid][pTD_ActBtnID]) {
            HidePlayerActMenu(playerid);
            ShowStatsDialog(playerid);
            return 1;
        }
        if (playertextid == gTD_PlayerText[playerid][pTD_ActBtnVeh]) {
            HidePlayerActMenu(playerid);
            ShowPlayerVehDash(playerid);
            return 1;
        }
        if (playertextid == gTD_PlayerText[playerid][pTD_ActBtnStats]) {
            HidePlayerActMenu(playerid);
            ShowStatsDialog(playerid);
            return 1;
        }
        if (playertextid == gTD_PlayerText[playerid][pTD_ActBtnHelp]) {
            HidePlayerActMenu(playerid);
            ShowHelpDialog(playerid);
            return 1;
        }
        if (playertextid == gTD_PlayerText[playerid][pTD_ActBtnClose]) {
            HidePlayerActMenu(playerid);
            return 1;
        }
    }
    return 1;
}
