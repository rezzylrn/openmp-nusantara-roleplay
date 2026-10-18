#if defined _INC_rp_impound
	#endinput
#endif
#define _INC_rp_impound

/*
==============================================================================
 impound.inc
 NUSANTARA ROLEPLAY - Tow Truck & Police Vehicle Impound Yard System
==============================================================================
*/

#define IMPOUND_FEE             200
#define IMPOUND_YARD_X          1540.0
#define IMPOUND_YARD_Y          -1610.0
#define IMPOUND_YARD_Z          13.5

new gPlayerTowedVeh[MAX_PLAYERS] = {INVALID_VEHICLE_ID, ...};

stock Impound_Init()
{
	for (new i = 0; i < MAX_PLAYERS; i++) {
		gPlayerTowedVeh[i] = INVALID_VEHICLE_ID;
	}

	CreatePickup(1239, 1, IMPOUND_YARD_X, IMPOUND_YARD_Y, IMPOUND_YARD_Z, 0);
	Create3DTextLabel(
		"{33CCFF}[ DEPO SITAAN KENDARAAN KOTA ]\n{FFFFFF}Ambil & tebus kendaraan sitaan polisi/mekanik\n{FFFF00}Gunakan /claimveh atau /tebus ($200)",
		COLOR_LIGHTBLUE, IMPOUND_YARD_X, IMPOUND_YARD_Y, IMPOUND_YARD_Z + 0.8, 15.0, 0, true);

	print("[IMPOUND] Sistem truk derek towtruck & depo sitaan LSPD aktif.");
	return 1;
}

stock Impound_ResetPlayer(playerid)
{
	if (playerid >= 0 && playerid < MAX_PLAYERS) {
		if (gPlayerTowedVeh[playerid] != INVALID_VEHICLE_ID) {
			if (IsPlayerConnected(playerid)) {
				new myVeh = GetPlayerVehicleID(playerid);
				if (myVeh > 0 && IsValidVehicle(myVeh)) DetachTrailerFromVehicle(myVeh);
			}
			gPlayerTowedVeh[playerid] = INVALID_VEHICLE_ID;
		}
	}
	return 1;
}

stock ProcessTowVehicle(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;

	if (!IsPlayerInAnyVehicle(playerid) || GetPlayerState(playerid) != PLAYER_STATE_DRIVER) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus mengendarai Truk Derek (Towtruck).");
		return 0;
	}

	new myVeh = GetPlayerVehicleID(playerid);
	if (GetVehicleModel(myVeh) != 525) { // 525 = Towtruck
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Kendaraan yang Anda kemudikan bukan Truk Derek (Towtruck)!");
		return 0;
	}

	if (gPlayerData[playerid][pJob] != JOB_MECHANIC && (gPlayerData[playerid][pFaction] != FACTION_POLICE || !gPlayerData[playerid][pDuty])) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Pekerjaan menderek kendaraan khusus untuk Mekanik atau Polisi SAPD.");
		return 0;
	}

	if (gPlayerTowedVeh[playerid] != INVALID_VEHICLE_ID) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda sudah menderek sebuah kendaraan. Gunakan /untow terlebih dahulu.");
		return 0;
	}

	// Cari kendaraan terdekat di belakang truk derek (radius 6.0m)
	new Float:tx, Float:ty, Float:tz;
	GetPlayerPos(playerid, tx, ty, tz);

	new targetVeh = INVALID_VEHICLE_ID;
	new Float:minDist = 999.0;

	for (new v = 1; v < MAX_VEHICLES; v++) {
		if (v == myVeh) continue;
		new Float:vx, Float:vy, Float:vz;
		GetVehiclePos(v, vx, vy, vz);
		new Float:dist = VectorSize(tx - vx, ty - vy, tz - vz);
		if (dist < 6.0 && dist < minDist) {
			minDist = dist;
			targetVeh = v;
		}
	}

	if (targetVeh == INVALID_VEHICLE_ID) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Tidak ada kendaraan terdekat di belakang derek untuk dikaitkan.");
		return 0;
	}

	AttachTrailerToVehicle(targetVeh, myVeh);
	gPlayerTowedVeh[playerid] = targetVeh;

	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
	SendClientMessage(playerid, COLOR_SUCCESS,
		"SUKSES: Kabel derek berhasil dikaitkan ke kendaraan! Bawa ke Depo Sitaan (/impound) atau gunakan /untow.");
	return 1;
}

stock ProcessUntowVehicle(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;

	if (gPlayerTowedVeh[playerid] == INVALID_VEHICLE_ID) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda tidak sedang menderek kendaraan apa pun.");
		return 0;
	}

	new myVeh = GetPlayerVehicleID(playerid);
	if (myVeh > 0) {
		DetachTrailerFromVehicle(myVeh);
	}
	gPlayerTowedVeh[playerid] = INVALID_VEHICLE_ID;

	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
	SendClientMessage(playerid, COLOR_INFO, "INFO: Kabel sling truk derek berhasil dilepas.");
	return 1;
}

stock ProcessImpoundVehicle(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;

	if (!IsPlayerNearPoint(playerid, IMPOUND_YARD_X, IMPOUND_YARD_Y, IMPOUND_YARD_Z, 15.0)) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus berada di area Depo Sitaan Kendaraan LSPD!");
		return 0;
	}

	if (gPlayerTowedVeh[playerid] == INVALID_VEHICLE_ID) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda tidak sedang menderek kendaraan yang akan disita.");
		return 0;
	}

	new towedVeh = gPlayerTowedVeh[playerid];
	new slot = -1;

	for (new i = 0; i < MAX_OWNED_VEHICLES; i++) {
		if (gVehicleData[i][vUsed] && gVehicleData[i][vVehicleHandle] == towedVeh) {
			slot = i;
			break;
		}
	}

	if (slot == -1) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Kendaraan yang diderek bukan kendaraan pribadi warga kota.");
		return 0;
	}

	// Lepas derek
	new myVeh = GetPlayerVehicleID(playerid);
	if (myVeh > 0) DetachTrailerFromVehicle(myVeh);
	gPlayerTowedVeh[playerid] = INVALID_VEHICLE_ID;

	// Sita kendaraan
	gVehicleData[slot][vImpounded] = true;
	gVehicleData[slot][vLocked] = true;
	SetVehiclePos(towedVeh, IMPOUND_YARD_X, IMPOUND_YARD_Y, IMPOUND_YARD_Z);
	DB_SaveVehicle(slot);

	// Hadiah biaya jasa derek untuk mekanik/polisi
	GivePlayerMoneyEx(playerid, 100, "upah jasa derek kendaraan sitaan");
	PlayerPlaySound(playerid, 1058, 0.0, 0.0, 0.0);

	new succMsg[160];
	format(succMsg, sizeof(succMsg),
		"[SITA KENDARAAN] Kendaraan plat '%s' resmi disita di depo LSPD! Anda menerima $100 upah jasa derek.",
		gVehicleData[slot][vPlate]);
	SendClientMessage(playerid, COLOR_SUCCESS, succMsg);
	return 1;
}

stock ProcessClaimImpoundedVehicle(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;

	if (!IsPlayerNearPoint(playerid, IMPOUND_YARD_X, IMPOUND_YARD_Y, IMPOUND_YARD_Z, 6.0)) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus berada di loket Depo Sitaan Kendaraan LSPD.");
		return 0;
	}

	new slot = -1;
	for (new i = 0; i < MAX_OWNED_VEHICLES; i++) {
		if (gVehicleData[i][vUsed] && gVehicleData[i][vOwnerID] == gPlayerData[playerid][pID] && gVehicleData[i][vImpounded]) {
			slot = i;
			break;
		}
	}

	if (slot == -1) {
		SendClientMessage(playerid, COLOR_INFO, "INFO: Anda tidak memiliki kendaraan pribadi yang sedang disita di depo ini.");
		return 0;
	}

	if (gPlayerData[playerid][pMoney] < IMPOUND_FEE && gPlayerData[playerid][pBank] < IMPOUND_FEE) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Dana tidak mencukupi untuk menebus kendaraan sitaan ($200).");
		return 0;
	}

	// [FIX] Cek pembayaran: kendaraan hanya ditebus bila transaksi BERHASIL.
	// Sebelumnya return TakePlayerMoneyEx/BankEx diabaikan.
	new bool:paid = false;
	if (gPlayerData[playerid][pMoney] >= IMPOUND_FEE) {
		paid = (TakePlayerMoneyEx(playerid, IMPOUND_FEE, "denda tebus kendaraan sitaan") == 1);
	}
	if (!paid && TakePlayerBankEx(playerid, IMPOUND_FEE, "denda tebus kendaraan sitaan") == 1) {
		paid = true;
	}
	if (!paid) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Pembayaran denda tebus gagal. Dana tidak mencukupi.");
		return 0;
	}

	gVehicleData[slot][vImpounded] = false;
	gVehicleData[slot][vLocked] = false;
	DB_SaveVehicle(slot);

	// Respawn ke luar depo sitaan
	if (gVehicleData[slot][vVehicleHandle] > 0) {
		SetVehiclePos(gVehicleData[slot][vVehicleHandle], IMPOUND_YARD_X + 5.0, IMPOUND_YARD_Y, IMPOUND_YARD_Z);
	}

	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
	new succMsg[160];
	format(succMsg, sizeof(succMsg),
		"SUKSES: Berhasil menebus kendaraan sitaan plat '%s' seharga $%d! Kunci kendaraan telah dibuka.",
		gVehicleData[slot][vPlate], IMPOUND_FEE);
	SendClientMessage(playerid, COLOR_SUCCESS, succMsg);
	return 1;
}
