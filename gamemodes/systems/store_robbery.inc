#if defined _INC_store_robbery
	#endinput
#endif
#define _INC_store_robbery

/*
==============================================================================
 store_robbery.inc
 NUSANTARA ROLEPLAY - Convenience Store Robbery & Cash Register Heist System
==============================================================================
*/

#define MAX_ROBBABLE_STORES     3
#define STORE_ROBBERY_DURATION  35
#define STORE_ROBBERY_COOLDOWN  900 // 15 Menit

enum E_ROB_STORE {
	robStoreID,
	robStoreName[48],
	Float:robX,
	Float:robY,
	Float:robZ,
	robLastRobbed
};

new gRobStores[MAX_ROBBABLE_STORES][E_ROB_STORE] = {
	{0, "Toko 24/7 Idlewood Central",   STORE_IDLEWOOD_X, STORE_IDLEWOOD_Y, STORE_IDLEWOOD_Z, 0},
	{1, "Toko 24/7 Commerce Plaza",      1352.0, -1758.0, 13.5, 0},
	{2, "Toko 24/7 Mulholland Crossing", 1315.0, -898.0, 39.5, 0}
};

new gActiveRobber = INVALID_PLAYER_ID;
new gActiveRobStore = -1;
new gActiveRobTimer = 0;
new gActiveRobSecondsLeft = 0;

forward StoreRobbery_Timer();

stock StoreRobbery_Init()
{
	gActiveRobber = INVALID_PLAYER_ID;
	gActiveRobStore = -1;
	gActiveRobTimer = 0;
	gActiveRobSecondsLeft = 0;

	print("[ROBBERY] Sistem perampokan minimarket kasir 24/7 Los Santos aktif.");
	return 1;
}

stock StoreRobbery_ResetPlayer(playerid)
{
	if (gActiveRobber == playerid) {
		if (gActiveRobTimer != 0) {
			KillTimer(gActiveRobTimer);
			gActiveRobTimer = 0;
		}
		gActiveRobber = INVALID_PLAYER_ID;
		gActiveRobStore = -1;
		gActiveRobSecondsLeft = 0;
	}
	return 1;
}

stock ProcessRobStore(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda tidak dapat merampok toko saat pingsan atau terborgol.");
		return 0;
	}
	if (IsPlayerInAnyVehicle(playerid)) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus turun dari kendaraan.");
		return 0;
	}

	if (gPlayerData[playerid][pFaction] == FACTION_POLICE && gPlayerData[playerid][pDuty]) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Petugas kepolisian tidak dapat melakukan perampokan!");
		return 0;
	}

	// Cek apakah pemain memegang senjata api jika client terhubung
	if (IsPlayerConnected(playerid)) {
		new WEAPON:gun = GetPlayerWeapon(playerid);
		if (gun != WEAPON_COLT45 && gun != WEAPON_SILENCED && gun != WEAPON_DEAGLE &&
		    gun != WEAPON_SHOTGUN && gun != WEAPON_SAWEDOFF && gun != WEAPON_SHOTGSPA &&
		    gun != WEAPON_UZI && gun != WEAPON_MP5 && gun != WEAPON_AK47 && gun != WEAPON_M4) {
			SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus menodongkan senjata api berpeluru ke kasir!");
			return 0;
		}
	}

	new storeSlot = -1;
	for (new i = 0; i < MAX_ROBBABLE_STORES; i++) {
		if (IsPlayerNearPoint(playerid, gRobStores[i][robX], gRobStores[i][robY], gRobStores[i][robZ], 4.0)) {
			storeSlot = i;
			break;
		}
	}

	if (storeSlot == -1) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus berada di dekat meja kasir toko 24/7.");
		return 0;
	}

	if (gActiveRobber != INVALID_PLAYER_ID) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Sedang ada perampokan toko lain yang berlangsung!");
		return 0;
	}

	new now = gettime();
	if ((now - gRobStores[storeSlot][robLastRobbed]) < STORE_ROBBERY_COOLDOWN) {
		new sLeft = STORE_ROBBERY_COOLDOWN - (now - gRobStores[storeSlot][robLastRobbed]);
		new errWait[128];
		format(errWait, sizeof(errWait), "ERROR: Kasir toko ini baru saja dirampok! Brankas kosong selama %d detik.", sLeft);
		SendClientMessage(playerid, COLOR_ERROR, errWait);
		return 0;
	}

	// Mulai perampokan
	gActiveRobber = playerid;
	gActiveRobStore = storeSlot;
	gActiveRobSecondsLeft = STORE_ROBBERY_DURATION;
	gRobStores[storeSlot][robLastRobbed] = now;

	gActiveRobTimer = SetTimer("StoreRobbery_Timer", 1000, true);

	// Suara alarm kasir
	PlayerPlaySound(playerid, 1058, 0.0, 0.0, 0.0);

	new msg[160];
	format(msg, sizeof(msg),
		"[PERAMPOKAN TOKO] Menodong kasir %s! Bertahanlah di toko selama %d detik sampai brankas terbuka!",
		gRobStores[storeSlot][robStoreName], STORE_ROBBERY_DURATION);
	SendClientMessage(playerid, COLOR_YELLOW, msg);

	// Broadcast alarm darurat ke semua polisi SAPD on-duty
	new policeRadio[160];
	format(policeRadio, sizeof(policeRadio),
		"[ALARM 911 PERAMPOKAN] Alarm diam-diam berbunyi: Perampokan bersenjata sedang berlangsung di %s!",
		gRobStores[storeSlot][robStoreName]);

	for (new p = 0; p < MAX_PLAYERS; p++) {
		if (IsPlayerConnected(p) && gPlayerData[p][pLoggedIn] && gPlayerData[p][pDuty] && gPlayerData[p][pFaction] == FACTION_POLICE) {
			SendClientMessage(p, COLOR_RADIO, policeRadio);
			PlayerPlaySound(p, 1057, 0.0, 0.0, 0.0);
		}
	}
	return 1;
}

public StoreRobbery_Timer()
{
	if (gActiveRobber == INVALID_PLAYER_ID || !IsPlayerConnected(gActiveRobber) || !gPlayerData[gActiveRobber][pLoggedIn]) {
		KillTimer(gActiveRobTimer);
		gActiveRobTimer = 0;
		gActiveRobber = INVALID_PLAYER_ID;
		gActiveRobStore = -1;
		return 1;
	}

	// [SECURITY] Batalkan jika pelaku pingsan atau terborgol saat menodong kasir
	if (gPlayerData[gActiveRobber][pInjured] || gPlayerData[gActiveRobber][pCuffed]) {
		SendClientMessage(gActiveRobber, COLOR_ERROR, "ERROR: Anda pingsan atau terborgol! Perampokan kasir dibatalkan.");
		KillTimer(gActiveRobTimer);
		gActiveRobTimer = 0;
		gActiveRobber = INVALID_PLAYER_ID;
		gActiveRobStore = -1;
		gActiveRobSecondsLeft = 0;
		return 1;
	}

	new storeSlot = gActiveRobStore;
	if (storeSlot < 0 || storeSlot >= MAX_ROBBABLE_STORES) {
		KillTimer(gActiveRobTimer);
		gActiveRobTimer = 0;
		gActiveRobber = INVALID_PLAYER_ID;
		return 1;
	}

	// Cek jarak perampok ke kasir (tidak boleh kabur lebih dari 9 meter)
	if (!IsPlayerNearPoint(gActiveRobber, gRobStores[storeSlot][robX], gRobStores[storeSlot][robY], gRobStores[storeSlot][robZ], 9.0)) {
		SendClientMessage(gActiveRobber, COLOR_ERROR, "ERROR: Anda meninggalkan area kasir toko! Perampokan dibatalkan.");
		KillTimer(gActiveRobTimer);
		gActiveRobTimer = 0;
		gActiveRobber = INVALID_PLAYER_ID;
		gActiveRobStore = -1;
		return 1;
	}

	gActiveRobSecondsLeft--;

	if (gActiveRobSecondsLeft <= 0) {
		// Perampokan sukses
		KillTimer(gActiveRobTimer);
		gActiveRobTimer = 0;

		new lootCash = 1200 + random(1301); // $1,200 - $2,500
		GivePlayerMoneyEx(gActiveRobber, lootCash, "hasil perampokan minimarket 24/7");

		PlayerPlaySound(gActiveRobber, 1052, 0.0, 0.0, 0.0);

		new succMsg[160];
		format(succMsg, sizeof(succMsg),
			"[PERAMPOKAN SUKSES] Brankas kasir berhasil dibobol! Anda membawa kabur $%d uang tunai!", lootCash);
		SendClientMessage(gActiveRobber, COLOR_SUCCESS, succMsg);

		// Notifikasi ke polisi bahwa perampok berhasil kabur
		new policeRadio[160];
		format(policeRadio, sizeof(policeRadio),
			"[ALARM 911] Pelaku telah membobol brankas di %s dan melarikan diri! Lakukan penyisiran!",
			gRobStores[storeSlot][robStoreName]);

		for (new p = 0; p < MAX_PLAYERS; p++) {
			if (IsPlayerConnected(p) && gPlayerData[p][pLoggedIn] && gPlayerData[p][pDuty] && gPlayerData[p][pFaction] == FACTION_POLICE) {
				SendClientMessage(p, COLOR_RADIO, policeRadio);
			}
		}

		gActiveRobber = INVALID_PLAYER_ID;
		gActiveRobStore = -1;
		return 1;
	}

	if (gActiveRobSecondsLeft % 10 == 0 || gActiveRobSecondsLeft <= 5) {
		new countMsg[96];
		format(countMsg, sizeof(countMsg), "~y~BRANKAS KASIR: ~r~%d DETIK", gActiveRobSecondsLeft);
		GameTextForPlayer(gActiveRobber, countMsg, 1000, 3);
	}
	return 1;
}
