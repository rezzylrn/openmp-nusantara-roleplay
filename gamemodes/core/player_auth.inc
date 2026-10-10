#if defined _INC_rp_player_auth
	#endinput
#endif
#define _INC_rp_player_auth

// ucp pending buffers
new gPendingUCPCode[MAX_PLAYERS][16];

forward OnPlayerPasswordHashed(playerid);
public OnPlayerPasswordHashed(playerid)
{
	if (!IsPlayerConnected(playerid)) return 0;
	new hash[BCRYPT_HASH_LENGTH];
	bcrypt_get_hash(hash, sizeof(hash));

	format(gPlayerData[playerid][pPassword], 65, "%s", hash);
	format(gPlayerData[playerid][pSalt], 65, "bcrypt");

	DB_RegisterUCP(playerid, gPlayerData[playerid][pUCPName], hash, "bcrypt", gPendingUCPCode[playerid]);
	return 1;
}

forward OnPlayerPasswordVerified(playerid, bool:success);
public OnPlayerPasswordVerified(playerid, bool:success)
{
	if (!IsPlayerConnected(playerid)) return 0;

	if (!success) {
		SendErr(playerid, "Kata sandi yang Anda masukkan salah.");
		ShowUCPLoginDialog(playerid);
		return 0;
	}

	SendSucc(playerid, "Autentikasi UCP berhasil! Memuat karakter Anda...");
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
// UCP Authentication Dialogs
// ---------------------------------------------------------------------------
stock ShowUCPInputPrompt(playerid)
{
	if (!IsPlayerConnected(playerid)) return 0;
	new name[MAX_PLAYER_NAME], str[320];
	GetPlayerName(playerid, name, sizeof(name));

	format(str, sizeof(str),
		"{FFFFFF}Selamat datang di {33CCFF}Nusantara Roleplay{FFFFFF}!\n\n\
Nama Client Anda: {FFFF00}%s\n\n\
Silakan masukkan {33CCFF}Nama Akun UCP{FFFFFF} Anda:\n\
(Jika belum memiliki akun UCP, ketik nama akun UCP yang ingin Anda daftarkan)", name);

	ShowPlayerDialog(playerid, DIALOG_UCP_LOGIN, DIALOG_STYLE_INPUT,
		"LOGIN AKUN UCP - NUSANTARA RP", str, "Lanjut", "Keluar");
	return 1;
}

stock ProcessUCPInput(playerid, const input[])
{
	if (!IsPlayerConnected(playerid)) return 0;
	if (strlen(input) < 3 || strlen(input) > 20) {
		SendErr(playerid, "Nama UCP minimal 3 dan maksimal 20 karakter.");
		ShowUCPInputPrompt(playerid);
		return 0;
	}

	for (new i = 0, len = strlen(input); i < len; i++) {
		if (!((input[i] >= 'a' && input[i] <= 'z') || (input[i] >= 'A' && input[i] <= 'Z') || (input[i] >= '0' && input[i] <= '9') || input[i] == '_')) {
			SendErr(playerid, "Nama UCP hanya boleh mengandung huruf, angka, dan underscore.");
			ShowUCPInputPrompt(playerid);
			return 0;
		}
	}

	format(gPlayerData[playerid][pUCPName], 24, "%s", input);
	DB_CheckUCP(playerid, input);
	return 1;
}

stock ShowUCPLoginDialog(playerid)
{
	if (!IsPlayerConnected(playerid)) return 0;
	new str[320];
	format(str, sizeof(str),
		"{FFFFFF}Selamat datang kembali di {33CCFF}Nusantara Roleplay{FFFFFF}!\n\n\
Akun UCP: {FFFF00}%s\n\
Status: {2ECC71}Terdaftar di Database\n\n\
Silakan masukkan kata sandi UCP Anda untuk masuk:", gPlayerData[playerid][pUCPName]);

	ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD,
		"MASUK AKUN UCP - NUSANTARA RP", str, "Masuk", "Kembali");
	return 1;
}

stock ProcessUCPLogin(playerid, const password[])
{
	if (!IsPlayerConnected(playerid)) return 0;

	// Asynchronous 12-round bcrypt password verification
	bcrypt_verify(playerid, "OnPlayerPasswordVerified", password, gPlayerData[playerid][pPassword]);
	return 1;
}

stock ShowUCPRegisterCodeDialog(playerid)
{
	if (!IsPlayerConnected(playerid)) return 0;
	new str[360];
	format(str, sizeof(str),
		"{FFFFFF}Akun UCP {FFFF00}%s {FFFFFF}belum terdaftar di server.\n\n\
Silakan buat {33CCFF}4 - 8 digit Kode Keamanan / PIN{FFFFFF} Anda:\n\
(Kode ini berguna untuk keamanan akun Anda dan verifikasi masa depan)", gPlayerData[playerid][pUCPName]);

	ShowPlayerDialog(playerid, DIALOG_UCP_REG_CODE, DIALOG_STYLE_INPUT,
		"REGISTRASI UCP - KODE KEAMANAN", str, "Lanjut", "Kembali");
	return 1;
}

stock ProcessUCPRegisterCode(playerid, const code[])
{
	if (!IsPlayerConnected(playerid)) return 0;
	if (strlen(code) < 4 || strlen(code) > 8) {
		SendErr(playerid, "Kode keamanan harus berupa 4 - 8 digit angka.");
		ShowUCPRegisterCodeDialog(playerid);
		return 0;
	}

	format(gPendingUCPCode[playerid], sizeof(gPendingUCPCode[]), "%s", code);
	ShowUCPRegisterPassDialog(playerid);
	return 1;
}

stock ShowUCPRegisterPassDialog(playerid)
{
	if (!IsPlayerConnected(playerid)) return 0;
	new str[300];
	format(str, sizeof(str),
		"{FFFFFF}UCP: {FFFF00}%s\n{FFFFFF}Kode Keamanan: {2ECC71}%s\n\n\
Silakan buat {33CCFF}Kata Sandi{FFFFFF} untuk akun UCP Anda (Minimal 6 karakter):",
		gPlayerData[playerid][pUCPName], gPendingUCPCode[playerid]);

	ShowPlayerDialog(playerid, DIALOG_UCP_REG_PASS, DIALOG_STYLE_PASSWORD,
		"REGISTRASI UCP - BUAT KATA SANDI", str, "Daftar", "Kembali");
	return 1;
}

stock ProcessUCPRegisterPass(playerid, const password[])
{
	if (!IsPlayerConnected(playerid)) return 0;
	if (strlen(password) < 6) {
		SendErr(playerid, "Kata sandi minimal harus 6 karakter.");
		ShowUCPRegisterPassDialog(playerid);
		return 0;
	}

	// Asynchronous 12-round bcrypt password hash
	bcrypt_hash(playerid, "OnPlayerPasswordHashed", password, BCRYPT_COST);
	return 1;
}

// ---------------------------------------------------------------------------
// Character Selection & Creation Dialog Handlers
// ---------------------------------------------------------------------------
stock ProcessCharSelect(playerid, listitem)
{
	if (!IsPlayerConnected(playerid)) return 0;

	if (listitem >= 0 && listitem < 3 && gUserCharIDs[playerid][listitem] > 0) {
		// Muat karakter yang sudah ada
		DB_LoadCharacter(playerid, gUserCharIDs[playerid][listitem]);
		return 1;
	}

	// Buat karakter baru
	ShowCharCreateNameDialog(playerid);
	return 1;
}

stock ShowCharCreateNameDialog(playerid)
{
	if (!IsPlayerConnected(playerid)) return 0;
	new str[380];
	format(str, sizeof(str),
		"{FFFFFF}Buat Karakter Baru untuk Akun UCP {33CCFF}%s{FFFFFF}.\n\n\
Masukkan Nama Karakter Roleplay Anda:\n\
{FFFF00}Format Wajib: Nama_Belakang{FFFFFF} (Contoh: {2ECC71}Javier_Travors{FFFFFF}, {2ECC71}Bryan_Calister{FFFFFF})\n\n\
Aturan:\n\
- Wajib diawali huruf kapital di kedua kata\n\
- Wajib dipisah satu tanda garis bawah (underscore)\n\
- Tidak boleh menggunakan angka atau simbol lain", gPlayerData[playerid][pUCPName]);

	ShowPlayerDialog(playerid, DIALOG_CHAR_CREATE_NAME, DIALOG_STYLE_INPUT,
		"BUAT KARAKTER - NAMA ROLEPLAY", str, "Lanjut", "Batal");
	return 1;
}

stock ProcessCharCreateName(playerid, const name[])
{
	if (!IsPlayerConnected(playerid)) return 0;

	if (!IsValidRoleplayName(name)) {
		SendErr(playerid, "Format nama salah! Gunakan format Nama_Belakang (Contoh: Javier_Travors) tanpa angka.");
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
	new str[300];
	format(str, sizeof(str),
		"{FFFFFF}Karakter: {FFFF00}%s{FFFFFF}\n\n\
Masukkan {33CCFF}Tanggal Lahir{FFFFFF} Karakter Anda:\n\
Format: {FFFF00}DD/MM/YYYY{FFFFFF} (Contoh: 15/08/2000)", gPendingCharName[playerid]);

	ShowPlayerDialog(playerid, DIALOG_CHAR_CREATE_DOB, DIALOG_STYLE_INPUT,
		"BUAT KARAKTER - TANGGAL LAHIR", str, "Lanjut", "Kembali");
	return 1;
}

stock ProcessCharCreateDOB(playerid, const dob[])
{
	if (!IsPlayerConnected(playerid)) return 0;
	if (strlen(dob) < 8 || strlen(dob) > 10) {
		SendErr(playerid, "Format tanggal lahir salah! Contoh: 15/08/2000");
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
		"BUAT KARAKTER - JENIS KELAMIN",
		"1. Laki-Laki (Pria)\n2. Perempuan (Wanita)",
		"Pilih", "Kembali");
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
// Ingame Spawn & Disconnect Handlers
// ---------------------------------------------------------------------------
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
	SendRP(playerid, COLOR_WHITE, "Akun UCP: {33CCFF}%s {FFFFFF}| Karakter: {FFFF00}%s", gPlayerData[playerid][pUCPName], gPlayerData[playerid][pName]);
	SendRP(playerid, COLOR_WHITE, "Tips: Tekan {FFFF00}Y{FFFFFF} untuk interaksi cepat, ketik {FFFF00}/help{FFFFFF} atau {FFFF00}/inv{FFFFFF}.");
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
