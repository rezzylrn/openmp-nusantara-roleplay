#if defined _INC_rp_tire_service
	#endinput
#endif
#define _INC_rp_tire_service

/*
==============================================================================
 tire_service.inc
 NUSANTARA ROLEPLAY - Roadside Flat Tire Repair & Emergency Fuel Canister Pouring
==============================================================================
*/

stock TireService_Init()
{
	print("[TIRE SERVICE] Sistem tambal ban darurat & tuang jerigen bensin aktif.");
	return 1;
}

stock ProcessRepairTire(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;

	if (IsPlayerInAnyVehicle(playerid)) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus keluar dan berdiri di dekat roda kendaraan untuk menambal ban.");
		return 0;
	}

	if (GetPlayerItemCount(playerid, ITEM_REPAIRKIT) <= 0) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda memerlukan Alat Perbaikan (Repairkit) di inventaris. Beli di toko 24/7.");
		return 0;
	}

	new Float:px, Float:py, Float:pz;
	GetPlayerPos(playerid, px, py, pz);

	new targetVeh = INVALID_VEHICLE_ID;
	new Float:minDist = 999.0;

	for (new v = 1; v < MAX_VEHICLES; v++) {
		new Float:vx, Float:vy, Float:vz;
		GetVehiclePos(v, vx, vy, vz);
		new Float:dist = VectorSize(px - vx, py - vy, pz - vz);
		if (dist < 4.5 && dist < minDist) {
			minDist = dist;
			targetVeh = v;
		}
	}

	if (targetVeh == INVALID_VEHICLE_ID) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Tidak ada kendaraan dalam jarak 4.5 meter di dekat Anda.");
		return 0;
	}

	new VEHICLE_PANEL_STATUS:panels, VEHICLE_DOOR_STATUS:doors, VEHICLE_LIGHT_STATUS:lights, VEHICLE_TYRE_STATUS:tires;
	GetVehicleDamageStatus(targetVeh, panels, doors, lights, tires);

	// Perbaiki seluruh ban kempes (tires = 0)
	UpdateVehicleDamageStatus(targetVeh, panels, doors, lights, VEHICLE_TYRE_STATUS:0);

	// Animasi mekanik tambal ban
	ApplyAnimation(playerid, "CAR_CHAT", "car_talkm_loop", 4.1, false, false, false, false, 0);
	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);

	TakePlayerItem(playerid, ITEM_REPAIRKIT, 1);

	SendClientMessage(playerid, COLOR_SUCCESS,
		"SUKSES: Ban kempes kendaraan berhasil ditambal dan tekanan angin terisi normal!");
	return 1;
}

stock ProcessPourFuelCanister(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;

	if (IsPlayerInAnyVehicle(playerid)) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus berdiri di samping lubang tangki bensin mobil.");
		return 0;
	}

	if (GetPlayerItemCount(playerid, ITEM_JERRYCAN) <= 0) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda memerlukan Jerigen Bensin (20L) di inventaris. Beli di SPBU atau toko 24/7.");
		return 0;
	}

	new Float:px, Float:py, Float:pz;
	GetPlayerPos(playerid, px, py, pz);

	new targetSlot = -1;
	new Float:minDist = 999.0;

	for (new i = 0; i < MAX_OWNED_VEHICLES; i++) {
		if (gVehicleData[i][vUsed] && gVehicleData[i][vVehicleHandle] > 0) {
			new Float:vx, Float:vy, Float:vz;
			GetVehiclePos(gVehicleData[i][vVehicleHandle], vx, vy, vz);
			new Float:dist = VectorSize(px - vx, py - vy, pz - vz);
			if (dist < 4.5 && dist < minDist) {
				minDist = dist;
				targetSlot = i;
			}
		}
	}

	if (targetSlot == -1) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Tidak ada kendaraan pribadi terdekat untuk diisi bensin.");
		return 0;
	}

	if (gVehicleData[targetSlot][vFuel] >= 95.0) {
		SendClientMessage(playerid, COLOR_INFO, "INFO: Tangki bahan bakar kendaraan ini masih penuh.");
		return 0;
	}

	// Tuang 20 liter bensin
	gVehicleData[targetSlot][vFuel] += 20.0;
	if (gVehicleData[targetSlot][vFuel] > 100.0) gVehicleData[targetSlot][vFuel] = 100.0;
	DB_SaveVehicle(targetSlot);

	TakePlayerItem(playerid, ITEM_JERRYCAN, 1);

	ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.1, false, false, false, false, 0);
	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);

	new succMsg[160];
	format(succMsg, sizeof(succMsg),
		"SUKSES: Menuangkan 20 Liter bensin cadangan ke tangki kendaraan (Bahan bakar sekarang: %.0f Liter).",
		gVehicleData[targetSlot][vFuel]);
	SendClientMessage(playerid, COLOR_SUCCESS, succMsg);
	return 1;
}
