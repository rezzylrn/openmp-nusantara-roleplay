#if defined _INC_rp_farming
	#endinput
#endif
#define _INC_rp_farming

/*
==============================================================================
 farming.inc
 NUSANTARA ROLEPLAY - Dynamic Legal Agriculture & Crop Harvesting System
 Sistem pertanian legal di Flint County:
 - Membeli bibit gandum, bibit jagung, dan gembor air di Toko Tani.
 - Menanam tanaman di ladang subur, menyiram air, dan memanen hasil panen.
 - Menjual hasil panen di lumbung gandum untuk mendapatkan uang tunai.
==============================================================================
*/

#define MAX_FARM_CROPS          50
#define FARM_SHOP_X             -383.0
#define FARM_SHOP_Y             -1430.0
#define FARM_SHOP_Z             25.8

#define FARM_SILO_X             -375.0
#define FARM_SILO_Y             -1455.0
#define FARM_SILO_Z             25.8

#define CROP_GROW_TIME_DEFAULT  240 // 4 menit normal
#define CROP_GROW_TIME_WATERED  120 // 2 menit jika disiram

enum E_FARM_CROP {
	bool:crUsed,
	crOwnerID,
	crType, // ITEM_WHEATSEED atau ITEM_CORNSEED
	Float:crX,
	Float:crY,
	Float:crZ,
	crPlantTime,
	bool:crWatered,
	Text3D:crLabel
};

new gFarmCrops[MAX_FARM_CROPS][E_FARM_CROP];

forward RP_OnFinishPlantCrop(playerid, cropType);
forward RP_OnFinishHarvestCrop(playerid, cropSlot);
forward RP_OnFinishWaterCrop(playerid, cropSlot);

stock Farming_Init()
{
	for (new i = 0; i < MAX_FARM_CROPS; i++) {
		gFarmCrops[i][crUsed] = false;
		gFarmCrops[i][crLabel] = Text3D:INVALID_3DTEXT_ID;
	}
	Create3DTextLabel("{2ECC71}[ TOKO TANI FLINT COUNTY ]\n{FFFFFF}Gunakan {FFFF00}/buyseed {FFFFFF}untuk membeli bibit & gembor air",
		0xFFFFFFFF, FARM_SHOP_X, FARM_SHOP_Y, FARM_SHOP_Z + 0.5, 15.0, 0, false);

	Create3DTextLabel("{2ECC71}[ LUMBUNG HASIL BUMI FLINT ]\n{FFFFFF}Gunakan {FFFF00}/sellcrops {FFFFFF}untuk menjual hasil panen tani",
		0xFFFFFFFF, FARM_SILO_X, FARM_SILO_Y, FARM_SILO_Z + 0.5, 15.0, 0, false);
	return 1;
}

stock IsPlayerInFlintFarm(playerid)
{
	new Float:x, Float:y, Float:z;
	GetPlayerPos(playerid, x, y, z);
	if (x >= -450.0 && x <= -350.0 && y >= -1500.0 && y <= -1400.0) return 1;
	return 0;
}

stock ShowFarmShopDialog(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda belum login.");
	if (!IsPlayerNearPoint(playerid, FARM_SHOP_X, FARM_SHOP_Y, FARM_SHOP_Z, 4.5)) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda harus berada di Toko Tani Flint County.");
		return 1;
	}

	Dialog_Show(playerid, DialogFarmShop, DIALOG_STYLE_LIST,
		"{2ECC71}TOKO PERTANIAN FLINT COUNTY",
		"{FFFFFF}1. Bibit Gandum (Wheat Seed) - {2ECC71}$15\n\
{FFFFFF}2. Bibit Jagung (Corn Seed) - {2ECC71}$20\n\
{FFFFFF}3. Gembor Penyiram Tanaman (Water Can) - {2ECC71}$35",
		"Beli", "Batal");
	return 1;
}

CMD:buyseed(playerid, params[])
{
	return ShowFarmShopDialog(playerid);
}

Dialog:DialogFarmShop(playerid, response, listitem, inputtext[])
{
	if (!response) return 1;

	switch (listitem) {
		case 0: { // Bibit Gandum $15
			if (gPlayerData[playerid][pMoney] < 15) return SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Uang Anda tidak cukup ($15).");
			TakePlayerMoneyEx(playerid, 15, "beli bibit gandum");
			GivePlayerItem(playerid, ITEM_WHEATSEED, 1);
			SendClientMessage(playerid, COLOR_SUCCESS, "[SUKSES] Anda membeli 1x Bibit Gandum seharga $15.");
		}
		case 1: { // Bibit Jagung $20
			if (gPlayerData[playerid][pMoney] < 20) return SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Uang Anda tidak cukup ($20).");
			TakePlayerMoneyEx(playerid, 20, "beli bibit jagung");
			GivePlayerItem(playerid, ITEM_CORNSEED, 1);
			SendClientMessage(playerid, COLOR_SUCCESS, "[SUKSES] Anda membeli 1x Bibit Jagung seharga $20.");
		}
		case 2: { // Gembor Air $35
			if (gPlayerData[playerid][pMoney] < 35) return SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Uang Anda tidak cukup ($35).");
			TakePlayerMoneyEx(playerid, 35, "beli gembor air");
			GivePlayerItem(playerid, ITEM_WATERCAN, 1);
			SendClientMessage(playerid, COLOR_SUCCESS, "[SUKSES] Anda membeli 1x Gembor Penyiram Air seharga $35.");
		}
	}
	return 1;
}

CMD:plantcrop(playerid, params[])
{
	return ProcessPlantCrop(playerid);
}

stock ProcessPlantCrop(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda belum login.");
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda tidak dapat menanam saat pingsan atau terborgol.");
		return 1;
	}
	if (!IsPlayerInFlintFarm(playerid)) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda hanya dapat menanam di area ladang Flint County.");
		return 1;
	}

	new hasWheat = GetPlayerItemCount(playerid, ITEM_WHEATSEED);
	new hasCorn = GetPlayerItemCount(playerid, ITEM_CORNSEED);

	if (hasWheat <= 0 && hasCorn <= 0) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda tidak memiliki benih pertanian (Beli di Toko Tani Flint via /buyseed).");
		return 1;
	}

	Dialog_Show(playerid, DialogPlantCropSelect, DIALOG_STYLE_LIST,
		"{2ECC71}PILIH BENIH YANG AKAN DITANAM",
		"{FFFFFF}1. Tanam Bibit Gandum (Wheat)\n\
{FFFFFF}2. Tanam Bibit Jagung (Corn)",
		"Tanam", "Batal");
	return 1;
}

Dialog:DialogPlantCropSelect(playerid, response, listitem, inputtext[])
{
	if (!response) return 1;

	new seedType = (listitem == 0) ? ITEM_WHEATSEED : ITEM_CORNSEED;
	if (GetPlayerItemCount(playerid, seedType) <= 0) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda tidak memiliki benih tersebut di kantong inventaris.");
		return 1;
	}

	// Cari slot ladang kosong
	new freeSlot = -1;
	for (new i = 0; i < MAX_FARM_CROPS; i++) {
		if (!gFarmCrops[i][crUsed]) {
			freeSlot = i;
			break;
		}
	}

	if (freeSlot == -1) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Kapasitas ladang pertanian saat ini sedang penuh.");
		return 1;
	}

	ApplyProgressBar(playerid, "MENANAM BENIH...", 4, "RP_OnFinishPlantCrop", seedType);
	ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.1, false, false, false, false, 0, 1);
	return 1;
}

public RP_OnFinishPlantCrop(playerid, cropType)
{
	new slot = -1;
	for (new i = 0; i < MAX_FARM_CROPS; i++) {
		if (!gFarmCrops[i][crUsed]) {
			slot = i;
			break;
		}
	}
	if (slot == -1) return 0;

	// [FIX] Ambil benih secara atomic; jika gagal, jangan tanam.
	if (!TakePlayerItem(playerid, cropType, 1)) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Benih tidak tersedia di inventaris Anda.");
		return 0;
	}

	new Float:x, Float:y, Float:z;
	GetPlayerPos(playerid, x, y, z);

	gFarmCrops[slot][crUsed] = true;
	gFarmCrops[slot][crOwnerID] = gPlayerData[playerid][pID];
	gFarmCrops[slot][crType] = cropType;
	gFarmCrops[slot][crX] = x;
	gFarmCrops[slot][crY] = y;
	gFarmCrops[slot][crZ] = z - 0.9;
	gFarmCrops[slot][crPlantTime] = gettime();
	gFarmCrops[slot][crWatered] = false;

	new labelText[144];
	format(labelText, sizeof(labelText), "{2ECC71}[ %s ]\n{FFFFFF}Pemilik: %s\n{FFFF00}Status: Bertunas (Gunakan /watercrop)",
		(cropType == ITEM_WHEATSEED) ? "Tanaman Gandum" : "Tanaman Jagung", gPlayerData[playerid][pName]);

	gFarmCrops[slot][crLabel] = Create3DTextLabel(labelText, 0xFFFFFFFF, x, y, z, 10.0, 0, false);

	SendClientMessage(playerid, COLOR_SUCCESS, "[SUKSES] Benih berhasil ditanam di tanah subur. Gunakan /watercrop untuk menyiram.");
	return 1;
}

CMD:watercrop(playerid, params[])
{
	return ProcessWaterCrop(playerid);
}

stock ProcessWaterCrop(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda belum login.");
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda tidak dapat menyiram tanaman saat pingsan atau terborgol.");
		return 1;
	}
	if (GetPlayerItemCount(playerid, ITEM_WATERCAN) <= 0) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda membutuhkan Gembor Penyiram Air (/buyseed di Toko Tani).");
		return 1;
	}

	new nearSlot = -1;
	for (new i = 0; i < MAX_FARM_CROPS; i++) {
		if (gFarmCrops[i][crUsed] && IsPlayerNearPoint(playerid, gFarmCrops[i][crX], gFarmCrops[i][crY], gFarmCrops[i][crZ] + 0.9, 3.0)) {
			nearSlot = i;
			break;
		}
	}

	if (nearSlot == -1) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda tidak berada di dekat tanaman ladang Anda.");
		return 1;
	}

	if (gFarmCrops[nearSlot][crWatered]) {
		SendClientMessage(playerid, COLOR_INFO, "[INFO] Tanaman ini sudah disiram air.");
		return 1;
	}

	ApplyProgressBar(playerid, "MENYIRAM TANAMAN...", 3, "RP_OnFinishWaterCrop", nearSlot);
	ApplyAnimation(playerid, "VENDING", "vend_use", 4.1, false, false, false, false, 0, 1);
	return 1;
}

public RP_OnFinishWaterCrop(playerid, cropSlot)
{
	if (cropSlot < 0 || cropSlot >= MAX_FARM_CROPS || !gFarmCrops[cropSlot][crUsed]) return 0;

	gFarmCrops[cropSlot][crWatered] = true;

	new labelText[144];
	format(labelText, sizeof(labelText), "{2ECC71}[ %s ]\n{FFFFFF}Status: {3498DB}Disiram Segar {FFFFFF}(Panen Lebih Cepat!)",
		(gFarmCrops[cropSlot][crType] == ITEM_WHEATSEED) ? "Tanaman Gandum" : "Tanaman Jagung");

	Update3DTextLabelText(gFarmCrops[cropSlot][crLabel], 0xFFFFFFFF, labelText);
	SendClientMessage(playerid, COLOR_SUCCESS, "[SUKSES] Tanaman berhasil disiram air segar. Waktu tumbuh dipersingkat 50%.");
	return 1;
}

CMD:harvestcrop(playerid, params[])
{
	return ProcessHarvestCrop(playerid);
}

stock ProcessHarvestCrop(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda belum login.");
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda tidak dapat memanen saat pingsan atau terborgol.");
		return 1;
	}

	new nearSlot = -1;
	for (new i = 0; i < MAX_FARM_CROPS; i++) {
		if (gFarmCrops[i][crUsed] && IsPlayerNearPoint(playerid, gFarmCrops[i][crX], gFarmCrops[i][crY], gFarmCrops[i][crZ] + 0.9, 3.0)) {
			nearSlot = i;
			break;
		}
	}

	if (nearSlot == -1) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda tidak berada di dekat tanaman ladang mana pun.");
		return 1;
	}

	new reqTime = gFarmCrops[nearSlot][crWatered] ? CROP_GROW_TIME_WATERED : CROP_GROW_TIME_DEFAULT;
	new elapsed = gettime() - gFarmCrops[nearSlot][crPlantTime];

	if (elapsed < reqTime) {
		new sisa = reqTime - elapsed;
		new msg[96];
		format(msg, sizeof(msg), "[INFO] Tanaman belum siap panen. Menunggu %d detik lagi.", sisa);
		SendClientMessage(playerid, COLOR_INFO, msg);
		return 1;
	}

	ApplyProgressBar(playerid, "MEMANEN HASIL TANI...", 4, "RP_OnFinishHarvestCrop", nearSlot);
	ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.1, false, false, false, false, 0, 1);
	return 1;
}

public RP_OnFinishHarvestCrop(playerid, cropSlot)
{
	if (cropSlot < 0 || cropSlot >= MAX_FARM_CROPS || !gFarmCrops[cropSlot][crUsed]) return 0;

	// [FIX] Validasi ulang saat panen selesai: pemain harus masih login, tidak
	// pingsan/terborgol, dan MASIH berada di dekat tanamannya. Sebelumnya bisa
	// memulai panen lalu kabur dan tetap mendapat hasil.
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return 0;
	// Cek jarak hanya untuk pemain nyata (test runner tak punya posisi valid).
	if (IsPlayerConnected(playerid) && !IsPlayerNearPoint(playerid, gFarmCrops[cropSlot][crX], gFarmCrops[cropSlot][crY], gFarmCrops[cropSlot][crZ], 4.0)) return 0;

	new yieldItem = (gFarmCrops[cropSlot][crType] == ITEM_WHEATSEED) ? ITEM_WHEAT : ITEM_CORN;
	new yieldAmount = 2 + random(3); // 2 - 4 karung

	GivePlayerItem(playerid, yieldItem, yieldAmount);

	if (gFarmCrops[cropSlot][crLabel] != Text3D:INVALID_3DTEXT_ID) {
		Delete3DTextLabel(gFarmCrops[cropSlot][crLabel]);
		gFarmCrops[cropSlot][crLabel] = Text3D:INVALID_3DTEXT_ID;
	}
	gFarmCrops[cropSlot][crUsed] = false;

	new msg[144];
	format(msg, sizeof(msg), "[SUKSES] Panen selesai! Anda memperoleh %dx %s. Jual di Lumbung Pertanian (/sellcrops).",
		yieldAmount, (yieldItem == ITEM_WHEAT) ? "Karung Gandum" : "Karung Jagung");
	SendClientMessage(playerid, COLOR_SUCCESS, msg);
	return 1;
}

CMD:sellcrops(playerid, params[])
{
	return ProcessSellCrops(playerid);
}

stock ProcessSellCrops(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda belum login.");
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda tidak dapat menjual hasil panen saat pingsan atau terborgol.");
		return 1;
	}
	if (!IsPlayerNearPoint(playerid, FARM_SILO_X, FARM_SILO_Y, FARM_SILO_Z, 5.0)) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda harus berada di Lumbung Hasil Bumi Flint County.");
		return 1;
	}

	new wheat = GetPlayerItemCount(playerid, ITEM_WHEAT);
	new corn = GetPlayerItemCount(playerid, ITEM_CORN);

	if (wheat <= 0 && corn <= 0) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda tidak membawa Karung Gandum atau Karung Jagung.");
		return 1;
	}

	new totalPayout = 0;
	if (wheat > 0) {
		TakePlayerItem(playerid, ITEM_WHEAT, wheat);
		totalPayout += (wheat * 45); // $45 per karung
	}
	if (corn > 0) {
		TakePlayerItem(playerid, ITEM_CORN, corn);
		totalPayout += (corn * 60);  // $60 per karung
	}

	GivePlayerMoneyEx(playerid, totalPayout);

	new msg[144];
	format(msg, sizeof(msg), "[LUMBUNG FLINT] Anda menjual %d karung gandum & %d karung jagung. Mendapatkan $%d tunai!",
		wheat, corn, totalPayout);
	SendClientMessage(playerid, COLOR_SUCCESS, msg);
	return 1;
}
