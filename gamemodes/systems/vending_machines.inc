#if defined _INC_vending_machines
	#endinput
#endif
#define _INC_vending_machines

/*
==============================================================================
 vending_machines.inc
 NUSANTARA ROLEPLAY - Interactive Sidewalk Snack & Beverage Vending Machines
==============================================================================
*/

#define MAX_VENDING_LOCATIONS   6
#define RANGE_VENDING_MACHINE   3.0

enum E_VENDING_LOC {
	Float:vmX,
	Float:vmY,
	Float:vmZ,
	Float:vmA,
	vmModel, // 1775 = Sprunk, 1977 = Snack
	vmName[48],
	vmObject,
	Text3D:vmLabel
};

new gVendingMachines[MAX_VENDING_LOCATIONS][E_VENDING_LOC] = {
	{1785.0, -1880.0, 13.5, 0.0, 1775, "Mesin Sprunk Unity Station", INVALID_OBJECT_ID, Text3D:INVALID_3DTEXT_ID},
	{1485.0, -1720.0, 13.5, 90.0, 1977, "Mesin Snack Pershing Square", INVALID_OBJECT_ID, Text3D:INVALID_3DTEXT_ID},
	{1178.0, -1330.0, 14.0, 180.0, 1775, "Mesin Sprunk ASGH Hospital", INVALID_OBJECT_ID, Text3D:INVALID_3DTEXT_ID},
	{385.0,  -2040.0, 8.0,  270.0, 1977, "Mesin Snack Santa Maria Pier", INVALID_OBJECT_ID, Text3D:INVALID_3DTEXT_ID},
	{1938.0, -1772.0, 13.5, 0.0, 1775, "Mesin Sprunk SPBU Idlewood", INVALID_OBJECT_ID, Text3D:INVALID_3DTEXT_ID},
	{1360.0, -1755.0, 13.5, 90.0, 1977, "Mesin Snack Commerce Strip", INVALID_OBJECT_ID, Text3D:INVALID_3DTEXT_ID}
};

stock VendingMachines_Init()
{
	for (new i = 0; i < MAX_VENDING_LOCATIONS; i++) {
		// Spawn objek vending jika belum ada
		gVendingMachines[i][vmObject] = CreateObject(gVendingMachines[i][vmModel],
			gVendingMachines[i][vmX],
			gVendingMachines[i][vmY],
			gVendingMachines[i][vmZ],
			0.0, 0.0, gVendingMachines[i][vmA], 100.0);

		new lbl[180];
		format(lbl, sizeof(lbl),
			"{33CCFF}[ %s ]\n{FFFFFF}Sprunk ($3) / Snack ($4)\n{FFFF00}Tekan 'Y' atau gunakan /vending",
			gVendingMachines[i][vmName]);

		gVendingMachines[i][vmLabel] = Create3DTextLabel(lbl, 0x33CCFFFF,
			gVendingMachines[i][vmX],
			gVendingMachines[i][vmY],
			gVendingMachines[i][vmZ] + 1.2,
			10.0, 0, true);
	}

	print("[VENDING] 6 Mesin vending otomatis sprunk & snack Los Santos aktif.");
	return 1;
}

stock IsPlayerNearVendingMachine(playerid, Float:range = RANGE_VENDING_MACHINE)
{
	for (new i = 0; i < MAX_VENDING_LOCATIONS; i++) {
		if (IsPlayerNearPoint(playerid, gVendingMachines[i][vmX], gVendingMachines[i][vmY], gVendingMachines[i][vmZ], range)) {
			return i;
		}
	}
	return -1;
}

stock ShowVendingDialog(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;

	new slot = IsPlayerNearVendingMachine(playerid);
	if (slot == -1) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus berada di dekat mesin vending otomatis.");
		return 0;
	}

	new dialogContent[256];
	format(dialogContent, sizeof(dialogContent),
		"Produk\tHarga\tEfek Pemulihan\n\
{FFFFFF}1. Minuman Kaleng Dingin Sprunk\t{4CD964}$3\t{33CCFF}+40%% Rasa Haus\n\
{FFFFFF}2. Makanan Ringan Cokelat Energi\t{4CD964}$4\t{FFCC00}+35%% Rasa Lapar");

	RP_ShowPlayerDialog(playerid, DIALOG_VENDING_SNACK, DIALOG_STYLE_TABLIST_HEADERS,
		gVendingMachines[slot][vmName], dialogContent, "Beli", "Batal");
	return 1;
}

stock OnVendingDialogResponse(playerid, listitem)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return 1;
	if (IsPlayerConnected(playerid) && IsPlayerNearVendingMachine(playerid) == -1) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda terlalu jauh dari mesin vending.");
		return 1;
	}

	if (listitem == 0) {
		// Beli Minuman Sprunk ($3)
		if (gPlayerData[playerid][pMoney] < 3) {
			SendClientMessage(playerid, COLOR_ERROR, "ERROR: Uang tunai tidak cukup ($3).");
			return 1;
		}
		if (!TakePlayerMoneyEx(playerid, 3, "beli sprunk mesin vending")) {
			SendClientMessage(playerid, COLOR_ERROR, "ERROR: Pembayaran mesin vending gagal.");
			return 1;
		}
		gPlayerData[playerid][pThirst] += 40.0;
		if (gPlayerData[playerid][pThirst] > 100.0) gPlayerData[playerid][pThirst] = 100.0;
		UpdatePlayerHUD(playerid);

		ApplyAnimation(playerid, "VENDING", "VEND_Drink_P", 4.1, false, false, false, false, 0);
		PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
		SendClientMessage(playerid, COLOR_SUCCESS, "SUKSES: Membeli dan meminum sekaleng Sprunk dingin seharga $3.");
	} else if (listitem == 1) {
		// Beli Snack Cokelat ($4)
		if (gPlayerData[playerid][pMoney] < 4) {
			SendClientMessage(playerid, COLOR_ERROR, "ERROR: Uang tunai tidak cukup ($4).");
			return 1;
		}
		if (!TakePlayerMoneyEx(playerid, 4, "beli snack mesin vending")) {
			SendClientMessage(playerid, COLOR_ERROR, "ERROR: Pembayaran mesin vending gagal.");
			return 1;
		}
		gPlayerData[playerid][pHunger] += 35.0;
		if (gPlayerData[playerid][pHunger] > 100.0) gPlayerData[playerid][pHunger] = 100.0;
		UpdatePlayerHUD(playerid);

		ApplyAnimation(playerid, "FOOD", "EAT_Burger", 4.1, false, false, false, false, 0);
		PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
		SendClientMessage(playerid, COLOR_SUCCESS, "SUKSES: Membeli dan memakan cokelat batang energi seharga $4.");
	}
	return 1;
}
