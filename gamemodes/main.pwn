#include <open.mp>
#include <a_mysql>
#include <sscanf2>
#include <izcmd>
#include <samp_bcrypt>

// core modules
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

main()
{
	print("--------------------------------------------------");
	print("  NUSANTARA ROLEPLAY - open.mp Gamemode Loaded    ");
	print("  Komunitas Roleplay Indonesia                    ");
	print("--------------------------------------------------");
}

// gamemode init
public OnGameModeInit()
{
	SetGameModeText("Nusantara RP v1.0");
	ShowNameTags(true);
	EnableStuntBonusForAll(false);
	UsePlayerPedAnims();
	DisableInteriorEnterExits();

	// db init
	DB_Init();

	// default class spawn
	AddPlayerClass(0, SPAWN_X, SPAWN_Y, SPAWN_Z, SPAWN_A, WEAPON_FIST, 0, WEAPON_FIST, 0, WEAPON_FIST, 0);

	// pickups
	gBankPickup = CreatePickup(1274, 1, BANK_X, BANK_Y, BANK_Z);
	gATMPickup = CreatePickup(1239, 1, ATM_X, ATM_Y, ATM_Z);
	gJobPickup = CreatePickup(1239, 1, COURIER_X, COURIER_Y, COURIER_Z);
	gDealerPickup = CreatePickup(1239, 1, DEALER_X, DEALER_Y, DEALER_Z);
	gFuelPickup = CreatePickup(1650, 1, FUEL_STATION_X, FUEL_STATION_Y, FUEL_STATION_Z);
	gStorePickup = CreatePickup(1239, 1, STORE_IDLEWOOD_X, STORE_IDLEWOOD_Y, STORE_IDLEWOOD_Z);
	gPolicePickup = CreatePickup(1247, 1, POLICE_X, POLICE_Y, POLICE_Z);
	gMedicPickup = CreatePickup(1240, 1, MEDIC_X, MEDIC_Y, MEDIC_Z);
	gGovPickup = CreatePickup(1239, 1, GOV_X, GOV_Y, GOV_Z);

	// 3d text labels
	Create3DTextLabel("{33CCFF}[ BANK NUSANTARA ]\n{FFFFFF}Gunakan /bank untuk transaksi", COLOR_LIGHTBLUE, BANK_X, BANK_Y, BANK_Z + 0.8, 15.0, 0, false);
	Create3DTextLabel("{33CCFF}[ ATM ]\n{FFFFFF}Gunakan /atm untuk transaksi", COLOR_LIGHTBLUE, ATM_X, ATM_Y, ATM_Z + 0.8, 10.0, 0, false);
	Create3DTextLabel("{FFFF44}[ PUSAT PEKERJAAN ]\n{FFFFFF}Gunakan /job untuk memilih pekerjaan", COLOR_YELLOW, COURIER_X, COURIER_Y, COURIER_Z + 0.8, 15.0, 0, false);
	Create3DTextLabel("{00FF00}[ DEALERSHIP ]\n{FFFFFF}Gunakan /buyveh untuk membeli kendaraan", COLOR_GREEN, DEALER_X, DEALER_Y, DEALER_Z + 0.8, 15.0, 0, false);
	Create3DTextLabel("{FF9900}[ SPBU ]\n{FFFFFF}Gunakan /refuel untuk isi bensin", COLOR_ORANGE, FUEL_STATION_X, FUEL_STATION_Y, FUEL_STATION_Z + 0.8, 15.0, 0, false);
	Create3DTextLabel("{33CCFF}[ TOKO 24/7 ]\n{FFFFFF}Gunakan /buy untuk membeli kebutuhan", COLOR_LIGHTBLUE, STORE_IDLEWOOD_X, STORE_IDLEWOOD_Y, STORE_IDLEWOOD_Z + 0.8, 15.0, 0, false);
	Create3DTextLabel("{33CCFF}[ KANTOR KEPOLISIAN (SAPD) ]\n{FFFFFF}Markas Komando Polisi Los Santos", COLOR_LIGHTBLUE, POLICE_X, POLICE_Y, POLICE_Z + 0.8, 15.0, 0, false);
	Create3DTextLabel("{FF6B6B}[ RUMAH SAKIT (SAMD) ]\n{FFFFFF}Pelayanan Medis Darurat All Saints", COLOR_RED, MEDIC_X, MEDIC_Y, MEDIC_Z + 0.8, 15.0, 0, false);
	Create3DTextLabel("{00FF00}[ PASAR IKAN DERMAGA ]\n{FFFFFF}Gunakan /sellfish untuk menjual hasil tangkapan", COLOR_GREEN, FISH_PIER_X, FISH_PIER_Y, FISH_PIER_Z + 0.8, 15.0, 0, false);
	Create3DTextLabel("{FFFF44}[ PENGEPUL RONGSOCKAN ]\n{FFFFFF}Gunakan /selltrash untuk menjual barang bekas", COLOR_YELLOW, TRASH_BUYER_X, TRASH_BUYER_Y, TRASH_BUYER_Z + 0.8, 15.0, 0, false);
	Create3DTextLabel("{E67E22}[ PABRIK KAYU SAWMILL ]\n{FFFFFF}Gunakan /sellwood untuk menjual balok kayu", COLOR_ORANGE, SAWMILL_X, SAWMILL_Y, SAWMILL_Z + 0.8, 15.0, 0, false);

	// timers
	gTimerTick = SetTimer("RP_Tick", 1000, true);
	gTimerSpeedo = SetTimer("RP_SpeedoTick", 150, true);
	gTimerSave = SetTimer("RP_SaveTick", 60000, true);

	// run automated tests
	SetTimer("RunAutomatedRPTests", 1500, false);
	return 1;
}

// gamemode exit
public OnGameModeExit()
{
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

// player connection & auth
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
	ShowUCPInputPrompt(playerid);
	return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
	if (gPlayerData[playerid][pLoggedIn]) {
		if (gJobActive[playerid]) ProcessJobQuit(playerid);
		SavePlayerOnDisconnect(playerid);
	}
	TextdrawDestroyAll(playerid);
	return 1;
}

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

	ProcessPlayerKnockout(playerid, "luka tembak / serangan fisik");
	return 1;
}

// local text chat & phone call routing
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

	// phone call routing
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

// key handling
public OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys)
{
	// Key Y (KEY_YES) untuk interaksi pekerjaan dan fasilitas umum
	if ((newkeys & KEY_YES) && !(oldkeys & KEY_YES)) {
		ProcessKeyActivity(playerid);
		return 1;
	}

	// Key N (KEY_NO) untuk Dashboard Kendaraan (di mobil) atau Menu Cepat (jalan kaki)
	if ((newkeys & KEY_NO) && !(oldkeys & KEY_NO)) {
		if (IsPlayerInAnyVehicle(playerid)) {
			if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER) {
				ShowPlayerVehDash(playerid);
			}
		} else {
			ShowPlayerActMenu(playerid);
		}
		return 1;
	}
	return 1;
}

// clickable textdraws
public OnPlayerClickPlayerTextDraw(playerid, PlayerText:playertextid)
{
	ProcessPlayerClickTD(playerid, playertextid);
	return 1;
}

public OnPlayerClickTextDraw(playerid, Text:clickedid)
{
	if (clickedid == Text:INVALID_TEXT_DRAW) {
		ProcessPlayerClickTD(playerid, PlayerText:INVALID_TEXT_DRAW);
	}
	return 1;
}

// speedometer toggle
public OnPlayerStateChange(playerid, PLAYER_STATE:newstate, PLAYER_STATE:oldstate)
{
	if (newstate == PLAYER_STATE_DRIVER) ShowPlayerSpeedo(playerid);
	else if (oldstate == PLAYER_STATE_DRIVER && newstate == PLAYER_STATE_ONFOOT) HidePlayerSpeedo(playerid);
	return 1;
}

// dialog responses
public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	if (!response) {
		if (dialogid == DIALOG_UCP_LOGIN || dialogid == DIALOG_LOGIN || dialogid == DIALOG_UCP_REG_CODE || dialogid == DIALOG_CHAR_SELECT) {
			SendErr(playerid, "Anda harus masuk atau mendaftar untuk bermain.");
			SetTimerEx("RP_DelayLogin", 1000, false, "i", playerid);
		}
		return 1;
	}

	switch (dialogid) {
		case DIALOG_UCP_LOGIN:          ProcessUCPInput(playerid, inputtext);
		case DIALOG_LOGIN:              ProcessUCPLogin(playerid, inputtext);
		case DIALOG_UCP_REG_CODE:       ProcessUCPRegisterCode(playerid, inputtext);
		case DIALOG_UCP_REG_PASS:       ProcessUCPRegisterPass(playerid, inputtext);
		case DIALOG_CHAR_SELECT:        ProcessCharSelect(playerid, listitem);
		case DIALOG_CHAR_CREATE_NAME:   ProcessCharCreateName(playerid, inputtext);
		case DIALOG_CHAR_CREATE_DOB:    ProcessCharCreateDOB(playerid, inputtext);
		case DIALOG_CHAR_CREATE_GENDER: ProcessCharCreateGender(playerid, listitem);
		case DIALOG_INVENTORY: {
			gSelectedInvSlot[playerid] = listitem;
			ShowPlayerDialog(playerid, DIALOG_ITEM_ACTION, DIALOG_STYLE_LIST,
				"TINDAKAN BARANG", "1. Gunakan Barang\n2. Buang Barang", "Pilih", "Kembali");
		}
		case DIALOG_ITEM_ACTION: {
			if (listitem == 0) {
				UsePlayerItemBySlot(playerid, gSelectedInvSlot[playerid]);
			} else if (listitem == 1) {
				new slot = gSelectedInvSlot[playerid];
				TakePlayerItem(playerid, gPlayerItems[playerid][slot][itType], 1);
				SendInfo(playerid, "Barang telah dibuang dari kantong.");
			}
		}
		case DIALOG_GIVE_PLAYER: {
			new targetid, amount;
			if (!sscanf(inputtext, "ud", targetid, amount)) {
				new slot = gSelectedInvSlot[playerid];
				new itemType = gPlayerItems[playerid][slot][itType];
				if (itemType != ITEM_NONE) {
					ProcessGiveItem(playerid, targetid, itemType, amount);
				}
			} else {
				SendSyntax(playerid, "[ID/Nama Pemain] [Jumlah]");
			}
		}
		case DIALOG_STORE_BUY:         ProcessStorePurchase(playerid, listitem);
		case DIALOG_PHONE_MENU: {
			if (listitem == 0) ShowPlayerDialog(playerid, DIALOG_PHONE_CALL, DIALOG_STYLE_INPUT, "PANGGILAN", "Nomor tujuan:", "Panggil", "Batal");
			else if (listitem == 1) ShowPlayerDialog(playerid, DIALOG_PHONE_SMS, DIALOG_STYLE_INPUT, "SMS", "Format: [Nomor] [Pesan]", "Kirim", "Batal");
			else if (listitem == 2) {
				gPlayerData[playerid][pPhoneStatus] = !gPlayerData[playerid][pPhoneStatus];
				SendInfo(playerid, "Mode dering: %s.", gPlayerData[playerid][pPhoneStatus] ? "Aktif" : "Hening");
			}
		}
		case DIALOG_PHONE_CALL:        ProcessCall(playerid, inputtext);
		case DIALOG_PHONE_SMS: {
			new num, msg[128];
			if (!sscanf(inputtext, "ds[128]", num, msg)) ProcessSMS(playerid, num, msg);
			else SendSyntax(playerid, "[Nomor] [Pesan]");
		}
		case DIALOG_RADIO_FREQ: {
			new f = strval(inputtext);
			if (f >= 1 && f <= 999) {
				gPlayerData[playerid][pRadioFreq] = f;
				SendSucc(playerid, "Frekuensi HT disetel ke %d MHz.", f);
			} else SendErr(playerid, "Frekuensi 1 - 999 MHz.");
		}
		case DIALOG_GPS_MENU: {
			switch (listitem) {
				case 0: { SetPlayerCheckpoint(playerid, BANK_X, BANK_Y, BANK_Z, 6.0); SendSucc(playerid, "GPS: Bank Nusantara."); }
				case 1: { SetPlayerCheckpoint(playerid, GOV_X, GOV_Y, GOV_Z, 6.0); SendSucc(playerid, "GPS: Balai Kota (SAGOV)."); }
				case 2: { SetPlayerCheckpoint(playerid, MEDIC_X, MEDIC_Y, MEDIC_Z, 6.0); SendSucc(playerid, "GPS: RS All Saints (SAMD)."); }
				case 3: { SetPlayerCheckpoint(playerid, POLICE_X, POLICE_Y, POLICE_Z, 6.0); SendSucc(playerid, "GPS: Kantor Polisi LSPD."); }
				case 4: { SetPlayerCheckpoint(playerid, COURIER_X, COURIER_Y, COURIER_Z, 6.0); SendSucc(playerid, "GPS: Pusat Pekerjaan Umum."); }
				case 5: { SetPlayerCheckpoint(playerid, FUEL_STATION_X, FUEL_STATION_Y, FUEL_STATION_Z, 6.0); SendSucc(playerid, "GPS: SPBU."); }
				case 6: { SetPlayerCheckpoint(playerid, STORE_IDLEWOOD_X, STORE_IDLEWOOD_Y, STORE_IDLEWOOD_Z, 6.0); SendSucc(playerid, "GPS: Toko 24/7 Idlewood."); }
				case 7: { SetPlayerCheckpoint(playerid, DEALER_X, DEALER_Y, DEALER_Z, 6.0); SendSucc(playerid, "GPS: Dealership Los Santos."); }
				case 8: { SetPlayerCheckpoint(playerid, FISH_PIER_X, FISH_PIER_Y, FISH_PIER_Z, 6.0); SendSucc(playerid, "GPS: Dermaga Santa Maria (Mancing)."); }
				case 9: { SetPlayerCheckpoint(playerid, LUMBER_X, LUMBER_Y, LUMBER_Z, 6.0); SendSucc(playerid, "GPS: Hutan Shady Creeks (Tebang Kayu)."); }
				case 10: { DisablePlayerCheckpoint(playerid); SendInfo(playerid, "Tanda navigasi dimatikan."); }
			}
		}
		case DIALOG_BANK_MENU:         OnBankDialogResponse(playerid, listitem);
		case DIALOG_BANK_DEPOSIT:      OnBankDepositResponse(playerid, inputtext);
		case DIALOG_BANK_WITHDRAW:     OnBankWithdrawResponse(playerid, inputtext);
		case DIALOG_BANK_TRANSFER:     OnBankTransferResponse(playerid, inputtext);
		case DIALOG_ATM_MENU:          OnATMMenuResponse(playerid, listitem);
		case DIALOG_ATM_WITHDRAW:      OnATMWithdrawResponse(playerid, inputtext);
		case DIALOG_ATM_DEPOSIT:       OnATMDepositResponse(playerid, inputtext);
		case DIALOG_JOB_MENU:          OnJobMenuResponse(playerid, listitem);
		case DIALOG_DEALERSHIP:        OnDealershipResponse(playerid, listitem);
		case DIALOG_MYVEH:             ShowVehicleActionDialog(playerid, listitem);
		case DIALOG_MYVEH_ACTION:      OnMyVehActionResponse(playerid, listitem);
	}
	return 1;
}

// checkpoint enter
public OnPlayerEnterCheckpoint(playerid)
{
	if (gJobActive[playerid]) OnJobCheckpoint(playerid);
	return 1;
}

// game loops
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
		if (IsPlayerConnected(i) && gPlayerData[i][pLoggedIn]) DB_SavePlayerData(i);
	}
	for (new i = 0; i < MAX_OWNED_VEHICLES; i++) {
		if (gVehicleData[i][vUsed]) DB_SaveVehicle(i);
	}
	return 1;
}
