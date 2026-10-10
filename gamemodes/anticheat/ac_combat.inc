#if defined _INC_rp_ac_combat
	#endinput
#endif
#define _INC_rp_ac_combat

// ---------------------------------------------------------------------------
// Anti-Cheat: Combat, Silent Aim, Camera FOV Vectors, Fake Kill, and Godmode
// Countering: UltraFuck Silent Aim, Cleo FakeKill, Damager, Godmode / Health-Lock
// ---------------------------------------------------------------------------

// Validasi tembakan berpeluru terhadap sasaran pemain (Silent Aim & Damager)
stock AC_ValidateBulletHit(playerid, WEAPON:weaponid, hitid, Float:fX, Float:fY, Float:fZ)
{
	if (!IsPlayerConnected(playerid) || !IsPlayerConnected(hitid)) return 1;

	// 1. Target pemain harus sudah terhubung, login, dan berstatus spawned
	if (!gPlayerData[hitid][pLoggedIn] || !gPlayerData[hitid][pSpawned]) {
		AC_AlertAdmins(playerid, "Damager / Ghost Target", "Menembak sasaran pemain yang belum spawn");
		return 0;
	}

	// 2. Stream Check: Client tidak mungkin melihat/menembak pemain di luar streaming radius
	if (!IsPlayerStreamedIn(hitid, playerid)) {
		new detail[64];
		format(detail, sizeof(detail), "Sasaran ID %d tidak berada dalam stream", hitid);
		AC_AlertAdmins(playerid, "Silent Aim / Out-of-Stream Damager", detail);
		return 0;
	}

	// 3. Jarak jangkauan peluru (Range check)
	new Float:px, Float:py, Float:pz;
	new Float:tx, Float:ty, Float:tz;
	GetPlayerPos(playerid, px, py, pz);
	GetPlayerPos(hitid, tx, ty, tz);

	new Float:dist = floatsqroot((px-tx)*(px-tx) + (py-ty)*(py-ty) + (pz-tz)*(pz-tz));
	new Float:maxRange = AC_GetMaxWeaponRange(_:weaponid);

	if (dist > maxRange + 5.0) {
		new detail[64];
		format(detail, sizeof(detail), "Jarak tembak: %.1fm (Maks: %.1fm)", dist, maxRange);
		AC_AlertAdmins(playerid, "Silent Aim / Range Hack", detail);
		return 0;
	}

	// 4. Hit Offset Bounding Box Check (fX, fY, fZ adalah offset dari pusat model sasaran)
	new Float:offsetDist = floatsqroot(fX*fX + fY*fY + fZ*fZ);
	if (offsetDist > 3.8) {
		new detail[64];
		format(detail, sizeof(detail), "Offset model: %.2fm (Di luar hitbox)", offsetDist);
		AC_AlertAdmins(playerid, "Silent Aim / Offset Exploit", detail);
		return 0;
	}

	// 5. Camera Front Vector FOV Check (Mencegah tembakan ke belakang / sudut mustahil)
	new Float:camX, Float:camY, Float:camZ;
	if (GetPlayerCameraFrontVector(playerid, camX, camY, camZ) && dist > 2.5) {
		new Float:dirX = (tx - px) / dist;
		new Float:dirY = (ty - py) / dist;
		new Float:dirZ = (tz - pz) / dist;

		// Dot product antara arah kamera dengan arah ke sasaran
		new Float:dot = (camX * dirX) + (camY * dirY) + (camZ * dirZ);

		// dot < 0.1 berarti sudut kemiringan > 84 derajat (di luar bidang pandang crosshair)
		if (dot < 0.1) {
			new detail[64];
			format(detail, sizeof(detail), "Sudut pandang dot: %.2f (Di luar FOV)", dot);
			AC_AlertAdmins(playerid, "Silent Aim / Field-of-View Bypass", detail);
			return 0;
		}
	}

	return 1;
}

// Validasi paket kematian (Anti-Fake Kill / Death Flooder / mod_sa death spam)
stock AC_ValidateDeath(playerid, killerid, WEAPON:reason)
{
	if (!IsPlayerConnected(playerid)) return 0;

	// 1. Cegah eksekusi ganda jika pemain sudah berstatus tewas
	if (gACData[playerid][acIsDead] || !gPlayerData[playerid][pSpawned]) {
		return 0;
	}

	// 2. Rate-limit kematian: Pemain tidak bisa tewas berulang dalam selang waktu < 3 detik
	new now = gettime();
	if (now - gACData[playerid][acLastDeathTick] < 3) {
		AC_AlertAdmins(playerid, "Fake Kill / Death Flooder", "Memicu paket OnPlayerDeath berulang kali");
		return 0;
	}

	// 3. Validasi identitas dan posisi pembunuh bila ada
	if (killerid != INVALID_PLAYER_ID) {
		if (!IsPlayerConnected(killerid) || !gPlayerData[killerid][pSpawned]) {
			AC_AlertAdmins(playerid, "Fake Kill / Invalid Killer", "Pembunuh tidak aktif di server");
			return 0;
		}

		new Float:px, Float:py, Float:pz;
		new Float:kx, Float:ky, Float:kz;
		GetPlayerPos(playerid, px, py, pz);
		GetPlayerPos(killerid, kx, ky, kz);
		new Float:dist = floatsqroot((px-kx)*(px-kx) + (py-ky)*(py-ky) + (pz-kz)*(pz-kz));

		new rId = _:reason;
		// Jika penyebab adalah senjata melee (0-15), jarak maksimal wajar adalah 5.5 meter
		if (rId >= 0 && rId <= 15) {
			if (dist > 5.5) {
				new detail[64];
				format(detail, sizeof(detail), "Jarak melee: %.1fm (Maks: 5.5m)", dist);
				AC_AlertAdmins(playerid, "Fake Kill / Remote Melee", detail);
				return 0;
			}
		}
		// Jika senjata api, jarak tidak boleh melebihi jangkauan senjata + toleransi
		else if (rId >= 22 && rId <= 34) {
			new Float:maxR = AC_GetMaxWeaponRange(rId);
			if (dist > maxR + 10.0) {
				new detail[64];
				format(detail, sizeof(detail), "Jarak senjata api: %.1fm (Maks: %.1fm)", dist, maxR);
				AC_AlertAdmins(playerid, "Fake Kill / Remote Bullet Death", detail);
				return 0;
			}
		}
	}

	gACData[playerid][acIsDead] = true;
	gACData[playerid][acLastDeathTick] = now;
	return 1;
}

// Validasi Damage & Deteksi Godmode / Health-Lock (s0beit & Lua GM)
stock AC_ValidateDamage(playerid, damagedid, Float:amount, WEAPON:weaponid, bodypart)
{
	#pragma unused amount, bodypart
	if (!IsPlayerConnected(playerid) || !IsPlayerConnected(damagedid)) return 1;

	// Validasi kepemilikan senjata pada pihak penembak
	new wId = _:weaponid;
	if (wId > 1 && wId < 47) {
		if (!gACData[playerid][acHasWeapon][wId]) {
			AC_AlertAdmins(playerid, "Unauthorized Weapon Damage", "Menimbulkan damage tanpa izin senjata");
			return 0;
		}
	}

	// Validasi jarak tembak
	new Float:px, Float:py, Float:pz;
	new Float:tx, Float:ty, Float:tz;
	GetPlayerPos(playerid, px, py, pz);
	GetPlayerPos(damagedid, tx, ty, tz);

	new Float:dist = floatsqroot((px-tx)*(px-tx) + (py-ty)*(py-ty) + (pz-tz)*(pz-tz));
	new Float:maxRange = AC_GetMaxWeaponRange(wId);

	if (dist > maxRange + 5.0) {
		new detail[64];
		format(detail, sizeof(detail), "Jarak tembak: %.1fm (Maks: %.1fm)", dist, maxRange);
		AC_AlertAdmins(playerid, "Silent Aim / Damage Range", detail);
		return 0;
	}

	return 1;
}
