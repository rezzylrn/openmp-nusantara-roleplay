#if defined _INC_weed_farm
	#endinput
#endif
#define _INC_weed_farm

/*
==============================================================================
 weed_farm.inc
 NUSANTARA ROLEPLAY - Dynamic Illegal Weed Planting, Growth & Harvest System
==============================================================================
*/

#define MAX_WEED_PLANTS         50
#define WEED_STAGE_2_TIME       90   // 1.5 Menit (Tunas)
#define WEED_STAGE_3_TIME       180  // 3 Menit (Siap Panen)

enum E_WEED_PLANT {
	bool:wpUsed,
	wpDBID,
	wpOwnerID,
	wpOwnerName[MAX_PLAYER_NAME],
	Float:wpX,
	Float:wpY,
	Float:wpZ,
	wpStage, // 1 = Bibit, 2 = Tumbuh, 3 = Siap Panen
	wpPlantTime,
	wpObject,
	Text3D:wpLabel
};

new gWeedPlants[MAX_WEED_PLANTS][E_WEED_PLANT];
new gTotalWeedPlants = 0;

forward OnWeedPlantsLoaded();
forward OnWeedPlantCreated(slot);
forward WeedFarm_Tick();

stock WeedFarm_Init()
{
	for (new i = 0; i < MAX_WEED_PLANTS; i++) {
		gWeedPlants[i][wpUsed] = false;
		gWeedPlants[i][wpDBID] = 0;
		gWeedPlants[i][wpObject] = INVALID_OBJECT_ID;
		gWeedPlants[i][wpLabel] = Text3D:INVALID_3DTEXT_ID;
	}
	gTotalWeedPlants = 0;

	if (gDBReady) {
		mysql_tquery(gDB, "SELECT * FROM `weed_plants` ORDER BY `id` ASC", "OnWeedPlantsLoaded", "");
	}
	SetTimer("WeedFarm_Tick", 10000, true); // Tick per 10 detik

	print("[WEED FARM] Sistem pertanian ganja bawah tanah dinamis aktif.");
	return 1;
}

public OnWeedPlantsLoaded()
{
	new rows = cache_num_rows();
	for (new i = 0; i < rows && i < MAX_WEED_PLANTS; i++) {
		gWeedPlants[i][wpUsed] = true;
		cache_get_value_name_int(i, "id", gWeedPlants[i][wpDBID]);
		cache_get_value_name_int(i, "ownerid", gWeedPlants[i][wpOwnerID]);
		cache_get_value_name_float(i, "posx", gWeedPlants[i][wpX]);
		cache_get_value_name_float(i, "posy", gWeedPlants[i][wpY]);
		cache_get_value_name_float(i, "posz", gWeedPlants[i][wpZ]);
		cache_get_value_name_int(i, "growth_stage", gWeedPlants[i][wpStage]);
		cache_get_value_name_int(i, "plant_time", gWeedPlants[i][wpPlantTime]);

		WeedFarm_CreateVisuals(i);
		gTotalWeedPlants++;
	}
	printf("[WEED FARM] %d Tanaman ganja dinamis berhasil dimuat dari MySQL.", gTotalWeedPlants);
	return 1;
}

stock WeedFarm_CreateVisuals(slot)
{
	if (slot < 0 || slot >= MAX_WEED_PLANTS || !gWeedPlants[slot][wpUsed]) return 0;

	// Model pot tanaman hijau (3409 / 861)
	new objModel = (gWeedPlants[slot][wpStage] >= 3) ? 3409 : 861;

	if (IsValidObject(gWeedPlants[slot][wpObject])) {
		DestroyObject(gWeedPlants[slot][wpObject]);
	}
	if (gWeedPlants[slot][wpLabel] != Text3D:INVALID_3DTEXT_ID) {
		Delete3DTextLabel(gWeedPlants[slot][wpLabel]);
	}

	gWeedPlants[slot][wpObject] = CreateObject(objModel,
		gWeedPlants[slot][wpX],
		gWeedPlants[slot][wpY],
		gWeedPlants[slot][wpZ] - 0.4,
		0.0, 0.0, 0.0, 100.0);

	new lbl[200];
	new stageStr[32];
	if (gWeedPlants[slot][wpStage] == 1) format(stageStr, sizeof(stageStr), "{FFFF00}Bibit Baru (30%%)");
	else if (gWeedPlants[slot][wpStage] == 2) format(stageStr, sizeof(stageStr), "{33CCFF}Tunas Daun (70%%)");
	else format(stageStr, sizeof(stageStr), "{4CD964}SIAP PANEN (100%%)");

	format(lbl, sizeof(lbl),
		"{00FF00}[ POT TANAMAN GANJA #%d ]\n{FFFFFF}Status: %s\n{FFFF00}Gunakan /harvestweed atau /panen",
		slot + 1, stageStr);

	gWeedPlants[slot][wpLabel] = Create3DTextLabel(lbl, 0x00FF00FF,
		gWeedPlants[slot][wpX],
		gWeedPlants[slot][wpY],
		gWeedPlants[slot][wpZ] + 0.5,
		15.0, 0, true);
	return 1;
}

stock ProcessPlantWeed(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;

	if (GetPlayerItemCount(playerid, ITEM_WEEDSEED) <= 0) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda tidak memiliki Benih Ganja di inventaris. Beli di Black Market.");
		return 0;
	}

	// [FIX] Tanaman ganja hanya boleh di dunia terbuka (bukan interior/rumah/
	// virtual world) dan di daratan. Sebelumnya bisa ditanam di mana saja,
	// termasuk interior bisnis / atap gedung.
	if (GetPlayerInterior(playerid) != 0 || GetPlayerVirtualWorld(playerid) != 0) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Benih ganja hanya bisa ditanam di alam terbuka, bukan di dalam ruangan.");
		return 0;
	}

	new Float:px, Float:py, Float:pz;
	GetPlayerPos(playerid, px, py, pz);

	// Tolak menanam di dalam air (z lebih rendah dari permukaan laut umum).
	if (pz < 0.0) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda tidak bisa menanam ganja di dalam air.");
		return 0;
	}

	// Periksa jarak agar tidak menumpuk pot tanaman terlalu dekat
	for (new i = 0; i < MAX_WEED_PLANTS; i++) {
		if (gWeedPlants[i][wpUsed] && IsPlayerNearPoint(playerid, gWeedPlants[i][wpX], gWeedPlants[i][wpY], gWeedPlants[i][wpZ], 2.0)) {
			SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda terlalu dekat dengan pot tanaman ganja lain!");
			return 0;
		}
	}

	new slot = -1;
	for (new i = 0; i < MAX_WEED_PLANTS; i++) {
		if (!gWeedPlants[i][wpUsed]) {
			slot = i;
			break;
		}
	}

	if (slot == -1) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Batas maksimal tanaman ganja kota telah tercapai.");
		return 0;
	}

	// Gunakan 1 benih
	TakePlayerItem(playerid, ITEM_WEEDSEED, 1);

	gWeedPlants[slot][wpUsed] = true;
	gWeedPlants[slot][wpOwnerID] = gPlayerData[playerid][pID];
	format(gWeedPlants[slot][wpOwnerName], MAX_PLAYER_NAME, "%s", gPlayerData[playerid][pName]);
	gWeedPlants[slot][wpX] = px;
	gWeedPlants[slot][wpY] = py;
	gWeedPlants[slot][wpZ] = pz;
	gWeedPlants[slot][wpStage] = 1;
	gWeedPlants[slot][wpPlantTime] = gettime();

	WeedFarm_CreateVisuals(slot);
	gTotalWeedPlants++;

	// Animasi menanam pot
	ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.1, false, false, false, false, 0);
	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);

	new succMsg[160];
	format(succMsg, sizeof(succMsg),
		"SUKSES: Benih ganja berhasil ditanam di pot #%d. Tunggu sekitar 3 menit hingga tanaman matang siap panen.", slot + 1);
	SendClientMessage(playerid, COLOR_SUCCESS, succMsg);

	if (gDBReady) {
		new query[320];
		mysql_format(gDB, query, sizeof(query),
			"INSERT INTO `weed_plants` (`ownerid`, `posx`, `posy`, `posz`, `growth_stage`, `plant_time`) \
			VALUES (%d, %.2f, %.2f, %.2f, 1, %d)",
			gPlayerData[playerid][pID], px, py, pz, gWeedPlants[slot][wpPlantTime]);
		mysql_tquery(gDB, query, "OnWeedPlantCreated", "i", slot);
	}
	return 1;
}

public OnWeedPlantCreated(slot)
{
	if (slot >= 0 && slot < MAX_WEED_PLANTS && gWeedPlants[slot][wpUsed]) {
		gWeedPlants[slot][wpDBID] = cache_insert_id();
	}
	return 1;
}

stock ProcessHarvestWeed(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;

	new plantSlot = -1;
	for (new i = 0; i < MAX_WEED_PLANTS; i++) {
		if (gWeedPlants[i][wpUsed] && IsPlayerNearPoint(playerid, gWeedPlants[i][wpX], gWeedPlants[i][wpY], gWeedPlants[i][wpZ], 2.5)) {
			plantSlot = i;
			break;
		}
	}

	if (plantSlot == -1) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda tidak berada di dekat tanaman ganja mana pun.");
		return 0;
	}

	if (gWeedPlants[plantSlot][wpStage] < 3) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Tanaman ini masih dalam proses pertumbuhan (belum siap panen)!");
		return 0;
	}

	// Panen 2 - 4 lembar paket daun ganja
	new yieldAmount = 2 + random(3);
	if (!GivePlayerItem(playerid, ITEM_WEED, yieldAmount)) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Kantung inventaris Anda penuh untuk menampung hasil panen.");
		return 0;
	}

	WeedFarm_DeletePlant(plantSlot);
	ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.1, false, false, false, false, 0);
	PlayerPlaySound(playerid, 1058, 0.0, 0.0, 0.0);

	new succMsg[160];
	format(succMsg, sizeof(succMsg),
		"SUKSES: Berhasil memanen %dx Paket Daun Ganja Kering! Jual ke Black Market atau linting sendiri.", yieldAmount);
	SendClientMessage(playerid, COLOR_SUCCESS, succMsg);
	return 1;
}

stock ProcessBurnWeed(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;

	if (gPlayerData[playerid][pFaction] != FACTION_POLICE || !gPlayerData[playerid][pDuty]) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Perintah ini khusus untuk petugas kepolisian SAPD yang bertugas!");
		return 0;
	}

	new plantSlot = -1;
	for (new i = 0; i < MAX_WEED_PLANTS; i++) {
		if (gWeedPlants[i][wpUsed] && IsPlayerNearPoint(playerid, gWeedPlants[i][wpX], gWeedPlants[i][wpY], gWeedPlants[i][wpZ], 2.8)) {
			plantSlot = i;
			break;
		}
	}

	if (plantSlot == -1) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Tidak ada pot tanaman ganja terdekat yang dapat dimusnahkan.");
		return 0;
	}

	WeedFarm_DeletePlant(plantSlot);
	PlayerPlaySound(playerid, 1133, 0.0, 0.0, 0.0);

	SendClientMessage(playerid, COLOR_LIGHTBLUE, "[SAPD] Berhasil membakar dan memusnahkan pot tanaman narkotika ilegal di tempat!");
	return 1;
}

stock WeedFarm_DeletePlant(slot)
{
	if (slot < 0 || slot >= MAX_WEED_PLANTS || !gWeedPlants[slot][wpUsed]) return 0;

	if (IsValidObject(gWeedPlants[slot][wpObject])) {
		DestroyObject(gWeedPlants[slot][wpObject]);
		gWeedPlants[slot][wpObject] = INVALID_OBJECT_ID;
	}
	if (gWeedPlants[slot][wpLabel] != Text3D:INVALID_3DTEXT_ID) {
		Delete3DTextLabel(gWeedPlants[slot][wpLabel]);
		gWeedPlants[slot][wpLabel] = Text3D:INVALID_3DTEXT_ID;
	}

	if (gDBReady && gWeedPlants[slot][wpDBID] > 0) {
		new query[128];
		mysql_format(gDB, query, sizeof(query), "DELETE FROM `weed_plants` WHERE `id`=%d", gWeedPlants[slot][wpDBID]);
		mysql_tquery(gDB, query, "", "");
	}

	gWeedPlants[slot][wpUsed] = false;
	gWeedPlants[slot][wpDBID] = 0;
	gTotalWeedPlants--;
	return 1;
}

public WeedFarm_Tick()
{
	new now = gettime();
	for (new i = 0; i < MAX_WEED_PLANTS; i++) {
		if (!gWeedPlants[i][wpUsed]) continue;

		new age = now - gWeedPlants[i][wpPlantTime];
		new oldStage = gWeedPlants[i][wpStage];

		if (age >= WEED_STAGE_3_TIME) {
			gWeedPlants[i][wpStage] = 3;
		} else if (age >= WEED_STAGE_2_TIME) {
			gWeedPlants[i][wpStage] = 2;
		}

		if (gWeedPlants[i][wpStage] != oldStage) {
			WeedFarm_CreateVisuals(i);
			if (gDBReady && gWeedPlants[i][wpDBID] > 0) {
				new query[128];
				mysql_format(gDB, query, sizeof(query),
					"UPDATE `weed_plants` SET `growth_stage`=%d WHERE `id`=%d",
					gWeedPlants[i][wpStage], gWeedPlants[i][wpDBID]);
				mysql_tquery(gDB, query, "", "");
			}
		}
	}
	return 1;
}
