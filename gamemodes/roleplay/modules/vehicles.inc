#if defined _INC_rp_vehicles
	#endinput
#endif
#define _INC_rp_vehicles

/*
==============================================================================
  NUSANTARA ROLEPLAY - vehicles.inc
  Sistem Kendaraan Dinamis: Mesin, Lampu, Kap, Bagasi, Kunci, Bahan Bakar,
  Plat Nomor, Kondisi Kerusakan & Dealership
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
	new slot = Vehicle_GetSlot(vehicleid);
	if (slot == -1) return 1;
	return gVehicleData[slot][vEngine];
}

stock Vehicle_SetEngineOn(vehicleid, bool:engine)
{
	new slot = Vehicle_GetSlot(vehicleid);
	if (slot != -1) gVehicleData[slot][vEngine] = engine;
	new eng, lights, alarm, doors, bonnet, boot, objective;
	GetVehicleParamsEx(vehicleid, eng, lights, alarm, doors, bonnet, boot, objective);
	SetVehicleParamsEx(vehicleid, _:engine, lights, alarm, doors, bonnet, boot, objective);
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
stock ShowDealershipDialog(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (!IsPlayerNearPoint(playerid, DEALER_X, DEALER_Y, DEALER_Z, RANGE_DEALER)) {
		SendErr(playerid, "Anda harus berada di showroom Dealership (/gps).");
		return 0;
	}

	new list[700], vname[32];
	format(list, sizeof(list), "Model Kendaraan\tHarga\tKategori\n");
	for (new i = 0; i < MAX_DEALER_STOCK; i++) {
		GetVehicleModelName(gDealerStock[i][0], vname, sizeof(vname));
		format(list, sizeof(list), "%s%s\t$%d\tShowroom Resmi\n", list, vname, gDealerStock[i][1]);
	}

	ShowPlayerDialog(playerid, DIALOG_DEALERSHIP, DIALOG_STYLE_TABLIST_HEADERS,
		"DEALERSHIP LOS SANTOS", list, "Beli", "Batal");
	return 1;
}

stock OnDealershipResponse(playerid, listitem)
{
	if (listitem < 0 || listitem >= MAX_DEALER_STOCK) return 0;
	new price = gDealerStock[listitem][1];
	new model = gDealerStock[listitem][0];

	if (gPlayerData[playerid][pMoney] < price) {
		SendErr(playerid, "Uang tunai Anda tidak mencukupi ($%d diperlukan).", price);
		return 0;
	}

	new slot = -1;
	for (new i = 0; i < MAX_OWNED_VEHICLES; i++) {
		if (!gVehicleData[i][vUsed]) {
			slot = i;
			break;
		}
	}
	if (slot == -1) {
		SendErr(playerid, "Garasi kota sedang penuh.");
		return 0;
	}

	TakePlayerMoneyEx(playerid, price, "beli kendaraan");

	new Float:px, Float:py, Float:pz, Float:pa;
	GetPlayerPos(playerid, px, py, pz);
	GetPlayerFacingAngle(playerid, pa);

	// Generate plat nomor acak
	new plate[16];
	format(plate, sizeof(plate), "LS-%04d", 1000 + random(8999));

	new dbid = DB_CreateVehicleRow(gPlayerData[playerid][pID], model, px + 2.5, py + 2.5, pz, pa,
		gDealerStock[listitem][2], gDealerStock[listitem][3]);

	gVehicleData[slot][vUsed] = true;
	gVehicleData[slot][vDBID] = dbid;
	gVehicleData[slot][vOwnerID] = gPlayerData[playerid][pID];
	gVehicleData[slot][vModel] = model;
	gVehicleData[slot][vPosX] = px + 2.5;
	gVehicleData[slot][vPosY] = py + 2.5;
	gVehicleData[slot][vPosZ] = pz;
	gVehicleData[slot][vPosA] = pa;
	gVehicleData[slot][vColor1] = gDealerStock[listitem][2];
	gVehicleData[slot][vColor2] = gDealerStock[listitem][3];
	gVehicleData[slot][vFuel] = 100.0;
	gVehicleData[slot][vHealth] = 1000.0;
	format(gVehicleData[slot][vPlate], 16, "%s", plate);
	gVehicleData[slot][vEngine] = false;
	gVehicleData[slot][vLocked] = false;
	gVehicleData[slot][vLights] = false;
	gVehicleData[slot][vHood] = false;
	gVehicleData[slot][vTrunkOpen] = false;
	gVehicleData[slot][vTrunk] = 0;

	Vehicle_Spawn(slot);

	new vname[32];
	GetVehicleModelName(model, vname, sizeof(vname));
	SendSucc(playerid, "Anda resmi membeli 1 unit %s (Plat: %s) seharga $%d.", vname, plate, price);
	SendInfo(playerid, "Gunakan /engine untuk menyalakan mesin dan /lock untuk mengunci pintu.");
	return 1;
}

// ---------------------------------------------------------------------------
// Manajemen Kendaraan (/myveh atau /v)
// ---------------------------------------------------------------------------
stock ShowMyVehiclesDialog(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	new list[700], vname[32], count = 0;
	format(list, sizeof(list), "Slot\tModel\tBahan Bakar\tPlat Nomor\n");

	for (new i = 0; i < MAX_OWNED_VEHICLES; i++) {
		if (gVehicleData[i][vUsed] && gVehicleData[i][vOwnerID] == gPlayerData[playerid][pID]) {
			GetVehicleModelName(gVehicleData[i][vModel], vname, sizeof(vname));
			format(list, sizeof(list), "%s#%d\t%s\t%.0f Liter\t%s\n",
				list, i + 1, vname, gVehicleData[i][vFuel], gVehicleData[i][vPlate]);
			count++;
		}
	}

	if (count == 0) {
		SendInfo(playerid, "Anda belum memiliki kendaraan pribadi. Kunjungi dealership (/gps).");
		return 0;
	}

	ShowPlayerDialog(playerid, DIALOG_MYVEH, DIALOG_STYLE_TABLIST_HEADERS,
		"KENDARAAN PRIBADI SAYA", list, "Pilih", "Batal");
	return 1;
}

stock ShowVehicleActionDialog(playerid, slot)
{
	gPlayerVehicleSlot[playerid] = slot;
	new vname[32];
	GetVehicleModelName(gVehicleData[slot][vModel], vname, sizeof(vname));

	new header[64];
	format(header, sizeof(header), "PILIHAN - %s (%s)", vname, gVehicleData[slot][vPlate]);

	ShowPlayerDialog(playerid, DIALOG_MYVEH_ACTION, DIALOG_STYLE_LIST,
		header,
		"1. Lacak Posisi di GPS\n\
2. Panggil ke Posisi Saya\n\
3. Simpan Posisi Parkir (/park)\n\
4. Kunci / Buka Kunci Pintu (/lock)\n\
5. Buka / Tutup Kap Mesin (/hood)\n\
6. Buka / Tutup Bagasi (/trunk)",
		"Pilih", "Kembali");
	return 1;
}

stock OnMyVehActionResponse(playerid, listitem)
{
	new slot = gPlayerVehicleSlot[playerid];
	if (slot < 0 || slot >= MAX_OWNED_VEHICLES || !gVehicleData[slot][vUsed]) return 0;
	new handle = gVehicleData[slot][vVehicleHandle];

	switch (listitem) {
		case 0: { // Lacak GPS
			if (handle > 0 && IsValidVehicle(handle)) {
				new Float:vx, Float:vy, Float:vz;
				GetVehiclePos(handle, vx, vy, vz);
				SetPlayerCheckpoint(playerid, vx, vy, vz, 6.0);
				SendSucc(playerid, "Lokasi kendaraan telah ditandai di peta radar (checkpoint merah).");
			} else {
				SendErr(playerid, "Kendaraan sedang tidak aktif di dunia.");
			}
		}
		case 1: { // Panggil
			new Float:px, Float:py, Float:pz, Float:pa;
			GetPlayerPos(playerid, px, py, pz);
			GetPlayerFacingAngle(playerid, pa);
			gVehicleData[slot][vPosX] = px + 2.0;
			gVehicleData[slot][vPosY] = py + 2.0;
			gVehicleData[slot][vPosZ] = pz;
			gVehicleData[slot][vPosA] = pa;
			Vehicle_Spawn(slot);
			SendSucc(playerid, "Kendaraan telah dipanggil ke dekat posisi Anda.");
		}
		case 2: { // Parkir
			ProcessVehiclePark(playerid, slot);
		}
		case 3: { // Kunci / Buka
			ProcessVehicleLock(playerid, slot);
		}
		case 4: { // Kap mesin
			if (handle <= 0 || !IsValidVehicle(handle)) return 0;
			new bool:newhood = !gVehicleData[slot][vHood];
			Vehicle_SetHood(handle, newhood);
			SendRP(playerid, COLOR_INFO, "Kap mesin kendaraan %s.", newhood ? "dibuka" : "ditutup");
		}
		case 5: { // Bagasi
			if (handle <= 0 || !IsValidVehicle(handle)) return 0;
			new bool:newtrunk = !gVehicleData[slot][vTrunkOpen];
			Vehicle_SetTrunk(handle, newtrunk);
			SendRP(playerid, COLOR_INFO, "Pintu bagasi kendaraan %s.", newtrunk ? "dibuka" : "ditutup");
		}
	}
	return 1;
}

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
