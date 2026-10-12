#if defined _INC_radial_menu
	#endinput
#endif
#define _INC_radial_menu

/*
==============================================================================
 radial_menu.inc
 NUSANTARA ROLEPLAY - Oldschool N-Key Radial Action Menu (JGRP Style)
 Bersih, stabil, sinkron 100% tanpa pergeseran listitem dan bebas emoji corrupt.
==============================================================================
*/

forward RP_OnFinishRefuelAction(playerid, veh);

stock ShowPlayerRadialMenu(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !gPlayerData[playerid][pLoggedIn]) return 0;

	// =========================================================================
	// 1. KENDARAAN (SEDANG MENYETIR)
	// =========================================================================
	if (IsPlayerInAnyVehicle(playerid) && GetPlayerState(playerid) == PLAYER_STATE_DRIVER) {
		new bodyVeh[600];
		format(bodyVeh, sizeof(bodyVeh),
			"{FFFFFF}1. Mesin Kendaraan (Toggle Engine)\n\
{FFFFFF}2. Kunci Pintu Kendaraan (Toggle Lock)\n\
{FFFFFF}3. Lampu Utama Kendaraan (Toggle Lights)\n\
{FFFFFF}4. Kap Mesin Depan (Toggle Hood)\n\
{FFFFFF}5. Pintu Bagasi Belakang (Toggle Trunk)\n\
{FFFFFF}6. Laci Dashboard Mobil (/glovebox)\n\
{FFFFFF}7. Akses Isi Bagasi Mobil (/trunk)\n\
{FFFFFF}8. Dokumen STNK Kendaraan (/stnk)\n\
{FFFFFF}9. Daftar Kendaraan Pribadi Saya (/v)\n\
{FFFFFF}10. Daftarkan Plat Nomor Kustom DMV");

		Dialog_Show(playerid, DialogRadialVehMenu, DIALOG_STYLE_LIST,
			"{FFFFFF}MENU KENDARAAN (N-KEY)", bodyVeh, "Pilih", "Tutup");
		return 1;
	}

	// =========================================================================
	// 2. JALAN KAKI (ON FOOT) - FITUR UTAMA KARAKTER KONSISTEN
	// =========================================================================
	new bodyFoot[600];
	format(bodyFoot, sizeof(bodyFoot),
		"{FFFFFF}1. Inventaris Barang (/items /inv)\n\
{FFFFFF}2. Handphone Pintar (/phone)\n\
{FFFFFF}3. Kendaraan Pribadi & Garasi (/v)\n\
{FFFFFF}4. Statistik Karakter (/stats)\n\
{FFFFFF}5. Dompet & Lisensi (KTP, SIM, STNK)\n\
{FFFFFF}6. Lowongan Pekerjaan Kota (/job)\n\
{FFFFFF}7. Panggilan Darurat 911 (SAPD / SAMD)\n\
{FFFFFF}8. Buku Panduan & Bantuan (/help)");

	Dialog_Show(playerid, DialogRadialFootMenu, DIALOG_STYLE_LIST,
		"{FFFFFF}RADIAL ACTION MENU (N-KEY)", bodyFoot, "Pilih", "Tutup");
	return 1;
}

// ---------------------------------------------------------------------------
// Dialog: Menu Kendaraan (N-Key saat mengemudi)
// ---------------------------------------------------------------------------
Dialog:DialogRadialVehMenu(playerid, response, listitem, inputtext[])
{
	if (!response) return 1;
	new veh = GetPlayerVehicleID(playerid);
	if (veh <= 0 || !IsValidVehicle(veh)) return 1;
	new slot = Vehicle_GetSlot(veh);

	switch (listitem) {
		case 0: { // 1. Mesin
			Vehicle_SetEngineOn(veh, !Vehicle_IsEngineOn(veh));
		}
		case 1: { // 2. Kunci
			Vehicle_SetLocked(veh, !Vehicle_IsLocked(veh));
		}
		case 2: { // 3. Lampu
			if (slot != -1) Vehicle_SetLights(veh, !gVehicleData[slot][vLights]);
		}
		case 3: { // 4. Kap Mesin
			if (slot != -1) Vehicle_SetHood(veh, !gVehicleData[slot][vHood]);
		}
		case 4: { // 5. Pintu Bagasi Belakang
			if (slot != -1) Vehicle_SetTrunk(veh, !gVehicleData[slot][vTrunkOpen]);
		}
		case 5: { // 6. Glovebox Dashboard
			if (slot != -1) ShowVehicleGloveboxDialog(playerid, slot);
			else SendErr(playerid, "Kendaraan ini tidak memiliki laci dashboard pribadi.");
		}
		case 6: { // 7. Isi Bagasi Mobil
			if (slot != -1) ShowVehicleTrunkDialog(playerid, slot);
			else SendErr(playerid, "Kendaraan ini tidak memiliki bagasi pribadi.");
		}
		case 7: { // 8. STNK
			ProcessInspectSTNK(playerid);
		}
		case 8: { // 9. Kendaraan Saya (/v)
			ShowMyVehiclesDialog(playerid);
		}
		case 9: { // 10. Plat Kustom
			Dialog_Show(playerid, DialogRadialCustomPlate, DIALOG_STYLE_INPUT,
				"{FFFFFF}PLAT NOMOR KUSTOM DMV",
				"{FFFFFF}Masukkan nomor plat baru (3-8 karakter alfanumerik):", "Daftarkan", "Batal");
		}
	}
	return 1;
}

// ---------------------------------------------------------------------------
// Dialog: Menu Karakter (N-Key saat on-foot)
// ---------------------------------------------------------------------------
Dialog:DialogRadialFootMenu(playerid, response, listitem, inputtext[])
{
	if (!response) return 1;

	switch (listitem) {
		case 0: { // 1. Inventaris
			ShowPlayerInventoryDialog(playerid);
		}
		case 1: { // 2. Handphone
			ShowPhoneMenu(playerid);
		}
		case 2: { // 3. Kendaraan Saya (/v)
			ShowMyVehiclesDialog(playerid);
		}
		case 3: { // 4. Statistik Karakter (/stats)
			ShowStatsDialog(playerid);
		}
		case 4: { // 5. Dompet & Lisensi
			new licStr[320];
			format(licStr, sizeof(licStr),
				"{FFFFFF}KTP / Identitas: {FFFF00}%s (Usia: %s)\n\
{FFFFFF}Lisensi Mengemudi (SIM): %s\n\
{FFFFFF}Lisensi Senjata Api: %s\n\
{FFFFFF}Total Surat Tilang: {FFFF00}%d lembar ($%d)\n\
--------------------------------------------------\n\
{FFFFFF}Gunakan /stnk untuk memeriksa dokumen resmi kendaraan Anda.",
				gPlayerData[playerid][pName], gPlayerData[playerid][pBirthDate],
				gPlayerData[playerid][pDriveLic] ? ("{2ECC71}AKTIF") : ("{E74C3C}TIDAK ADA"),
				gPlayerData[playerid][pGunLic] ? ("{2ECC71}TERDAFTAR") : ("{E74C3C}TIDAK ADA"),
				gPlayerData[playerid][pTickets], gPlayerData[playerid][pTicketAmount]);
			Dialog_Show(playerid, DialogWalletInfo, DIALOG_STYLE_MSGBOX, "{FFFFFF}DOMPET & LISENSI SAYA", licStr, "Tutup", "");
		}
		case 5: { // 6. Lowongan Pekerjaan
			ShowJobDialog(playerid);
		}
		case 6: { // 7. Panggilan Darurat 911
			Dialog_Show(playerid, DialogRadial911, DIALOG_STYLE_LIST,
				"{FFFFFF}LAYANAN DARURAT 911",
				"{FFFFFF}1. Kepolisian (SAPD)\n{FFFFFF}2. Ambulans & Medis (SAMD)", "Panggil", "Batal");
		}
		case 7: { // 8. Bantuan & Panduan
			ShowHelpDialog(playerid);
		}
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
	new Float:px, Float:py, Float:pz;
	GetPlayerPos(playerid, px, py, pz);
	new targetid = -1;
	for (new i = 0; i < MAX_PLAYERS; i++) {
		if (i != playerid && IsPlayerConnected(i) && gPlayerData[i][pLoggedIn] && IsPlayerNear(playerid, i, 4.0)) {
			targetid = i;
			break;
		}
	}

	if (targetid == -1) {
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
