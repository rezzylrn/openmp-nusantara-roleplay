#if defined _INC_rp_anticheat
	#endinput
#endif
#define _INC_rp_anticheat

// ---------------------------------------------------------------------------
// Server-Side Anti-Cheat Engine (open.mp)
// ---------------------------------------------------------------------------

#define AC_MAX_SPEED_ONFOOT     16.0  // unit/s max sprint
#define AC_MAX_TELEPORT_DIST    75.0  // delta distance per second
#define AC_RAPID_FIRE_MS        80    // minimum shot delay (ms)

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

stock AC_Init()
{
	print("[ANTICHEAT] Server-Side Anti-Cheat Engine aktif (Weapons, Teleport, Speed, Money, Rapid-Fire).");
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

// deteksi senjata ilegal
stock AC_CheckWeaponHack(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;
	new WEAPON:weapon = GetPlayerWeapon(playerid);
	new wId = _:weapon;

	if (wId > 0 && wId < 47) {
		// Senjata fist / brass knuckle dilewati
		if (wId == 0 || wId == 1) return 1;

		if (!gACData[playerid][acHasWeapon][wId]) {
			// Strip illegal weapon
			ResetPlayerWeapons(playerid);
			AC_AlertAdmins(playerid, "Weapon Hack", "Membawa senjata tanpa izin server");

			gACData[playerid][acWarnings]++;
			if (gACData[playerid][acWarnings] >= 3) {
				SendErr(playerid, "Terdeteksi modifikasi senjata ilegal. Anda dikeluarkan dari server.");
				Kick(playerid);
			}
			return 0;
		}
	}
	return 1;
}

// deteksi airbreak / teleport
stock AC_CheckMovementHack(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn] || !gPlayerData[playerid][pSpawned]) return 0;

	if (gACData[playerid][acIgnoreTeleport]) {
		if (gettime() >= gACData[playerid][acIgnoreTick]) {
			gACData[playerid][acIgnoreTeleport] = false;
		} else {
			return 1;
		}
	}

	new Float:px, Float:py, Float:pz;
	GetPlayerPos(playerid, px, py, pz);

	// Jangan cek jika pemain sedang di udara / skydiving
	if (pz > 900.0 && GetPlayerInterior(playerid) == 0) return 1;

	new Float:dx = px - gACData[playerid][acLastX];
	new Float:dy = py - gACData[playerid][acLastY];
	new Float:dz = pz - gACData[playerid][acLastZ];
	new Float:dist = floatsqroot(dx*dx + dy*dy + dz*dz);

	new now = gettime();
	new deltaSec = now - gACData[playerid][acLastPosTick];
	if (deltaSec < 1) deltaSec = 1;

	new Float:speedPerSec = dist / float(deltaSec);

	if (GetPlayerState(playerid) == PLAYER_STATE_ONFOOT) {
		if (speedPerSec > AC_MAX_TELEPORT_DIST) {
			new detail[48];
			format(detail, sizeof(detail), "Jarak: %.1fm / %ddtk", dist, deltaSec);
			AC_AlertAdmins(playerid, "Airbreak / Teleport Hack", detail);

			// Kembalikan ke posisi terakhir yang sah
			SetPlayerPos(playerid, gACData[playerid][acLastX], gACData[playerid][acLastY], gACData[playerid][acLastZ]);
			return 0;
		}
	} else if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER) {
		new veh = GetPlayerVehicleID(playerid);
		if (veh > 0) {
			new Float:vx, Float:vy, Float:vz;
			GetVehicleVelocity(veh, vx, vy, vz);
			new Float:vehSpeed = floatsqroot(vx*vx + vy*vy + vz*vz) * 180.0;

			// Kecepatan abnormal kendaraan (> 230 KM/J untuk kendaraan darat biasa)
			if (vehSpeed > 240.0) {
				new model = GetVehicleModel(veh);
				// Kecualikan pesawat jet Hydra / Shamal
				if (model != 520 && model != 519 && model != 511) {
					SetVehicleVelocity(veh, vx * 0.5, vy * 0.5, vz * 0.5);
					AC_AlertAdmins(playerid, "Vehicle Speed Hack", "Kecepatan melebihi batas fisik");
				}
			}
		}
	}

	gACData[playerid][acLastX] = px;
	gACData[playerid][acLastY] = py;
	gACData[playerid][acLastZ] = pz;
	gACData[playerid][acLastPosTick] = now;
	return 1;
}

// sinkronisasi server-side uang (anti cheat-engine uang client)
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

// deteksi rapid-fire pada tembakan
stock AC_OnPlayerWeaponShot(playerid, WEAPON:weaponid)
{
	if (!IsPlayerConnected(playerid)) return 1;

	new nowMs = GetTickCount();
	new deltaMs = nowMs - gACData[playerid][acLastShotTick];
	gACData[playerid][acLastShotTick] = nowMs;

	// Cek interval minimum antar tembakan
	if (deltaMs > 0 && deltaMs < AC_RAPID_FIRE_MS) {
		new detail[48];
		format(detail, sizeof(detail), "Interval: %dms (Senjata ID %d)", deltaMs, _:weaponid);
		AC_AlertAdmins(playerid, "Rapid Fire Hack", detail);
		return 0; // Batalkan peluru
	}
	return 1;
}

// loop anticheat periodik (dipanggil dari RP_Tick)
stock AC_Tick(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;
	AC_CheckWeaponHack(playerid);
	AC_CheckMovementHack(playerid);
	AC_SyncMoney(playerid);
	return 1;
}
