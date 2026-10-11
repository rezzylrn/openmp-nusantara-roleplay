#if defined _INC_rp_taxi_meter
	#endinput
#endif
#define _INC_rp_taxi_meter

/*
==============================================================================
 taxi_meter.inc
 NUSANTARA ROLEPLAY - Dynamic Taxi Fare Meter & Passenger Dispatch Ride System
==============================================================================
*/

#define DEFAULT_TAXI_FARE       5

new bool:gTaxiMeterActive[MAX_PLAYERS];
new gTaxiFareRate[MAX_PLAYERS];
new gTaxiPassenger[MAX_PLAYERS] = {INVALID_PLAYER_ID, ...};
new gTaxiTotalFare[MAX_PLAYERS];

stock TaxiMeter_Init()
{
	for (new i = 0; i < MAX_PLAYERS; i++) {
		gTaxiMeterActive[i] = false;
		gTaxiFareRate[i] = DEFAULT_TAXI_FARE;
		gTaxiPassenger[i] = INVALID_PLAYER_ID;
		gTaxiTotalFare[i] = 0;
	}
	print("[TAXI] Sistem argometer tarif taksi & pemanggilan armada aktif.");
	return 1;
}

stock ProcessToggleTaxiMeter(playerid, fare)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;

	if (!IsPlayerInAnyVehicle(playerid) || GetPlayerState(playerid) != PLAYER_STATE_DRIVER) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus menjadi pengemudi armada Taksi.");
		return 0;
	}

	new veh = GetPlayerVehicleID(playerid);
	new model = GetVehicleModel(veh);
	if (model != 420 && model != 438) { // 420 = Taxi, 438 = Cabbie
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Kendaraan ini bukan armada taksi resmi kota (Taxi / Cabbie)!");
		return 0;
	}

	if (gTaxiMeterActive[playerid]) {
		// Matikan argometer
		gTaxiMeterActive[playerid] = false;
		SendClientMessage(playerid, COLOR_INFO, "INFO: Argometer tarif taksi dinonaktifkan.");
		return 1;
	}

	if (fare < 1 || fare > 20) fare = DEFAULT_TAXI_FARE;

	gTaxiMeterActive[playerid] = true;
	gTaxiFareRate[playerid] = fare;
	gTaxiTotalFare[playerid] = 0;

	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);

	new succMsg[160];
	format(succMsg, sizeof(succMsg),
		"[TAXI METER] Argometer taksi AKTIF dengan tarif: $%d / kilometer. Menunggu penumpang naik.", fare);
	SendClientMessage(playerid, COLOR_SUCCESS, succMsg);
	return 1;
}

stock ProcessCallTaxi(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;

	new Float:px, Float:py, Float:pz;
	GetPlayerPos(playerid, px, py, pz);

	new foundTaxi = 0;
	new taxiDispatch[180];
	format(taxiDispatch, sizeof(taxiDispatch),
		"[TAXI ORDER] Pesanan taksi baru dari %s! Gunakan rute GPS untuk menjemput penumpang.",
		gPlayerData[playerid][pName]);

	for (new p = 0; p < MAX_PLAYERS; p++) {
		if (IsPlayerConnected(p) && gPlayerData[p][pLoggedIn] && gTaxiMeterActive[p]) {
			SendClientMessage(p, 0xFFCC00FF, taxiDispatch);
			SetPlayerCheckpoint(p, px, py, pz, 5.0);
			PlayerPlaySound(p, 1057, 0.0, 0.0, 0.0);
			foundTaxi++;
		}
	}

	if (foundTaxi == 0) {
		SendClientMessage(playerid, COLOR_ERROR,
			"ERROR: Tidak ada pengemudi taksi dengan argometer aktif yang bertugas saat ini.");
		return 0;
	}

	SendClientMessage(playerid, COLOR_SUCCESS,
		"SUKSES: Panggilan taksi terkirim! Pengemudi taksi terdekat sedang menuju ke lokasi Anda.");
	return 1;
}
