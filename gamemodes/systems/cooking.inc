#if defined _INC_rp_cooking
	#endinput
#endif
#define _INC_rp_cooking

/*
==============================================================================
 cooking.inc
 NUSANTARA ROLEPLAY - Campfire Building & Outdoor Fish Cooking System
==============================================================================
*/

#define MAX_CAMPFIRES           20
#define CAMPFIRE_DURATION       600 // 10 Menit

enum E_CAMPFIRE {
	bool:cfUsed,
	Float:cfX,
	Float:cfY,
	Float:cfZ,
	cfExpire,
	cfObject,
	Text3D:cfLabel
};

new gCampfires[MAX_CAMPFIRES][E_CAMPFIRE];

stock Cooking_Init()
{
	for (new i = 0; i < MAX_CAMPFIRES; i++) {
		gCampfires[i][cfUsed] = false;
		gCampfires[i][cfObject] = INVALID_OBJECT_ID;
		gCampfires[i][cfLabel] = Text3D:INVALID_3DTEXT_ID;
	}
	print("[COOKING] Sistem api unggun & memasak ikan bakar luar ruangan aktif.");
	return 1;
}

stock ProcessCreateCampfire(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;

	if (IsPlayerInAnyVehicle(playerid)) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus berjalan kaki untuk menyalakan api unggun.");
		return 0;
	}

	if (GetPlayerItemCount(playerid, ITEM_WOOD) <= 0) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda memerlukan Balok Kayu (Wood) di inventaris. Tebang pohon di hutan (/gps).");
		return 0;
	}

	if (GetPlayerItemCount(playerid, ITEM_LIGHTER) <= 0) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda memerlukan Korek Api (Lighter) untuk menyulut api. Beli di toko 24/7.");
		return 0;
	}

	new slot = -1;
	for (new i = 0; i < MAX_CAMPFIRES; i++) {
		if (!gCampfires[i][cfUsed]) {
			slot = i;
			break;
		}
	}

	if (slot == -1) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Batas maksimal api unggun kota telah tercapai.");
		return 0;
	}

	new Float:px, Float:py, Float:pz, Float:pa;
	GetPlayerPos(playerid, px, py, pz);
	GetPlayerFacingAngle(playerid, pa);

	new Float:fx = px + (1.2 * floatsin(-pa, degrees));
	new Float:fy = py + (1.2 * floatcos(-pa, degrees));

	// Gunakan 1 balok kayu
	TakePlayerItem(playerid, ITEM_WOOD, 1);

	gCampfires[slot][cfUsed] = true;
	gCampfires[slot][cfX] = fx;
	gCampfires[slot][cfY] = fy;
	gCampfires[slot][cfZ] = pz;
	gCampfires[slot][cfExpire] = gettime() + CAMPFIRE_DURATION;

	// Objek api unggun (Model 3525 / 841)
	gCampfires[slot][cfObject] = CreateObject(3525, fx, fy, pz - 0.2, 0.0, 0.0, 0.0, 80.0);

	new lbl[160];
	format(lbl, sizeof(lbl),
		"{FF6600}[ API UNGGUN MEMBARA ]\n{FFFFFF}Dibuat oleh: %s\n{FFFF00}Gunakan /cook atau /bakar untuk memasak ikan",
		gPlayerData[playerid][pName]);

	gCampfires[slot][cfLabel] = Create3DTextLabel(lbl, 0xFF6600FF, fx, fy, pz + 0.6, 12.0, 0, true);

	ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.1, false, false, false, false, 0);
	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);

	SendClientMessage(playerid, COLOR_SUCCESS,
		"SUKSES: Api unggun berhasil dinyalakan! Api akan bertahan selama 10 menit. Dekati untuk membakar ikan (/cook).");
	return 1;
}

stock ProcessCookFish(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;

	new fireSlot = -1;
	for (new i = 0; i < MAX_CAMPFIRES; i++) {
		if (gCampfires[i][cfUsed] && IsPlayerNearPoint(playerid, gCampfires[i][cfX], gCampfires[i][cfY], gCampfires[i][cfZ], 3.5)) {
			fireSlot = i;
			break;
		}
	}

	if (fireSlot == -1) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus berada di dekat api unggun yang menyala (/campfire).");
		return 0;
	}

	if (gPlayerData[playerid][pFishWeight] < 1) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda tidak memiliki ikan mentah hasil pancingan (/fish). Minimal 1 Kg.");
		return 0;
	}

	if (!GivePlayerItem(playerid, ITEM_COOKEDFISH, 1)) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Kantung inventaris Anda penuh.");
		return 0;
	}

	gPlayerData[playerid][pFishWeight] -= 1;
	if (gPlayerData[playerid][pFishWeight] < 0) gPlayerData[playerid][pFishWeight] = 0;

	ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.1, false, false, false, false, 0);
	PlayerPlaySound(playerid, 1058, 0.0, 0.0, 0.0);

	new succMsg[160];
	format(succMsg, sizeof(succMsg),
		"SUKSES: Ikan mentah berhasil dipanggang menjadi 1x Ikan Bakar Matang! Gunakan /makanikan untuk menyantap.");
	SendClientMessage(playerid, COLOR_SUCCESS, succMsg);
	return 1;
}

stock ProcessEatCookedFish(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;

	if (GetPlayerItemCount(playerid, ITEM_COOKEDFISH) <= 0) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda tidak memiliki Ikan Bakar di inventaris (/inv). Masak via /cook.");
		return 0;
	}

	TakePlayerItem(playerid, ITEM_COOKEDFISH, 1);

	// Pulihkan lapar +60% dan darah HP +25%
	gPlayerData[playerid][pHunger] += 60.0;
	if (gPlayerData[playerid][pHunger] > 100.0) gPlayerData[playerid][pHunger] = 100.0;

	new Float:hp;
	GetPlayerHealth(playerid, hp);
	hp += 25.0;
	if (hp > 100.0) hp = 100.0;
	SetPlayerHealth(playerid, hp);

	UpdatePlayerHUD(playerid);
	ApplyAnimation(playerid, "FOOD", "EAT_Burger", 4.1, false, false, false, false, 0);
	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);

	SendClientMessage(playerid, COLOR_SUCCESS,
		"SUKSES: Menyantap Ikan Bakar lezat! Rasa lapar pulih +60% dan kondisi darah bertambah +25 HP.");
	return 1;
}
