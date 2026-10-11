#if defined _INC_rp_anticheat_suite
	#endinput
#endif
#define _INC_rp_anticheat_suite

// ---------------------------------------------------------------------------
// Server-Side Anti-Cheat Master Controller (open.mp)
// Multi-layer counter-measure suite against open-source cheats:
// mod_sa (s0beit), UltraFuck, MoonLoader lua cheats, CLEO, and packet flooders
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
	acIgnoreTick,
	// Extended fields
	acShotsInClip,
	acLastWeaponFired,
	acLastDeathTick,
	bool:acIsDead,
	Float:acExpectedHealth,
	acGodmodeHits,
	Float:acPreEnterX,
	Float:acPreEnterY,
	Float:acPreEnterZ,
	acLastPickupTick,
	acFlyTicks
};

new gACData[MAX_PLAYERS][E_AC_DATA];

forward RP_DelayedKick(playerid);
public RP_DelayedKick(playerid)
{
	if (IsPlayerConnected(playerid)) Kick(playerid);
	return 1;
}

stock AC_AlertAdmins(playerid, const cheatName[], const details[])
{
	if (playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid)) return 0;

	new kickMsg[144];
	format(kickMsg, sizeof(kickMsg), "[SERVER] Anda dikeluarkan dari server karena terdeteksi menggunakan program ilegal (%s).", cheatName);
	SendClientMessage(playerid, COLOR_RED, kickMsg);

	new msg[144];
	format(msg, sizeof(msg), "[ANTICHEAT] {E74C3C}%s (ID %d) {FFFFFF}dikeluarkan karena {F1C40F}%s {FFFFFF}(%s).",
		gPlayerData[playerid][pName], playerid, cheatName, details);

	for (new i = 0; i < MAX_PLAYERS; i++) {
		if (IsPlayerConnected(i) && gPlayerData[i][pAdmin] > 0) {
			SendClientMessage(i, COLOR_RED, msg);
		}
	}
	printf("[ANTICHEAT KICK] %s (ID %d) kicked for %s [%s]", gPlayerData[playerid][pName], playerid, cheatName, details);
	SetTimerEx("RP_DelayedKick", 350, false, "i", playerid);
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

	gACData[playerid][acShotsInClip] = 0;
	gACData[playerid][acLastWeaponFired] = 0;
	gACData[playerid][acLastDeathTick] = 0;
	gACData[playerid][acIsDead] = false;
	gACData[playerid][acExpectedHealth] = 100.0;
	gACData[playerid][acGodmodeHits] = 0;
	gACData[playerid][acPreEnterX] = 0.0;
	gACData[playerid][acPreEnterY] = 0.0;
	gACData[playerid][acPreEnterZ] = 0.0;
	gACData[playerid][acLastPickupTick] = 0;
	gACData[playerid][acFlyTicks] = 0;
	return 1;
}

stock AC_OnPlayerSpawn(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;
	gACData[playerid][acIsDead] = false;
	gACData[playerid][acGodmodeHits] = 0;
	gACData[playerid][acIgnoreTeleport] = true;
	gACData[playerid][acIgnoreTick] = gettime() + 4;
	gACData[playerid][acShotsInClip] = 0;
	gACData[playerid][acFlyTicks] = 0;

	new Float:px, Float:py, Float:pz;
	GetPlayerPos(playerid, px, py, pz);
	gACData[playerid][acLastX] = px;
	gACData[playerid][acLastY] = py;
	gACData[playerid][acLastZ] = pz;
	gACData[playerid][acPreEnterX] = px;
	gACData[playerid][acPreEnterY] = py;
	gACData[playerid][acPreEnterZ] = pz;
	gACData[playerid][acLastPosTick] = gettime();
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
	gACData[playerid][acPreEnterX] = x;
	gACData[playerid][acPreEnterY] = y;
	gACData[playerid][acPreEnterZ] = z;
	SetPlayerPos(playerid, x, y, z);
	return 1;
}

stock AC_PutPlayerInVehicle(playerid, vehicleid, seatid)
{
	if (!IsPlayerConnected(playerid) || !IsValidVehicle(vehicleid)) return 0;
	gACData[playerid][acIgnoreTeleport] = true;
	gACData[playerid][acIgnoreTick] = gettime() + 3;
	new Float:vx, Float:vy, Float:vz;
	GetVehiclePos(vehicleid, vx, vy, vz);
	gACData[playerid][acLastX] = vx;
	gACData[playerid][acLastY] = vy;
	gACData[playerid][acLastZ] = vz;
	gACData[playerid][acPreEnterX] = vx;
	gACData[playerid][acPreEnterY] = vy;
	gACData[playerid][acPreEnterZ] = vz;
	PutPlayerInVehicle(playerid, vehicleid, seatid);
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
#include "ac_combat.inc"
#include "ac_troll.inc"

stock AC_Init()
{
	print("[ANTICHEAT] Server-Side Anti-Cheat Engine Loaded:");
	print("  - ac_weapons  : Anti-Weapon Spawner, Rapid Fire, No-Reload, Crasher Bullets");
	print("  - ac_movement : Anti-Airbreak, Teleport/Click Warp, Flyhack, Foot Speedhack");
	print("  - ac_vehicles : Anti-Car Jacker/Warp, Speedhack, Car Fly, Car Slapper/Crasher");
	print("  - ac_combat   : Anti-Silent Aim, FOV Camera Check, Fake Kill Flooder, Godmode");
	print("  - ac_troll    : Anti-Pickup Teleport, Invalid Vehicle Tuning Crasher");
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
