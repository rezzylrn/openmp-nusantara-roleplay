#if defined _INC_rp_anticheat_suite
	#endinput
#endif
#define _INC_rp_anticheat_suite

// ---------------------------------------------------------------------------
// Server-Side Anti-Cheat Master Controller (open.mp)
// ---------------------------------------------------------------------------

enum E_AC_DATA {
	bool:acHasWeapon[47],
	Float:acLastX,
	Float:acLastY,
	Float:acLastZ,
	acLastPosTick,
	Float:acLastHealth,
	Float:acLastArmour,
	acLastShotTick,
	acWarnings,
	bool:acIgnoreTeleport,
	acIgnoreTick
};

new gACData[MAX_PLAYERS][E_AC_DATA];

stock AC_AlertAdmins(playerid, const cheatName[], const details[])
{
	new msg[144];
	format(msg, sizeof(msg), "[ANTICHEAT] {E74C3C}%s (ID %d) {FFFFFF}terdeteksi {F1C40F}%s {FFFFFF}(%s)!",
		gPlayerData[playerid][pName], playerid, cheatName, details);

	for (new i = 0; i < MAX_PLAYERS; i++) {
		if (IsPlayerConnected(i) && gPlayerData[i][pAdmin] > 0) {
			SendClientMessage(i, COLOR_RED, msg);
		}
	}
	printf("[ANTICHEAT LOG] %s (ID %d) flagged for %s [%s]", gPlayerData[playerid][pName], playerid, cheatName, details);
	return 1;
}

stock AC_ResetPlayer(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;
	for (new w = 0; w < 47; w++) {
		gACData[playerid][acHasWeapon][w] = false;
	}
	gACData[playerid][acLastX] = 0.0;
	gACData[playerid][acLastY] = 0.0;
	gACData[playerid][acLastZ] = 0.0;
	gACData[playerid][acLastPosTick] = gettime();
	gACData[playerid][acLastHealth] = 100.0;
	gACData[playerid][acLastArmour] = 0.0;
	gACData[playerid][acLastShotTick] = 0;
	gACData[playerid][acWarnings] = 0;
	gACData[playerid][acIgnoreTeleport] = true;
	gACData[playerid][acIgnoreTick] = gettime() + 4;
	return 1;
}

stock AC_GiveWeapon(playerid, weaponid, ammo)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || weaponid < 0 || weaponid >= 47) return 0;
	gACData[playerid][acHasWeapon][weaponid] = true;
	if (IsPlayerConnected(playerid)) {
		GivePlayerWeapon(playerid, WEAPON:weaponid, ammo);
	}
	return 1;
}

stock AC_ResetWeapons(playerid)
{
	if (!IsPlayerConnected(playerid)) return 0;
	for (new w = 0; w < 47; w++) {
		gACData[playerid][acHasWeapon][w] = false;
	}
	ResetPlayerWeapons(playerid);
	return 1;
}

stock AC_SetPlayerPos(playerid, Float:x, Float:y, Float:z)
{
	if (!IsPlayerConnected(playerid)) return 0;
	gACData[playerid][acIgnoreTeleport] = true;
	gACData[playerid][acIgnoreTick] = gettime() + 3;
	gACData[playerid][acLastX] = x;
	gACData[playerid][acLastY] = y;
	gACData[playerid][acLastZ] = z;
	SetPlayerPos(playerid, x, y, z);
	return 1;
}

// Sinkronisasi otoritas saldo uang server (Anti-Cheat Engine / S0beit)
stock AC_SyncMoney(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;
	new clientMoney = GetPlayerMoney(playerid);
	if (clientMoney != gPlayerData[playerid][pMoney]) {
		ResetPlayerMoney(playerid);
		GivePlayerMoney(playerid, gPlayerData[playerid][pMoney]);
	}
	return 1;
}

// Submodul deteksi spesifik
#include "ac_weapons.inc"
#include "ac_movement.inc"
#include "ac_vehicles.inc"

stock AC_Init()
{
	print("[ANTICHEAT] Server-Side Anti-Cheat Engine Loaded:");
	print("  - ac_weapons  : Anti-Weapon Spawner, Rapid Fire, Silent-Aim Range");
	print("  - ac_movement : Anti-Airbreak, Teleport, Noclip, Jetpack");
	print("  - ac_vehicles : Anti-Car Slapper, Crasher, Speedhack, Remote Warp");
	return 1;
}

stock AC_Tick(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;
	AC_CheckWeaponIntegrity(playerid);
	AC_CheckMovement(playerid);
	AC_CheckVehicleSpeed(playerid);
	AC_SyncMoney(playerid);
	return 1;
}
