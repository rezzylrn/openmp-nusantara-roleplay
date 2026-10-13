#if defined _INC_rp_vehicle_core
	#endinput
#endif
#define _INC_rp_vehicle_core

/*
==============================================================================
 vehicle_core.inc
 NUSANTARA ROLEPLAY - Core Dynamic Vehicles State, Engine, Fuel & Params
==============================================================================
*/

stock Vehicle_GetSlot(vehicleid)
{
	for (new i = 0; i < MAX_OWNED_VEHICLES; i++) {
		if (gVehicleData[i][vUsed] && gVehicleData[i][vVehicleHandle] == vehicleid) {
			return i;
		}
	}
	return -1;
}

stock Float:Vehicle_GetFuel(vehicleid)
{
	new slot = Vehicle_GetSlot(vehicleid);
	if (slot == -1) return 100.0;
	return gVehicleData[slot][vFuel];
}

stock Vehicle_SetFuel(vehicleid, Float:fuel)
{
	new slot = Vehicle_GetSlot(vehicleid);
	if (slot == -1) return 0;
	if (fuel < 0.0) fuel = 0.0;
	if (fuel > 100.0) fuel = 100.0;
	gVehicleData[slot][vFuel] = fuel;
	return 1;
}

stock Vehicle_IsEngineOn(vehicleid)
{
	if (vehicleid <= 0 || vehicleid >= MAX_VEHICLES) return 0;
	if (IsValidVehicle(vehicleid)) {
		new eng = -1, lights = -1, alarm = -1, doors = -1, bonnet = -1, boot = -1, objective = -1;
		GetVehicleParamsEx(vehicleid, eng, lights, alarm, doors, bonnet, boot, objective);
		if (eng == 1) return 1;
		if (eng == 0) return 0;
	}
	new slot = Vehicle_GetSlot(vehicleid);
	if (slot != -1) return (gVehicleData[slot][vEngine]) ? 1 : 0;
	return (gVehicleEngineState[vehicleid]) ? 1 : 0;
}

stock Vehicle_SetEngineOn(vehicleid, bool:engine)
{
	if (vehicleid <= 0 || vehicleid >= MAX_VEHICLES) return 0;
	new slot = Vehicle_GetSlot(vehicleid);
	if (slot != -1) gVehicleData[slot][vEngine] = engine;
	gVehicleEngineState[vehicleid] = engine;
	if (IsValidVehicle(vehicleid)) {
		new eng = -1, lights = -1, alarm = -1, doors = -1, bonnet = -1, boot = -1, objective = -1;
		GetVehicleParamsEx(vehicleid, eng, lights, alarm, doors, bonnet, boot, objective);
		SetVehicleParamsEx(vehicleid, (engine) ? 1 : 0, lights, alarm, doors, bonnet, boot, objective);
	}
	return 1;
}

stock Vehicle_IsLocked(vehicleid)
{
	new slot = Vehicle_GetSlot(vehicleid);
	if (slot == -1) return 0;
	return gVehicleData[slot][vLocked];
}

stock Vehicle_SetLocked(vehicleid, bool:locked)
{
	new slot = Vehicle_GetSlot(vehicleid);
	if (slot != -1) gVehicleData[slot][vLocked] = locked;
	new eng, lights, alarm, doors, bonnet, boot, objective;
	GetVehicleParamsEx(vehicleid, eng, lights, alarm, doors, bonnet, boot, objective);
	SetVehicleParamsEx(vehicleid, eng, lights, alarm, _:locked, bonnet, boot, objective);
	return 1;
}

stock Vehicle_SetLights(vehicleid, bool:lightsState)
{
	new slot = Vehicle_GetSlot(vehicleid);
	if (slot != -1) gVehicleData[slot][vLights] = lightsState;
	new eng, lights, alarm, doors, bonnet, boot, objective;
	GetVehicleParamsEx(vehicleid, eng, lights, alarm, doors, bonnet, boot, objective);
	SetVehicleParamsEx(vehicleid, eng, _:lightsState, alarm, doors, bonnet, boot, objective);
	return 1;
}

stock Vehicle_SetHood(vehicleid, bool:hoodState)
{
	new slot = Vehicle_GetSlot(vehicleid);
	if (slot != -1) gVehicleData[slot][vHood] = hoodState;
	new eng, lights, alarm, doors, bonnet, boot, objective;
	GetVehicleParamsEx(vehicleid, eng, lights, alarm, doors, bonnet, boot, objective);
	SetVehicleParamsEx(vehicleid, eng, lights, alarm, doors, _:hoodState, boot, objective);
	return 1;
}

stock Vehicle_SetTrunk(vehicleid, bool:trunkState)
{
	new slot = Vehicle_GetSlot(vehicleid);
	if (slot != -1) gVehicleData[slot][vTrunkOpen] = trunkState;
	new eng, lights, alarm, doors, bonnet, boot, objective;
	GetVehicleParamsEx(vehicleid, eng, lights, alarm, doors, bonnet, boot, objective);
	SetVehicleParamsEx(vehicleid, eng, lights, alarm, doors, bonnet, _:trunkState, objective);
	return 1;
}

// ---------------------------------------------------------------------------
// Spawn / Despawn Kendaraan
// ---------------------------------------------------------------------------
stock Vehicle_Spawn(slot)
{
	if (slot < 0 || slot >= MAX_OWNED_VEHICLES || !gVehicleData[slot][vUsed]) return -1;

	if (gVehicleData[slot][vVehicleHandle] > 0 && IsValidVehicle(gVehicleData[slot][vVehicleHandle])) {
		DestroyVehicle(gVehicleData[slot][vVehicleHandle]);
	}

	new handle = CreateVehicle(
		gVehicleData[slot][vModel],
		gVehicleData[slot][vPosX],
		gVehicleData[slot][vPosY],
		gVehicleData[slot][vPosZ],
		gVehicleData[slot][vPosA],
		gVehicleData[slot][vColor1],
		gVehicleData[slot][vColor2],
		0);

	gVehicleData[slot][vVehicleHandle] = handle;
	if (gVehicleData[slot][vPlate][0] != '\0') {
		SetVehicleNumberPlate(handle, gVehicleData[slot][vPlate]);
	}

	// Terapkan parameter
	Vehicle_SetEngineOn(handle, gVehicleData[slot][vEngine]);
	Vehicle_SetLocked(handle, gVehicleData[slot][vLocked]);
	Vehicle_SetLights(handle, gVehicleData[slot][vLights]);
	return handle;
}

stock Vehicle_SpawnAll()
{
	for (new i = 0; i < MAX_OWNED_VEHICLES; i++) {
		if (gVehicleData[i][vUsed]) Vehicle_Spawn(i);
	}
	return 1;
}

// ---------------------------------------------------------------------------
// Dealership (/buyveh)
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Perintah Kontrol Cepat: /park, /lock, /engine, /lights, /hood, /trunk
// ---------------------------------------------------------------------------
stock ProcessVehiclePark(playerid, slot)
{
	if (slot < 0 || slot >= MAX_OWNED_VEHICLES || !gVehicleData[slot][vUsed]) return 0;
	if (gVehicleData[slot][vOwnerID] != gPlayerData[playerid][pID]) {
		SendErr(playerid, "Ini bukan kendaraan milik Anda.");
		return 0;
	}

	new handle = gVehicleData[slot][vVehicleHandle];
	if (handle <= 0 || !IsValidVehicle(handle)) {
		SendErr(playerid, "Kendaraan tidak berada di dunia.");
		return 0;
	}

	new Float:vx, Float:vy, Float:vz, Float:va;
	GetVehiclePos(handle, vx, vy, vz);
	GetVehicleZAngle(handle, va);

	gVehicleData[slot][vPosX] = vx;
	gVehicleData[slot][vPosY] = vy;
	gVehicleData[slot][vPosZ] = vz;
	gVehicleData[slot][vPosA] = va;

	DB_SaveVehicle(slot);
	SendSucc(playerid, "Posisi parkir permanen kendaraan berhasil diperbarui.");
	return 1;
}

stock ProcessVehicleLock(playerid, slot)
{
	if (slot < 0 || slot >= MAX_OWNED_VEHICLES || !gVehicleData[slot][vUsed]) return 0;
	if (gVehicleData[slot][vOwnerID] != gPlayerData[playerid][pID]) {
		SendErr(playerid, "Ini bukan kendaraan milik Anda.");
		return 0;
	}

	new handle = gVehicleData[slot][vVehicleHandle];
	if (handle <= 0 || !IsValidVehicle(handle)) return 0;

	new bool:newlock = !gVehicleData[slot][vLocked];
	Vehicle_SetLocked(handle, newlock);

	new msg[128];
	format(msg, sizeof(msg), "* %s %s pintu kendaraan miliknya.",
		gPlayerData[playerid][pName], newlock ? "mengunci" : "membuka kunci");
	SendNearbyPlayer(playerid, COLOR_RP_ME, msg, RANGE_ME);
	return 1;
}

stock ProcessEngineToggle(playerid)
{
	if (!IsPlayerInAnyVehicle(playerid) || GetPlayerState(playerid) != PLAYER_STATE_DRIVER) {
		SendErr(playerid, "Anda harus berada di kursi pengemudi kendaraan.");
		return 0;
	}
	new handle = GetPlayerVehicleID(playerid);
	new slot = Vehicle_GetSlot(handle);

	// Cek bensin
	if (slot != -1 && gVehicleData[slot][vFuel] <= 0.0) {
		SendErr(playerid, "Mesin tidak dapat menyala: Tangki bahan bakar kosong.");
		return 0;
	}

	// Cek kerusakan mesin
	new Float:vhp;
	GetVehicleHealth(handle, vhp);
	if (vhp < 350.0) {
		SendErr(playerid, "Mesin rusak parah dan mengeluarkan asap. Gunakan Alat Perbaikan atau panggil Mekanik.");
		return 0;
	}

	new bool:newengine = !Vehicle_IsEngineOn(handle);
	Vehicle_SetEngineOn(handle, newengine);

	new msg[128];
	format(msg, sizeof(msg), "* %s %s mesin kendaraan.",
		gPlayerData[playerid][pName], newengine ? "menyalakan" : "mematikan");
	SendNearbyPlayer(playerid, COLOR_RP_ME, msg, RANGE_ME);
	return 1;
}

stock ProcessLightsToggle(playerid)
{
	if (!IsPlayerInAnyVehicle(playerid) || GetPlayerState(playerid) != PLAYER_STATE_DRIVER) {
		SendErr(playerid, "Anda harus berada di kursi pengemudi kendaraan.");
		return 0;
	}
	new handle = GetPlayerVehicleID(playerid);
	new slot = Vehicle_GetSlot(handle);

	new bool:curLights = (slot != -1) ? gVehicleData[slot][vLights] : false;
	new bool:newLights = !curLights;
	Vehicle_SetLights(handle, newLights);

	SendRP(playerid, COLOR_INFO, "Lampu depan kendaraan %s.", newLights ? "dinyalakan" : "dimatikan");
	return 1;
}

stock ProcessRefuel(playerid)
{
	if (!IsPlayerInAnyVehicle(playerid) || GetPlayerState(playerid) != PLAYER_STATE_DRIVER) {
		SendErr(playerid, "Anda harus berada di kursi pengemudi kendaraan.");
		return 0;
	}
	if (!IsPlayerNearPoint(playerid, FUEL_STATION_X, FUEL_STATION_Y, FUEL_STATION_Z, RANGE_FUEL)) {
		SendErr(playerid, "Anda harus berada di SPBU Stasiun Bahan Bakar (/gps).");
		return 0;
	}

	new handle = GetPlayerVehicleID(playerid);
	new slot = Vehicle_GetSlot(handle);
	if (slot == -1) {
		SendErr(playerid, "Kendaraan dinas ini tidak memerlukan pengisian bensin berbayar.");
		return 0;
	}

	new Float:needed = 100.0 - gVehicleData[slot][vFuel];
	if (needed <= 1.0) {
		SendErr(playerid, "Tangki bahan bakar sudah penuh.");
		return 0;
	}

	new cost = floatround(needed * FUEL_PRICE);
	if (gPlayerData[playerid][pMoney] < cost) {
		SendErr(playerid, "Uang tunai Anda tidak cukup ($%d diperlukan untuk %.0f Liter).", cost, needed);
		return 0;
	}

	TakePlayerMoneyEx(playerid, cost, "pengisian bensin");
	gVehicleData[slot][vFuel] = 100.0;
	DB_SaveVehicle(slot);

	SendSucc(playerid, "Pengisian bensin selesai! Tangki penuh (100L) dengan total biaya $%d.", cost);
	return 1;
}

// Loop penurunan bahan bakar
stock UpdateVehicleFuelLoop()
{
	for (new i = 0; i < MAX_OWNED_VEHICLES; i++) {
		if (!gVehicleData[i][vUsed]) continue;
		new handle = gVehicleData[i][vVehicleHandle];
		if (handle <= 0 || !IsValidVehicle(handle)) continue;

		if (gVehicleData[i][vEngine]) {
			gVehicleData[i][vFuel] -= FUEL_CONSUMPTION_RATE;
			if (gVehicleData[i][vFuel] <= 0.0) {
				gVehicleData[i][vFuel] = 0.0;
				Vehicle_SetEngineOn(handle, false);
			}
		}
	}
	return 1;
}

