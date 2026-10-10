#if defined _INC_rp_player_auth
	#endinput
#endif
#define _INC_rp_player_auth

/*
==============================================================================
  NUSANTARA ROLEPLAY - player_auth.inc
  Sistem Autentikasi: Registrasi, Login, Hashing Password, & Starter Pack
==============================================================================
*/

stock HashPassword(const password[], const salt[], hash[], len = sizeof(hash))
{
	new buf[128];
	format(buf, sizeof(buf), "%s", password);
	SHA256_Hash(buf, salt, hash, len);
	return 1;
}

stock VerifyPassword(const password[], const salt[], const hash[])
{
	new buf[128], out[65];
	format(buf, sizeof(buf), "%s", password);
	SHA256_Hash(buf, salt, out, sizeof(out));
	return (strcmp(out, hash, false, 64) == 0);
}

stock ResetPlayerData(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;
	new name[MAX_PLAYER_NAME];
	GetPlayerName(playerid, name, sizeof(name));

	gPlayerData[playerid][pID] = 0;
	format(gPlayerData[playerid][pName], MAX_PLAYER_NAME, "%s", name);
	gPlayerData[playerid][pPassword][0] = '\0';
	gPlayerData[playerid][pSalt][0] = '\0';
	gPlayerData[playerid][pLoggedIn] = false;
	gPlayerData[playerid][pMoney] = 0;
	gPlayerData[playerid][pBank] = 0;
	gPlayerData[playerid][pLevel] = 1;
	gPlayerData[playerid][pExp] = 0;
	gPlayerData[playerid][pAdmin] = 0;
	gPlayerData[playerid][pFaction] = FACTION_NONE;
	gPlayerData[playerid][pFactionRank] = 0;
	gPlayerData[playerid][pDuty] = false;
	gPlayerData[playerid][pJob] = JOB_NONE;
	gPlayerData[playerid][pHunger] = START_HUNGER;
	gPlayerData[playerid][pThirst] = START_THIRST;
	gPlayerData[playerid][pPaycheckTimer] = PAYCHECK_INTERVAL;
	gPlayerData[playerid][pPaycheckAmount] = 0;
	gPlayerData[playerid][pSkin] = START_SKIN;
	gPlayerData[playerid][pLastX] = SPAWN_X;
	gPlayerData[playerid][pLastY] = SPAWN_Y;
	gPlayerData[playerid][pLastZ] = SPAWN_Z;
	gPlayerData[playerid][pLastA] = SPAWN_A;
	gPlayerData[playerid][pInterior] = 0;
	gPlayerData[playerid][pVirtualWorld] = 0;
	gPlayerData[playerid][pCuffed] = false;
	gPlayerData[playerid][pFrozen] = false;
	gPlayerData[playerid][pWarnings] = 0;
	gPlayerData[playerid][pKills] = 0;
	gPlayerData[playerid][pDeaths] = 0;
	gPlayerData[playerid][pPlayHours] = 0;
	gPlayerData[playerid][pMuted] = false;
	gPlayerData[playerid][pSpawned] = false;

	gPlayerData[playerid][pPhone] = 0;
	gPlayerData[playerid][pPhoneStatus] = true;
	gPlayerData[playerid][pCallTarget] = -1;
	gPlayerData[playerid][pCallState] = 0;
	gPlayerData[playerid][pRadioFreq] = 0;
	gPlayerData[playerid][pRadioStatus] = false;
	gPlayerData[playerid][pInjured] = false;
	gPlayerData[playerid][pInjuredTime] = 0;

	ClearPlayerInventory(playerid);

	gPlayerJobVehicle[playerid] = -1;
	gPlayerJobTrailer[playerid] = -1;
	gJobCheckpoint[playerid] = 0;
	gJobCash[playerid] = 0;
	gJobActive[playerid] = false;
	gJobType[playerid] = 0;
	gPlayerVehicleSlot[playerid] = 0;
	return 1;
}

stock ShowRegisterDialog(playerid)
{
	if (!IsPlayerConnected(playerid)) return 0;
	new name[MAX_PLAYER_NAME], str[300];
	GetPlayerName(playerid, name, sizeof(name));

	format(str, sizeof(str),
		"{FFFFFF}Selamat datang di {33CCFF}Nusantara Roleplay{FFFFFF}.\n\n\
Nama karakter: {FFFF00}%s\n\
Status akun: {E74C3C}Belum Terdaftar\n\n\
Silakan buat kata sandi untuk mendaftarkan akun baru:", name);

	ShowPlayerDialog(playerid, DIALOG_REGISTER, DIALOG_STYLE_PASSWORD,
		"REGISTRASI AKUN - NUSANTARA RP", str, "Daftar", "Keluar");
	return 1;
}

stock ShowRegisterConfirmDialog(playerid)
{
	if (!IsPlayerConnected(playerid)) return 0;
	ShowPlayerDialog(playerid, DIALOG_REGISTER_CONFIRM, DIALOG_STYLE_PASSWORD,
		"KONFIRMASI KATA SANDI",
		"{FFFFFF}Ketik ulang kata sandi Anda untuk memastikan tidak ada kesalahan pengetikan:",
		"Konfirmasi", "Batal");
	return 1;
}

stock ShowLoginDialog(playerid)
{
	if (!IsPlayerConnected(playerid)) return 0;
	new name[MAX_PLAYER_NAME], str[300];
	GetPlayerName(playerid, name, sizeof(name));

	format(str, sizeof(str),
		"{FFFFFF}Selamat datang kembali di {33CCFF}Nusantara Roleplay{FFFFFF}.\n\n\
Nama karakter: {FFFF00}%s\n\
Status akun: {2ECC71}Terdaftar di Database (SQLite)\n\n\
Silakan masukkan kata sandi akun Anda untuk masuk:", name);

	ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD,
		"MASUK KE AKUN - NUSANTARA RP", str, "Masuk", "Keluar");
	return 1;
}

new gPendingPassword[MAX_PLAYERS][80];

stock ProcessRegister(playerid, const password[])
{
	if (!IsPlayerConnected(playerid)) return 0;

	if (strlen(password) < 6) {
		SendErr(playerid, "Kata sandi minimal harus 6 karakter.");
		ShowRegisterDialog(playerid);
		return 0;
	}

	format(gPendingPassword[playerid], sizeof(gPendingPassword[]), "%s", password);
	ShowRegisterConfirmDialog(playerid);
	return 1;
}

stock ProcessRegisterConfirm(playerid, const confirmpassword[])
{
	if (!IsPlayerConnected(playerid)) return 0;
	new name[MAX_PLAYER_NAME];
	GetPlayerName(playerid, name, sizeof(name));

	if (strcmp(gPendingPassword[playerid], confirmpassword, false, 79) != 0) {
		SendErr(playerid, "Kata sandi konfirmasi tidak cocok. Ulangi pembuatan kata sandi.");
		gPendingPassword[playerid][0] = '\0';
		ShowRegisterDialog(playerid);
		return 0;
	}

	new salt[65], hash[65];
	format(salt, sizeof(salt), "%s-%d-%d", name, random(999999), GetTickCount());
	HashPassword(gPendingPassword[playerid], salt, hash, sizeof(hash));
	gPendingPassword[playerid][0] = '\0';

	format(gPlayerData[playerid][pPassword], 65, "%s", hash);
	format(gPlayerData[playerid][pSalt], 65, "%s", salt);
	gPlayerData[playerid][pMoney] = START_MONEY;
	gPlayerData[playerid][pBank] = START_BANK;
	gPlayerData[playerid][pLevel] = START_LEVEL;
	gPlayerData[playerid][pExp] = START_EXP;
	gPlayerData[playerid][pHunger] = START_HUNGER;
	gPlayerData[playerid][pThirst] = START_THIRST;
	gPlayerData[playerid][pSkin] = START_SKIN;
	gPlayerData[playerid][pLastX] = SPAWN_X;
	gPlayerData[playerid][pLastY] = SPAWN_Y;
	gPlayerData[playerid][pLastZ] = SPAWN_Z;
	gPlayerData[playerid][pLastA] = SPAWN_A;
	gPlayerData[playerid][pLoggedIn] = true;

	// Starter Pack untuk pemain baru
	gPlayerData[playerid][pPhone] = 500000 + random(499999);
	gPlayerData[playerid][pPhoneStatus] = true;

	GivePlayerItem(playerid, ITEM_SNACK, 2);
	GivePlayerItem(playerid, ITEM_WATER, 2);
	GivePlayerItem(playerid, ITEM_PHONE, 1);
	GivePlayerItem(playerid, ITEM_GPS, 1);

	DB_RegisterAccount(playerid, hash, salt);
	return 1;
}

stock ProcessLogin(playerid, const password[])
{
	if (!IsPlayerConnected(playerid)) return 0;

	if (!VerifyPassword(password, gPlayerData[playerid][pSalt], gPlayerData[playerid][pPassword])) {
		SendErr(playerid, "Kata sandi yang Anda masukkan salah.");
		ShowLoginDialog(playerid);
		return 0;
	}

	DB_LoadPlayerData(playerid);
	gPlayerData[playerid][pLoggedIn] = true;
	return 1;
}

stock SpawnPlayerEx(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;

	SetPlayerSkin(playerid, gPlayerData[playerid][pSkin]);

	new Float:px = gPlayerData[playerid][pLastX];
	new Float:py = gPlayerData[playerid][pLastY];
	new Float:pz = gPlayerData[playerid][pLastZ];
	new Float:pa = gPlayerData[playerid][pLastA];
	if (px == 0.0 && py == 0.0 && pz == 0.0) {
		px = SPAWN_X; py = SPAWN_Y; pz = SPAWN_Z; pa = SPAWN_A;
	}
	SetPlayerPos(playerid, px, py, pz);
	SetPlayerFacingAngle(playerid, pa);
	SetPlayerInterior(playerid, gPlayerData[playerid][pInterior]);
	SetPlayerVirtualWorld(playerid, gPlayerData[playerid][pVirtualWorld]);

	SetPlayerHealth(playerid, 100.0);
	SetPlayerArmour(playerid, 0.0);
	gPlayerData[playerid][pSpawned] = true;

	SetPlayerMoneyEx(playerid, gPlayerData[playerid][pMoney]);
	Faction_ApplyWeapons(playerid);

	ShowPlayerHUD(playerid);

	SendRP(playerid, COLOR_LIGHTBLUE, "============================================================");
	SendRP(playerid, COLOR_WHITE, "Selamat datang di Nusantara Roleplay [open.mp v1.5.8]");
	SendRP(playerid, COLOR_WHITE, "Ketik {FFFF00}/help {FFFFFF}untuk panduan perintah atau {FFFF00}/gps {FFFFFF}untuk peta navigasi.");
	SendRP(playerid, COLOR_LIGHTBLUE, "============================================================");

	DB_SavePlayerData(playerid);
	gPlayerData[playerid][pPaycheckTimer] = PAYCHECK_INTERVAL;
	return 1;
}

stock SavePlayerOnDisconnect(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;

	new Float:px, Float:py, Float:pz, Float:pa;
	if (GetPlayerPos(playerid, px, py, pz)) {
		gPlayerData[playerid][pLastX] = px;
		gPlayerData[playerid][pLastY] = py;
		gPlayerData[playerid][pLastZ] = pz;
		GetPlayerFacingAngle(playerid, pa);
		gPlayerData[playerid][pLastA] = pa;
	}

	Vehicle_SavePlayerVehicles(playerid);
	gPlayerData[playerid][pPlayHours] += 1;
	DB_SavePlayerData(playerid);
	return 1;
}

stock Vehicle_SavePlayerVehicles(playerid)
{
	for (new i = 0; i < MAX_OWNED_VEHICLES; i++) {
		if (gVehicleData[i][vUsed] && gVehicleData[i][vOwnerID] == gPlayerData[playerid][pID]) {
			new handle = gVehicleData[i][vVehicleHandle];
			if (handle > 0 && IsValidVehicle(handle)) {
				new Float:vx, Float:vy, Float:vz, Float:va;
				GetVehiclePos(handle, vx, vy, vz);
				GetVehicleZAngle(handle, va);
				gVehicleData[i][vPosX] = vx;
				gVehicleData[i][vPosY] = vy;
				gVehicleData[i][vPosZ] = vz;
				gVehicleData[i][vPosA] = va;
			}
			DB_SaveVehicle(i);
		}
	}
	return 1;
}
