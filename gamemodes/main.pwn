#include <open.mp>
#include <a_mysql>
#include <sscanf2>
#include <izcmd>
#include <samp_bcrypt>

// 1. Core modules
#include "core/header.inc"
#include "core/database.inc"
#include "core/player_auth.inc"

// 2. Anti-Cheat Engine (Multi-Vector Security)
#include "anticheat/anticheat.inc"

// 3. User Interface (Full Clickable TextDraws)
#include "ui/textdraws.inc"

// 4. World Systems
#include "systems/economy.inc"
#include "systems/inventory.inc"
#include "systems/properties.inc"
#include "systems/vehicles.inc"
#include "systems/shops.inc"
#include "systems/workshop.inc"

// 5. Gameplay & Factions
#include "gameplay/factions.inc"
#include "gameplay/jobs.inc"
#include "gameplay/police_mdc.inc"
#include "gameplay/phone.inc"
#include "gameplay/player_stats.inc"

// 6. Commands & Admin
#include "commands/commands_rp.inc"
#include "commands/admin.inc"

// 7. Test Suite
#include "tests/debug_tests.inc"

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
	AC_Init();
	House_Init();

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
	AC_ResetPlayer(playerid);

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

	// spectating mode: kamera melayang, block class selection screen
	TogglePlayerSpectating(playerid, true);
	SetPlayerColor(playerid, COLOR_GREY);

	// auto-detect UCP dari nama samp player
	DB_CheckUCPByPlayerName(playerid);
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
	// block tombol spawn & class selection screen
	return 0;
}

public OnPlayerRequestSpawn(playerid)
{
	// block tombol spawn (hanya bisa via dialog character selection)
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	return 0;
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
	// anti-cheat validation (Fake Kill & Death Flooder)
	if (!AC_ValidateDeath(playerid, killerid, reason)) return 0;

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
	// Key Y (KEY_YES) untuk interaksi properti rumah, pekerjaan, dan fasilitas umum
	if ((newkeys & KEY_YES) && !(oldkeys & KEY_YES)) {
		if (ProcessHouseInteraction(playerid)) return 1;
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

// weapon anti-cheat hook (Rapid Fire, No-Reload, Crasher Bullets, Silent Aim)
public OnPlayerWeaponShot(playerid, WEAPON:weaponid, BULLET_HIT_TYPE:hittype, hitid, Float:fX, Float:fY, Float:fZ)
{
	if (!AC_ValidateShot(playerid, weaponid, hittype, hitid, fX, fY, fZ)) return 0;
	if (hittype == BULLET_HIT_TYPE_PLAYER) {
		if (!AC_ValidateBulletHit(playerid, weaponid, hitid, fX, fY, fZ)) return 0;
	}
	return 1;
}

// damage anti-cheat hook (Silent Aim & Range validation)
public OnPlayerGiveDamage(playerid, damagedid, Float:amount, WEAPON:weaponid, bodypart)
{
	if (!AC_ValidateDamage(playerid, damagedid, amount, weaponid, bodypart)) return 0;
	return 1;
}

// unoccupied vehicle anti-cheat hook (Anti-Car Slapper & Crasher)
public OnUnoccupiedVehicleUpdate(vehicleid, playerid, passenger_seat, Float:new_x, Float:new_y, Float:new_z, Float:vel_x, Float:vel_y, Float:vel_z)
{
	#pragma unused passenger_seat, vel_x, vel_y, vel_z
	if (!AC_OnUnoccupiedVehicleUpdate(vehicleid, playerid, new_x, new_y, new_z)) return 0;
	return 1;
}

// pickup anti-cheat hook (Anti-Pickup Teleport)
public OnPlayerPickUpPickup(playerid, pickupid)
{
	if (!AC_ValidatePickup(playerid, pickupid)) return 0;
	return 1;
}

// vehicle mod anti-cheat hook (Anti-Vehicle Crasher / Invalid Component Tuning)
public OnVehicleMod(playerid, vehicleid, componentid)
{
	if (!AC_ValidateVehicleMod(playerid, vehicleid, componentid)) {
		RemoveVehicleComponent(vehicleid, componentid);
		return 0;
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

// speedometer toggle & Car Jacker anti-cheat hook
public OnPlayerStateChange(playerid, PLAYER_STATE:newstate, PLAYER_STATE:oldstate)
{
	if (oldstate == PLAYER_STATE_ONFOOT && newstate == PLAYER_STATE_DRIVER) {
		if (!AC_ValidateVehicleEntry(playerid, GetPlayerVehicleID(playerid))) {
			return 1;
		}
		ShowPlayerSpeedo(playerid);
	}
	else if (oldstate == PLAYER_STATE_DRIVER && newstate == PLAYER_STATE_ONFOOT) {
		HidePlayerSpeedo(playerid);
	}
	return 1;
}

// admin click map teleport
public OnPlayerClickMap(playerid, Float:fX, Float:fY, Float:fZ)
{
	if (!IsPlayerConnected(playerid)) return 1;
	if (gPlayerData[playerid][pAdmin] >= 1) {
		AC_SetPlayerPos(playerid, fX, fY, fZ + 3.0);
		SetPlayerPosFindZ(playerid, fX, fY, fZ);
		SendInfo(playerid, "Teleport ke koordinat peta (%.1f, %.1f, %.1f).", fX, fY, fZ);
		return 1;
	}
	return 1;
}

// dialog responses
public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	if (!response) {
		if (dialogid == DIALOG_LOGIN || dialogid == DIALOG_UCP_OTP_VERIFY || dialogid == DIALOG_CHAR_SELECT) {
			SendClientMessage(playerid, COLOR_ERROR, "[NUSANTARA RP] Sesi login dibatalkan.");
			Kick(playerid);
		}
		return 1;
	}

	switch (dialogid) {
		case DIALOG_LOGIN:              ProcessUCPLogin(playerid, inputtext);
		case DIALOG_UCP_OTP_VERIFY:     ProcessUCPOtpVerify(playerid, inputtext);
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
				case 0:  { SetPlayerCheckpoint(playerid, BANK_X, BANK_Y, BANK_Z, 6.0); SendSucc(playerid, "GPS: Rute disetel ke Bank Nusantara (%.0fm).", GetPlayerDistanceFromPoint(playerid, BANK_X, BANK_Y, BANK_Z)); }
				case 1:  { SetPlayerCheckpoint(playerid, POLICE_X, POLICE_Y, POLICE_Z, 6.0); SendSucc(playerid, "GPS: Rute disetel ke Kantor Polisi LSPD (%.0fm).", GetPlayerDistanceFromPoint(playerid, POLICE_X, POLICE_Y, POLICE_Z)); }
				case 2:  { SetPlayerCheckpoint(playerid, MEDIC_X, MEDIC_Y, MEDIC_Z, 6.0); SendSucc(playerid, "GPS: Rute disetel ke RS All Saints ASGH (%.0fm).", GetPlayerDistanceFromPoint(playerid, MEDIC_X, MEDIC_Y, MEDIC_Z)); }
				case 3:  { SetPlayerCheckpoint(playerid, GOV_X, GOV_Y, GOV_Z, 6.0); SendSucc(playerid, "GPS: Rute disetel ke Balai Kota SAGOV (%.0fm).", GetPlayerDistanceFromPoint(playerid, GOV_X, GOV_Y, GOV_Z)); }
				case 4:  { SetPlayerCheckpoint(playerid, FUEL_STATION_X, FUEL_STATION_Y, FUEL_STATION_Z, 6.0); SendSucc(playerid, "GPS: Rute disetel ke SPBU Bahan Bakar Idlewood (%.0fm).", GetPlayerDistanceFromPoint(playerid, FUEL_STATION_X, FUEL_STATION_Y, FUEL_STATION_Z)); }
				case 5:  { SetPlayerCheckpoint(playerid, STORE_IDLEWOOD_X, STORE_IDLEWOOD_Y, STORE_IDLEWOOD_Z, 6.0); SendSucc(playerid, "GPS: Rute disetel ke Toko 24/7 Idlewood (%.0fm).", GetPlayerDistanceFromPoint(playerid, STORE_IDLEWOOD_X, STORE_IDLEWOOD_Y, STORE_IDLEWOOD_Z)); }
				case 6:  { SetPlayerCheckpoint(playerid, WORKSHOP_X, WORKSHOP_Y, WORKSHOP_Z, 6.0); SendSucc(playerid, "GPS: Rute disetel ke Bengkel Modifikasi Mekanik (%.0fm).", GetPlayerDistanceFromPoint(playerid, WORKSHOP_X, WORKSHOP_Y, WORKSHOP_Z)); }
				case 7:  { SetPlayerCheckpoint(playerid, DEALER_X, DEALER_Y, DEALER_Z, 6.0); SendSucc(playerid, "GPS: Rute disetel ke Showroom Dealership (%.0fm).", GetPlayerDistanceFromPoint(playerid, DEALER_X, DEALER_Y, DEALER_Z)); }
				case 8:  { SetPlayerCheckpoint(playerid, COURIER_X, COURIER_Y, COURIER_Z, 6.0); SendSucc(playerid, "GPS: Rute disetel ke Job Kurir Paket (%.0fm).", GetPlayerDistanceFromPoint(playerid, COURIER_X, COURIER_Y, COURIER_Z)); }
				case 9:  { SetPlayerCheckpoint(playerid, SWEEPER_X, SWEEPER_Y, SWEEPER_Z, 6.0); SendSucc(playerid, "GPS: Rute disetel ke Job Pembersih Jalan Sweeper (%.0fm).", GetPlayerDistanceFromPoint(playerid, SWEEPER_X, SWEEPER_Y, SWEEPER_Z)); }
				case 10: { SetPlayerCheckpoint(playerid, TRUCKER_X, TRUCKER_Y, TRUCKER_Z, 6.0); SendSucc(playerid, "GPS: Rute disetel ke Job Truk Hauler (%.0fm).", GetPlayerDistanceFromPoint(playerid, TRUCKER_X, TRUCKER_Y, TRUCKER_Z)); }
				case 11: { SetPlayerCheckpoint(playerid, BUS_DEPOT_X, BUS_DEPOT_Y, BUS_DEPOT_Z, 6.0); SendSucc(playerid, "GPS: Rute disetel ke Job Terminal Bus Umum (%.0fm).", GetPlayerDistanceFromPoint(playerid, BUS_DEPOT_X, BUS_DEPOT_Y, BUS_DEPOT_Z)); }
				case 12: { SetPlayerCheckpoint(playerid, FISH_PIER_X, FISH_PIER_Y, FISH_PIER_Z, 6.0); SendSucc(playerid, "GPS: Rute disetel ke Dermaga Mancing Santa Maria (%.0fm).", GetPlayerDistanceFromPoint(playerid, FISH_PIER_X, FISH_PIER_Y, FISH_PIER_Z)); }
				case 13: { SetPlayerCheckpoint(playerid, LUMBER_X, LUMBER_Y, LUMBER_Z, 6.0); SendSucc(playerid, "GPS: Rute disetel ke Hutan Penebangan Kayu (%.0fm).", GetPlayerDistanceFromPoint(playerid, LUMBER_X, LUMBER_Y, LUMBER_Z)); }
				case 14: { SetPlayerCheckpoint(playerid, SAWMILL_X, SAWMILL_Y, SAWMILL_Z, 6.0); SendSucc(playerid, "GPS: Rute disetel ke Pabrik Pengolahan Kayu Sawmill (%.0fm).", GetPlayerDistanceFromPoint(playerid, SAWMILL_X, SAWMILL_Y, SAWMILL_Z)); }
				case 15: { SetPlayerCheckpoint(playerid, TRASH_BUYER_X, TRASH_BUYER_Y, TRASH_BUYER_Z, 6.0); SendSucc(playerid, "GPS: Rute disetel ke Daur Ulang Sampah (%.0fm).", GetPlayerDistanceFromPoint(playerid, TRASH_BUYER_X, TRASH_BUYER_Y, TRASH_BUYER_Z)); }
				case 16: { DisablePlayerCheckpoint(playerid); SendInfo(playerid, "GPS: Tanda navigasi telah dimatikan."); }
			}
			PlayerPlaySound(playerid, 1083, 0.0, 0.0, 0.0);
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
	if (gJobActive[playerid]) {
		OnJobCheckpoint(playerid);
		return 1;
	}
	// Sampai di tujuan GPS navigasi
	DisablePlayerCheckpoint(playerid);
	SendSucc(playerid, "GPS: Anda telah sampai di lokasi tujuan navigasi.");
	PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
	return 1;
}

// game loops
public RP_Tick()
{
	for (new i = 0; i < MAX_PLAYERS; i++) {
		if (!IsPlayerConnected(i) || !gPlayerData[i][pLoggedIn]) continue;
		UpdateHungerThirst(i);
		TickPaycheck(i);
		AC_Tick(i);
		ProcessPoliceRadarTick(i);
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
