#if defined _INC_rp_bar_alcohol
	#endinput
#endif
#define _INC_rp_bar_alcohol

/*
==============================================================================
 bar_alcohol.inc
 NUSANTARA ROLEPLAY - Nightclub Bar Drinks, Intoxication & Police DUI Breathalyzer
==============================================================================
*/

#define MAX_BAR_LOCATIONS       3

stock const Float:gBarSpots[MAX_BAR_LOCATIONS][3] = {
	{1830.0, -1680.0, 13.5}, // Alhambra Club Bar
	{2306.0, -1650.0, 14.5}, // Ten Green Bottles Ganton
	{2422.0, -1226.0, 25.5}  // Pig Pen Club
};

stock BarAlcohol_Init()
{
	for (new i = 0; i < MAX_BAR_LOCATIONS; i++) {
		CreatePickup(1239, 1, gBarSpots[i][0], gBarSpots[i][1], gBarSpots[i][2], 0);
		Create3DTextLabel(
			"{FF66CC}[ BAR & KLUB MINUMAN ]\n{FFFFFF}Penyedia aneka bir & minuman beralkohol\n{FFFF00}Gunakan /bar atau /orderdrink",
			0xFF66CCFF, gBarSpots[i][0], gBarSpots[i][1], gBarSpots[i][2] + 0.8, 10.0, 0, true);
	}
	print("[BAR] Sistem bar minuman beralkohol & tes breathalyzer aktif.");
	return 1;
}

stock ShowBarDrinksMenu(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;

	new bool:isNear = false;
	for (new i = 0; i < MAX_BAR_LOCATIONS; i++) {
		if (IsPlayerNearPoint(playerid, gBarSpots[i][0], gBarSpots[i][1], gBarSpots[i][2], 4.0)) {
			isNear = true;
			break;
		}
	}

	if (!isNear) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus berada di meja bar klub malam (/gps).");
		return 0;
	}

	new dialogContent[320];
	format(dialogContent, sizeof(dialogContent),
		"Minuman Bar\tHarga\tKadar Alkohol\n\
{FFFFFF}1. Kaleng Bir Dingin (Beer)\t{4CD964}$8\t{FFFF00}Kadar Ringan (5%%)\n\
{FFFFFF}2. Sloki Minuman Whiskey\t{4CD964}$25\t{FF9900}Kadar Sedang (25%%)\n\
{FFFFFF}3. Shot Minuman Keras Vodka\t{4CD964}$40\t{FF3B30}Kadar Tinggi (40%%)");

	ShowPlayerDialog(playerid, DIALOG_BAR_MENU, DIALOG_STYLE_TABLIST_HEADERS,
		"MENU BAR & MINUMAN BERALKOHOL", dialogContent, "Pesan", "Batal");
	return 1;
}

stock OnBarMenuDialogResponse(playerid, listitem)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;

	new cost = 0, drunkAdd = 0;
	new drinkName[24];

	switch (listitem) {
		case 0: { cost = 8;  drunkAdd = 800;  format(drinkName, sizeof(drinkName), "Kaleng Bir Dingin"); }
		case 1: { cost = 25; drunkAdd = 2200; format(drinkName, sizeof(drinkName), "Sloki Whiskey"); }
		case 2: { cost = 40; drunkAdd = 4500; format(drinkName, sizeof(drinkName), "Shot Vodka Keras"); }
		default: return 1;
	}

	if (gPlayerData[playerid][pMoney] < cost) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Uang tunai Anda tidak mencukupi untuk memesan minuman ini.");
		return 1;
	}

	TakePlayerMoneyEx(playerid, cost, "beli minuman bar");

	gPlayerData[playerid][pDrunkLevel] += drunkAdd;
	if (gPlayerData[playerid][pDrunkLevel] > 10000) gPlayerData[playerid][pDrunkLevel] = 10000;
	SetPlayerDrunkLevel(playerid, gPlayerData[playerid][pDrunkLevel]);

	// Animasi minum
	ApplyAnimation(playerid, "BAR", "dnk_stnd_p", 4.1, false, false, false, false, 0);
	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);

	new succMsg[160];
	format(succMsg, sizeof(succMsg),
		"SUKSES: Menenggak 1x %s seharga $%d! Anda mulai merasakan efek hangat alkohol.",
		drinkName, cost);
	SendClientMessage(playerid, COLOR_SUCCESS, succMsg);
	return 1;
}

stock ProcessBreathalyzerTest(playerid, targetid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;

	if (gPlayerData[playerid][pFaction] != FACTION_POLICE || !gPlayerData[playerid][pDuty]) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Pemeriksaan tes kadar alkohol khusus petugas Kepolisian SAPD!");
		return 0;
	}

	if (!IsPlayerConnected(targetid) || !gPlayerData[targetid][pLoggedIn]) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Pemain sasaran tidak terhubung.");
		return 0;
	}

	if (!IsPlayerNear(playerid, targetid, 4.0)) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus berada di dekat pengemudi yang diuji.");
		return 0;
	}

	new drunk = gPlayerData[targetid][pDrunkLevel];
	new statusStr[32];
	if (drunk >= 3000) format(statusStr, sizeof(statusStr), "{FF3B30}POSITIF MABUK BERAT (DUI)");
	else if (drunk > 500) format(statusStr, sizeof(statusStr), "{FFFF00}TERPENGARUH ALKOHOL");
	else format(statusStr, sizeof(statusStr), "{4CD964}NEGATIF (SADAR)");

	PlayerPlaySound(playerid, 1058, 0.0, 0.0, 0.0);

	new copMsg[160];
	format(copMsg, sizeof(copMsg),
		"[SAPD BREATHALYZER] Subjek: %s | Level BAC: %d | Hasil Uji: %s",
		gPlayerData[targetid][pName], drunk, statusStr);
	SendClientMessage(playerid, COLOR_LIGHTBLUE, copMsg);

	new vicMsg[160];
	format(vicMsg, sizeof(vicMsg),
		"[SAPD BREATHALYZER] Petugas %s telah menguji kadar alkohol napas Anda: %s",
		gPlayerData[playerid][pName], statusStr);
	SendClientMessage(targetid, COLOR_YELLOW, vicMsg);
	return 1;
}
