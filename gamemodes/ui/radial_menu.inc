#if defined _INC_radial_menu
	#endinput
#endif
#define _INC_radial_menu

/*
==============================================================================
 radial_menu.inc
 NUSANTARA ROLEPLAY - Oldschool N-Key Radial Action Menu (JGRP Style)
 Menggunakan easyDialog untuk mempermudah navigasi mobile & desktop tanpa
 mengharuskan pemain menghafal perintah teks (slash command).
==============================================================================
*/

#define MAX_RADIAL_OPTIONS 35

static gRadialAction[MAX_PLAYERS][MAX_RADIAL_OPTIONS];
static gRadialTarget[MAX_PLAYERS];

enum {
	RADIAL_ACT_NONE = 0,
	// Kendaraan (Driver)
	RADIAL_ACT_ENGINE,
	RADIAL_ACT_LOCK,
	RADIAL_ACT_LIGHTS,
	RADIAL_ACT_HOOD,
	RADIAL_ACT_TRUNK,
	RADIAL_ACT_TRUNK_STORAGE,
	RADIAL_ACT_REFUEL,
	RADIAL_ACT_TAXI_METER,
	RADIAL_ACT_TOW,
	RADIAL_ACT_UNTOW,
	RADIAL_ACT_IMPOUND,
	RADIAL_ACT_PLATE,
	RADIAL_ACT_MYVEH,
	// Jalan Kaki (Umum)
	RADIAL_ACT_INV,
	RADIAL_ACT_PHONE,
	RADIAL_ACT_STATS,
	RADIAL_ACT_WALLET,
	RADIAL_ACT_INTERACT_PLAYER,
	RADIAL_ACT_JOBS,
	RADIAL_ACT_CALL_911,
	RADIAL_ACT_HELP,
	// Fasilitas & Konteks Lingkungan
	RADIAL_ACT_NEAR_TRUNK,
	RADIAL_ACT_LOCKPICK,
	RADIAL_ACT_CHOPSHOP,
	RADIAL_ACT_REPAIR_TIRE,
	RADIAL_ACT_POUR_FUEL,
	RADIAL_ACT_STORE_BUY,
	RADIAL_ACT_STORE_ROB,
	RADIAL_ACT_BANK,
	RADIAL_ACT_ATM,
	RADIAL_ACT_PAY_TICKET,
	RADIAL_ACT_TRASH_SCAVENGE,
	RADIAL_ACT_WEED_HARVEST,
	RADIAL_ACT_WEED_BURN,
	RADIAL_ACT_DETECT,
	RADIAL_ACT_DIG,
	RADIAL_ACT_CAMPFIRE_COOK,
	RADIAL_ACT_GRAFFITI_TAG,
	RADIAL_ACT_GRAFFITI_CLEAN,
	RADIAL_ACT_VENDING,
	RADIAL_ACT_BLACKMARKET,
	RADIAL_ACT_SMUGGLE,
	RADIAL_ACT_BAR,
	RADIAL_ACT_GYM,
	RADIAL_ACT_FASTFOOD,
	RADIAL_ACT_AMMUNATION,
	RADIAL_ACT_DMV,
	RADIAL_ACT_CLAIM_VEH,
	RADIAL_ACT_RENTAL,
	RADIAL_ACT_DOOR_ENTER,
	RADIAL_ACT_DOOR_EXIT,
	RADIAL_ACT_DRILL_ATM,
	RADIAL_ACT_FARM_PLANT,
	RADIAL_ACT_FARM_WATER,
	RADIAL_ACT_FARM_HARVEST,
	RADIAL_ACT_FARM_SELL,
	RADIAL_ACT_TRASH_JOB
};

forward RP_OnFinishRefuelAction(playerid, veh);
forward RP_OnFinishRepairTireAction(playerid, veh);
forward RP_OnFinishPourFuelAction(playerid, veh);

stock ShowPlayerRadialMenu(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !gPlayerData[playerid][pLoggedIn]) return 0;

	new count = 0;
	gRadialTarget[playerid] = -1;
	new menuList[1800];
	menuList[0] = '\0';

	// =========================================================================
	// 1. KENDARAAN (SEDANG MENYETIR)
	// =========================================================================
	if (IsPlayerInAnyVehicle(playerid) && GetPlayerState(playerid) == PLAYER_STATE_DRIVER) {
		new veh = GetPlayerVehicleID(playerid);
		new model = GetVehicleModel(veh);

		strcat(menuList, "{2ECC71}=== KONTROL KENDARAAN ===\n");

		gRadialAction[playerid][count++] = RADIAL_ACT_ENGINE;
		strcat(menuList, "{FFFF00}1. Mesin Kendaraan (Toggle Engine)\n");

		gRadialAction[playerid][count++] = RADIAL_ACT_LOCK;
		strcat(menuList, "{FFFF00}2. Kunci Pintu (Toggle Lock)\n");

		gRadialAction[playerid][count++] = RADIAL_ACT_LIGHTS;
		strcat(menuList, "{FFFF00}3. Lampu Kendaraan (Toggle Lights)\n");

		gRadialAction[playerid][count++] = RADIAL_ACT_HOOD;
		strcat(menuList, "{FFFF00}4. Kap Mesin Depan (Toggle Hood)\n");

		gRadialAction[playerid][count++] = RADIAL_ACT_TRUNK;
		strcat(menuList, "{FFFF00}5. Buka / Tutup Bagasi Belakang\n");

		gRadialAction[playerid][count++] = RADIAL_ACT_TRUNK_STORAGE;
		strcat(menuList, "{FFFF00}6. Akses Bagasi (Uang, Senjata, Barang)\n");

		if (model == 420 || model == 438) {
			gRadialAction[playerid][count++] = RADIAL_ACT_TAXI_METER;
			strcat(menuList, "{3498DB}7. Argometer Tarif Taksi\n");
		}

		if (model == 525) {
			if (gPlayerTowedVeh[playerid] == INVALID_VEHICLE_ID) {
				gRadialAction[playerid][count++] = RADIAL_ACT_TOW;
				strcat(menuList, "{3498DB}8. Pasang Kabel Derek Towtruck\n");
			} else {
				gRadialAction[playerid][count++] = RADIAL_ACT_UNTOW;
				strcat(menuList, "{E74C3C}8. Lepaskan Mobil Derek\n");
			}

			if (IsPlayerNearPoint(playerid, IMPOUND_YARD_X, IMPOUND_YARD_Y, IMPOUND_YARD_Z, 15.0)) {
				gRadialAction[playerid][count++] = RADIAL_ACT_IMPOUND;
				strcat(menuList, "{E74C3C}9. Sita Mobil Derek ke Depo LSPD\n");
			}
		}

		if (IsPlayerNearPoint(playerid, FUEL_STATION_X, FUEL_STATION_Y, FUEL_STATION_Z, RANGE_FUEL)) {
			gRadialAction[playerid][count++] = RADIAL_ACT_REFUEL;
			strcat(menuList, "{2ECC71}10. Isi Bahan Bakar Kendaraan\n");
		}

		gRadialAction[playerid][count++] = RADIAL_ACT_MYVEH;
		strcat(menuList, "{FFFFFF}11. Panel Garasi & Kendaraan Saya (/v)\n");

		gRadialAction[playerid][count++] = RADIAL_ACT_PLATE;
		strcat(menuList, "{FFFFFF}12. Daftarkan Plat Kustom DMV ($250)\n");

		Dialog_Show(playerid, DialogRadialMenu, DIALOG_STYLE_LIST,
			"{FFFFFF}MENU KENDARAAN (N-KEY)", menuList, "Pilih", "Tutup");
		return 1;
	}

	// =========================================================================
	// 2. JALAN KAKI (ON FOOT)
	// =========================================================================
	new bool:hasContext = false;

	// 2.1 Dekat Mobil Lain (<= 4.0m)
	new nearVehSlot = -1;
	for (new i = 0; i < MAX_OWNED_VEHICLES; i++) {
		if (gVehicleData[i][vUsed] && gVehicleData[i][vVehicleHandle] > 0) {
			if (IsPlayerNearPoint(playerid, gVehicleData[i][vPosX], gVehicleData[i][vPosY], gVehicleData[i][vPosZ], 4.0)) {
				nearVehSlot = i;
				break;
			}
		}
	}

	if (nearVehSlot != -1) {
		strcat(menuList, "{2ECC71}=== KENDARAAN TERDEKAT ===\n");
		hasContext = true;
		gRadialTarget[playerid] = nearVehSlot;

		gRadialAction[playerid][count++] = RADIAL_ACT_NEAR_TRUNK;
		strcat(menuList, "{FFFF00}⚡ Akses Bagasi Kendaraan (Uang / Senjata / Barang)\n");

		gRadialAction[playerid][count++] = RADIAL_ACT_LOCKPICK;
		strcat(menuList, "{E74C3C}⚡ Bobol Kunci Mobil (Lockpick)\n");

		if (IsPlayerNearPoint(playerid, CHOPSHOP_X, CHOPSHOP_Y, CHOPSHOP_Z, 6.0)) {
			gRadialAction[playerid][count++] = RADIAL_ACT_CHOPSHOP;
			strcat(menuList, "{E74C3C}⚡ Mutilasi Suku Cadang Chop Shop\n");
		}

		if (GetPlayerItemCount(playerid, ITEM_REPAIRKIT) > 0) {
			gRadialAction[playerid][count++] = RADIAL_ACT_REPAIR_TIRE;
			strcat(menuList, "{3498DB}⚡ Tambal Ban Kempes (Repairkit)\n");
		}

		if (GetPlayerItemCount(playerid, ITEM_JERRYCAN) > 0) {
			gRadialAction[playerid][count++] = RADIAL_ACT_POUR_FUEL;
			strcat(menuList, "{3498DB}⚡ Tuang Bensin 20L (Jerigen)\n");
		}
	}

	// 2.2 Dekat Pemain Lain (<= 3.5m)
	new nearPlayer = -1;
	for (new p = 0; p < MAX_PLAYERS; p++) {
		if (p != playerid && IsPlayerConnected(p) && gPlayerData[p][pLoggedIn] && IsPlayerNear(playerid, p, 3.5)) {
			nearPlayer = p;
			break;
		}
	}

	if (nearPlayer != -1) {
		if (!hasContext) { strcat(menuList, "{2ECC71}=== WARGA TERDEKAT ===\n"); hasContext = true; }
		gRadialTarget[playerid] = nearPlayer;
		gRadialAction[playerid][count++] = RADIAL_ACT_INTERACT_PLAYER;
		new pRow[96];
		format(pRow, sizeof(pRow), "{FFFF00}⚡ Interaksi dengan %s (ID %d)\n", gPlayerData[nearPlayer][pName], nearPlayer);
		strcat(menuList, pRow);
	}

	// 2.3 Fasilitas Sekitar
	if (IsPlayerNearPoint(playerid, STORE_IDLEWOOD_X, STORE_IDLEWOOD_Y, STORE_IDLEWOOD_Z, 4.5) ||
	    IsPlayerNearPoint(playerid, 1352.0, -1758.0, 13.5, 4.5)) {
		if (!hasContext) { strcat(menuList, "{2ECC71}=== FASILITAS SEKITAR ===\n"); hasContext = true; }
		gRadialAction[playerid][count++] = RADIAL_ACT_STORE_BUY;
		strcat(menuList, "{2ECC71}⚡ Kasir Toko 24/7 (Belanja Kebutuhan)\n");
		gRadialAction[playerid][count++] = RADIAL_ACT_STORE_ROB;
		strcat(menuList, "{E74C3C}⚡ Rampok Kasir Toko (Bersenjata)\n");
	}

	if (IsPlayerNearPoint(playerid, BANK_X, BANK_Y, BANK_Z, 4.0)) {
		if (!hasContext) { strcat(menuList, "{2ECC71}=== FASILITAS SEKITAR ===\n"); hasContext = true; }
		gRadialAction[playerid][count++] = RADIAL_ACT_BANK;
		strcat(menuList, "{2ECC71}⚡ Teller Bank Nusantara\n");
		gRadialAction[playerid][count++] = RADIAL_ACT_PAY_TICKET;
		strcat(menuList, "{FFFF00}⚡ Lunasi Surat Tilang ETLE\n");
	} else if (IsPlayerNearAnyATM(playerid, 3.0)) {
		if (!hasContext) { strcat(menuList, "{2ECC71}=== FASILITAS SEKITAR ===\n"); hasContext = true; }
		gRadialAction[playerid][count++] = RADIAL_ACT_ATM;
		strcat(menuList, "{2ECC71}⚡ Mesin ATM Tunai\n");
		gRadialAction[playerid][count++] = RADIAL_ACT_PAY_TICKET;
		strcat(menuList, "{FFFF00}⚡ Lunasi Surat Tilang ETLE\n");
		gRadialAction[playerid][count++] = RADIAL_ACT_DRILL_ATM;
		strcat(menuList, "{E74C3C}⚡ Bobol Brankas ATM (Bor/Lockpick)\n");
	}

	if (IsPlayerInFlintFarm(playerid)) {
		if (!hasContext) { strcat(menuList, "{2ECC71}=== LADANG PERTANIAN FLINT ===\n"); hasContext = true; }
		gRadialAction[playerid][count++] = RADIAL_ACT_FARM_PLANT;
		strcat(menuList, "{2ECC71}⚡ Tanam Bibit Gandum/Jagung (/plantcrop)\n");
		gRadialAction[playerid][count++] = RADIAL_ACT_FARM_WATER;
		strcat(menuList, "{3498DB}⚡ Siram Tanaman Air Segar (/watercrop)\n");
		gRadialAction[playerid][count++] = RADIAL_ACT_FARM_HARVEST;
		strcat(menuList, "{FFFF00}⚡ Panen Hasil Tani Siap Panen (/harvestcrop)\n");
	}

	if (IsPlayerNearPoint(playerid, FARM_SILO_X, FARM_SILO_Y, FARM_SILO_Z, 6.0)) {
		if (!hasContext) { strcat(menuList, "{2ECC71}=== LUMBUNG FLINT ===\n"); hasContext = true; }
		gRadialAction[playerid][count++] = RADIAL_ACT_FARM_SELL;
		strcat(menuList, "{2ECC71}⚡ Jual Hasil Panen Gandum & Jagung (/sellcrops)\n");
	}

	if (IsPlayerNearPoint(playerid, TRASH_DEPOT_X, TRASH_DEPOT_Y, TRASH_DEPOT_Z, 8.0)) {
		if (!hasContext) { strcat(menuList, "{2ECC71}=== DINAS KEBERSIHAN ===\n"); hasContext = true; }
		gRadialAction[playerid][count++] = RADIAL_ACT_TRASH_JOB;
		strcat(menuList, "{3498DB}⚡ Mulai Rute Truk Sampah Trashmaster (/starttrash)\n");
	}

	if (IsPlayerNearAnyTrash(playerid, 3.0)) {
		if (!hasContext) { strcat(menuList, "{2ECC71}=== FASILITAS SEKITAR ===\n"); hasContext = true; }
		gRadialAction[playerid][count++] = RADIAL_ACT_TRASH_SCAVENGE;
		strcat(menuList, "{FFFF00}⚡ Mengais Tong Sampah (Rongsokan)\n");
	}

	if (IsPlayerNearAnyEntranceDoor(playerid)) {
		if (!hasContext) { strcat(menuList, "{2ECC71}=== FASILITAS SEKITAR ===\n"); hasContext = true; }
		gRadialAction[playerid][count++] = RADIAL_ACT_DOOR_ENTER;
		strcat(menuList, "{FFFFFF}⚡ Masuk ke Dalam Gedung\n");
	} else if (IsPlayerNearAnyExitDoor(playerid)) {
		if (!hasContext) { strcat(menuList, "{2ECC71}=== FASILITAS SEKITAR ===\n"); hasContext = true; }
		gRadialAction[playerid][count++] = RADIAL_ACT_DOOR_EXIT;
		strcat(menuList, "{FFFFFF}⚡ Keluar dari Gedung\n");
	}

	// =========================================================================
	// MENU UTAMA KARAKTER (SELALU TERSEDIA)
	// =========================================================================
	strcat(menuList, "{3498DB}=== FITUR UTAMA KARAKTER ===\n");

	gRadialAction[playerid][count++] = RADIAL_ACT_INV;
	strcat(menuList, "{FFFFFF}[1] 🎒 Inventaris Barang (/items /inv)\n");

	gRadialAction[playerid][count++] = RADIAL_ACT_PHONE;
	strcat(menuList, "{FFFFFF}[2] 📱 Handphone Pintar (/phone)\n");

	gRadialAction[playerid][count++] = RADIAL_ACT_MYVEH;
	strcat(menuList, "{FFFFFF}[3] 🚗 Kendaraan Pribadi & Garasi (/v)\n");

	gRadialAction[playerid][count++] = RADIAL_ACT_STATS;
	strcat(menuList, "{FFFFFF}[4] 📊 Statistik Karakter (/stats)\n");

	gRadialAction[playerid][count++] = RADIAL_ACT_WALLET;
	strcat(menuList, "{FFFFFF}[5] 💳 Dompet & Lisensi (KTP, SIM, Senjata)\n");

	gRadialAction[playerid][count++] = RADIAL_ACT_JOBS;
	strcat(menuList, "{FFFFFF}[6] 💼 Lowongan Pekerjaan (/job)\n");

	gRadialAction[playerid][count++] = RADIAL_ACT_CALL_911;
	strcat(menuList, "{FFFFFF}[7] 🚨 Panggilan Darurat 911 (SAPD / SAMD)\n");

	gRadialAction[playerid][count++] = RADIAL_ACT_HELP;
	strcat(menuList, "{AFAFAF}[?] 📖 Buku Panduan & Bantuan (/help)\n");

	#pragma unused count

	Dialog_Show(playerid, DialogRadialMenu, DIALOG_STYLE_LIST,
		"{FFFFFF}RADIAL ACTION MENU (N-KEY)", menuList, "Pilih", "Tutup");
	return 1;
}

Dialog:DialogRadialMenu(playerid, response, listitem, inputtext[])
{
	if (!response) return 1;
	if (listitem < 0 || listitem >= MAX_RADIAL_OPTIONS) return 1;

	new act = gRadialAction[playerid][listitem];
	new target = gRadialTarget[playerid];

	switch (act) {
		case RADIAL_ACT_ENGINE: {
			new veh = GetPlayerVehicleID(playerid);
			if (veh > 0) Vehicle_SetEngineOn(veh, !Vehicle_IsEngineOn(veh));
		}
		case RADIAL_ACT_LOCK: {
			new veh = GetPlayerVehicleID(playerid);
			if (veh > 0) Vehicle_SetLocked(veh, !Vehicle_IsLocked(veh));
		}
		case RADIAL_ACT_LIGHTS: {
			new veh = GetPlayerVehicleID(playerid);
			new slot = Vehicle_GetSlot(veh);
			if (slot != -1) Vehicle_SetLights(veh, !gVehicleData[slot][vLights]);
		}
		case RADIAL_ACT_HOOD: {
			new veh = GetPlayerVehicleID(playerid);
			new slot = Vehicle_GetSlot(veh);
			if (slot != -1) Vehicle_SetHood(veh, !gVehicleData[slot][vHood]);
		}
		case RADIAL_ACT_TRUNK: {
			new veh = GetPlayerVehicleID(playerid);
			new slot = Vehicle_GetSlot(veh);
			if (slot != -1) Vehicle_SetTrunk(veh, !gVehicleData[slot][vTrunkOpen]);
		}
		case RADIAL_ACT_TRUNK_STORAGE: {
			new veh = GetPlayerVehicleID(playerid);
			new slot = Vehicle_GetSlot(veh);
			if (slot != -1) ShowVehicleTrunkDialog(playerid, slot);
		}
		case RADIAL_ACT_REFUEL: {
			new veh = GetPlayerVehicleID(playerid);
			if (veh > 0) {
				ApplyProgressBar(playerid, "MENGISI BENSIN...", 4, "RP_OnFinishRefuelAction", veh);
				ApplyAnimation(playerid, "VENDING", "vend_use", 4.1, false, false, false, false, 0, 1);
			}
		}
		case RADIAL_ACT_TAXI_METER:      ProcessToggleTaxiMeter(playerid, DEFAULT_TAXI_FARE);
		case RADIAL_ACT_TOW:             ProcessTowVehicle(playerid);
		case RADIAL_ACT_UNTOW:           ProcessUntowVehicle(playerid);
		case RADIAL_ACT_IMPOUND:         ProcessImpoundVehicle(playerid);
		case RADIAL_ACT_MYVEH:           ShowVehicleActionDialog(playerid, 0);
		case RADIAL_ACT_PLATE: {
			Dialog_Show(playerid, DialogRadialCustomPlate, DIALOG_STYLE_INPUT,
				"{FFFFFF}PLAT NOMOR KUSTOM DMV",
				"{FFFFFF}Masukkan nomor plat baru (3-8 karakter alfanumerik):", "Daftarkan", "Batal");
		}
		case RADIAL_ACT_INV:             ShowPlayerInventoryDialog(playerid);
		case RADIAL_ACT_PHONE:           ShowPhoneMenu(playerid);
		case RADIAL_ACT_STATS:           ShowStatsDialog(playerid);
		case RADIAL_ACT_WALLET: {
			new licStr[256];
			format(licStr, sizeof(licStr),
				"{FFFFFF}KTP / Identitas: {FFFF00}%s (Usia: %s)\n\
{FFFFFF}Lisensi Mengemudi: %s\n\
{FFFFFF}Lisensi Senjata Api: %s\n\
{FFFFFF}Total Surat Tilang: {FFFF00}%d lembar ($%d)",
				gPlayerData[playerid][pName], gPlayerData[playerid][pBirthDate],
				gPlayerData[playerid][pDriveLic] ? ("{2ECC71}AKTIF") : ("{E74C3C}TIDAK ADA"),
				gPlayerData[playerid][pGunLic] ? ("{2ECC71}TERDAFTAR") : ("{E74C3C}TIDAK ADA"),
				gPlayerData[playerid][pTickets], gPlayerData[playerid][pTicketAmount]);
			Dialog_Show(playerid, DialogWalletInfo, DIALOG_STYLE_MSGBOX, "{FFFFFF}DOMPET & LISENSI SAYA", licStr, "Tutup", "");
		}
		case RADIAL_ACT_INTERACT_PLAYER: {
			if (target >= 0 && target < MAX_PLAYERS) ShowPlayerNearRadialMenu(playerid, target);
		}
		case RADIAL_ACT_JOBS:            ShowJobDialog(playerid);
		case RADIAL_ACT_CALL_911: {
			Dialog_Show(playerid, DialogRadial911, DIALOG_STYLE_LIST,
				"{FFFFFF}LAYANAN DARURAT 911",
				"{FFFFFF}1. Kepolisian (SAPD)\n{FFFFFF}2. Ambulans & Medis (SAMD)", "Panggil", "Batal");
		}
		case RADIAL_ACT_HELP:            ShowHelpDialog(playerid);
		case RADIAL_ACT_NEAR_TRUNK: {
			if (target >= 0 && target < MAX_OWNED_VEHICLES) ShowVehicleTrunkDialog(playerid, target);
		}
		case RADIAL_ACT_LOCKPICK:        ProcessLockpickVehicle(playerid);
		case RADIAL_ACT_CHOPSHOP:        ProcessChopVehicle(playerid);
		case RADIAL_ACT_REPAIR_TIRE:     ProcessRepairTire(playerid);
		case RADIAL_ACT_POUR_FUEL:       ProcessPourFuelCanister(playerid);
		case RADIAL_ACT_STORE_BUY:       ShowStoreBuyMenu(playerid);
		case RADIAL_ACT_STORE_ROB:       ProcessRobStore(playerid);
		case RADIAL_ACT_BANK:            ShowBankDialog(playerid);
		case RADIAL_ACT_ATM:             ShowATMDialog(playerid);
		case RADIAL_ACT_PAY_TICKET:      ProcessPayTrafficTickets(playerid);
		case RADIAL_ACT_TRASH_SCAVENGE:  ProcessPlayerScavenge(playerid);
		case RADIAL_ACT_DOOR_ENTER:      ProcessDoorEnter(playerid);
		case RADIAL_ACT_DOOR_EXIT:       ProcessDoorExit(playerid);
		case RADIAL_ACT_DRILL_ATM:       ProcessDrillATM(playerid);
		case RADIAL_ACT_FARM_PLANT:      ProcessPlantCrop(playerid);
		case RADIAL_ACT_FARM_WATER:      ProcessWaterCrop(playerid);
		case RADIAL_ACT_FARM_HARVEST:    ProcessHarvestCrop(playerid);
		case RADIAL_ACT_FARM_SELL:       ProcessSellCrops(playerid);
		case RADIAL_ACT_TRASH_JOB:       ProcessStartTrash(playerid);
	}
	return 1;
}

public RP_OnFinishRefuelAction(playerid, veh)
{
	ProcessRefuel(playerid);
	return 1;
}

Dialog:DialogRadialCustomPlate(playerid, response, listitem, inputtext[])
{
	if (!response) return 1;
	if (strlen(inputtext) > 0) ProcessCustomPlate(playerid, inputtext);
	return 1;
}

Dialog:DialogWalletInfo(playerid, response, listitem, inputtext[])
{
	return 1;
}

Dialog:DialogRadial911(playerid, response, listitem, inputtext[])
{
	if (!response) return 1;
	gPending911Service[playerid] = (listitem == 0) ? 1 : 2;
	Dialog_Show(playerid, DialogRadial911Msg, DIALOG_STYLE_INPUT,
		"{FFFFFF}KETERANGAN LAPORAN 911",
		"{FFFFFF}Jelaskan situasi darurat Anda secara singkat:", "Kirim", "Batal");
	return 1;
}

Dialog:DialogRadial911Msg(playerid, response, listitem, inputtext[])
{
	if (!response || strlen(inputtext) == 0) return 1;
	CreateDispatchReport(playerid, gPending911Service[playerid], inputtext);
	return 1;
}

stock ShowPlayerNearRadialMenu(playerid, targetid)
{
	if (!IsPlayerConnected(targetid)) return 0;
	gRadialTarget[playerid] = targetid;

	new body[512];
	format(body, sizeof(body),
		"{FFFFFF}1. Berikan Uang Tunai (/pay)\n\
{FFFFFF}2. Berikan Barang dari Kantong (/items)\n\
{FFFFFF}3. Ajak Main Taruhan Dadu ($10 - $5.000)\n\
{FFFFFF}4. Bagikan Kunci Cadangan Rumah\n\
{FFFFFF}5. Cabut Izin Kunci Rumah");

	if (gPlayerData[playerid][pFaction] == FACTION_POLICE && gPlayerData[playerid][pDuty]) {
		format(body, sizeof(body), "%s\n\
{3498DB}6. [SAPD] Borgol / Lepas Borgol Tersangka\n\
{3498DB}7. [SAPD] Geledah Barang Bawaan (Frisk)\n\
{3498DB}8. [SAPD] Sita Barang Bukti Kejahatan\n\
{3498DB}9. [SAPD] Periksa Kelayakan SIM Pengemudi\n\
{3498DB}10. [SAPD] Uji Kadar Alkohol Breathalyzer", body);
	} else if (gPlayerData[playerid][pFaction] == FACTION_MEDIC && gPlayerData[playerid][pDuty]) {
		format(body, sizeof(body), "%s\n\
{2ECC71}6. [SAMD] Berikan Perawatan P3K (/treat)\n\
{2ECC71}7. [SAMD] Resusitasi CPR Korban Sekarat (/revive)", body);
	}

	new title[64];
	format(title, sizeof(title), "{FFFFFF}INTERAKSI: %s", gPlayerData[targetid][pName]);
	Dialog_Show(playerid, DialogNearPlayerMenu, DIALOG_STYLE_LIST, title, body, "Pilih", "Batal");
	return 1;
}

Dialog:DialogNearPlayerMenu(playerid, response, listitem, inputtext[])
{
	if (!response) return 1;
	new targetid = gRadialTarget[playerid];
	if (!IsPlayerConnected(targetid) || !IsPlayerNear(playerid, targetid, 4.0)) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Warga tersebut berada terlalu jauh.");
		return 1;
	}

	switch (listitem) {
		case 0: {
			gGiveTarget[playerid] = targetid;
			Dialog_Show(playerid, DialogPayCashAmount, DIALOG_STYLE_INPUT,
				"{FFFFFF}BERIKAN UANG TUNAI",
				"{FFFFFF}Masukkan nominal uang yang ingin diberikan:", "Beri", "Batal");
		}
		case 1: {
			gGiveTarget[playerid] = targetid;
			ShowPlayerInventoryDialog(playerid);
		}
		case 2: ProcessPlayDice(playerid, targetid, 100);
		case 3: ProcessShareHouseKey(playerid, targetid);
		case 4: ProcessRevokeHouseKey(playerid, targetid);
		case 5: {
			if (gPlayerData[playerid][pFaction] == FACTION_POLICE) {
				gPlayerData[targetid][pCuffed] = !gPlayerData[targetid][pCuffed];
				SetPlayerSpecialAction(targetid, gPlayerData[targetid][pCuffed] ? SPECIAL_ACTION_CUFFED : SPECIAL_ACTION_NONE);
				SendClientMessage(playerid, COLOR_SUCCESS, gPlayerData[targetid][pCuffed] ? "[SUKSES] Tersangka diborgol." : "[INFO] Borgol tersangka dilepas.");
			} else if (gPlayerData[playerid][pFaction] == FACTION_MEDIC) {
				gPlayerData[targetid][pInjured] = false;
				SetPlayerHealth(targetid, 40.0);
				SendClientMessage(playerid, COLOR_SUCCESS, "[SUKSES] Pertolongan P3K berhasil diberikan.");
			}
		}
		case 6: {
			if (gPlayerData[playerid][pFaction] == FACTION_POLICE) {
				new friskInfo[256];
				format(friskInfo, sizeof(friskInfo),
					"{FFFFFF}Uang Tunai: ${2ECC71}%d\n\
{FFFFFF}Kontraband: %d | Spareparts: %d | Lockpick: %d",
					gPlayerData[targetid][pMoney],
					GetPlayerItemCount(targetid, ITEM_CONTRABAND),
					GetPlayerItemCount(targetid, ITEM_CARPARTS),
					GetPlayerItemCount(targetid, ITEM_LOCKPICK));
				Dialog_Show(playerid, DialogFriskResult, DIALOG_STYLE_MSGBOX, "{FFFFFF}HASIL PENGGELEDAHAN", friskInfo, "Tutup", "");
			} else if (gPlayerData[playerid][pFaction] == FACTION_MEDIC) {
				gPlayerData[targetid][pInjured] = false;
				SetPlayerHealth(targetid, 100.0);
				SendClientMessage(playerid, COLOR_SUCCESS, "[SUKSES] Resusitasi CPR berhasil dilakukan.");
			}
		}
		case 7: {
			if (gPlayerData[playerid][pFaction] == FACTION_POLICE) ProcessConfiscateIllicitItems(playerid, targetid);
		}
		case 8: {
			if (gPlayerData[playerid][pFaction] == FACTION_POLICE) ProcessCheckDriverLicense(playerid, targetid);
		}
		case 9: {
			if (gPlayerData[playerid][pFaction] == FACTION_POLICE) ProcessBreathalyzerTest(playerid, targetid);
		}
	}
	return 1;
}

Dialog:DialogPayCashAmount(playerid, response, listitem, inputtext[])
{
	if (!response) return 1;
	new targetid = gGiveTarget[playerid];
	if (!IsPlayerConnected(targetid) || !IsPlayerNear(playerid, targetid, 4.0)) return 1;

	new amount = strval(inputtext);
	if (amount <= 0 || amount > gPlayerData[playerid][pMoney]) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Nominal uang tidak valid atau uang Anda tidak cukup.");
		return 1;
	}

	TakePlayerMoneyEx(playerid, amount, "beri uang");
	GivePlayerMoneyEx(targetid, amount, "terima uang");

	new msg[144];
	format(msg, sizeof(msg), "[INFO] Anda memberikan uang sebesar $%d kepada %s.", amount, gPlayerData[targetid][pName]);
	SendClientMessage(playerid, COLOR_SUCCESS, msg);

	format(msg, sizeof(msg), "[INFO] Anda menerima uang sebesar $%d dari %s.", amount, gPlayerData[playerid][pName]);
	SendClientMessage(targetid, COLOR_SUCCESS, msg);
	return 1;
}

Dialog:DialogFriskResult(playerid, response, listitem, inputtext[])
{
	return 1;
}
