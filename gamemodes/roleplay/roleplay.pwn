/*
==============================================================================
  NUSANTARA ROLEPLAY - open.mp Indonesian Roleplay Gamemode
  Master Gamemode File
  Standard: Clean, modular, professional open.mp architecture.
==============================================================================
*/

#include <open.mp>

// Modul-modul Nusantara Roleplay
#include "modules/header.inc"
#include "modules/database.inc"
#include "modules/textdraws.inc"
#include "modules/inventory.inc"
#include "modules/phone.inc"
#include "modules/shops.inc"
#include "modules/player_stats.inc"
#include "modules/economy.inc"
#include "modules/factions.inc"
#include "modules/vehicles.inc"
#include "modules/jobs.inc"
#include "modules/commands_rp.inc"
#include "modules/player_auth.inc"
#include "modules/admin.inc"
#include "modules/debug_tests.inc"

// ---------------------------------------------------------------------------
// Entry Point open.mp
// ---------------------------------------------------------------------------
main()
{
	print("--------------------------------------------------");
	print("  NUSANTARA ROLEPLAY - open.mp Gamemode Loaded    ");
	print("  Komunitas Roleplay Indonesia                    ");
	print("--------------------------------------------------");
}

// ---------------------------------------------------------------------------
// OnGameModeInit
// ---------------------------------------------------------------------------
public OnGameModeInit()
{
	SetGameModeText("Nusantara RP v1.0");
	ShowNameTags(true);
	EnableStuntBonusForAll(false);
	UsePlayerPedAnims();
	DisableInteriorEnterExits();

	// Inisialisasi Database SQLite & Kendaraan
	DB_Init();
	Vehicle_SpawnAll();

	// Default class spawn
	AddPlayerClass(0, SPAWN_X, SPAWN_Y, SPAWN_Z, SPAWN_A, WEAPON_FIST, 0, WEAPON_FIST, 0, WEAPON_FIST, 0);

	// Buat Pickup Fasilitas Kota
	gBankPickup = CreatePickup(1274, 1, BANK_X, BANK_Y, BANK_Z);
	gATMPickup = CreatePickup(1239, 1, ATM_X, ATM_Y, ATM_Z);
	gJobPickup = CreatePickup(1239, 1, COURIER_X, COURIER_Y, COURIER_Z);
	gDealerPickup = CreatePickup(1239, 1, DEALER_X, DEALER_Y, DEALER_Z);
	gFuelPickup = CreatePickup(1650, 1, FUEL_STATION_X, FUEL_STATION_Y, FUEL_STATION_Z);
	gStorePickup = CreatePickup(1239, 1, STORE_IDLEWOOD_X, STORE_IDLEWOOD_Y, STORE_IDLEWOOD_Z);
	gPolicePickup = CreatePickup(1247, 1, POLICE_X, POLICE_Y, POLICE_Z);
	gMedicPickup = CreatePickup(1240, 1, MEDIC_X, MEDIC_Y, MEDIC_Z);
	gGovPickup = CreatePickup(1239, 1, GOV_X, GOV_Y, GOV_Z);
	gGangPickup = CreatePickup(1239, 1, GANG_X, GANG_Y, GANG_Z);

	// 3D Text Labels Lokasi
	Create3DTextLabel("{33CCFF}[ BANK NUSANTARA ]\n{FFFFFF}Gunakan /bank untuk transaksi", COLOR_LIGHTBLUE, BANK_X, BANK_Y, BANK_Z + 0.8, 15.0, 0, false);
	Create3DTextLabel("{33CCFF}[ ATM ]\n{FFFFFF}Gunakan /atm untuk transaksi", COLOR_LIGHTBLUE, ATM_X, ATM_Y, ATM_Z + 0.8, 10.0, 0, false);
	Create3DTextLabel("{FFFF44}[ PUSAT PEKERJAAN ]\n{FFFFFF}Gunakan /job untuk memilih pekerjaan", COLOR_YELLOW, COURIER_X, COURIER_Y, COURIER_Z + 0.8, 15.0, 0, false);
	Create3DTextLabel("{00FF00}[ DEALERSHIP ]\n{FFFFFF}Gunakan /buyveh untuk membeli kendaraan", COLOR_GREEN, DEALER_X, DEALER_Y, DEALER_Z + 0.8, 15.0, 0, false);
	Create3DTextLabel("{FF9900}[ SPBU ]\n{FFFFFF}Gunakan /refuel untuk isi bensin", COLOR_ORANGE, FUEL_STATION_X, FUEL_STATION_Y, FUEL_STATION_Z + 0.8, 15.0, 0, false);
	Create3DTextLabel("{33CCFF}[ TOKO 24/7 ]\n{FFFFFF}Gunakan /buy untuk membeli kebutuhan", COLOR_LIGHTBLUE, STORE_IDLEWOOD_X, STORE_IDLEWOOD_Y, STORE_IDLEWOOD_Z + 0.8, 15.0, 0, false);
	Create3DTextLabel("{33CCFF}[ KANTOR KEPOLISIAN (SAPD) ]\n{FFFFFF}Markas Komando Polisi Los Santos", COLOR_LIGHTBLUE, POLICE_X, POLICE_Y, POLICE_Z + 0.8, 15.0, 0, false);
	Create3DTextLabel("{FF6B6B}[ RUMAH SAKIT (SAMD) ]\n{FFFFFF}Pelayanan Medis Darurat All Saints", COLOR_RED, MEDIC_X, MEDIC_Y, MEDIC_Z + 0.8, 15.0, 0, false);

	// Timer Utama
	gTimerTick = SetTimer("RP_Tick", 1000, true);
	gTimerSpeedo = SetTimer("RP_SpeedoTick", 150, true);
	gTimerSave = SetTimer("RP_SaveTick", 60000, true);

	// Jalankan Automated Feature Test Suite
	SetTimer("RunAutomatedRPTests", 1200, false);

	print("[open.mp] Gamemode OnGameModeInit selesai dijalankan.");
	return 1;
}

// ---------------------------------------------------------------------------
// OnGameModeExit
// ---------------------------------------------------------------------------
public OnGameModeExit()
{
	print("[open.mp] Gamemode keluar. Menyimpan seluruh data...");
	for (new i = 0; i < MAX_PLAYERS; i++) {
		if (IsPlayerConnected(i) && gPlayerData[i][pLoggedIn]) {
			DB_SavePlayerData(i);
			TextdrawDestroyAll(i);
		}
	}
	if (gTimerTick) KillTimer(gTimerTick);
	if (gTimerSpeedo) KillTimer(gTimerSpeedo);
	if (gTimerSave) KillTimer(gTimerSave);

	DB_Shutdown();
	return 1;
}

// ---------------------------------------------------------------------------
// OnPlayerConnect
// ---------------------------------------------------------------------------
public OnPlayerConnect(playerid)
{
	ResetPlayerData(playerid);

	new name[MAX_PLAYER_NAME], ip[20];
	GetPlayerName(playerid, name, sizeof(name));
	GetPlayerIp(playerid, ip, sizeof(ip));

	if (DB_IsBanned(name, ip)) {
		SendClientMessage(playerid, COLOR_RED, "[BANNED] Akun atau IP Anda diblokir dari server ini!");
		Kick(playerid);
		return 1;
	}

	TextdrawCreateAll(playerid);
	SetTimerEx("RP_DelayLogin", 500, false, "i", playerid);
	return 1;
}

public RP_DelayLogin(playerid)
{
	if (!IsPlayerConnected(playerid)) return 1;
	new name[MAX_PLAYER_NAME];
	GetPlayerName(playerid, name, sizeof(name));

	if (DB_PlayerExists(name)) {
		ShowLoginDialog(playerid);
	} else {
		ShowRegisterDialog(playerid);
	}
	return 1;
}

// ---------------------------------------------------------------------------
// OnPlayerDisconnect
// ---------------------------------------------------------------------------
public OnPlayerDisconnect(playerid, reason)
{
	if (gPlayerData[playerid][pLoggedIn]) {
		if (gJobActive[playerid]) {
			ProcessJobQuit(playerid);
		}
		SavePlayerOnDisconnect(playerid);
	}
	TextdrawDestroyAll(playerid);
	return 1;
}

// ---------------------------------------------------------------------------
// OnPlayerRequestClass & OnPlayerSpawn
// ---------------------------------------------------------------------------
public OnPlayerRequestClass(playerid, classid)
{
	if (gPlayerData[playerid][pLoggedIn]) {
		SpawnPlayer(playerid);
		return 1;
	}
	SetPlayerPos(playerid, SPAWN_X, SPAWN_Y, SPAWN_Z);
	return 1;
}

public OnPlayerSpawn(playerid)
{
	if (gPlayerData[playerid][pLoggedIn]) {
		SpawnPlayerEx(playerid);
	}
	return 1;
}

// ---------------------------------------------------------------------------
// OnPlayerDeath
// ---------------------------------------------------------------------------
public OnPlayerDeath(playerid, killerid, WEAPON:reason)
{
	gPlayerData[playerid][pDeaths]++;

	if (killerid != INVALID_PLAYER_ID && killerid != playerid && IsPlayerConnected(killerid)) {
		gPlayerData[killerid][pKills]++;
		GivePlayerExp(killerid, 10, "menumbangkan lawan");
	}

	gPlayerData[playerid][pHunger] = 60.0;
	gPlayerData[playerid][pThirst] = 60.0;
	UpdatePlayerHUD(playerid);

	ProcessPlayerKnockout(playerid, "luka tembak / serangan fatal");
	return 1;
}

// ---------------------------------------------------------------------------
// OnPlayerText (IC Speech 20m + Active Phone Routing)
// ---------------------------------------------------------------------------
public OnPlayerText(playerid, text[])
{
	if (!gPlayerData[playerid][pLoggedIn]) {
		SendErr(playerid, "Anda belum login.");
		return 0;
	}
	if (gPlayerData[playerid][pMuted]) {
		SendErr(playerid, "Anda sedang dibisukan.");
		return 0;
	}

	// Jika sedang dalam panggilan aktif telepon
	if (gPlayerData[playerid][pCallState] == 2) {
		new targetid = gPlayerData[playerid][pCallTarget];
		if (targetid != -1 && IsPlayerConnected(targetid) && gPlayerData[targetid][pCallState] == 2) {
			SendRP(targetid, COLOR_YELLOW, "[PANGGILAN] %s: %s", gPlayerData[playerid][pName], text);
			SendRP(playerid, COLOR_YELLOW, "[PANGGILAN] Anda: %s", text);

			new localMsg[128];
			format(localMsg, sizeof(localMsg), "(telepon) %s: %s", gPlayerData[playerid][pName], text);
			SendNearbyPlayer(playerid, COLOR_DARKGREY, localMsg, 8.0);
			return 0;
		}
	}

	new msg[192];
	format(msg, sizeof(msg), "%s berkata: %s", gPlayerData[playerid][pName], text);
	SendNearbyPlayer(playerid, COLOR_WHITE, msg, 20.0);
	return 0;
}

// ---------------------------------------------------------------------------
// Parser Perintah
// ---------------------------------------------------------------------------
stock ParseCommand(const cmdtext[], cmd[], params[], cmdLen = 32, paramsLen = 128)
{
	new i = 0, j = 0;
	while (cmdtext[i] == ' ' || cmdtext[i] == '/') i++;
	while (cmdtext[i] != '\0' && cmdtext[i] != ' ' && j < cmdLen - 1) {
		cmd[j++] = cmdtext[i++];
	}
	cmd[j] = '\0';
	while (cmdtext[i] == ' ') i++;
	j = 0;
	while (cmdtext[i] != '\0' && j < paramsLen - 1) {
		params[j++] = cmdtext[i++];
	}
	params[j] = '\0';
	return 1;
}

stock SplitParams2(const params[], arg1[], arg2[], len1 = 32, len2 = 128)
{
	new i = 0, j = 0;
	while (params[i] == ' ') i++;
	while (params[i] != '\0' && params[i] != ' ' && j < len1 - 1) {
		arg1[j++] = params[i++];
	}
	arg1[j] = '\0';
	while (params[i] == ' ') i++;
	j = 0;
	while (params[i] != '\0' && j < len2 - 1) {
		arg2[j++] = params[i++];
	}
	arg2[j] = '\0';
	return 1;
}

stock SplitParams3(const params[], arg1[], arg2[], arg3[], len1 = 32, len2 = 32, len3 = 64)
{
	new i = 0, j = 0;
	while (params[i] == ' ') i++;
	while (params[i] != '\0' && params[i] != ' ' && j < len1 - 1) {
		arg1[j++] = params[i++];
	}
	arg1[j] = '\0';
	while (params[i] == ' ') i++;
	j = 0;
	while (params[i] != '\0' && j < len2 - 1) {
		arg2[j++] = params[i++];
	}
	arg2[j] = '\0';
	while (params[i] == ' ') i++;
	j = 0;
	while (params[i] != '\0' && j < len3 - 1) {
		arg3[j++] = params[i++];
	}
	arg3[j] = '\0';
	return 1;
}

// ---------------------------------------------------------------------------
// OnPlayerCommandText
// ---------------------------------------------------------------------------
public OnPlayerCommandText(playerid, cmdtext[])
{
	new cmd[32], params[128];
	ParseCommand(cmdtext, cmd, params);

	// Perintah Umum & Roleplay
	if (strcmp(cmd, "help", true) == 0) {
		ShowHelpDialog(playerid);
		return 1;
	}
	if (strcmp(cmd, "stats", true) == 0) {
		ShowStatsDialog(playerid);
		return 1;
	}
	if (strcmp(cmd, "inv", true) == 0 || strcmp(cmd, "inventory", true) == 0 || strcmp(cmd, "items", true) == 0) {
		ShowPlayerInventory(playerid);
		return 1;
	}
	if (strcmp(cmd, "gps", true) == 0) {
		ShowGPSMenu(playerid);
		return 1;
	}
	if (strcmp(cmd, "me", true) == 0) {
		ProcessMe(playerid, params);
		return 1;
	}
	if (strcmp(cmd, "do", true) == 0) {
		ProcessDo(playerid, params);
		return 1;
	}
	if (strcmp(cmd, "b", true) == 0) {
		ProcessB(playerid, params);
		return 1;
	}
	if (strcmp(cmd, "o", true) == 0) {
		ProcessO(playerid, params);
		return 1;
	}
	if (strcmp(cmd, "w", true) == 0 || strcmp(cmd, "whisper", true) == 0) {
		new target[32], msg[128];
		SplitParams2(params, target, msg);
		ProcessWhisper(playerid, strval(target), msg);
		return 1;
	}
	if (strcmp(cmd, "s", true) == 0 || strcmp(cmd, "shout", true) == 0) {
		ProcessShout(playerid, params);
		return 1;
	}
	if (strcmp(cmd, "l", true) == 0 || strcmp(cmd, "low", true) == 0) {
		ProcessLow(playerid, params);
		return 1;
	}
	if (strcmp(cmd, "id", true) == 0) {
		ProcessFindID(playerid, params);
		return 1;
	}
	if (strcmp(cmd, "admins", true) == 0) {
		ProcessAdmins(playerid);
		return 1;
	}
	if (strcmp(cmd, "pm", true) == 0) {
		new target[32], msg[128];
		SplitParams2(params, target, msg);
		ProcessPM(playerid, strval(target), msg);
		return 1;
	}
	if (strcmp(cmd, "report", true) == 0) {
		ProcessReport(playerid, params);
		return 1;
	}
	if (strcmp(cmd, "ask", true) == 0) {
		ProcessAsk(playerid, params);
		return 1;
	}

	// Telekomunikasi
	if (strcmp(cmd, "call", true) == 0) {
		ProcessCall(playerid, params);
		return 1;
	}
	if (strcmp(cmd, "p", true) == 0) {
		ProcessPickup(playerid);
		return 1;
	}
	if (strcmp(cmd, "h", true) == 0) {
		ProcessHangup(playerid);
		return 1;
	}
	if (strcmp(cmd, "sms", true) == 0) {
		new numStr[32], msg[128];
		SplitParams2(params, numStr, msg);
		ProcessSMS(playerid, strval(numStr), msg);
		return 1;
	}
	if (strcmp(cmd, "setfreq", true) == 0) {
		new f = strval(params);
		if (f < 1 || f > 999) {
			SendSyntax(playerid, "/setfreq [1-999]");
			return 1;
		}
		gPlayerData[playerid][pRadioFreq] = f;
		SendSucc(playerid, "Frekuensi Handy Talkie (HT) disetel ke CH: %d MHz.", f);
		return 1;
	}
	if (strcmp(cmd, "r", true) == 0) {
		ProcessRadio(playerid, params);
		return 1;
	}
	if (strcmp(cmd, "d", true) == 0) {
		ProcessDepartmentRadio(playerid, params);
		return 1;
	}
	if (strcmp(cmd, "m", true) == 0) {
		ProcessMegaphone(playerid, params);
		return 1;
	}

	// Toko & Ekonomi
	if (strcmp(cmd, "buy", true) == 0) {
		ShowStoreBuyMenu(playerid);
		return 1;
	}
	if (strcmp(cmd, "pay", true) == 0) {
		new target[32], amt_s[32];
		SplitParams2(params, target, amt_s);
		ProcessPay(playerid, target, strval(amt_s));
		return 1;
	}
	if (strcmp(cmd, "bank", true) == 0) {
		ShowBankDialog(playerid);
		return 1;
	}
	if (strcmp(cmd, "atm", true) == 0) {
		ShowATMDialog(playerid);
		return 1;
	}

	// Pekerjaan (Jobs)
	if (strcmp(cmd, "job", true) == 0) {
		ShowJobDialog(playerid);
		return 1;
	}
	if (strcmp(cmd, "quitjob", true) == 0) {
		ProcessJobQuit(playerid);
		return 1;
	}

	// Kendaraan Pribadi
	if (strcmp(cmd, "buyveh", true) == 0) {
		ShowDealershipDialog(playerid);
		return 1;
	}
	if (strcmp(cmd, "myveh", true) == 0 || strcmp(cmd, "v", true) == 0) {
		ShowMyVehiclesDialog(playerid);
		return 1;
	}
	if (strcmp(cmd, "park", true) == 0) {
		new vid = GetPlayerVehicleID(playerid);
		new slot = Vehicle_GetSlot(vid);
		if (slot != -1) ProcessVehiclePark(playerid, slot);
		else SendErr(playerid, "Anda harus berada di dalam kendaraan pribadi Anda.");
		return 1;
	}
	if (strcmp(cmd, "lock", true) == 0) {
		new vid = GetPlayerVehicleID(playerid);
		new slot = Vehicle_GetSlot(vid);
		if (slot != -1) ProcessVehicleLock(playerid, slot);
		else SendErr(playerid, "Anda harus berada di dalam kendaraan pribadi Anda.");
		return 1;
	}
	if (strcmp(cmd, "engine", true) == 0) {
		ProcessEngineToggle(playerid);
		return 1;
	}
	if (strcmp(cmd, "lights", true) == 0) {
		ProcessLightsToggle(playerid);
		return 1;
	}
	if (strcmp(cmd, "trunk", true) == 0) {
		new vid = GetPlayerVehicleID(playerid);
		new slot = Vehicle_GetSlot(vid);
		if (slot != -1) {
			new bool:newTrunk = !gVehicleData[slot][vTrunkOpen];
			Vehicle_SetTrunk(vid, newTrunk);
			SendRP(playerid, COLOR_INFO, "Pintu bagasi kendaraan %s.", newTrunk ? "dibuka" : "ditutup");
		}
		return 1;
	}
	if (strcmp(cmd, "hood", true) == 0) {
		new vid = GetPlayerVehicleID(playerid);
		new slot = Vehicle_GetSlot(vid);
		if (slot != -1) {
			new bool:newHood = !gVehicleData[slot][vHood];
			Vehicle_SetHood(vid, newHood);
			SendRP(playerid, COLOR_INFO, "Kap mesin kendaraan %s.", newHood ? "dibuka" : "ditutup");
		}
		return 1;
	}
	if (strcmp(cmd, "refuel", true) == 0 || strcmp(cmd, "fill", true) == 0) {
		ProcessRefuel(playerid);
		return 1;
	}

	// Faksi
	if (strcmp(cmd, "duty", true) == 0) {
		new bool:newduty = !gPlayerData[playerid][pDuty];
		ProcessFactionDuty(playerid, newduty);
		return 1;
	}
	if (strcmp(cmd, "cuff", true) == 0) {
		new targetid = strval(params);
		ProcessCuff(playerid, targetid);
		return 1;
	}
	if (strcmp(cmd, "uncuff", true) == 0) {
		new targetid = strval(params);
		ProcessUncuff(playerid, targetid);
		return 1;
	}
	if (strcmp(cmd, "frisk", true) == 0) {
		new targetid = strval(params);
		ProcessFrisk(playerid, targetid);
		return 1;
	}
	if (strcmp(cmd, "ticket", true) == 0) {
		new target[32], amt_s[32], reason[64];
		SplitParams3(params, target, amt_s, reason);
		ProcessTicket(playerid, strval(target), strval(amt_s), reason);
		return 1;
	}
	if (strcmp(cmd, "arrest", true) == 0) {
		new target[32], time_s[32];
		SplitParams2(params, target, time_s);
		new t = strval(time_s);
		if (t < 1) t = 5;
		ProcessArrest(playerid, strval(target), t, 100);
		return 1;
	}
	if (strcmp(cmd, "heal", true) == 0) {
		new targetid = strval(params);
		ProcessHeal(playerid, targetid);
		return 1;
	}
	if (strcmp(cmd, "revive", true) == 0) {
		new targetid = strval(params);
		ProcessRevive(playerid, targetid);
		return 1;
	}
	if (strcmp(cmd, "gov", true) == 0) {
		ProcessGovBroadcast(playerid, params);
		return 1;
	}
	if (strcmp(cmd, "settax", true) == 0) {
		ProcessSetTax(playerid, strval(params));
		return 1;
	}

	// Admin
	if (strcmp(cmd, "a", true) == 0) {
		ProcessAdminChat(playerid, params);
		return 1;
	}
	if (strcmp(cmd, "goto", true) == 0) {
		ProcessGoto(playerid, params);
		return 1;
	}
	if (strcmp(cmd, "gethere", true) == 0) {
		ProcessGethere(playerid, params);
		return 1;
	}
	if (strcmp(cmd, "setskin", true) == 0) {
		new target[32], skin_s[32];
		SplitParams2(params, target, skin_s);
		ProcessSetSkin(playerid, target, strval(skin_s));
		return 1;
	}
	if (strcmp(cmd, "setfaction", true) == 0) {
		new target[32], fac_s[32], rank_s[32];
		SplitParams3(params, target, fac_s, rank_s);
		ProcessSetFaction(playerid, target, strval(fac_s), strval(rank_s));
		return 1;
	}
	if (strcmp(cmd, "givemoney", true) == 0) {
		new target[32], amt_s[32];
		SplitParams2(params, target, amt_s);
		ProcessGiveMoney(playerid, target, strval(amt_s));
		return 1;
	}
	if (strcmp(cmd, "freeze", true) == 0) {
		ProcessFreeze(playerid, params);
		return 1;
	}
	if (strcmp(cmd, "unfreeze", true) == 0) {
		ProcessUnfreeze(playerid, params);
		return 1;
	}
	if (strcmp(cmd, "kick", true) == 0) {
		new target[32], reason[96];
		SplitParams2(params, target, reason);
		ProcessKick(playerid, target, reason);
		return 1;
	}
	if (strcmp(cmd, "ban", true) == 0) {
		new target[32], reason[96];
		SplitParams2(params, target, reason);
		ProcessBan(playerid, target, reason);
		return 1;
	}

	SendErr(playerid, "Perintah tidak terdaftar. Ketik /help untuk melihat panduan perintah.");
	return 1;
}

// ---------------------------------------------------------------------------
// OnPlayerKeyStateChange (KEY_NO -> Engine toggle)
// ---------------------------------------------------------------------------
public OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys)
{
	if ((newkeys & KEY_NO) && !(oldkeys & KEY_NO)) {
		if (IsPlayerInAnyVehicle(playerid) && GetPlayerState(playerid) == PLAYER_STATE_DRIVER) {
			ProcessEngineToggle(playerid);
			return 1;
		}
	}
	return 1;
}

// ---------------------------------------------------------------------------
// OnPlayerStateChange (Speedometer Show/Hide)
// ---------------------------------------------------------------------------
public OnPlayerStateChange(playerid, PLAYER_STATE:newstate, PLAYER_STATE:oldstate)
{
	if (newstate == PLAYER_STATE_DRIVER) {
		ShowPlayerSpeedo(playerid);
	}
	else if (oldstate == PLAYER_STATE_DRIVER && newstate == PLAYER_STATE_ONFOOT) {
		HidePlayerSpeedo(playerid);
	}
	return 1;
}

// ---------------------------------------------------------------------------
// OnDialogResponse
// ---------------------------------------------------------------------------
public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	if (!response) {
		if (dialogid == DIALOG_LOGIN || dialogid == DIALOG_REGISTER) {
			SendErr(playerid, "Anda harus masuk atau mendaftar untuk dapat bermain.");
			SetTimerEx("RP_DelayLogin", 1000, false, "i", playerid);
		}
		return 1;
	}

	switch (dialogid) {
		case DIALOG_REGISTER: {
			ProcessRegister(playerid, inputtext);
			return 1;
		}
		case DIALOG_REGISTER_CONFIRM: {
			ProcessRegisterConfirm(playerid, inputtext);
			return 1;
		}
		case DIALOG_LOGIN: {
			ProcessLogin(playerid, inputtext);
			return 1;
		}
		case DIALOG_INVENTORY: {
			// Simpan slot yang dipilih lalu buka menu aksi
			gSelectedInvSlot[playerid] = listitem;
			ShowPlayerDialog(playerid, DIALOG_ITEM_ACTION, DIALOG_STYLE_LIST,
				"TINDAKAN BARANG", "1. Gunakan Barang\n2. Buang Barang", "Pilih", "Kembali");
			return 1;
		}
		case DIALOG_ITEM_ACTION: {
			if (listitem == 0) {
				UsePlayerItemBySlot(playerid, gSelectedInvSlot[playerid]);
			} else if (listitem == 1) {
				new slot = gSelectedInvSlot[playerid];
				TakePlayerItem(playerid, gPlayerItems[playerid][slot][itType], 1);
				SendInfo(playerid, "Barang telah dibuang dari kantong.");
			}
			return 1;
		}
		case DIALOG_STORE_BUY: {
			ProcessStorePurchase(playerid, listitem);
			return 1;
		}
		case DIALOG_PHONE_MENU: {
			if (listitem == 0) {
				ShowPlayerDialog(playerid, DIALOG_PHONE_CALL, DIALOG_STYLE_INPUT,
					"PANGGILAN TELEPON", "Masukkan nomor telepon tujuan:", "Panggil", "Batal");
			} else if (listitem == 1) {
				ShowPlayerDialog(playerid, DIALOG_PHONE_SMS, DIALOG_STYLE_INPUT,
					"KIRIM PESAN (SMS)", "Masukkan: [Nomor] [Pesan]", "Kirim", "Batal");
			} else if (listitem == 2) {
				gPlayerData[playerid][pPhoneStatus] = !gPlayerData[playerid][pPhoneStatus];
				SendInfo(playerid, "Mode dering ponsel dialihkan ke: %s.", gPlayerData[playerid][pPhoneStatus] ? "Aktif" : "Hening");
			}
			return 1;
		}
		case DIALOG_PHONE_CALL: {
			ProcessCall(playerid, inputtext);
			return 1;
		}
		case DIALOG_PHONE_SMS: {
			new numStr[32], msg[128];
			SplitParams2(inputtext, numStr, msg);
			ProcessSMS(playerid, strval(numStr), msg);
			return 1;
		}
		case DIALOG_RADIO_FREQ: {
			new f = strval(inputtext);
			if (f < 1 || f > 999) {
				SendErr(playerid, "Frekuensi harus antara 1 - 999 MHz.");
				return 1;
			}
			gPlayerData[playerid][pRadioFreq] = f;
			SendSucc(playerid, "Frekuensi Handy Talkie disetel ke %d MHz.", f);
			return 1;
		}
		case DIALOG_GPS_MENU: {
			switch (listitem) {
				case 0: { SetPlayerCheckpoint(playerid, BANK_X, BANK_Y, BANK_Z, 6.0); SendSucc(playerid, "Navigasi disetel ke: Bank Nusantara."); }
				case 1: { SetPlayerCheckpoint(playerid, GOV_X, GOV_Y, GOV_Z, 6.0); SendSucc(playerid, "Navigasi disetel ke: Balai Kota (SAGOV)."); }
				case 2: { SetPlayerCheckpoint(playerid, MEDIC_X, MEDIC_Y, MEDIC_Z, 6.0); SendSucc(playerid, "Navigasi disetel ke: Rumah Sakit All Saints (SAMD)."); }
				case 3: { SetPlayerCheckpoint(playerid, POLICE_X, POLICE_Y, POLICE_Z, 6.0); SendSucc(playerid, "Navigasi disetel ke: Kantor Polisi LSPD (SAPD)."); }
				case 4: { SetPlayerCheckpoint(playerid, COURIER_X, COURIER_Y, COURIER_Z, 6.0); SendSucc(playerid, "Navigasi disetel ke: Pusat Pekerjaan Umum."); }
				case 5: { SetPlayerCheckpoint(playerid, FUEL_STATION_X, FUEL_STATION_Y, FUEL_STATION_Z, 6.0); SendSucc(playerid, "Navigasi disetel ke: SPBU Bahan Bakar."); }
				case 6: { SetPlayerCheckpoint(playerid, STORE_IDLEWOOD_X, STORE_IDLEWOOD_Y, STORE_IDLEWOOD_Z, 6.0); SendSucc(playerid, "Navigasi disetel ke: Toko 24/7 Idlewood."); }
				case 7: { SetPlayerCheckpoint(playerid, DEALER_X, DEALER_Y, DEALER_Z, 6.0); SendSucc(playerid, "Navigasi disetel ke: Dealership Los Santos."); }
				case 8: { DisablePlayerCheckpoint(playerid); SendInfo(playerid, "Tanda navigasi GPS dimatikan."); }
			}
			return 1;
		}
		case DIALOG_BANK_MENU: {
			OnBankDialogResponse(playerid, listitem);
			return 1;
		}
		case DIALOG_BANK_DEPOSIT: {
			OnBankDepositResponse(playerid, inputtext);
			return 1;
		}
		case DIALOG_BANK_WITHDRAW: {
			OnBankWithdrawResponse(playerid, inputtext);
			return 1;
		}
		case DIALOG_BANK_TRANSFER: {
			OnBankTransferResponse(playerid, inputtext);
			return 1;
		}
		case DIALOG_ATM_MENU: {
			OnATMMenuResponse(playerid, listitem);
			return 1;
		}
		case DIALOG_ATM_WITHDRAW: {
			OnATMWithdrawResponse(playerid, inputtext);
			return 1;
		}
		case DIALOG_ATM_DEPOSIT: {
			OnATMDepositResponse(playerid, inputtext);
			return 1;
		}
		case DIALOG_JOB_MENU: {
			OnJobMenuResponse(playerid, listitem);
			return 1;
		}
		case DIALOG_DEALERSHIP: {
			OnDealershipResponse(playerid, listitem);
			return 1;
		}
		case DIALOG_MYVEH: {
			ShowVehicleActionDialog(playerid, listitem);
			return 1;
		}
		case DIALOG_MYVEH_ACTION: {
			OnMyVehActionResponse(playerid, listitem);
			return 1;
		}
	}
	return 1;
}

// ---------------------------------------------------------------------------
// OnPlayerEnterCheckpoint
// ---------------------------------------------------------------------------
public OnPlayerEnterCheckpoint(playerid)
{
	if (gJobActive[playerid]) {
		OnJobCheckpoint(playerid);
	}
	return 1;
}

// ---------------------------------------------------------------------------
// Loop Timer Global
// ---------------------------------------------------------------------------
public RP_Tick()
{
	for (new i = 0; i < MAX_PLAYERS; i++) {
		if (!IsPlayerConnected(i) || !gPlayerData[i][pLoggedIn]) continue;
		UpdateHungerThirst(i);
		TickPaycheck(i);
	}
	UpdateVehicleFuelLoop();
	return 1;
}

public RP_SaveTick()
{
	for (new i = 0; i < MAX_PLAYERS; i++) {
		if (IsPlayerConnected(i) && gPlayerData[i][pLoggedIn]) {
			DB_SavePlayerData(i);
		}
	}
	for (new i = 0; i < MAX_OWNED_VEHICLES; i++) {
		if (gVehicleData[i][vUsed]) {
			DB_SaveVehicle(i);
		}
	}
	return 1;
}
