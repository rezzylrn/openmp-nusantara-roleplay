#if defined _INC_rp_atms
	#endinput
#endif
#define _INC_rp_atms

/*
==============================================================================
  NUSANTARA ROLEPLAY - atms.inc
  Sistem Mesin ATM Dinamis (Dynamic Automated Teller Machine)
==============================================================================
*/

#define MAX_DYNAMIC_ATMS        50
#define RANGE_ATM_INTERACTION   2.5

enum E_ATM_DATA {
	bool:atmUsed,
	atmDBID,
	Float:atmX,
	Float:atmY,
	Float:atmZ,
	Float:atmRX,
	Float:atmRY,
	Float:atmRZ,
	atmInterior,
	atmVW,
	// Runtime handles
	atmObject,
	Text3D:atmLabel
};

new gATMs[MAX_DYNAMIC_ATMS][E_ATM_DATA];
new gTotalATMs = 0;

forward OnATMsLoaded();
forward OnATMInserted(slot);

stock ATMs_Init()
{
	for (new i = 0; i < MAX_DYNAMIC_ATMS; i++) {
		gATMs[i][atmUsed] = false;
		gATMs[i][atmObject] = INVALID_OBJECT_ID;
		gATMs[i][atmLabel] = Text3D:INVALID_3DTEXT_ID;
	}

	if (!gDBReady) return 0;

	mysql_tquery(gDB, "CREATE TABLE IF NOT EXISTS `atms` (\
		`id` INT AUTO_INCREMENT PRIMARY KEY, \
		`x` FLOAT NOT NULL, \
		`y` FLOAT NOT NULL, \
		`z` FLOAT NOT NULL, \
		`rx` FLOAT DEFAULT 0.0, \
		`ry` FLOAT DEFAULT 0.0, \
		`rz` FLOAT DEFAULT 0.0, \
		`interior` INT DEFAULT 0, \
		`vw` INT DEFAULT 0\
	) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4", "", "");

	mysql_tquery(gDB, "SELECT * FROM `atms`", "OnATMsLoaded", "");
	return 1;
}

public OnATMsLoaded()
{
	new rows = cache_num_rows();
	gTotalATMs = 0;

	for (new i = 0; i < rows && i < MAX_DYNAMIC_ATMS; i++) {
		gATMs[i][atmUsed] = true;
		cache_get_value_name_int(i, "id", gATMs[i][atmDBID]);
		cache_get_value_name_float(i, "x", gATMs[i][atmX]);
		cache_get_value_name_float(i, "y", gATMs[i][atmY]);
		cache_get_value_name_float(i, "z", gATMs[i][atmZ]);
		cache_get_value_name_float(i, "rx", gATMs[i][atmRX]);
		cache_get_value_name_float(i, "ry", gATMs[i][atmRY]);
		cache_get_value_name_float(i, "rz", gATMs[i][atmRZ]);
		cache_get_value_name_int(i, "interior", gATMs[i][atmInterior]);
		cache_get_value_name_int(i, "vw", gATMs[i][atmVW]);

		ATMs_CreateVisuals(i);
		gTotalATMs++;
	}

	printf("[ATM] Berhasil memuat %d mesin ATM dinamis dari database MySQL.", gTotalATMs);

	if (gTotalATMs == 0) {
		ATMs_CreateDefaultSet();
	}
	return 1;
}

stock ATMs_CreateVisuals(slot)
{
	if (slot < 0 || slot >= MAX_DYNAMIC_ATMS || !gATMs[slot][atmUsed]) return 0;

	if (gATMs[slot][atmObject] != INVALID_OBJECT_ID) DestroyObject(gATMs[slot][atmObject]);
	if (gATMs[slot][atmLabel] != Text3D:INVALID_3DTEXT_ID) Delete3DTextLabel(gATMs[slot][atmLabel]);

	// Model 2942 = Mesin ATM GTA San Andreas
	gATMs[slot][atmObject] = CreateObject(2942,
		gATMs[slot][atmX], gATMs[slot][atmY], gATMs[slot][atmZ],
		gATMs[slot][atmRX], gATMs[slot][atmRY], gATMs[slot][atmRZ], 150.0);

	new lblText[144];
	format(lblText, sizeof(lblText),
		"{4CD964}[ ATM NUSANTARA #%d ]\n{FFFFFF}Tekan {FFFF00}'Y'{FFFFFF} atau gunakan {FFFF00}/atm",
		slot + 1);

	gATMs[slot][atmLabel] = Create3DTextLabel(lblText, COLOR_GREEN,
		gATMs[slot][atmX], gATMs[slot][atmY], gATMs[slot][atmZ] + 0.8,
		12.0, gATMs[slot][atmVW], true);
	return 1;
}

stock ATM_Create(Float:x, Float:y, Float:z, Float:rx = 0.0, Float:ry = 0.0, Float:rz = 0.0, interior = 0, vw = 0)
{
	new slot = -1;
	for (new i = 0; i < MAX_DYNAMIC_ATMS; i++) {
		if (!gATMs[i][atmUsed]) {
			slot = i;
			break;
		}
	}
	if (slot == -1) return -1;

	gATMs[slot][atmUsed] = true;
	gATMs[slot][atmX] = x;
	gATMs[slot][atmY] = y;
	gATMs[slot][atmZ] = z;
	gATMs[slot][atmRX] = rx;
	gATMs[slot][atmRY] = ry;
	gATMs[slot][atmRZ] = rz;
	gATMs[slot][atmInterior] = interior;
	gATMs[slot][atmVW] = vw;

	ATMs_CreateVisuals(slot);
	gTotalATMs++;

	if (gDBReady) {
		new query[350];
		mysql_format(gDB, query, sizeof(query),
			"INSERT INTO `atms` (`x`, `y`, `z`, `rx`, `ry`, `rz`, `interior`, `vw`) \
			VALUES (%.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %d, %d)",
			x, y, z, rx, ry, rz, interior, vw);

		mysql_tquery(gDB, query, "OnATMInserted", "d", slot);
	}
	return slot;
}

public OnATMInserted(slot)
{
	if (slot < 0 || slot >= MAX_DYNAMIC_ATMS || !gATMs[slot][atmUsed]) return 0;
	gATMs[slot][atmDBID] = cache_insert_id();
	return 1;
}

stock ATM_Delete(slot)
{
	if (slot < 0 || slot >= MAX_DYNAMIC_ATMS || !gATMs[slot][atmUsed]) return 0;

	if (gATMs[slot][atmObject] != INVALID_OBJECT_ID) DestroyObject(gATMs[slot][atmObject]);
	if (gATMs[slot][atmLabel] != Text3D:INVALID_3DTEXT_ID) Delete3DTextLabel(gATMs[slot][atmLabel]);

	if (gDBReady && gATMs[slot][atmDBID] > 0) {
		new query[128];
		mysql_format(gDB, query, sizeof(query), "DELETE FROM `atms` WHERE `id`=%d", gATMs[slot][atmDBID]);
		mysql_tquery(gDB, query, "", "");
	}

	gATMs[slot][atmUsed] = false;
	gTotalATMs--;
	return 1;
}

stock IsPlayerNearAnyATM(playerid, Float:range = RANGE_ATM_INTERACTION)
{
	new Float:px, Float:py, Float:pz;
	new pVW = 0, pInt = 0;
	if (IsPlayerConnected(playerid)) {
		GetPlayerPos(playerid, px, py, pz);
		pVW = GetPlayerVirtualWorld(playerid);
		pInt = GetPlayerInterior(playerid);
	} else {
		px = gPlayerData[playerid][pLastX];
		py = gPlayerData[playerid][pLastY];
		pz = gPlayerData[playerid][pLastZ];
	}

	for (new i = 0; i < MAX_DYNAMIC_ATMS; i++) {
		if (!gATMs[i][atmUsed]) continue;
		if (gATMs[i][atmVW] == pVW && gATMs[i][atmInterior] == pInt) {
			new Float:dist = VectorSize(px - gATMs[i][atmX], py - gATMs[i][atmY], pz - gATMs[i][atmZ]);
			if (dist <= range) {
				return 1;
			}
		}
	}
	return 0;
}

stock ATMs_CreateDefaultSet()
{
	print("[ATM] Memasang mesin ATM dinamis bawaan (Pershing Square, ASGH, Idlewood, Unity Station)...");

	// 1. Dinding Depan Bank Nusantara Pershing Square
	ATM_Create(1496.8, -1738.5, 13.54, 0.0, 0.0, 0.0);
	ATM_Create(1498.2, -1738.5, 13.54, 0.0, 0.0, 0.0);

	// 2. Drop-off Rumah Sakit All Saints (ASGH)
	ATM_Create(1177.5, -1320.0, 13.80, 0.0, 0.0, 90.0);

	// 3. Toko Serba Ada 24/7 Idlewood
	ATM_Create(1350.5, -1753.0, 13.50, 0.0, 0.0, 0.0);

	// 4. Stasiun Kereta Api Unity Station
	ATM_Create(1743.0, -1862.0, 13.50, 0.0, 0.0, 90.0);

	// 5. Dermaga Mancing Santa Maria Beach
	ATM_Create(376.0, -2040.0, 8.00, 0.0, 0.0, 270.0);

	return 1;
}
