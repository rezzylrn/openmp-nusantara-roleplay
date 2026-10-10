#if defined _INC_rp_ac_weapons
	#endinput
#endif
#define _INC_rp_ac_weapons

// ---------------------------------------------------------------------------
// Anti-Cheat: Weapons, Fire-Rate, Ammo/Clip Capacity, and Bullet Integrity
// Countering: mod_sa (s0beit), UltraFuck No-Reload, Rapid-Fire, Crasher Bullets
// ---------------------------------------------------------------------------

stock AC_GetWeaponClipCapacity(weaponid)
{
	switch (weaponid) {
		case 22, 23:    return 17; // 9mm Pistol & Silenced
		case 24:        return 7;  // Desert Eagle
		case 25:        return 8;  // Pump Shotgun (tube)
		case 26:        return 2;  // Sawnoff Shotgun
		case 27:        return 7;  // Combat Shotgun (SPAS-12)
		case 28, 32:    return 50; // Micro Uzi / Tec-9
		case 29, 30:    return 30; // MP5 / AK-47
		case 31:        return 50; // M4
		case 33, 34:    return 1;  // Country Rifle & Sniper Rifle (single chamber)
		case 35, 36:    return 1;  // Rocket Launchers
	}
	return 100;
}

stock AC_GetMinFireInterval(weaponid)
{
	switch (weaponid) {
		case 22, 23:    return 130; // 9mm Pistol / Silenced (~160ms nominal)
		case 24:        return 600; // Desert Eagle (~800ms nominal)
		case 25:        return 800; // Pump Shotgun (~1000ms nominal)
		case 26:        return 120; // Sawnoff
		case 27:        return 180; // Combat Shotgun
		case 28, 32:    return 75;  // Uzi / Tec9
		case 29:        return 80;  // MP5
		case 30, 31:    return 80;  // AK47 / M4
		case 33, 34:    return 800; // Sniper / Country Rifle
	}
	return 70;
}

stock Float:AC_GetMaxWeaponRange(weaponid)
{
	switch (weaponid) {
		case 0..15:     return 4.0;   // Melee
		case 22..24:    return 35.0;  // Pistols (9mm, Silenced, Deagle)
		case 25..27:    return 40.0;  // Shotguns
		case 28, 29, 32:return 45.0;  // SMG, MP5, Tec9
		case 30, 31:    return 90.0;  // AK47, M4
		case 33, 34:    return 105.0; // Country Rifle, Sniper
		default:        return 50.0;
	}
	return 50.0;
}

// Validasi kepemilikan senjata (Anti-Weapon Spawner / s0beit weapon hack)
stock AC_CheckWeaponIntegrity(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 1;

	new WEAPON:w = GetPlayerWeapon(playerid);
	new wId = _:w;

	if (wId > 1 && wId < 47) {
		if (!gACData[playerid][acHasWeapon][wId]) {
			ResetPlayerWeapons(playerid);
			AC_AlertAdmins(playerid, "Weapon Spawner Hack", "Senjata ilegal di luar izin server");

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

// Validasi tembakan (Rapid Fire, No-Reload / Infinite Clip, Invalid Bullet Packet)
stock AC_ValidateShot(playerid, WEAPON:weaponid, BULLET_HIT_TYPE:hittype, hitid, Float:fX, Float:fY, Float:fZ)
{
	#pragma unused hittype, hitid
	if (!IsPlayerConnected(playerid)) return 1;
	new wId = _:weaponid;

	// 1. Tangkal eksploit bullet packet pada senjata non-peluru (Melee, Pisau, Granat, dsb.)
	if (wId < 22 || wId > 38) {
		new detail[48];
		format(detail, sizeof(detail), "Senjata non-peluru ID: %d", wId);
		AC_AlertAdmins(playerid, "Invalid Bullet Packet", detail);
		return 0;
	}

	// 2. Tangkal eksploit vektor koordinat rusak/NaN (Crasher bullets)
	if (fX > 10000.0 || fX < -10000.0 || fY > 10000.0 || fY < -10000.0 || fZ > 10000.0 || fZ < -10000.0) {
		AC_AlertAdmins(playerid, "Bullet Vector Crasher", "Koordinat peluru tidak valid / corrupt");
		return 0;
	}

	new nowMs = GetTickCount();
	new deltaMs = nowMs - gACData[playerid][acLastShotTick];
	gACData[playerid][acLastShotTick] = nowMs;

	// 3. Tangkal Rapid-Fire (tembakan terlalu cepat di luar rate of fire GTA)
	new minInterval = AC_GetMinFireInterval(wId);
	if (deltaMs > 0 && deltaMs < minInterval) {
		new detail[64];
		format(detail, sizeof(detail), "Interval: %dms (Min: %dms, Senjata ID: %d)", deltaMs, minInterval, wId);
		AC_AlertAdmins(playerid, "Rapid Fire Hack", detail);
		return 0;
	}

	// 4. Tangkal No-Reload / Infinite Clip Hack (menembak terus tanpa animasi reload)
	new maxClip = AC_GetWeaponClipCapacity(wId);
	if (wId != gACData[playerid][acLastWeaponFired]) {
		gACData[playerid][acLastWeaponFired] = wId;
		gACData[playerid][acShotsInClip] = 1;
	} else {
		// Jika jeda antar tembakan > 1250ms, asumsikan reload telah terjadi
		if (deltaMs > 1250) {
			gACData[playerid][acShotsInClip] = 1;
		} else {
			gACData[playerid][acShotsInClip]++;
			if (gACData[playerid][acShotsInClip] > maxClip + 2) {
				new detail[64];
				format(detail, sizeof(detail), "Tembakan tanpa reload: %d (Maks Clip: %d)", gACData[playerid][acShotsInClip], maxClip);
				AC_AlertAdmins(playerid, "No-Reload / Infinite Clip", detail);
				return 0;
			}
		}
	}

	return 1;
}
