#if defined _INC_rp_player_auth
	#endinput
#endif
#define _INC_rp_player_auth

// pending buffers
new gPendingUCPOtp[MAX_PLAYERS][16];

// bcrypt callbacks
forward OnPlayerPasswordHashed(playerid);
public OnPlayerPasswordHashed(playerid)
{
	if (!IsPlayerConnected(playerid)) return 0;
	new hash[BCRYPT_HASH_LENGTH];
	bcrypt_get_hash(hash, sizeof(hash));

	format(gPlayerData[playerid][pPassword], 65, "%s", hash);
	format(gPlayerData[playerid][pSalt], 65, "bcrypt");

	DB_UpdateUCPPassword(playerid, hash);
	return 1;
}

forward OnPlayerPasswordVerified(playerid, bool:success);
public OnPlayerPasswordVerified(playerid, bool:success)
{
	if (!IsPlayerConnected(playerid)) return 0;

	if (!success) {
		SendErr(playerid, "Kata sandi salah.");
		ShowUCPLoginDialog(playerid);
		return 0;
	}

	DB_LoadUCPCharacters(playerid);
	return 1;
}

stock ResetPlayerData(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;
	new name[MAX_PLAYER_NAME];
	GetPlayerName(playerid, name, sizeof(name));

	gPlayerData[playerid][pID] = 0;
	format(gPlayerData[playerid][pName], MAX_PLAYER_NAME, "%s", name);
	format(gPlayerData[playerid][pUCPName], 24, "%s", name);
	gPlayerData[playerid][pUCPID] = 0;
	format(gPlayerData[playerid][pBirthDate], 16, "01/01/2000");
	gPlayerData[playerid][pGender] = 1;

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
	gPlayerData[playerid][pLanguage] = LANG_ID;
	gPlayerData[playerid][pTickets] = 0;
	gPlayerData[playerid][pTicketAmount] = 0;
	gPlayerData[playerid][pDriveLic] = false;
	gPlayerData[playerid][pGunLic] = false;
	gPlayerData[playerid][pFightStyle] = 4;
	gPlayerData[playerid][pDrunkLevel] = 0;

	gPhoneOpen[playerid] = false;
	gInventoryOpen[playerid] = false;
	gGiveTarget[playerid] = -1;
	gGiveItemType[playerid] = 0;

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

// ---------------------------------------------------------------------------
// UCP Authentication Dialogs (Auto-detect dari nama samp player)
// ---------------------------------------------------------------------------
stock ShowUCPNotRegisteredKick(playerid)
{
	SendClientMessage(playerid, COLOR_ERROR,
		"[NUSANTARA RP] Akun UCP belum terdaftar. Silakan registrasi via bot resmi untuk mendapat Kode Verifikasi (OTP/PIN).");
	SendClientMessage(playerid, COLOR_INFO,
		"[INFO] Koneksi diputus otomatis dalam 5 detik.");
	SetTimerEx("RP_KickNotRegistered", 5000, false, "i", playerid);
	return 1;
}

forward RP_KickNotRegistered(playerid);
public RP_KickNotRegistered(playerid)
{
	if (IsPlayerConnected(playerid)) Kick(playerid);
	return 1;
}

stock ShowUCPLoginDialog(playerid)
{
	if (!IsPlayerConnected(playerid)) return 0;
	ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD,
		"Akun UCP Terdaftar",
		"Masukkan kata sandi akun UCP Anda.", "Masuk", "Keluar");
	return 1;
}

stock ProcessUCPLogin(playerid, const password[])
{
	if (!IsPlayerConnected(playerid)) return 0;
	if (strlen(password) < 6) {
		SendErr(playerid, "Kata sandi minimal 6 karakter.");
		ShowUCPLoginDialog(playerid);
		return 0;
	}
	bcrypt_verify(playerid, "OnPlayerPasswordVerified", password, gPlayerData[playerid][pPassword]);
	return 1;
}

// OTP/PIN verification untuk player yang sudah registrasi via bot tapi belum set password
stock ShowUCPOtpVerifyDialog(playerid)
{
	if (!IsPlayerConnected(playerid)) return 0;
	ShowPlayerDialog(playerid, DIALOG_UCP_OTP_VERIFY, DIALOG_STYLE_INPUT,
		"Verifikasi Kode Pendaftaran",
		"Masukkan 6 digit Kode Verifikasi (OTP/PIN) yang dikirim ke Anda melalui Bot resmi kami.", "Verifikasi", "Batal");
	return 1;
}

stock ProcessUCPOtpVerify(playerid, const otp[])
{
	if (!IsPlayerConnected(playerid)) return 0;
	if (strlen(otp) < 4 || strlen(otp) > 8) {
		SendErr(playerid, "Kode verifikasi 4-8 digit.");
		ShowUCPOtpVerifyDialog(playerid);
		return 0;
	}

	format(gPendingUCPOtp[playerid], sizeof(gPendingUCPOtp[]), "%s", otp);
	DB_VerifyUCPOtp(playerid, otp);
	return 1;
}

stock ShowUCPRegisterPassDialog(playerid)
{
	if (!IsPlayerConnected(playerid)) return 0;
	ShowPlayerDialog(playerid, DIALOG_UCP_REG_PASS, DIALOG_STYLE_PASSWORD,
		"Buat Kata Sandi UCP",
		"Kode verifikasi valid. Silakan buat kata sandi minimal 6 karakter untuk menyelesaikan pendaftaran akun UCP Anda.", "Buat Sandi", "Kembali");
	return 1;
}

stock ProcessUCPRegisterPass(playerid, const password[])
{
	if (!IsPlayerConnected(playerid)) return 0;
	if (strlen(password) < 6) {
		SendErr(playerid, "Kata sandi minimal 6 karakter.");
		ShowUCPRegisterPassDialog(playerid);
		return 0;
	}

	bcrypt_hash(playerid, "OnPlayerPasswordHashed", password, BCRYPT_COST);
	return 1;
}

// ---------------------------------------------------------------------------
// Character Selection & Creation
// ---------------------------------------------------------------------------
stock ProcessCharSelect(playerid, listitem)
{
	if (!IsPlayerConnected(playerid)) return 0;

	if (listitem >= 0 && listitem < 3 && gUserCharIDs[playerid][listitem] > 0) {
		DB_LoadCharacter(playerid, gUserCharIDs[playerid][listitem]);
		return 1;
	}

	ShowCharCreateNameDialog(playerid);
	return 1;
}

stock ShowCharCreateNameDialog(playerid)
{
	if (!IsPlayerConnected(playerid)) return 0;
	ShowPlayerDialog(playerid, DIALOG_CHAR_CREATE_NAME, DIALOG_STYLE_INPUT,
		"Nama Karakter Roleplay",
		"Masukkan nama karakter dengan format Nama_Belakang (contoh: Javier_Travors).", "Lanjut", "Batal");
	return 1;
}

stock ProcessCharCreateName(playerid, const name[])
{
	if (!IsPlayerConnected(playerid)) return 0;

	if (!IsValidRoleplayName(name)) {
		SendErr(playerid, "Format nama tidak valid. Contoh: Javier_Travors");
		ShowCharCreateNameDialog(playerid);
		return 0;
	}

	format(gPendingCharName[playerid], 24, "%s", name);
	ShowCharCreateDOBDialog(playerid);
	return 1;
}

stock ShowCharCreateDOBDialog(playerid)
{
	if (!IsPlayerConnected(playerid)) return 0;
	ShowPlayerDialog(playerid, DIALOG_CHAR_CREATE_DOB, DIALOG_STYLE_INPUT,
		"Tanggal Lahir",
		"Format DD/MM/YYYY (contoh: 15/08/2000).", "Lanjut", "Kembali");
	return 1;
}

stock ProcessCharCreateDOB(playerid, const dob[])
{
	if (!IsPlayerConnected(playerid)) return 0;
	if (strlen(dob) < 8 || strlen(dob) > 10) {
		SendErr(playerid, "Format tanggal lahir tidak valid.");
		ShowCharCreateDOBDialog(playerid);
		return 0;
	}

	format(gPendingCharDOB[playerid], 16, "%s", dob);
	ShowCharCreateGenderDialog(playerid);
	return 1;
}

stock ShowCharCreateGenderDialog(playerid)
{
	if (!IsPlayerConnected(playerid)) return 0;
	ShowPlayerDialog(playerid, DIALOG_CHAR_CREATE_GENDER, DIALOG_STYLE_LIST,
		"Jenis Kelamin",
		"Laki-Laki\nPerempuan", "Pilih", "Kembali");
	return 1;
}

stock ProcessCharCreateGender(playerid, listitem)
{
	if (!IsPlayerConnected(playerid)) return 0;
	new gender = (listitem == 0) ? 1 : 2;
	new skin = (gender == 1) ? 2 : 12;

	DB_CreateCharacter(playerid, gPendingCharName[playerid], gPendingCharDOB[playerid], gender, skin);
	return 1;
}

// ---------------------------------------------------------------------------
// In-game Spawn (Anti-tenggelam & automatic load)
// ---------------------------------------------------------------------------
stock SpawnPlayerEx(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;

	if (gPlayerData[playerid][pSpawned]) return 1;
	gPlayerData[playerid][pSpawned] = true;
	AC_OnPlayerSpawn(playerid);

	SetPlayerSkin(playerid, gPlayerData[playerid][pSkin]);

	new Float:px = gPlayerData[playerid][pLastX];
	new Float:py = gPlayerData[playerid][pLastY];
	new Float:pz = gPlayerData[playerid][pLastZ];
	new Float:pa = gPlayerData[playerid][pLastA];

	if (px == 0.0 && py == 0.0 && pz == 0.0) {
		px = SPAWN_X; py = SPAWN_Y; pz = SPAWN_Z; pa = SPAWN_A;
	}

	pz += 0.5;
	if (gPlayerData[playerid][pInterior] == 0) {
		SetPlayerPosFindZ(playerid, px, py, pz);
		SetPlayerFacingAngle(playerid, pa);
	} else {
		SetPlayerPos(playerid, px, py, pz);
		SetPlayerFacingAngle(playerid, pa);
	}

	SetPlayerInterior(playerid, gPlayerData[playerid][pInterior]);
	SetPlayerVirtualWorld(playerid, gPlayerData[playerid][pVirtualWorld]);

	TogglePlayerControllable(playerid, false);
	SetTimerEx("RP_UnfreezeAfterSpawn", 750, false, "i", playerid);

	SetPlayerHealth(playerid, 100.0);
	SetPlayerArmour(playerid, 0.0);

	SetPlayerMoneyEx(playerid, gPlayerData[playerid][pMoney]);
	Faction_ApplyWeapons(playerid);

	ShowPlayerHUD(playerid);
	UpdatePlayerHUD(playerid);

	gPlayerData[playerid][pPaycheckTimer] = PAYCHECK_INTERVAL;
	return 1;
}

forward RP_UnfreezeAfterSpawn(playerid);
public RP_UnfreezeAfterSpawn(playerid)
{
	if (!IsPlayerConnected(playerid)) return 0;
	TogglePlayerControllable(playerid, true);
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
