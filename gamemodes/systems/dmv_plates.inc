#if defined _INC_dmv_plates
	#endinput
#endif
#define _INC_dmv_plates

/*
==============================================================================
 dmv_plates.inc
 NUSANTARA ROLEPLAY - Custom Vehicle License Plate Registration System (SAGOV / DMV)
==============================================================================
*/

#define PLATE_REGISTRATION_FEE  500

stock ProcessCustomPlate(playerid, const newPlate[])
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return 0;

	if (!IsPlayerInAnyVehicle(playerid) || GetPlayerState(playerid) != PLAYER_STATE_DRIVER) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus duduk di kursi pengemudi kendaraan pribadi milik Anda.");
		return 0;
	}

	new vehid = GetPlayerVehicleID(playerid);
	new slot = -1;

	for (new i = 0; i < MAX_OWNED_VEHICLES; i++) {
		if (gVehicleData[i][vUsed] && gVehicleData[i][vVehicleHandle] == vehid) {
			slot = i;
			break;
		}
	}

	if (slot == -1) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Kendaraan ini bukan kendaraan pribadi yang terdaftar!");
		return 0;
	}

	if (gVehicleData[slot][vOwnerID] != gPlayerData[playerid][pID] && gPlayerData[playerid][pAdmin] < 2) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Ini bukan kendaraan milik Anda pribadi!");
		return 0;
	}

	new len = strlen(newPlate);
	if (len < 3 || len > 8) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Nomor plat kendaraan harus terdiri dari 3 hingga 8 karakter.");
		return 0;
	}

	// Validasi karakter alphanumeric
	for (new i = 0; i < len; i++) {
		new c = newPlate[i];
		if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == ' ' || c == '-')) {
			SendClientMessage(playerid, COLOR_ERROR, "ERROR: Plat nomor hanya boleh memuat huruf, angka, dan tanda hubung (-).");
			return 0;
		}
	}

	if (gPlayerData[playerid][pMoney] < PLATE_REGISTRATION_FEE && gPlayerData[playerid][pBank] < PLATE_REGISTRATION_FEE) {
		new errFund[128];
		format(errFund, sizeof(errFund), "ERROR: Biaya pendaftaran plat kustom sebesar $%d tidak mencukupi.", PLATE_REGISTRATION_FEE);
		SendClientMessage(playerid, COLOR_ERROR, errFund);
		return 0;
	}

	new bool:paid = false;
	if (gPlayerData[playerid][pMoney] >= PLATE_REGISTRATION_FEE) {
		paid = (TakePlayerMoneyEx(playerid, PLATE_REGISTRATION_FEE, "biaya plat kustom kendaraan") == 1);
	}
	if (!paid && gPlayerData[playerid][pBank] >= PLATE_REGISTRATION_FEE) {
		paid = (TakePlayerBankEx(playerid, PLATE_REGISTRATION_FEE, "biaya plat kustom kendaraan") == 1);
	}
	if (!paid) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Pembayaran pendaftaran plat kustom gagal diproses.");
		return 0;
	}

	format(gVehicleData[slot][vPlate], 16, "%s", newPlate);
	SetVehicleNumberPlate(vehid, newPlate);
	DB_SaveVehicle(slot);

	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);

	new succMsg[160];
	format(succMsg, sizeof(succMsg),
		"SUKSES: Plat nomor kustom kendaraan berhasil didaftarkan: '[ %s ]' seharga $%d.",
		newPlate, PLATE_REGISTRATION_FEE);
	SendClientMessage(playerid, COLOR_SUCCESS, succMsg);
	return 1;
}
