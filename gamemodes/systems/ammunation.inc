#if defined _INC_rp_ammunation
	#endinput
#endif
#define _INC_rp_ammunation

/*
==============================================================================
 ammunation.inc
 NUSANTARA ROLEPLAY - Legal Firearms Ammu-Nation Store & Firearm License Registry
==============================================================================
*/

#define AMMU_NATION_X           1368.5
#define AMMU_NATION_Y           -1279.5
#define AMMU_NATION_Z           13.5

stock AmmuNation_Init()
{
	CreatePickup(1239, 1, AMMU_NATION_X, AMMU_NATION_Y, AMMU_NATION_Z, 0);
	Create3DTextLabel(
		"{FFCC00}[ AMMU-NATION TOKO SENJATA RESMI ]\n{FFFFFF}Penyedia senjata api & amunisi berlisensi\n{FFFF00}Gunakan /buygun atau /ammunation",
		0xFFCC00FF, AMMU_NATION_X, AMMU_NATION_Y, AMMU_NATION_Z + 0.8, 15.0, 0, true);

	print("[AMMUNATION] Toko senjata legal Ammu-Nation Los Santos aktif.");
	return 1;
}

stock ShowAmmuNationMenu(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;

	if (!IsPlayerNearPoint(playerid, AMMU_NATION_X, AMMU_NATION_Y, AMMU_NATION_Z, 4.0)) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus berada di kasir toko Ammu-Nation Willowfield.");
		return 0;
	}

	if (!gPlayerData[playerid][pGunLic]) {
		SendClientMessage(playerid, COLOR_ERROR,
			"[KASIR AMMU-NATION] Maaf, Anda tidak memiliki Lisensi Kepemilikan Senjata Api Resmi! Ajukan di kantor polisi SAPD.");
		return 0;
	}

	new dialogContent[450];
	format(dialogContent, sizeof(dialogContent),
		"Senjata Berlisensi / Amunisi\tHarga\tKeterangan\n\
{FFFFFF}1. Pistol Bela Diri Colt .45\t{4CD964}$600\t50 Peluru resmi\n\
{FFFFFF}2. Desert Eagle .50 Pistol\t{4CD964}$1.500\t35 Peluru kaliber berat\n\
{FFFFFF}3. Shotgun Berburu Standar\t{4CD964}$1.200\t30 Peluru gotri\n\
{FFFFFF}4. Kotak Amunisi Resmi (50 Peluru)\t{4CD964}$200\tAmunisi cadangan legal\n\
{FFFFFF}5. Rompi Anti-Peluru Kevlar\t{4CD964}$300\t100%% Perlindungan dada");

	ShowPlayerDialog(playerid, DIALOG_AMMUNATION_BUY, DIALOG_STYLE_TABLIST_HEADERS,
		"AMMU-NATION GUN STORE", dialogContent, "Beli", "Batal");
	return 1;
}

stock OnAmmuNationDialogResponse(playerid, listitem)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;

	new cost = 0;
	switch (listitem) {
		case 0: cost = 600;
		case 1: cost = 1500;
		case 2: cost = 1200;
		case 3: cost = 200;
		case 4: cost = 300;
		default: return 1;
	}

	if (gPlayerData[playerid][pMoney] < cost) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Uang tunai Anda tidak mencukupi untuk pembelian ini.");
		return 1;
	}

	TakePlayerMoneyEx(playerid, cost, "beli senjata legal ammunation");

	switch (listitem) {
		case 0: {
			GivePlayerWeapon(playerid, WEAPON_COLT45, 50);
			SendClientMessage(playerid, COLOR_SUCCESS, "SUKSES: Membeli 1x Pistol Colt .45 berlisensi (50 Peluru) seharga $600.");
		}
		case 1: {
			GivePlayerWeapon(playerid, WEAPON_DEAGLE, 35);
			SendClientMessage(playerid, COLOR_SUCCESS, "SUKSES: Membeli 1x Desert Eagle .50 berlisensi (35 Peluru) seharga $1.500.");
		}
		case 2: {
			GivePlayerWeapon(playerid, WEAPON_SHOTGUN, 30);
			SendClientMessage(playerid, COLOR_SUCCESS, "SUKSES: Membeli 1x Shotgun Berburu berlisensi (30 Peluru) seharga $1.200.");
		}
		case 3: {
			GivePlayerItem(playerid, ITEM_AMMOBOX, 1);
			SendClientMessage(playerid, COLOR_SUCCESS, "SUKSES: Membeli 1x Kotak Amunisi Resmi (50 Peluru) seharga $200.");
		}
		case 4: {
			SetPlayerArmour(playerid, 100.0);
			SendClientMessage(playerid, COLOR_SUCCESS, "SUKSES: Membeli dan mengenakan Rompi Kevlar 100% seharga $300.");
		}
	}

	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
	return 1;
}

stock ProcessIssueGunLicense(playerid, targetid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;

	if (gPlayerData[playerid][pFaction] != FACTION_POLICE || !gPlayerData[playerid][pDuty]) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Penerbitan lisensi senjata api khusus untuk petugas Kepolisian SAPD!");
		return 0;
	}

	if (!IsPlayerConnected(targetid) || !gPlayerData[targetid][pLoggedIn]) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Pemain sasaran tidak terhubung.");
		return 0;
	}

	if (!IsPlayerNear(playerid, targetid, 4.0)) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus berada di dekat pemohon lisensi.");
		return 0;
	}

	gPlayerData[targetid][pGunLic] = true;
	DB_SavePlayerData(targetid);

	PlayerPlaySound(targetid, 1058, 0.0, 0.0, 0.0);

	new copMsg[144], vicMsg[144];
	format(copMsg, sizeof(copMsg), "[SAPD] Berhasil menerbitkan Lisensi Senjata Api Resmi kepada %s.", gPlayerData[targetid][pName]);
	format(vicMsg, sizeof(vicMsg), "[SAPD] Petugas %s telah mengesahkan Lisensi Senjata Api Resmi untuk Anda!", gPlayerData[playerid][pName]);

	SendClientMessage(playerid, COLOR_LIGHTBLUE, copMsg);
	SendClientMessage(targetid, COLOR_SUCCESS, vicMsg);
	return 1;
}
