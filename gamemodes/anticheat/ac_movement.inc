#if defined _INC_rp_ac_movement
	#endinput
#endif
#define _INC_rp_ac_movement

// ---------------------------------------------------------------------------
// Anti-Cheat: Airbreak, Teleport, Jetpack, Flyhack, and Foot Speedhack
// Countering: mod_sa Airbreak/Noclip, MoonLoader Flyhack, Click Warp, Fast Ped
// ---------------------------------------------------------------------------

#define AC_MAX_ONFOOT_SPEED     22.0  // Meter/detik (CJ sprint nominal ~9.5m/s, bunnyhop ~13m/s)
#define AC_MAX_TELEPORT_DIST    60.0  // Meter/detik lompatan seketika

stock AC_CheckMovement(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn] || !gPlayerData[playerid][pSpawned]) return 1;

	// 1. Deteksi Jetpack Hack (s0beit / CLEO)
	if (GetPlayerSpecialAction(playerid) == SPECIAL_ACTION_USEJETPACK) {
		if (gPlayerData[playerid][pAdmin] == 0) {
			SetPlayerSpecialAction(playerid, SPECIAL_ACTION_NONE);
			AC_AlertAdmins(playerid, "Jetpack Hack", "Menggunakan jetpack tanpa izin staf");
			return 0;
		}
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
	new Float:distHoriz = floatsqroot(dx*dx + dy*dy);
	new Float:distTotal = floatsqroot(dx*dx + dy*dy + dz*dz);

	new now = gettime();
	new deltaSec = now - gACData[playerid][acLastPosTick];
	if (deltaSec < 1) deltaSec = 1;

	new Float:speedHoriz = distHoriz / float(deltaSec);

	if (GetPlayerState(playerid) == PLAYER_STATE_ONFOOT) {
		// Simpan koordinat sebelum masuk kendaraan untuk deteksi Car Jacker
		gACData[playerid][acPreEnterX] = px;
		gACData[playerid][acPreEnterY] = py;
		gACData[playerid][acPreEnterZ] = pz;

		// 3. Deteksi Teleport / Click Warp Instan
		if (distTotal > AC_MAX_TELEPORT_DIST) {
			new detail[48];
			format(detail, sizeof(detail), "Lompatan jarak: %.1fm / %ddtk", distTotal, deltaSec);
			AC_AlertAdmins(playerid, "Teleport / Click Warp Hack", detail);

			// Rubberband: Kembalikan pemain ke posisi sah terakhir
			SetPlayerPos(playerid, gACData[playerid][acLastX], gACData[playerid][acLastY], gACData[playerid][acLastZ]);
			return 0;
		}

		// 4. Deteksi Airbreak / Speedhack On-Foot
		if (speedHoriz > AC_MAX_ONFOOT_SPEED) {
			new detail[48];
			format(detail, sizeof(detail), "Kecepatan jalan: %.1f m/s (Batas: 22m/s)", speedHoriz);
			AC_AlertAdmins(playerid, "Airbreak / Foot Speedhack", detail);

			SetPlayerPos(playerid, gACData[playerid][acLastX], gACData[playerid][acLastY], gACData[playerid][acLastZ]);
			return 0;
		}

		// 5. Deteksi Flyhack / Air Walk (Kenaikan elevasi Z konstan tanpa melompat/tangga)
		if (dz > 2.5 && pz > 5.0 && GetPlayerSurfingVehicleID(playerid) == INVALID_VEHICLE_ID && GetPlayerSurfingObjectID(playerid) == INVALID_OBJECT_ID) {
			gACData[playerid][acFlyTicks]++;
			if (gACData[playerid][acFlyTicks] >= 3) {
				new detail[48];
				format(detail, sizeof(detail), "Kenaikan elevasi: +%.1fm (Ticks: %d)", dz, gACData[playerid][acFlyTicks]);
				AC_AlertAdmins(playerid, "Flyhack / Air Walk Hack", detail);

				SetPlayerPos(playerid, gACData[playerid][acLastX], gACData[playerid][acLastY], gACData[playerid][acLastZ]);
				gACData[playerid][acFlyTicks] = 0;
				return 0;
			}
		} else {
			gACData[playerid][acFlyTicks] = 0;
		}
	}

	gACData[playerid][acLastX] = px;
	gACData[playerid][acLastY] = py;
	gACData[playerid][acLastZ] = pz;
	gACData[playerid][acLastPosTick] = now;
	return 1;
}
