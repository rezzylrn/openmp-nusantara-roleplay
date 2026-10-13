#if defined _INC_rp_gamemode_events
	#endinput
#endif
#define _INC_rp_gamemode_events

/*
==============================================================================
 gamemode_events.inc
 NUSANTARA ROLEPLAY - Core SA-MP / open.mp Event Callbacks & Timers
==============================================================================
*/

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
		if (gPlayerData[playerid][pCallState] != 0) ProcessHangup(playerid);
		if (gJobActive[playerid]) ProcessJobQuit(playerid);
		ProcessRentalDisconnect(playerid);
		SavePlayerOnDisconnect(playerid);
	}
	TextdrawDestroyAll(playerid);
	return 1;
}

public OnPlayerRequestClass(playerid, classid)
{
	if (gPlayerData[playerid][pLoggedIn]) {
		new Float:sx = gPlayerData[playerid][pLastX];
		new Float:sy = gPlayerData[playerid][pLastY];
		new Float:sz = gPlayerData[playerid][pLastZ];
		new Float:sa = gPlayerData[playerid][pLastA];
		if (sx == 0.0 && sy == 0.0 && sz == 0.0) {
			sx = SPAWN_X; sy = SPAWN_Y; sz = SPAWN_Z; sa = SPAWN_A;
		}

		SetSpawnInfo(playerid, NO_TEAM, gPlayerData[playerid][pSkin], sx, sy, sz, sa);
		SpawnPlayer(playerid);
		return 0;
	}
	return 0;
}

public OnPlayerRequestSpawn(playerid)
{
	if (gPlayerData[playerid][pLoggedIn]) return 1;
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
	if (gPlayerData[playerid][pInjured]) {
		SendErr(playerid, "Anda sedang pingsan tidak sadarkan diri dan tidak dapat berbicara.");
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
	// Jika karakter pingsan atau terborgol, nonaktifkan seluruh interaksi tombol
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return 1;

	// Key F / ENTER (KEY_SECONDARY_ATTACK) untuk masuk / keluar pintu dinamis (Indoor/Outdoor)
	if ((newkeys & KEY_SECONDARY_ATTACK) && !(oldkeys & KEY_SECONDARY_ATTACK)) {
		if (GetPlayerState(playerid) == PLAYER_STATE_ONFOOT) {
			if (ProcessDoorEnter(playerid)) return 1;
			if (ProcessDoorExit(playerid)) return 1;
		}
	}

	// Key Y (KEY_YES) untuk interaksi properti rumah, ATM, tong sampah, pekerjaan, dan fasilitas umum
	if ((newkeys & KEY_YES) && !(oldkeys & KEY_YES)) {
		if (ProcessHouseInteraction(playerid)) return 1;
		if (IsPlayerNearAnyATM(playerid)) {
			ShowATMDialog(playerid);
			return 1;
		}
		if (IsPlayerNearAnyTrash(playerid)) {
			ProcessPlayerScavenge(playerid);
			return 1;
		}
		if (IsPlayerNearVendingMachine(playerid) != -1) {
			ShowVendingDialog(playerid);
			return 1;
		}
		ProcessKeyActivity(playerid);
		return 1;
	}

	// Key N (KEY_NO) atau Key H (KEY_CTRL_BACK) untuk Pusat Aksi & Interaksi Cepat (Mobile / Desktop Hub)
	if (((newkeys & KEY_NO) && !(oldkeys & KEY_NO)) || ((newkeys & KEY_CTRL_BACK) && !(oldkeys & KEY_CTRL_BACK))) {
		ShowPlayerActionHub(playerid);
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
	Forensics_RecordShot(playerid, weaponid);
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

public OnPlayerEnterCheckpoint(playerid)
{
	if (gJobActive[playerid]) {
		OnJobCheckpoint(playerid);
		return 1;
	}
	if (gPlayerSmuggleActive[playerid]) {
		ProcessSmuggleDelivery(playerid);
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
		ProcessTrafficCameraTick(i);
		UpdatePlayerZoneHUD(i);
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
