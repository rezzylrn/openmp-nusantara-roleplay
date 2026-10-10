#if defined _INC_rp_ac_movement
	#endinput
#endif
#define _INC_rp_ac_movement

// ---------------------------------------------------------------------------
// Anti-Cheat: Airbreak, Teleport, Jetpack & Movement Speed
// ---------------------------------------------------------------------------

#define AC_MAX_TELEPORT_DIST    75.0

stock AC_CheckMovement(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn] || !gPlayerData[playerid][pSpawned]) return 1;

	// 1. Deteksi Jetpack Hack (s0beit / CLEO)
	if (GetPlayerSpecialAction(playerid) == SPECIAL_ACTION_USEJETPACK) {
		SetPlayerSpecialAction(playerid, SPECIAL_ACTION_NONE);
		AC_AlertAdmins(playerid, "Jetpack Hack", "Menggunakan jetpack tanpa otorisasi staf");
		return 0;
	}

	// 2. Abaikan jika sedang dalam masa transisi spawn / teleport server
	if (gACData[playerid][acIgnoreTeleport]) {
		if (gettime() >= gACData[playerid][acIgnoreTick]) {
			gACData[playerid][acIgnoreTeleport] = false;
		} else {
			return 1;
		}
	}

	new Float:px, Float:py, Float:pz;
	GetPlayerPos(playerid, px, py, pz);

	// Abaikan pemain yang sedang berada di langit / skydiving
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
		// Deteksi Airbreak / Noclip / Teleport
		if (speedPerSec > AC_MAX_TELEPORT_DIST) {
			new detail[48];
			format(detail, sizeof(detail), "Jarak: %.1fm / %ddtk", dist, deltaSec);
			AC_AlertAdmins(playerid, "Airbreak / Teleport Hack", detail);

			// Rubberband: Kembalikan pemain ke posisi sah terakhir
			SetPlayerPos(playerid, gACData[playerid][acLastX], gACData[playerid][acLastY], gACData[playerid][acLastZ]);
			return 0;
		}
	}

	gACData[playerid][acLastX] = px;
	gACData[playerid][acLastY] = py;
	gACData[playerid][acLastZ] = pz;
	gACData[playerid][acLastPosTick] = now;
	return 1;
}
