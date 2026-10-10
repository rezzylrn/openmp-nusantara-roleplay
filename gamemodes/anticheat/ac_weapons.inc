#if defined _INC_rp_ac_weapons
	#endinput
#endif
#define _INC_rp_ac_weapons

// ---------------------------------------------------------------------------
// Anti-Cheat: Weapons, Ammo, Rapid-Fire & Silent-Aim Range Validator
// ---------------------------------------------------------------------------

#define AC_RAPID_FIRE_MS        80

stock Float:AC_GetMaxWeaponRange(weaponid)
{
	switch (weaponid) {
		case 0..15:     return 4.0;   // Melee
		case 22..24:    return 35.0;  // Pistols (9mm, Silenced, Deagle)
		case 25..27:    return 40.0;  // Shotguns
		case 28, 29, 32:return 45.0;  // SMG, MP5, Tec9
		case 30, 31:    return 90.0;  // AK47, M4
		case 33, 34:    return 100.0; // Country Rifle, Sniper
		default:        return 50.0;
	}
	return 50.0;
}

stock AC_CheckWeaponIntegrity(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 1;

	new WEAPON:w = GetPlayerWeapon(playerid);
	new wId = _:w;

	if (wId > 1 && wId < 47) {
		if (!gACData[playerid][acHasWeapon][wId]) {
			ResetPlayerWeapons(playerid);
			AC_AlertAdmins(playerid, "Weapon Spawner Hack", "Senjata di luar izin server");

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

stock AC_ValidateShot(playerid, WEAPON:weaponid)
{
	if (!IsPlayerConnected(playerid)) return 1;

	new nowMs = GetTickCount();
	new deltaMs = nowMs - gACData[playerid][acLastShotTick];
	gACData[playerid][acLastShotTick] = nowMs;

	// Deteksi rapid fire
	if (deltaMs > 0 && deltaMs < AC_RAPID_FIRE_MS) {
		new detail[48];
		format(detail, sizeof(detail), "Interval: %dms (Senjata ID %d)", deltaMs, _:weaponid);
		AC_AlertAdmins(playerid, "Rapid Fire Hack", detail);
		return 0;
	}
	return 1;
}

stock AC_ValidateDamage(playerid, damagedid, WEAPON:weaponid)
{
	if (!IsPlayerConnected(playerid) || !IsPlayerConnected(damagedid)) return 1;

	new Float:px, Float:py, Float:pz;
	new Float:tx, Float:ty, Float:tz;
	GetPlayerPos(playerid, px, py, pz);
	GetPlayerPos(damagedid, tx, ty, tz);

	new Float:dist = floatsqroot((px-tx)*(px-tx) + (py-ty)*(py-ty) + (pz-tz)*(pz-tz));
	new Float:maxRange = AC_GetMaxWeaponRange(_:weaponid);

	// Deteksi silent aim & damage reach melebihi jangkauan senjata
	if (dist > maxRange + 5.0) {
		new detail[64];
		format(detail, sizeof(detail), "Jarak tembak: %.1fm (Maks: %.1fm)", dist, maxRange);
		AC_AlertAdmins(playerid, "Silent Aim / Range Hack", detail);
		return 0; // Batalkan damage
	}
	return 1;
}
