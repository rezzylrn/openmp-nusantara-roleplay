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
// --- penanda batas (harus terakhir) ---
#define pTD_COUNT           27

// Index pertama/terakhir tiap panel, untuk loop show/hide.
#define TD_HUD_FIRST   pTD_HudPanel
#define TD_HUD_LAST    pTD_HudLevelVal
#define TD_SPD_FIRST   pTD_SpdPanel
#define TD_SPD_LAST    pTD_SpdLockVal

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
    // Rata kanan: x Create = tepi kanan (620), x TextSize = tepi kiri (440).
    gTD_PlayerText[playerid][pTD_HudPanel] = CreatePlayerTextDraw(playerid, 620.0, 34.0, "_");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_HudPanel], 0.0, 11.6);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_HudPanel], 440.0, 0.0);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_HudPanel], TEXT_DRAW_ALIGN_RIGHT);
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
    gTD_PlayerText[playerid][pTD_SpdPanel] = CreatePlayerTextDraw(playerid, 620.0, 330.0, "_");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_SpdPanel], 0.0, 9.4);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_SpdPanel], 440.0, 0.0);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_SpdPanel], TEXT_DRAW_ALIGN_RIGHT);
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
    // Rata tengah: x Create = pusat, x TextSize = lebar total box.
    gTD_PlayerText[playerid][pTD_AlertPanel] = CreatePlayerTextDraw(playerid, 320.0, 418.0, "_");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_AlertPanel], 0.0, 2.6);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_AlertPanel], 380.0, 0.0);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_AlertPanel], TEXT_DRAW_ALIGN_CENTER);
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
    gTD_PlayerText[playerid][pTD_SubPanel] = CreatePlayerTextDraw(playerid, 320.0, 392.0, "_");
    PlayerTextDrawLetterSize(playerid, gTD_PlayerText[playerid][pTD_SubPanel], 0.0, 2.2);
    PlayerTextDrawTextSize(playerid, gTD_PlayerText[playerid][pTD_SubPanel], 330.0, 0.0);
    PlayerTextDrawAlignment(playerid, gTD_PlayerText[playerid][pTD_SubPanel], TEXT_DRAW_ALIGN_CENTER);
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
