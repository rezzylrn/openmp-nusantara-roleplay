#if defined _INC_rp_doors
	#endinput
#endif
#define _INC_rp_doors

/*
==============================================================================
  NUSANTARA ROLEPLAY - doors.inc
  Sistem Pintu & Teleportasi Gedung Dinamis (Indoor / Outdoor Door System)
==============================================================================
*/

#define MAX_DYNAMIC_DOORS       100
#define RANGE_DOOR_INTERACTION  2.2

enum E_DOOR_DATA {
	bool:dUsed,
	dDBID,
	dName[48],
	Float:dExtX,
	Float:dExtY,
	Float:dExtZ,
	Float:dExtA,
	dExtInterior,
	dExtVW,
	Float:dIntX,
	Float:dIntY,
	Float:dIntZ,
	Float:dIntA,
	dIntInterior,
	dIntVW,
	bool:dLocked,
	dAdminLevel,
	// Runtime handles
	dExtPickup,
	Text3D:dExtLabel,
	dIntPickup,
	Text3D:dIntLabel
};

new gDoors[MAX_DYNAMIC_DOORS][E_DOOR_DATA];
new gTotalDoors = 0;

forward OnDoorsLoaded();
forward OnDoorCreated(slot);

// Inisialisasi sistem pintu & tabel MySQL
stock Doors_Init()
{
	for (new i = 0; i < MAX_DYNAMIC_DOORS; i++) {
		gDoors[i][dUsed] = false;
		gDoors[i][dExtPickup] = -1;
		gDoors[i][dExtLabel] = Text3D:INVALID_3DTEXT_ID;
		gDoors[i][dIntPickup] = -1;
		gDoors[i][dIntLabel] = Text3D:INVALID_3DTEXT_ID;
	}

	if (!gDBReady) return 0;

	mysql_tquery(gDB, "CREATE TABLE IF NOT EXISTS `doors` (\
		`id` INT AUTO_INCREMENT PRIMARY KEY, \
		`name` VARCHAR(48) NOT NULL, \
		`ext_x` FLOAT NOT NULL, \
		`ext_y` FLOAT NOT NULL, \
		`ext_z` FLOAT NOT NULL, \
		`ext_a` FLOAT DEFAULT 0.0, \
		`ext_interior` INT DEFAULT 0, \
		`ext_vw` INT DEFAULT 0, \
		`int_x` FLOAT NOT NULL, \
		`int_y` FLOAT NOT NULL, \
		`int_z` FLOAT NOT NULL, \
		`int_a` FLOAT DEFAULT 0.0, \
		`int_interior` INT DEFAULT 0, \
		`int_vw` INT DEFAULT 0, \
		`locked` TINYINT DEFAULT 0, \
		`admin_level` INT DEFAULT 0\
	) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4", "", "");

	mysql_tquery(gDB, "SELECT * FROM `doors`", "OnDoorsLoaded", "");
	return 1;
}

public OnDoorsLoaded()
{
	new rows = cache_num_rows();
	gTotalDoors = 0;

	for (new i = 0; i < rows && i < MAX_DYNAMIC_DOORS; i++) {
		gDoors[i][dUsed] = true;
		cache_get_value_name_int(i, "id", gDoors[i][dDBID]);
		cache_get_value_name(i, "name", gDoors[i][dName], 48);

		cache_get_value_name_float(i, "ext_x", gDoors[i][dExtX]);
		cache_get_value_name_float(i, "ext_y", gDoors[i][dExtY]);
		cache_get_value_name_float(i, "ext_z", gDoors[i][dExtZ]);
		cache_get_value_name_float(i, "ext_a", gDoors[i][dExtA]);
		cache_get_value_name_int(i, "ext_interior", gDoors[i][dExtInterior]);
		cache_get_value_name_int(i, "ext_vw", gDoors[i][dExtVW]);

		cache_get_value_name_float(i, "int_x", gDoors[i][dIntX]);
		cache_get_value_name_float(i, "int_y", gDoors[i][dIntY]);
		cache_get_value_name_float(i, "int_z", gDoors[i][dIntZ]);
		cache_get_value_name_float(i, "int_a", gDoors[i][dIntA]);
		cache_get_value_name_int(i, "int_interior", gDoors[i][dIntInterior]);
		cache_get_value_name_int(i, "int_vw", gDoors[i][dIntVW]);

		new lockVal;
		cache_get_value_name_int(i, "locked", lockVal);
		gDoors[i][dLocked] = (lockVal != 0);

		cache_get_value_name_int(i, "admin_level", gDoors[i][dAdminLevel]);

		Doors_CreateVisuals(i);
		gTotalDoors++;
	}

	printf("[DOORS] Berhasil memuat %d pintu dinamis dari database MySQL.", gTotalDoors);

	// Jika database kosong, buat default pintu penting Los Santos
	if (gTotalDoors == 0) {
		Doors_CreateDefaultSet();
	}
	return 1;
}

stock Doors_CreateVisuals(slot)
{
	if (slot < 0 || slot >= MAX_DYNAMIC_DOORS || !gDoors[slot][dUsed]) return 0;

	// Hapus visual lama jika ada
	if (gDoors[slot][dExtPickup] != -1) DestroyPickup(gDoors[slot][dExtPickup]);
	if (gDoors[slot][dExtLabel] != Text3D:INVALID_3DTEXT_ID) Delete3DTextLabel(gDoors[slot][dExtLabel]);
	if (gDoors[slot][dIntPickup] != -1) DestroyPickup(gDoors[slot][dIntPickup]);
	if (gDoors[slot][dIntLabel] != Text3D:INVALID_3DTEXT_ID) Delete3DTextLabel(gDoors[slot][dIntLabel]);

	// 1. Exterior Visual (Luar Gedung)
	gDoors[slot][dExtPickup] = CreatePickup(1239, 1, gDoors[slot][dExtX], gDoors[slot][dExtY], gDoors[slot][dExtZ], gDoors[slot][dExtVW]);

	new extText[192];
	format(extText, sizeof(extText),
		"{33CCFF}[ PINTU: %s ]\n{FFFFFF}%s\n{FFFF00}Tekan F / ENTER atau /enter",
		gDoors[slot][dName],
		gDoors[slot][dLocked] ? "{FF3B30}[ TERKUNCI ]" : "{4CD964}[ TERBUKA ]");

	gDoors[slot][dExtLabel] = Create3DTextLabel(extText, COLOR_LIGHTBLUE,
		gDoors[slot][dExtX], gDoors[slot][dExtY], gDoors[slot][dExtZ] + 0.6,
		15.0, gDoors[slot][dExtVW], true);

	// 2. Interior Visual (Dalam Gedung)
	gDoors[slot][dIntPickup] = CreatePickup(1318, 1, gDoors[slot][dIntX], gDoors[slot][dIntY], gDoors[slot][dIntZ], gDoors[slot][dIntVW]);

	new intText[192];
	format(intText, sizeof(intText),
		"{FF6666}[ KELUAR: %s ]\n{FFFFFF}%s\n{FFFF00}Tekan F / ENTER atau /exit",
		gDoors[slot][dName],
		gDoors[slot][dLocked] ? "{FF3B30}[ TERKUNCI ]" : "{4CD964}[ MENUJU LUAR ]");

	gDoors[slot][dIntLabel] = Create3DTextLabel(intText, COLOR_RED,
		gDoors[slot][dIntX], gDoors[slot][dIntY], gDoors[slot][dIntZ] + 0.6,
		12.0, gDoors[slot][dIntVW], true);
	return 1;
}

stock Doors_Create(const name[], Float:extX, Float:extY, Float:extZ, Float:extA, extInt, extVW,
	Float:intX, Float:intY, Float:intZ, Float:intA, intInt, intVW, bool:locked = false, adminLevel = 0)
{
	new slot = -1;
	for (new i = 0; i < MAX_DYNAMIC_DOORS; i++) {
		if (!gDoors[i][dUsed]) {
			slot = i;
			break;
		}
	}
	if (slot == -1) return -1;

	gDoors[slot][dUsed] = true;
	format(gDoors[slot][dName], 48, "%s", name);
	gDoors[slot][dExtX] = extX;
	gDoors[slot][dExtY] = extY;
	gDoors[slot][dExtZ] = extZ;
	gDoors[slot][dExtA] = extA;
	gDoors[slot][dExtInterior] = extInt;
	gDoors[slot][dExtVW] = extVW;

	gDoors[slot][dIntX] = intX;
	gDoors[slot][dIntY] = intY;
	gDoors[slot][dIntZ] = intZ;
	gDoors[slot][dIntA] = intA;
	gDoors[slot][dIntInterior] = intInt;
	gDoors[slot][dIntVW] = intVW;

	gDoors[slot][dLocked] = locked;
	gDoors[slot][dAdminLevel] = adminLevel;

	Doors_CreateVisuals(slot);
	gTotalDoors++;

	if (gDBReady) {
		new query[650];
		mysql_format(gDB, query, sizeof(query),
			"INSERT INTO `doors` (`name`, `ext_x`, `ext_y`, `ext_z`, `ext_a`, `ext_interior`, `ext_vw`, \
			`int_x`, `int_y`, `int_z`, `int_a`, `int_interior`, `int_vw`, `locked`, `admin_level`) \
			VALUES ('%e', %.4f, %.4f, %.4f, %.4f, %d, %d, %.4f, %.4f, %.4f, %.4f, %d, %d, %d, %d)",
			name, extX, extY, extZ, extA, extInt, extVW,
			intX, intY, intZ, intA, intInt, intVW, locked ? 1 : 0, adminLevel);

		mysql_tquery(gDB, query, "OnDoorCreated", "d", slot);
	}
	return slot;
}

public OnDoorCreated(slot)
{
	if (slot < 0 || slot >= MAX_DYNAMIC_DOORS || !gDoors[slot][dUsed]) return 0;
	gDoors[slot][dDBID] = cache_insert_id();
	return 1;
}

stock Doors_Delete(slot)
{
	if (slot < 0 || slot >= MAX_DYNAMIC_DOORS || !gDoors[slot][dUsed]) return 0;

	if (gDoors[slot][dExtPickup] != -1) DestroyPickup(gDoors[slot][dExtPickup]);
	if (gDoors[slot][dExtLabel] != Text3D:INVALID_3DTEXT_ID) Delete3DTextLabel(gDoors[slot][dExtLabel]);
	if (gDoors[slot][dIntPickup] != -1) DestroyPickup(gDoors[slot][dIntPickup]);
	if (gDoors[slot][dIntLabel] != Text3D:INVALID_3DTEXT_ID) Delete3DTextLabel(gDoors[slot][dIntLabel]);

	if (gDBReady && gDoors[slot][dDBID] > 0) {
		new query[128];
		mysql_format(gDB, query, sizeof(query), "DELETE FROM `doors` WHERE `id`=%d", gDoors[slot][dDBID]);
		mysql_tquery(gDB, query, "", "");
	}

	gDoors[slot][dUsed] = false;
	gTotalDoors--;
	return 1;
}

// Interaksi Masuk / Keluar Pintu
stock ProcessDoorEnter(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;

	new pVW = GetPlayerVirtualWorld(playerid);
	new pInt = GetPlayerInterior(playerid);

	for (new i = 0; i < MAX_DYNAMIC_DOORS; i++) {
		if (!gDoors[i][dUsed]) continue;

		if (gDoors[i][dExtVW] == pVW && gDoors[i][dExtInterior] == pInt) {
			if (IsPlayerNearPoint(playerid, gDoors[i][dExtX], gDoors[i][dExtY], gDoors[i][dExtZ], RANGE_DOOR_INTERACTION)) {
				// Cek status kunci
				if (gDoors[i][dLocked]) {
					if (gPlayerData[playerid][pAdmin] < 1 && (gDoors[i][dAdminLevel] > 0 || gPlayerData[playerid][pFaction] == FACTION_NONE)) {
						if (gPlayerData[playerid][pLanguage] == LANG_EN) SendErr(playerid, "This door is locked.");
						else SendErr(playerid, "Pintu ini sedang terkunci.");
						PlayerPlaySound(playerid, 1055, 0.0, 0.0, 0.0);
						return 1;
					}
				}

				// Kalibrasi posisi & interior untuk Anti-Cheat
				AC_SetPlayerPos(playerid, gDoors[i][dIntX], gDoors[i][dIntY], gDoors[i][dIntZ]);
				SetPlayerFacingAngle(playerid, gDoors[i][dIntA]);
				SetPlayerInterior(playerid, gDoors[i][dIntInterior]);
				SetPlayerVirtualWorld(playerid, gDoors[i][dIntVW]);

				PlayerPlaySound(playerid, 1085, 0.0, 0.0, 0.0);
				if (gPlayerData[playerid][pLanguage] == LANG_EN) SendSucc(playerid, "Entering: %s", gDoors[i][dName]);
				else SendSucc(playerid, "Memasuki: %s", gDoors[i][dName]);
				return 1;
			}
		}
	}
	return 0;
}

stock ProcessDoorExit(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;

	new pVW = GetPlayerVirtualWorld(playerid);
	new pInt = GetPlayerInterior(playerid);

	for (new i = 0; i < MAX_DYNAMIC_DOORS; i++) {
		if (!gDoors[i][dUsed]) continue;

		if (gDoors[i][dIntVW] == pVW && gDoors[i][dIntInterior] == pInt) {
			if (IsPlayerNearPoint(playerid, gDoors[i][dIntX], gDoors[i][dIntY], gDoors[i][dIntZ], RANGE_DOOR_INTERACTION)) {
				// Keluar gedung
				AC_SetPlayerPos(playerid, gDoors[i][dExtX], gDoors[i][dExtY], gDoors[i][dExtZ]);
				SetPlayerFacingAngle(playerid, gDoors[i][dExtA]);
				SetPlayerInterior(playerid, gDoors[i][dExtInterior]);
				SetPlayerVirtualWorld(playerid, gDoors[i][dExtVW]);

				PlayerPlaySound(playerid, 1085, 0.0, 0.0, 0.0);
				if (gPlayerData[playerid][pLanguage] == LANG_EN) SendSucc(playerid, "Exiting: %s", gDoors[i][dName]);
				else SendSucc(playerid, "Keluar dari: %s", gDoors[i][dName]);
				return 1;
			}
		}
	}
	return 0;
}

// Default Presets untuk Kota Los Santos
stock Doors_CreateDefaultSet()
{
	print("[DOORS] Memasang daftar pintu bawaan Los Santos (City Hall, ASGH, LSPD, 24/7 Store)...");

	// 1. Balai Kota / City Hall Los Santos
	Doors_Create("Balai Kota Los Santos (SAGOV)",
		1481.0, -1771.5, 15.3, 0.0, 0, 0,
		384.80, 173.80, 1008.38, 90.0, 3, 101, false, 0);

	// 2. RS All Saints General Hospital (SAMD ASGH)
	Doors_Create("Rumah Sakit All Saints (SAMD)",
		1172.8, -1323.2, 15.4, 270.0, 0, 0,
		1149.60, -1308.50, 1005.90, 0.0, 5, 102, false, 0);

	// 3. Markas Komando Kepolisian Pusat (SAPD LSPD)
	Doors_Create("Markas Besar Kepolisian (SAPD)",
		1554.8, -1675.6, 16.1, 90.0, 0, 0,
		246.78, 63.90, 1003.64, 0.0, 6, 103, false, 0);

	// 4. Toko Serba Ada 24/7 Idlewood
	Doors_Create("Toko Serba Ada 24/7 Idlewood",
		1352.0, -1759.0, 13.5, 0.0, 0, 0,
		-25.88, -185.86, 1003.54, 0.0, 17, 104, false, 0);

	// 5. Kantor Perencanaan & Registrasi Warga
	Doors_Create("Kantor Arsip & Registrasi Warga",
		1460.0, -1780.0, 15.3, 0.0, 0, 0,
		356.12, 152.00, 1025.78, 270.0, 3, 105, false, 0);

	return 1;
}
