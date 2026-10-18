#if defined _INC_inventory_dialog
	#endinput
#endif
#define _INC_inventory_dialog

/*
==============================================================================
 inventory_dialog.inc
 NUSANTARA ROLEPLAY - Clean Dialog-Based Inventory System (JGRP Style)
 Menggantikan TextDraw inventaris dengan antarmuka dialog tablist yang
 ringan, cepat, dan 100% kompatibel di Desktop & SA-MP Android Mobile.
==============================================================================
*/

static gInvItemMap[MAX_PLAYERS][MAX_INVENTORY_SLOTS];

forward RP_OnFinishEatSnack(playerid, extra);
forward RP_OnFinishDrinkWater(playerid, extra);
forward RP_OnFinishUseBandage(playerid, extra);
forward RP_OnFinishUseMedkit(playerid, extra);

stock ShowPlayerInventoryDialog(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendErr(playerid, "Anda tidak sadarkan diri dan tidak dapat membuka inventaris.");
		return 0;
	}

	new itemCount = 0;
	new dialogContent[1024];
	format(dialogContent, sizeof(dialogContent), "Slot\tNama Barang\tJumlah\n");

	for (new s = 0; s < MAX_INVENTORY_SLOTS; s++) {
		if (gPlayerItems[playerid][s][itType] != ITEM_NONE && gPlayerItems[playerid][s][itAmount] > 0) {
			new itemName[48];
			GetItemName(gPlayerItems[playerid][s][itType], itemName, sizeof(itemName));

			new row[96];
			format(row, sizeof(row), "{FFFF00}%02d\t{FFFFFF}%s\t{2ECC71}%dx\n",
				s + 1, itemName, gPlayerItems[playerid][s][itAmount]);
			strcat(dialogContent, row);

			gInvItemMap[playerid][itemCount] = s;
			itemCount++;
		}
	}

	if (itemCount == 0) {
		Dialog_Show(playerid, DialogInvEmpty, DIALOG_STYLE_MSGBOX,
			"{FFFFFF}INVENTARIS SAYA",
			"{FFFFFF}Kantong pakaian Anda kosong.\nAnda tidak membawa barang apa pun saat ini.",
			"Tutup", "");
		return 1;
	}

	Dialog_Show(playerid, DialogInventoryList, DIALOG_STYLE_TABLIST_HEADERS,
		"{FFFFFF}INVENTARIS BARANG SAYA",
		dialogContent,
		"Pilih", "Tutup");
	return 1;
}

Dialog:DialogInvEmpty(playerid, response, listitem, inputtext[])
{
	return 1;
}

Dialog:DialogInventoryList(playerid, response, listitem, inputtext[])
{
	if (!response) return 1;
	if (listitem < 0 || listitem >= MAX_INVENTORY_SLOTS) return 1;

	new slot = gInvItemMap[playerid][listitem];
	if (slot < 0 || slot >= MAX_INVENTORY_SLOTS) return 1;
	if (gPlayerItems[playerid][slot][itType] == ITEM_NONE) return 1;

	gSelectedInvSlot[playerid] = slot;

	new itemName[48];
	GetItemName(gPlayerItems[playerid][slot][itType], itemName, sizeof(itemName));

	new caption[96], body[256];
	format(caption, sizeof(caption), "{FFFFFF}AKSI BARANG: {FFFF00}%s", itemName);
	format(body, sizeof(body),
		"{FFFFFF}Jumlah di kantong: {2ECC71}%dx\n\n\
{FFFFFF}1. Gunakan Barang (Use)\n\
{FFFFFF}2. Berikan ke Warga Lain (Give)\n\
{FFFFFF}3. Buang Barang ke Tanah (Drop)",
		gPlayerItems[playerid][slot][itAmount]);

	Dialog_Show(playerid, DialogInventoryAction, DIALOG_STYLE_LIST, caption, body, "Pilih", "Kembali");
	return 1;
}

Dialog:DialogInventoryAction(playerid, response, listitem, inputtext[])
{
	if (!response) {
		ShowPlayerInventoryDialog(playerid);
		return 1;
	}

	new slot = gSelectedInvSlot[playerid];
	if (slot < 0 || slot >= MAX_INVENTORY_SLOTS) return 1;
	new itemType = gPlayerItems[playerid][slot][itType];
	if (itemType == ITEM_NONE) return 1;

	switch (listitem) {
		case 0: { // 1. Gunakan Barang
			ProcessUseInventoryItem(playerid, slot);
		}
		case 1: { // 2. Berikan ke Warga Lain
			Dialog_Show(playerid, DialogInvGiveTarget, DIALOG_STYLE_INPUT,
				"{FFFFFF}BERIKAN BARANG",
				"{FFFFFF}Masukkan ID atau nama warga terdekat yang ingin Anda beri barang:",
				"Lanjut", "Batal");
		}
		case 2: { // 3. Buang Barang
			new itemName[48];
			GetItemName(itemType, itemName, sizeof(itemName));
			new prompt[192];
			format(prompt, sizeof(prompt),
				"{FFFFFF}Anda memiliki {2ECC71}%dx {FFFF00}%s{FFFFFF}.\nBerapa jumlah yang ingin dibuang ke tanah?",
				gPlayerItems[playerid][slot][itAmount], itemName);
			Dialog_Show(playerid, DialogInvDropAmount, DIALOG_STYLE_INPUT,
				"{FFFFFF}BUANG BARANG", prompt, "Buang", "Batal");
		}
	}
	return 1;
}

stock ProcessUseInventoryItem(playerid, slot)
{
	if (slot < 0 || slot >= MAX_INVENTORY_SLOTS) return 0;
	new itemType = gPlayerItems[playerid][slot][itType];
	if (itemType == ITEM_NONE) return 0;

	switch (itemType) {
		case ITEM_SNACK: {
			ApplyProgressBar(playerid, "MEMAKAN SNACK...", 2, "RP_OnFinishEatSnack", slot);
			ApplyAnimation(playerid, "FOOD", "EAT_Burger", 4.1, false, false, false, false, 0, 1);
		}
		case ITEM_WATER: {
			ApplyProgressBar(playerid, "MEMINUM AIR...", 2, "RP_OnFinishDrinkWater", slot);
			ApplyAnimation(playerid, "VENDING", "VEND_Drink_P", 4.1, false, false, false, false, 0, 1);
		}
		case ITEM_BANDAGE: {
			ApplyProgressBar(playerid, "MEMASANG PERBAN...", 3, "RP_OnFinishUseBandage", slot);
			ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.1, false, false, false, false, 0, 1);
		}
		case ITEM_MEDKIT: {
			ApplyProgressBar(playerid, "MENGGUNAKAN P3K MEDKIT...", 4, "RP_OnFinishUseMedkit", slot);
			ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.1, false, false, false, false, 0, 1);
		}
		case ITEM_BURGER: {
			if (!TakePlayerItem(playerid, ITEM_BURGER, 1)) return 0;
			gPlayerData[playerid][pHunger] += 50.0;
			if (gPlayerData[playerid][pHunger] > 100.0) gPlayerData[playerid][pHunger] = 100.0;
			new Float:hp;
			GetPlayerHealth(playerid, hp);
			if (hp < 100.0) { hp += 15.0; if (hp > 100.0) hp = 100.0; SetPlayerHealth(playerid, hp); }
			SendClientMessage(playerid, COLOR_SUCCESS, "[SUKSES] Anda telah menyantap Burger Shot (+50 Lapar, +15 HP).");
		}
		case ITEM_PIZZA: {
			if (!TakePlayerItem(playerid, ITEM_PIZZA, 1)) return 0;
			gPlayerData[playerid][pHunger] += 55.0;
			if (gPlayerData[playerid][pHunger] > 100.0) gPlayerData[playerid][pHunger] = 100.0;
			new Float:hp;
			GetPlayerHealth(playerid, hp);
			if (hp < 100.0) { hp += 15.0; if (hp > 100.0) hp = 100.0; SetPlayerHealth(playerid, hp); }
			SendClientMessage(playerid, COLOR_SUCCESS, "[SUKSES] Anda telah menyantap Pizza Stack (+55 Lapar, +15 HP).");
		}
		case ITEM_BEER: {
			if (!TakePlayerItem(playerid, ITEM_BEER, 1)) return 0;
			gPlayerData[playerid][pThirst] += 20.0;
			if (gPlayerData[playerid][pThirst] > 100.0) gPlayerData[playerid][pThirst] = 100.0;
			gPlayerData[playerid][pDrunkLevel] += 1500;
			SetPlayerDrunkLevel(playerid, gPlayerData[playerid][pDrunkLevel]);
			SendClientMessage(playerid, COLOR_SUCCESS, "[SUKSES] Anda meminum sekaleng Bir dingin.");
		}
		case ITEM_VODKA: {
			if (!TakePlayerItem(playerid, ITEM_VODKA, 1)) return 0;
			gPlayerData[playerid][pThirst] += 30.0;
			if (gPlayerData[playerid][pThirst] > 100.0) gPlayerData[playerid][pThirst] = 100.0;
			gPlayerData[playerid][pDrunkLevel] += 3000;
			SetPlayerDrunkLevel(playerid, gPlayerData[playerid][pDrunkLevel]);
			SendClientMessage(playerid, COLOR_SUCCESS, "[SUKSES] Anda meminum sebotol Vodka.");
		}
		case ITEM_COOKEDFISH: {
			ProcessEatCookedFish(playerid);
		}
		case ITEM_WEED: {
			if (!TakePlayerItem(playerid, ITEM_WEED, 1)) return 0;
			new Float:arm;
			GetPlayerArmour(playerid, arm);
			arm += 30.0;
			if (arm > 100.0) arm = 100.0;
			SetPlayerArmour(playerid, arm);
			SendClientMessage(playerid, COLOR_SUCCESS, "[SUKSES] Anda mengonsumsi daun ganja kering (+30 Armor).");
		}
		case ITEM_PHONE: {
			ShowPhoneMenu(playerid);
		}
		case ITEM_GPS: {
			ShowGPSMenu(playerid);
		}
		case ITEM_RADIO: {
			Dialog_Show(playerid, DialogRadioFreqSet, DIALOG_STYLE_INPUT,
				"{FFFFFF}PENGATURAN FREKUENSI RADIO",
				"{FFFFFF}Masukkan frekuensi radio walkie talkie (100 - 9999):\n\
{AFAFAF}Ketik 0 untuk mematikan radio.",
				"Setel", "Batal");
		}
		case ITEM_REPAIRKIT: {
			ProcessRepairTire(playerid);
		}
		case ITEM_JERRYCAN: {
			ProcessPourFuelCanister(playerid);
		}
		case ITEM_LOCKPICK: {
			ProcessLockpickVehicle(playerid);
		}
		case ITEM_DICE: {
			SendClientMessage(playerid, COLOR_INFO, "[USAGE] Gunakan /dice [playerid] [taruhan] untuk mengajak warga bermain dadu.");
		}
		case ITEM_SPRAYCAN: {
			SendClientMessage(playerid, COLOR_INFO, "[USAGE] Gunakan /tag [teks] di depan dinding grafiti kota.");
		}
		case ITEM_BOOMBOX: {
			ProcessBoombox(playerid);
		}
		case ITEM_WHEATSEED, ITEM_CORNSEED: {
			ProcessPlantCrop(playerid);
		}
		case ITEM_WATERCAN: {
			ProcessWaterCrop(playerid);
		}
		case ITEM_DRILL: {
			ProcessDrillATM(playerid);
		}
		default: {
			SendClientMessage(playerid, COLOR_INFO, "[INFO] Barang ini digunakan secara otomatis saat berinteraksi dengan lingkungan.");
		}
	}
	return 1;
}

public RP_OnFinishEatSnack(playerid, extra)
{
	#pragma unused extra
	if (!TakePlayerItem(playerid, ITEM_SNACK, 1)) return 0; // [FIX] atomic; cegah konsumsi ganda
	gPlayerData[playerid][pHunger] += 30.0;
	if (gPlayerData[playerid][pHunger] > 100.0) gPlayerData[playerid][pHunger] = 100.0;
	SendClientMessage(playerid, COLOR_SUCCESS, "[SUKSES] Anda selesai memakan snack (+30 Lapar).");
	return 1;
}

public RP_OnFinishDrinkWater(playerid, extra)
{
	#pragma unused extra
	if (!TakePlayerItem(playerid, ITEM_WATER, 1)) return 0; // [FIX] atomic
	gPlayerData[playerid][pThirst] += 35.0;
	if (gPlayerData[playerid][pThirst] > 100.0) gPlayerData[playerid][pThirst] = 100.0;
	SendClientMessage(playerid, COLOR_SUCCESS, "[SUKSES] Anda selesai meminum air mineral (+35 Haus).");
	return 1;
}

public RP_OnFinishUseBandage(playerid, extra)
{
	#pragma unused extra
	if (!TakePlayerItem(playerid, ITEM_BANDAGE, 1)) return 0; // [FIX] atomic
	new Float:hp;
	GetPlayerHealth(playerid, hp);
	hp += 25.0;
	if (hp > 100.0) hp = 100.0;
	SetPlayerHealth(playerid, hp);
	SendClientMessage(playerid, COLOR_SUCCESS, "[SUKSES] Perban berhasil dipasang (+25 HP).");
	return 1;
}

public RP_OnFinishUseMedkit(playerid, extra)
{
	#pragma unused extra
	if (!TakePlayerItem(playerid, ITEM_MEDKIT, 1)) return 0; // [FIX] atomic
	new Float:hp;
	GetPlayerHealth(playerid, hp);
	hp += 75.0;
	if (hp > 100.0) hp = 100.0;
	SetPlayerHealth(playerid, hp);
	SendClientMessage(playerid, COLOR_SUCCESS, "[SUKSES] Kotak P3K Medkit berhasil digunakan (+75 HP).");
	return 1;
}

// Berikan Barang (Give)
Dialog:DialogInvGiveTarget(playerid, response, listitem, inputtext[])
{
	if (!response) {
		ShowPlayerInventoryDialog(playerid);
		return 1;
	}

	new targetid = strval(inputtext);
	if (!IsPlayerConnected(targetid) || targetid == playerid) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] ID pemain tidak valid atau pemain tidak terhubung.");
		return 1;
	}

	if (!IsPlayerNear(playerid, targetid, 4.0)) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Warga tersebut berada terlalu jauh dari Anda.");
		return 1;
	}

	gGiveTarget[playerid] = targetid;

	new slot = gSelectedInvSlot[playerid];
	new itemType = gPlayerItems[playerid][slot][itType];
	new itemName[48];
	GetItemName(itemType, itemName, sizeof(itemName));

	new prompt[192];
	format(prompt, sizeof(prompt),
		"{FFFFFF}Warga tujuan: {FFFF00}%s (ID %d)\n\
{FFFFFF}Barang: {FFFF00}%s (Milik Anda: %dx)\n\n\
{FFFFFF}Masukkan jumlah barang yang ingin diberikan:",
		gPlayerData[targetid][pName], targetid, itemName, gPlayerItems[playerid][slot][itAmount]);

	Dialog_Show(playerid, DialogInvGiveAmount, DIALOG_STYLE_INPUT, "{FFFFFF}JUMLAH BARANG", prompt, "Beri", "Batal");
	return 1;
}

Dialog:DialogInvGiveAmount(playerid, response, listitem, inputtext[])
{
	if (!response) {
		ShowPlayerInventoryDialog(playerid);
		return 1;
	}
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return 1;

	new targetid = gGiveTarget[playerid];
	if (!IsPlayerConnected(targetid) || !IsPlayerNear(playerid, targetid, 4.0)) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Warga tujuan tidak ditemukan di sekitar Anda.");
		return 1;
	}

	new amount = strval(inputtext);
	new slot = gSelectedInvSlot[playerid];
	if (slot < 0 || slot >= MAX_INVENTORY_SLOTS) return 1;

	new itemType = gPlayerItems[playerid][slot][itType];
	if (itemType == ITEM_NONE) return 1;

	if (amount <= 0 || amount > gPlayerItems[playerid][slot][itAmount]) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Jumlah barang tidak valid.");
		return 1;
	}

	// [FIX] Pastikan pengambilan sukses sebelum memberi ke target (atomic),
	// dan jika inventaris target penuh, kembalikan barang ke pengirim.
	if (!TakePlayerItem(playerid, itemType, amount)) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Gagal memberikan barang: stok tidak mencukupi.");
		return 1;
	}
	if (!GivePlayerItem(targetid, itemType, amount)) {
		GivePlayerItem(playerid, itemType, amount);
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Gagal memberikan barang: kantung inventaris warga tujuan penuh.");
		return 1;
	}

	new itemName[48];
	GetItemName(itemType, itemName, sizeof(itemName));

	new msg[144];
	format(msg, sizeof(msg), "[INFO] Anda telah memberikan %dx %s kepada %s.", amount, itemName, gPlayerData[targetid][pName]);
	SendClientMessage(playerid, COLOR_SUCCESS, msg);

	format(msg, sizeof(msg), "[INFO] Anda menerima %dx %s dari %s.", amount, itemName, gPlayerData[playerid][pName]);
	SendClientMessage(targetid, COLOR_SUCCESS, msg);
	return 1;
}

// Buang Barang (Drop)
Dialog:DialogInvDropAmount(playerid, response, listitem, inputtext[])
{
	if (!response) {
		ShowPlayerInventoryDialog(playerid);
		return 1;
	}
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return 1;

	new amount = strval(inputtext);
	new slot = gSelectedInvSlot[playerid];
	if (slot < 0 || slot >= MAX_INVENTORY_SLOTS) return 1;

	new itemType = gPlayerItems[playerid][slot][itType];
	if (itemType == ITEM_NONE) return 1;

	if (amount <= 0 || amount > gPlayerItems[playerid][slot][itAmount]) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Jumlah barang tidak valid.");
		return 1;
	}

	if (!TakePlayerItem(playerid, itemType, amount)) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Gagal membuang barang: stok tidak mencukupi.");
		return 1;
	}

	new itemName[48];
	GetItemName(itemType, itemName, sizeof(itemName));

	new msg[144];
	format(msg, sizeof(msg), "[INFO] Anda telah membuang %dx %s ke tanah.", amount, itemName);
	SendClientMessage(playerid, COLOR_INFO, msg);
	return 1;
}

Dialog:DialogRadioFreqSet(playerid, response, listitem, inputtext[])
{
	if (!response) return 1;
	new freq = strval(inputtext);
	if (freq == 0) {
		gPlayerData[playerid][pRadioStatus] = false;
		SendClientMessage(playerid, COLOR_INFO, "[RADIO] Radio handy talkie telah dimatikan.");
		return 1;
	}
	if (freq < 100 || freq > 9999) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Frekuensi radio harus antara 100 hingga 9999.");
		return 1;
	}
	gPlayerData[playerid][pRadioFreq] = freq;
	gPlayerData[playerid][pRadioStatus] = true;
	new msg[96];
	format(msg, sizeof(msg), "[RADIO] Frekuensi radio diatur ke %d MHz (Status: AKTIF).", freq);
	SendClientMessage(playerid, COLOR_SUCCESS, msg);
	return 1;
}
