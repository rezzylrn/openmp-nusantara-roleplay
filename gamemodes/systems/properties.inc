#if defined _INC_rp_properties
	#endinput
#endif
#define _INC_rp_properties

// ---------------------------------------------------------------------------
// Dynamic Housing & Property Real Estate System
// ---------------------------------------------------------------------------

#define MAX_HOUSES              30
#define HOUSE_STORAGE_SLOTS     6

enum E_HOUSE_DATA {
	bool:hUsed,
	hDBID,
	hOwnerID,
	hOwnerName[24],
	hAddress[48],
	Float:hEnterX,
	Float:hEnterY,
	Float:hEnterZ,
	Float:hExitX,
	Float:hExitY,
	Float:hExitZ,
	hInterior,
	hPrice,
	bool:hLocked,
	hStorageCash,
	Text3D:hLabel,
	hPickup
};

new gHouseData[MAX_HOUSES][E_HOUSE_DATA];

// daftar rumah default los santos
static const Float:gDefaultHouseSpawns[6][7] = {
	// EnterX, EnterY, EnterZ, Price, ExitX, ExitY, ExitZ
	{2495.3, -1690.8, 13.5, 20000.0, 2496.0, -1692.0, 1014.7}, // Ganton Grove St
	{2262.1, -1647.5, 15.2, 28000.0, 2260.0, -1645.0, 1014.7}, // Ganton Timur
	{1978.2, -1582.4, 13.4, 22000.0, 235.2, 1186.7, 1080.2},   // Idlewood
	{1142.0, -2040.5, 13.8, 45000.0, 225.7, 1021.4, 1084.0},   // Verona Beach
	{834.1, -1102.3, 24.3, 65000.0, 235.2, 1186.7, 1080.2},    // Vinewood Hills
	{2140.5, -1193.2, 24.0, 32000.0, 2260.0, -1645.0, 1014.7}  // Jefferson
};

static const gDefaultHouseNames[6][32] = {
	"101 Grove Street",
	"104 East Ganton Blvd",
	"205 Idlewood Avenue",
	"502 Verona Beach Pierway",
	"701 Vinewood Hilltop",
	"303 Jefferson Parkway"
};

stock House_Init()
{
	for (new i = 0; i < 6; i++) {
		gHouseData[i][hUsed] = true;
		gHouseData[i][hDBID] = i + 1;
		gHouseData[i][hOwnerID] = 0;
		gHouseData[i][hLabel] = Text3D:INVALID_3DTEXT_ID;
		gHouseData[i][hPickup] = -1;
		format(gHouseData[i][hOwnerName], 24, "Pemerintah");
		format(gHouseData[i][hAddress], 48, "%s", gDefaultHouseNames[i]);

		gHouseData[i][hEnterX] = gDefaultHouseSpawns[i][0];
		gHouseData[i][hEnterY] = gDefaultHouseSpawns[i][1];
		gHouseData[i][hEnterZ] = gDefaultHouseSpawns[i][2];
		gHouseData[i][hPrice] = floatround(gDefaultHouseSpawns[i][3]);

		gHouseData[i][hExitX] = gDefaultHouseSpawns[i][4];
		gHouseData[i][hExitY] = gDefaultHouseSpawns[i][5];
		gHouseData[i][hExitZ] = gDefaultHouseSpawns[i][6];
		gHouseData[i][hInterior] = 3;

		gHouseData[i][hLocked] = true;
		gHouseData[i][hStorageCash] = 0;

		House_RefreshDisplay(i);
	}
	print("[PROPERTIES] 6 Rumah & Real Estate Kota Los Santos diinisialisasi.");
	return 1;
}

stock House_RefreshDisplay(id)
{
	if (id < 0 || id >= MAX_HOUSES || !gHouseData[id][hUsed]) return 0;

	if (gHouseData[id][hLabel] != Text3D:INVALID_3DTEXT_ID) {
		Delete3DTextLabel(gHouseData[id][hLabel]);
		gHouseData[id][hLabel] = Text3D:INVALID_3DTEXT_ID;
	}
	if (gHouseData[id][hPickup] != -1) {
		DestroyPickup(gHouseData[id][hPickup]);
		gHouseData[id][hPickup] = -1;
	}

	new labelStr[256];
	if (gHouseData[id][hOwnerID] == 0) {
		format(labelStr, sizeof(labelStr),
			"{33CCFF}[ RUMAH DIJUAL ]\n{FFFFFF}Alamat: {FFFF00}%s\n{FFFFFF}Harga: {2ECC71}$%d\n{FFFFFF}Tekan {FFFF00}Y {FFFFFF}untuk membeli rumah",
			gHouseData[id][hAddress], gHouseData[id][hPrice]);
		gHouseData[id][hPickup] = CreatePickup(1273, 1, gHouseData[id][hEnterX], gHouseData[id][hEnterY], gHouseData[id][hEnterZ]);
	} else {
		format(labelStr, sizeof(labelStr),
			"{33CCFF}[ PROPERTI RUMAH ]\n{FFFFFF}Alamat: {FFFF00}%s\n{FFFFFF}Pemilik: {FFFF00}%s\n{FFFFFF}Status: %s\n{FFFFFF}Tekan {FFFF00}Y {FFFFFF}untuk masuk",
			gHouseData[id][hAddress], gHouseData[id][hOwnerName],
			gHouseData[id][hLocked] ? "{E74C3C}Terkunci" : "{2ECC71}Terbuka");
		gHouseData[id][hPickup] = CreatePickup(1272, 1, gHouseData[id][hEnterX], gHouseData[id][hEnterY], gHouseData[id][hEnterZ]);
	}

	gHouseData[id][hLabel] = Create3DTextLabel(labelStr, COLOR_LIGHTBLUE,
		gHouseData[id][hEnterX], gHouseData[id][hEnterY], gHouseData[id][hEnterZ] + 0.8, 12.0, 0, false);
	return 1;
}

stock House_CheckNearby(playerid)
{
	if (!IsPlayerConnected(playerid)) return -1;
	for (new i = 0; i < MAX_HOUSES; i++) {
		if (!gHouseData[i][hUsed]) continue;
		if (IsPlayerInRangeOfPoint(playerid, 2.5, gHouseData[i][hEnterX], gHouseData[i][hEnterY], gHouseData[i][hEnterZ])) {
			return i;
		}
	}
	return -1;
}

stock House_CheckInside(playerid)
{
	if (!IsPlayerConnected(playerid)) return -1;
	new vw = GetPlayerVirtualWorld(playerid);
	if (vw > 100 && vw <= 100 + MAX_HOUSES) {
		new hIdx = vw - 101;
		if (gHouseData[hIdx][hUsed] && IsPlayerInRangeOfPoint(playerid, 3.0, gHouseData[hIdx][hExitX], gHouseData[hIdx][hExitY], gHouseData[hIdx][hExitZ])) {
			return hIdx;
		}
	}
	return -1;
}

stock ProcessHouseInteraction(playerid)
{
	new hEnter = House_CheckNearby(playerid);
	if (hEnter != -1) {
		// Pemain berada di depan pintu masuk rumah
		if (gHouseData[hEnter][hOwnerID] == 0) {
			// Rumah dijual -> Buka dialog beli
			new str[256];
			format(str, sizeof(str),
				"{FFFFFF}Alamat: {FFFF00}%s\n{FFFFFF}Harga Rumah: {2ECC71}$%d\n\n{FFFFFF}Apakah Anda yakin ingin membeli properti rumah ini?",
				gHouseData[hEnter][hAddress], gHouseData[hEnter][hPrice]);
			gSelectedInvSlot[playerid] = hEnter;
			ShowPlayerDialog(playerid, DIALOG_HELP, DIALOG_STYLE_MSGBOX, "BELI PROPERTI RUMAH", str, "Beli", "Batal");
			return 1;
		} else {
			// Rumah sudah ada pemilik
			if (gHouseData[hEnter][hLocked] && gHouseData[hEnter][hOwnerID] != gPlayerData[playerid][pID]) {
				SendErr(playerid, "Pintu rumah ini terkunci oleh pemiliknya.");
				return 1;
			}

			// Masuk ke interior rumah
			SetPlayerInterior(playerid, gHouseData[hEnter][hInterior]);
			SetPlayerVirtualWorld(playerid, 101 + hEnter);
			SetPlayerPos(playerid, gHouseData[hEnter][hExitX], gHouseData[hEnter][hExitY], gHouseData[hEnter][hExitZ]);
			SendInfo(playerid, "Anda memasuki rumah (%s).", gHouseData[hEnter][hAddress]);
			return 1;
		}
	}

	new hInside = House_CheckInside(playerid);
	if (hInside != -1) {
		// Keluar dari rumah
		SetPlayerInterior(playerid, 0);
		SetPlayerVirtualWorld(playerid, 0);
		SetPlayerPos(playerid, gHouseData[hInside][hEnterX], gHouseData[hInside][hEnterY], gHouseData[hInside][hEnterZ]);
		SendInfo(playerid, "Anda keluar dari rumah.");
		return 1;
	}
	return 0;
}
