#if defined _INC_rp_trash
	#endinput
#endif
#define _INC_rp_trash

/*
==============================================================================
  NUSANTARA ROLEPLAY - trash.inc
  Sistem Tong Sampah Dinamis & Pemulung Rongsokan (Dynamic Trash & Scavenge)
==============================================================================
*/

#define MAX_DYNAMIC_TRASH       60
#define RANGE_TRASH_INTERACTION 2.5
#define SCAVENGE_COOLDOWN_SEC   25

enum E_TRASH_DATA {
	bool:trUsed,
	trDBID,
	Float:trX,
	Float:trY,
	Float:trZ,
	Float:trRZ,
	trInterior,
	trVW,
	trCapacity,
	// Runtime handles
	trObject,
	Text3D:trLabel
};

new gTrashBins[MAX_DYNAMIC_TRASH][E_TRASH_DATA];
new gTotalTrash = 0;
new gPlayerScavengeCooldown[MAX_PLAYERS];

forward OnTrashLoaded();
forward OnTrashInserted(slot);

stock Trash_Init()
{
	for (new i = 0; i < MAX_DYNAMIC_TRASH; i++) {
		gTrashBins[i][trUsed] = false;
		gTrashBins[i][trObject] = INVALID_OBJECT_ID;
		gTrashBins[i][trLabel] = Text3D:INVALID_3DTEXT_ID;
	}
	for (new i = 0; i < MAX_PLAYERS; i++) {
		gPlayerScavengeCooldown[i] = 0;
	}

	if (!gDBReady) return 0;

	mysql_tquery(gDB, "CREATE TABLE IF NOT EXISTS `trash_bins` (\
		`id` INT AUTO_INCREMENT PRIMARY KEY, \
		`x` FLOAT NOT NULL, \
		`y` FLOAT NOT NULL, \
		`z` FLOAT NOT NULL, \
		`rz` FLOAT DEFAULT 0.0, \
		`interior` INT DEFAULT 0, \
		`vw` INT DEFAULT 0, \
		`capacity` INT DEFAULT 100\
	) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4", "", "");

	mysql_tquery(gDB, "SELECT * FROM `trash_bins`", "OnTrashLoaded", "");
	return 1;
}

public OnTrashLoaded()
{
	new rows = cache_num_rows();
	gTotalTrash = 0;

	for (new i = 0; i < rows && i < MAX_DYNAMIC_TRASH; i++) {
		gTrashBins[i][trUsed] = true;
		cache_get_value_name_int(i, "id", gTrashBins[i][trDBID]);
		cache_get_value_name_float(i, "x", gTrashBins[i][trX]);
		cache_get_value_name_float(i, "y", gTrashBins[i][trY]);
		cache_get_value_name_float(i, "z", gTrashBins[i][trZ]);
		cache_get_value_name_float(i, "rz", gTrashBins[i][trRZ]);
		cache_get_value_name_int(i, "interior", gTrashBins[i][trInterior]);
		cache_get_value_name_int(i, "vw", gTrashBins[i][trVW]);
		cache_get_value_name_int(i, "capacity", gTrashBins[i][trCapacity]);

		Trash_CreateVisuals(i);
		gTotalTrash++;
	}

	printf("[TRASH] Berhasil memuat %d tong sampah dinamis dari database MySQL.", gTotalTrash);

	if (gTotalTrash == 0) {
		Trash_CreateDefaultSet();
	}
	return 1;
}

stock Trash_CreateVisuals(slot)
{
	if (slot < 0 || slot >= MAX_DYNAMIC_TRASH || !gTrashBins[slot][trUsed]) return 0;

	if (gTrashBins[slot][trObject] != INVALID_OBJECT_ID) DestroyObject(gTrashBins[slot][trObject]);
	if (gTrashBins[slot][trLabel] != Text3D:INVALID_3DTEXT_ID) Delete3DTextLabel(gTrashBins[slot][trLabel]);

	// Model 1337 = Tong Sampah Trotoar Los Santos
	gTrashBins[slot][trObject] = CreateObject(1337,
		gTrashBins[slot][trX], gTrashBins[slot][trY], gTrashBins[slot][trZ],
		0.0, 0.0, gTrashBins[slot][trRZ], 120.0);

	new lblText[160];
	format(lblText, sizeof(lblText),
		"{A0A0A0}[ TONG SAMPAH #%d ]\n{FFFFFF}Gunakan {FFFF00}/rongsok{FFFFFF} untuk mencari barang\nAtau {FFFF00}/buang{FFFFFF} sampah",
		slot + 1);

	gTrashBins[slot][trLabel] = Create3DTextLabel(lblText, 0xA0A0A0FF,
		gTrashBins[slot][trX], gTrashBins[slot][trY], gTrashBins[slot][trZ] + 0.8,
		10.0, gTrashBins[slot][trVW], true);
	return 1;
}

stock Trash_Create(Float:x, Float:y, Float:z, Float:rz = 0.0, interior = 0, vw = 0, capacity = 100)
{
	new slot = -1;
	for (new i = 0; i < MAX_DYNAMIC_TRASH; i++) {
		if (!gTrashBins[i][trUsed]) {
			slot = i;
			break;
		}
	}
	if (slot == -1) return -1;

	gTrashBins[slot][trUsed] = true;
	gTrashBins[slot][trX] = x;
	gTrashBins[slot][trY] = y;
	gTrashBins[slot][trZ] = z;
	gTrashBins[slot][trRZ] = rz;
	gTrashBins[slot][trInterior] = interior;
	gTrashBins[slot][trVW] = vw;
	gTrashBins[slot][trCapacity] = capacity;

	Trash_CreateVisuals(slot);
	gTotalTrash++;

	if (gDBReady) {
		new query[350];
		mysql_format(gDB, query, sizeof(query),
			"INSERT INTO `trash_bins` (`x`, `y`, `z`, `rz`, `interior`, `vw`, `capacity`) \
			VALUES (%.4f, %.4f, %.4f, %.4f, %d, %d, %d)",
			x, y, z, rz, interior, vw, capacity);

		mysql_tquery(gDB, query, "OnTrashInserted", "d", slot);
	}
	return slot;
}

public OnTrashInserted(slot)
{
	if (slot < 0 || slot >= MAX_DYNAMIC_TRASH || !gTrashBins[slot][trUsed]) return 0;
	gTrashBins[slot][trDBID] = cache_insert_id();
	return 1;
}

stock Trash_Delete(slot)
{
	if (slot < 0 || slot >= MAX_DYNAMIC_TRASH || !gTrashBins[slot][trUsed]) return 0;

	if (gTrashBins[slot][trObject] != INVALID_OBJECT_ID) DestroyObject(gTrashBins[slot][trObject]);
	if (gTrashBins[slot][trLabel] != Text3D:INVALID_3DTEXT_ID) Delete3DTextLabel(gTrashBins[slot][trLabel]);

	if (gDBReady && gTrashBins[slot][trDBID] > 0) {
		new query[128];
		mysql_format(gDB, query, sizeof(query), "DELETE FROM `trash_bins` WHERE `id`=%d", gTrashBins[slot][trDBID]);
		mysql_tquery(gDB, query, "", "");
	}

	gTrashBins[slot][trUsed] = false;
	gTotalTrash--;
	return 1;
}

stock IsPlayerNearAnyTrash(playerid, Float:range = RANGE_TRASH_INTERACTION)
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

	for (new i = 0; i < MAX_DYNAMIC_TRASH; i++) {
		if (!gTrashBins[i][trUsed]) continue;
		if (gTrashBins[i][trVW] == pVW && gTrashBins[i][trInterior] == pInt) {
			new Float:dist = VectorSize(px - gTrashBins[i][trX], py - gTrashBins[i][trY], pz - gTrashBins[i][trZ]);
			if (dist <= range) {
				return 1;
			}
		}
	}
	return 0;
}

// Gameplay Pemulung / Scavenge Tong Sampah
stock ProcessPlayerScavenge(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendErr(playerid, "Anda tidak dapat memulung saat pingsan atau terborgol.");
		return 0;
	}
	if (gPlayerData[playerid][pJob] != JOB_SCAVENGER) {
		SendErr(playerid, "Aktivitas ini khusus untuk Pemulung Rongsokan (/job di Balai Kota).");
		return 0;
	}

	if (!IsPlayerNearAnyTrash(playerid)) {
		if (gPlayerData[playerid][pLanguage] == LANG_EN) SendErr(playerid, "You must be near a trash bin.");
		else SendErr(playerid, "Anda harus berada di dekat tong sampah.");
		return 0;
	}

	new now = gettime();
	if (now < gPlayerScavengeCooldown[playerid]) {
		new sLeft = gPlayerScavengeCooldown[playerid] - now;
		if (gPlayerData[playerid][pLanguage] == LANG_EN) SendErr(playerid, "Please wait %d seconds before searching this trash bin again.", sLeft);
		else SendErr(playerid, "Tunggu %d detik sebelum mengais tong sampah kembali.", sLeft);
		return 0;
	}

	gPlayerScavengeCooldown[playerid] = now + SCAVENGE_COOLDOWN_SEC;

	// Animasi membungkuk mencari barang di tong sampah
	ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.0, false, false, false, false, 2000, SYNC_ALL);
	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);

	new roll = random(100);
	if (roll < 45) {
		// Dapat barang rongsokan / daur ulang
		new count = 1 + random(3);
		GivePlayerItem(playerid, ITEM_RECYCLED, count);
		if (gPlayerData[playerid][pLanguage] == LANG_EN) SendSucc(playerid, "[SCAVENGE] You found %dx Recycled Scraps! Sell them at the Scrap Collector (/selltrash).", count);
		else SendSucc(playerid, "[RONGSOK] Anda menemukan %dx Barang Rongsokan! Jual ke pengepul rongsokan (/selltrash).", count);
	} else if (roll < 70) {
		// Dapat uang receh
		new cash = 5 + random(30);
		GivePlayerMoneyEx(playerid, cash, "temu receh di tong sampah");
		if (gPlayerData[playerid][pLanguage] == LANG_EN) SendSucc(playerid, "[SCAVENGE] You found $%d scattered in the trash!", cash);
		else SendSucc(playerid, "[RONGSOK] Anda menemukan uang receh $%d tercecer di tong sampah!", cash);
	} else if (roll < 85) {
		// Dapat air / snack sisa
		new it = (random(2) == 0) ? ITEM_WATER : ITEM_SNACK;
		GivePlayerItem(playerid, it, 1);
		new itName[32];
		GetItemName(it, itName, sizeof(itName));
		if (gPlayerData[playerid][pLanguage] == LANG_EN) SendSucc(playerid, "[SCAVENGE] You found an unopened %s in the bin!", itName);
		else SendSucc(playerid, "[RONGSOK] Anda menemukan 1x %s yang masih layak!", itName);
	} else {
		if (gPlayerData[playerid][pLanguage] == LANG_EN) SendInfo(playerid, "[SCAVENGE] This trash bin is empty. Try another one!");
		else SendInfo(playerid, "[RONGSOK] Tong sampah ini kosong. Coba cari di tong sampah lain!");
	}

	new nearbyMsg[128];
	format(nearbyMsg, sizeof(nearbyMsg), "* %s mengais dan merogoh ke dalam tong sampah.", gPlayerData[playerid][pName]);
	SendNearbyPlayer(playerid, COLOR_RP_ME, nearbyMsg, 8.0);
	return 1;
}

stock Trash_CreateDefaultSet()
{
	print("[TRASH] Memasang tong sampah dinamis bawaan (Pershing Square, ASGH, 24/7 Store, Unity Station)...");

	// 1. Pershing Square Plaza
	Trash_Create(1479.0, -1762.0, 13.50, 0.0);
	Trash_Create(1481.0, -1740.0, 13.50, 180.0);

	// 2. Rumah Sakit ASGH
	Trash_Create(1175.0, -1324.0, 13.80, 270.0);

	// 3. Toko Serba Ada 24/7 Idlewood
	Trash_Create(1348.0, -1758.0, 13.50, 0.0);

	// 4. Stasiun Unity Station
	Trash_Create(1753.0, -1860.0, 13.50, 180.0);

	// 5. Gang Ganton / Grove
	Trash_Create(2490.0, -1670.0, 13.50, 90.0);

	// 6. SPBU Idlewood
	Trash_Create(1935.0, -1770.0, 13.50, 0.0);

	return 1;
}
