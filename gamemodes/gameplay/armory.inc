#if defined _INC_rp_armory
	#endinput
#endif
#define _INC_rp_armory

/*
==============================================================================
 armory.inc
 NUSANTARA ROLEPLAY - Faction Duty Armory & Tactical Locker System
==============================================================================
*/

#define RANGE_ARMORY_INTERACTION 3.0

enum E_ARMORY_POINT {
	Float:armX,
	Float:armY,
	Float:armZ,
	armInterior,
	armVW,
	armFaction,
	armName[48],
	armPickup,
	Text3D:armLabel
};

new gArmories[4][E_ARMORY_POINT];

stock Armory_Init()
{
	// 1. SAPD Armory Locker (LSPD Central HQ Interior & Exterior)
	Armory_Register(0, 246.50, 63.90, 1003.64, 6, 103, FACTION_POLICE, "Loker Taktis Kepolisian (SAPD)");
	Armory_Register(1, 1554.8, -1675.6, 16.1, 0, 0, FACTION_POLICE, "Loker Luar Markas SAPD");

	// 2. SAMD Medical Supply Locker (ASGH General Hospital)
	Armory_Register(2, 1149.60, -1308.50, 1005.90, 5, 102, FACTION_MEDIC, "Loker Pasokan Medis (SAMD)");
	Armory_Register(3, 1172.8, -1323.2, 15.4, 0, 0, FACTION_MEDIC, "Loker Darurat IGD SAMD");

	print("[ARMORY] 4 Titik loker dinas kepolisian & medis Nusantara aktif.");
	return 1;
}

stock Armory_Register(slot, Float:x, Float:y, Float:z, interior, vw, faction, const name[])
{
	gArmories[slot][armX] = x;
	gArmories[slot][armY] = y;
	gArmories[slot][armZ] = z;
	gArmories[slot][armInterior] = interior;
	gArmories[slot][armVW] = vw;
	gArmories[slot][armFaction] = faction;
	format(gArmories[slot][armName], 48, "%s", name);

	gArmories[slot][armPickup] = CreatePickup(1242, 1, x, y, z, vw);

	new lbl[180];
	format(lbl, sizeof(lbl),
		"{33CCFF}[ LOKER DINAS %s ]\n{FFFFFF}%s\n{FFFF00}Gunakan /armory atau /locker",
		(faction == FACTION_POLICE) ? "KEPOLISIAN (SAPD)" : "MEDIS (SAMD)", name);

	gArmories[slot][armLabel] = Create3DTextLabel(lbl, COLOR_LIGHTBLUE, x, y, z + 0.8, 12.0, vw, true);
	return 1;
}

stock ShowFactionArmoryDialog(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;

	if (gPlayerData[playerid][pFaction] == FACTION_NONE) {
		if (gPlayerData[playerid][pLanguage] == LANG_EN) SendClientMessage(playerid, COLOR_ERROR, "ERROR: You are not a member of any government faction.");
		else SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda bukan anggota faksi dinas pemerintahan.");
		return 0;
	}

	if (!gPlayerData[playerid][pDuty]) {
		if (gPlayerData[playerid][pLanguage] == LANG_EN) SendClientMessage(playerid, COLOR_ERROR, "ERROR: You must be on-duty (/duty) to access the faction armory.");
		else SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus bertugas dinas (/duty) terlebih dahulu.");
		return 0;
	}

	new slot = -1;
	new pVW = GetPlayerVirtualWorld(playerid);
	new pInt = GetPlayerInterior(playerid);

	for (new i = 0; i < 4; i++) {
		if (gArmories[i][armFaction] == gPlayerData[playerid][pFaction] &&
			gArmories[i][armVW] == pVW && gArmories[i][armInterior] == pInt) {
			if (IsPlayerNearPoint(playerid, gArmories[i][armX], gArmories[i][armY], gArmories[i][armZ], RANGE_ARMORY_INTERACTION)) {
				slot = i;
				break;
			}
		}
	}

	if (slot == -1) {
		if (gPlayerData[playerid][pLanguage] == LANG_EN) SendClientMessage(playerid, COLOR_ERROR, "ERROR: You must be near your faction armory locker.");
		else SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus berada di dekat loker dinas faksi Anda.");
		return 0;
	}

	if (gPlayerData[playerid][pFaction] == FACTION_POLICE) {
		new dialogText[600];
		if (gPlayerData[playerid][pLanguage] == LANG_EN) {
			format(dialogText, sizeof(dialogText),
				"Equipment / Action\tSpecification\n\
{FFFFFF}1. Standard Patrol Loadout\t{33CCFF}Nightstick, Deagle, Shotgun\n\
{FFFFFF}2. Heavy Tactical SWAT Loadout\t{FF3B30}M4 Rifle, MP5, Sniper\n\
{FFFFFF}3. Refill Kevlar Body Armor\t{4CD964}100% Armor Vest\n\
{FFFFFF}4. Take Non-Lethal Taser\t{FFFF00}Silenced 9mm\n\
{FF6666}X. Return & Clear Duty Weapons\t{FF6666}Reset Weapons");

			RP_ShowPlayerDialog(playerid, DIALOG_ARMORY_MENU, DIALOG_STYLE_TABLIST_HEADERS,
				"SAPD TACTICAL ARMORY LOCKER", dialogText, "Take", "Close");
		} else {
			format(dialogText, sizeof(dialogText),
				"Perlengkapan / Tindakan\tSpesifikasi Dinas\n\
{FFFFFF}1. Perlengkapan Dinas Standar Patroli\t{33CCFF}Tongkat, Deagle, Shotgun\n\
{FFFFFF}2. Perlengkapan Taktis Berat SWAT\t{FF3B30}M4 Rifle, MP5, Sniper\n\
{FFFFFF}3. Isi Ulang Rompi Kevlar Anti-Peluru\t{4CD964}100% Armor Vest\n\
{FFFFFF}4. Ambil Senjata Kejut (Taser)\t{FFFF00}Silenced Pistol\n\
{FF6666}X. Kembalikan Seluruh Senjata Dinas\t{FF6666}Simpan Senjata");

			RP_ShowPlayerDialog(playerid, DIALOG_ARMORY_MENU, DIALOG_STYLE_TABLIST_HEADERS,
				"LOKER PERSENJATAAN DINAS SAPD", dialogText, "Ambil", "Tutup");
		}
	} else if (gPlayerData[playerid][pFaction] == FACTION_MEDIC) {
		new dialogText[500];
		if (gPlayerData[playerid][pLanguage] == LANG_EN) {
			format(dialogText, sizeof(dialogText),
				"Medical Supplies\tQuantity\n\
{FFFFFF}1. Paramedic First-Aid Kit\t{4CD964}5x Bandages, 3x Medkits\n\
{FFFFFF}2. Restore Full Health & Energy\t{33CCFF}100% Health Vitality\n\
{FFFFFF}3. Take Hospital Radio & Pager\t{FFFF00}Channel 911 MHz");

			RP_ShowPlayerDialog(playerid, DIALOG_ARMORY_MENU, DIALOG_STYLE_TABLIST_HEADERS,
				"SAMD MEDICAL SUPPLY LOCKER", dialogText, "Take", "Close");
		} else {
			format(dialogText, sizeof(dialogText),
				"Pasokan Medis\tJumlah Pasokan\n\
{FFFFFF}1. Kotak Pertolongan Pertama Paramedis\t{4CD964}5x Perban, 3x Kotak P3K\n\
{FFFFFF}2. Pulihkan Kondisi Fisik & Stamina\t{33CCFF}100% Kesehatan Penuh\n\
{FFFFFF}3. Ambil Handy Talkie Rumah Sakit\t{FFFF00}Frekuensi 911 MHz");

			RP_ShowPlayerDialog(playerid, DIALOG_ARMORY_MENU, DIALOG_STYLE_TABLIST_HEADERS,
				"LOKER PASOKAN MEDIS SAMD", dialogText, "Ambil", "Tutup");
		}
	}
	return 1;
}

stock OnFactionArmoryResponse(playerid, listitem)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pFaction] == FACTION_NONE || !gPlayerData[playerid][pDuty]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return 0;

	if (IsPlayerConnected(playerid)) {
		new slot = -1;
		new pVW = GetPlayerVirtualWorld(playerid);
		new pInt = GetPlayerInterior(playerid);

		for (new i = 0; i < 4; i++) {
			if (gArmories[i][armFaction] == gPlayerData[playerid][pFaction] &&
				gArmories[i][armVW] == pVW && gArmories[i][armInterior] == pInt) {
				if (IsPlayerNearPoint(playerid, gArmories[i][armX], gArmories[i][armY], gArmories[i][armZ], RANGE_ARMORY_INTERACTION)) {
					slot = i;
					break;
				}
			}
		}

		if (slot == -1) {
			SendErr(playerid, "Anda harus berada di dekat loker dinas faksi Anda.");
			return 0;
		}
	}

	if (gPlayerData[playerid][pFaction] == FACTION_POLICE) {
		switch (listitem) {
			case 0: {
				// Standar Patroli
				AC_GiveWeapon(playerid, WEAPON_NITESTICK, 1);
				AC_GiveWeapon(playerid, WEAPON_DEAGLE, 140);
				AC_GiveWeapon(playerid, WEAPON_SHOTGUN, 60);
				SetPlayerArmour(playerid, 100.0);
				GivePlayerItem(playerid, ITEM_RADIO, 1);
				gPlayerData[playerid][pRadioFreq] = 911;

				PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
				SendSucc(playerid, "Mengambil perlengkapan dinas standar patroli (Nightstick, Deagle, Shotgun, Kevlar 100%).");
			}
			case 1: {
				// Taktis SWAT (Rank 2+)
				if (gPlayerData[playerid][pFactionRank] < 2 && gPlayerData[playerid][pAdmin] < 1) {
					SendErr(playerid, "Perlengkapan taktis SWAT membutuhkan pangkat Sersan (Rank 2+) atau Admin.");
					return 1;
				}
				AC_GiveWeapon(playerid, WEAPON_M4, 300);
				AC_GiveWeapon(playerid, WEAPON_MP5, 200);
				AC_GiveWeapon(playerid, WEAPON_SNIPER, 30);
				SetPlayerArmour(playerid, 100.0);

				PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
				SendSucc(playerid, "Mengambil persenjataan taktis berat SWAT (M4, MP5, Sniper Rifle, Kevlar 100%).");
			}
			case 2: {
				// Refill Kevlar
				SetPlayerArmour(playerid, 100.0);
				PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
				SendSucc(playerid, "Rompi anti-peluru Kevlar berhasil diisi ulang menjadi 100%.");
			}
			case 3: {
				// Taser
				AC_GiveWeapon(playerid, WEAPON_SILENCED, 60);
				PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
				SendSucc(playerid, "Mengambil senjata kejut listrik non-mematikan (Taser Silenced Pistol).");
			}
			case 4: {
				// Clear Weapons
				AC_ResetWeapons(playerid);
				SetPlayerArmour(playerid, 0.0);
				gPlayerData[playerid][pRadioFreq] = 0;
				PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
				SendInfo(playerid, "Seluruh persenjataan dinas dikembalikan ke loker persenjataan.");
			}
		}
	} else if (gPlayerData[playerid][pFaction] == FACTION_MEDIC) {
		switch (listitem) {
			case 0: {
				// Pasokan Medis
				GivePlayerItem(playerid, ITEM_BANDAGE, 5);
				GivePlayerItem(playerid, ITEM_MEDKIT, 3);
				PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
				SendSucc(playerid, "Mengambil 5x Perban dan 3x Kotak P3K medis dari loker pasokan.");
			}
			case 1: {
				// Pulihkan Fisik
				SetPlayerHealth(playerid, 100.0);
				gPlayerData[playerid][pHunger] = 100.0;
				gPlayerData[playerid][pThirst] = 100.0;
				UpdatePlayerHUD(playerid);
				PlayerPlaySound(playerid, 1058, 0.0, 0.0, 0.0);
				SendSucc(playerid, "Kondisi fisik, darah, dan stamina telah pulih sepenuhnya.");
			}
			case 2: {
				// Radio HT
				GivePlayerItem(playerid, ITEM_RADIO, 1);
				gPlayerData[playerid][pRadioFreq] = 911;
				SendSucc(playerid, "Mengambil Handy Talkie radio darurat rumah sakit (Frekuensi 911 MHz).");
			}
		}
	}
	return 1;
}
