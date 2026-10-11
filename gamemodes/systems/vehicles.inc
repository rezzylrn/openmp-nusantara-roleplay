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
		gDealerStock[listitem][2], gDealerStock[listitem][3], plate);

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

// ---------------------------------------------------------------------------
// Sistem Bagasi Kendaraan (Trunk Storage & Interaksi)
// ---------------------------------------------------------------------------
stock Vehicle_FindNearest(playerid, Float:radi)
{
	new Float:px, Float:py, Float:pz;
	GetPlayerPos(playerid, px, py, pz);

	new closestSlot = -1;
	new Float:minDist = radi;

	for (new i = 0; i < MAX_OWNED_VEHICLES; i++) {
		if (gVehicleData[i][vUsed]) {
			new handle = gVehicleData[i][vVehicleHandle];
			if (handle > 0 && IsValidVehicle(handle)) {
				new Float:vx, Float:vy, Float:vz;
				GetVehiclePos(handle, vx, vy, vz);
				new Float:d = floatsqroot((px-vx)*(px-vx) + (py-vy)*(py-vy) + (pz-vz)*(pz-vz));
				if (d < minDist) {
					minDist = d;
					closestSlot = i;
				}
			}
		}
	}
	return closestSlot;
}

stock ShowVehicleTrunkDialog(playerid, slot)
{
	if (slot < 0 || slot >= MAX_OWNED_VEHICLES || !gVehicleData[slot][vUsed]) return 0;
	gSelectedTrunkVeh[playerid] = slot;

	new bool:isEn = (gPlayerData[playerid][pLanguage] == LANG_EN);
	new str[700];
	new itemStr[32];
	if (isEn) {
		format(str, sizeof(str), "Item / Option	Quantity\n{4CD964}[$] Cash in Trunk	$%d\n", gVehicleData[slot][vTrunk]);
	} else {
		format(str, sizeof(str), "Item / Opsi	Jumlah\n{4CD964}[$] Saldo Uang Tunai Bagasi	$%d\n", gVehicleData[slot][vTrunk]);
	}

	// Slot Senjata Api
	new gunName[32];
	if (gVehicleData[slot][vTrunkGun] != WEAPON:0 && gVehicleData[slot][vTrunkAmmo] > 0) {
		GetWeaponName(gVehicleData[slot][vTrunkGun], gunName, sizeof(gunName));
		format(str, sizeof(str), "%s{FF3B30}[SENJATA] %s	{FFCC00}%d Peluru\n", str, gunName, gVehicleData[slot][vTrunkAmmo]);
	} else {
		format(str, sizeof(str), isEn ? "%s{888888}[WEAPON SLOT] (Empty)	{888888}-\n" : "%s{888888}[SLOT SENJATA] (Kosong)	{888888}-\n", str);
	}

	for (new s = 0; s < MAX_TRUNK_SLOTS; s++) {
		new iType = gVehicleData[slot][vTrunkItemType][s];
		new iAmt  = gVehicleData[slot][vTrunkItemAmount][s];
		if (iType != ITEM_NONE && iAmt > 0) {
			GetItemName(iType, itemStr, sizeof(itemStr));
			format(str, sizeof(str), "%s{FFFF00}[Slot %d] %s	{FFFFFF}x%d\n", str, s + 1, itemStr, iAmt);
		} else {
			if (isEn) format(str, sizeof(str), "%s{888888}[Slot %d] (Empty)	{888888}-\n", str, s + 1);
			else format(str, sizeof(str), "%s{888888}[Slot %d] (Kosong)	{888888}-\n", str, s + 1);
		}
	}
	if (isEn) {
		format(str, sizeof(str), "%s{33CCFF}[+] STORE ITEM INTO TRUNK	{33CCFF}[INVENTORY]", str);
	} else {
		format(str, sizeof(str), "%s{33CCFF}[+] SIMPAN BARANG KE BAGASI	{33CCFF}[KANTUNG]", str);
	}

	new vName[32];
	GetVehicleModelName(gVehicleData[slot][vModel], vName, sizeof(vName));
	new title[64];
	if (isEn) format(title, sizeof(title), "TRUNK - %s (%s)", vName, gVehicleData[slot][vPlate]);
	else format(title, sizeof(title), "BAGASI - %s (%s)", vName, gVehicleData[slot][vPlate]);

	if (isEn) {
		ShowPlayerDialog(playerid, DIALOG_TRUNK, DIALOG_STYLE_TABLIST_HEADERS,
			title, str, "Select", "Close");
	} else {
		ShowPlayerDialog(playerid, DIALOG_TRUNK, DIALOG_STYLE_TABLIST_HEADERS,
			title, str, "Pilih", "Tutup");
	}
	return 1;
}

stock OnVehicleTrunkResponse(playerid, listitem)
{
	new slot = gSelectedTrunkVeh[playerid];
	if (slot < 0 || slot >= MAX_OWNED_VEHICLES || !gVehicleData[slot][vUsed]) return 0;

	// Opsi 0: Saldo Uang Tunai
	if (listitem == 0) {
		new prompt[144];
		format(prompt, sizeof(prompt), "1. Simpan Uang Tunai ke Bagasi (Dompet: $%d)\n2. Ambil Uang Tunai dari Bagasi (Tersedia: $%d)",
			gPlayerData[playerid][pMoney], gVehicleData[slot][vTrunk]);
		ShowPlayerDialog(playerid, DIALOG_TRUNK_DEPOSIT, DIALOG_STYLE_LIST, "KELOLA UANG BAGASI", prompt, "Pilih", "Kembali");
		return 1;
	}

	// Opsi 1: Slot Senjata Api
	if (listitem == 1) {
		new gunPrompt[256];
		new gunName[32];
		if (gVehicleData[slot][vTrunkGun] != WEAPON:0 && gVehicleData[slot][vTrunkAmmo] > 0) {
			GetWeaponName(gVehicleData[slot][vTrunkGun], gunName, sizeof(gunName));
			format(gunPrompt, sizeof(gunPrompt), "1. Ambil Senjata (%s - %d Peluru)\n2. Ganti / Simpan Senjata Yang Sedang Dipegang",
				gunName, gVehicleData[slot][vTrunkAmmo]);
		} else {
			format(gunPrompt, sizeof(gunPrompt), "1. Simpan Senjata Yang Sedang Dipegang ke Bagasi\n2. (Bagasi senjata sedang kosong)");
		}
		ShowPlayerDialog(playerid, DIALOG_TRUNK_WEAPON, DIALOG_STYLE_LIST, "KELOLA SENJATA BAGASI", gunPrompt, "Pilih", "Kembali");
		return 1;
	}

	// Opsi 2..7: Slot Item Bagasi (Ambil Barang)
	if (listitem >= 2 && listitem <= (MAX_TRUNK_SLOTS + 1)) {
		new s = listitem - 2;
		if (gVehicleData[slot][vTrunkItemType][s] == ITEM_NONE || gVehicleData[slot][vTrunkItemAmount][s] <= 0) {
			SendErr(playerid, "Slot bagasi ini kosong.");
			ShowVehicleTrunkDialog(playerid, slot);
			return 1;
		}
		gSelectedTrunkSlot[playerid] = s;
		new itemName[32];
		GetItemName(gVehicleData[slot][vTrunkItemType][s], itemName, sizeof(itemName));
		new prompt[128];
		format(prompt, sizeof(prompt), "Masukkan jumlah %s yang ingin diambil (Tersedia: %d):",
			itemName, gVehicleData[slot][vTrunkItemAmount][s]);
		ShowPlayerDialog(playerid, DIALOG_TRUNK_WITHDRAW_ITEM, DIALOG_STYLE_INPUT, "AMBIL BARANG DARI BAGASI", prompt, "Ambil", "Batal");
		return 1;
	}

	// Opsi Terakhir: [+] Simpan Barang ke Bagasi
	if (listitem == (MAX_TRUNK_SLOTS + 2)) {
		new dialogList[800];
		new itemName[32];
		dialogList[0] = '\0';
		for (new s = 0; s < MAX_INVENTORY_SLOTS; s++) {
			new iType = gPlayerItems[playerid][s][itType];
			new iAmt  = gPlayerItems[playerid][s][itAmount];
			if (iType != ITEM_NONE && iAmt > 0) {
				GetItemName(iType, itemName, sizeof(itemName));
				format(dialogList, sizeof(dialogList), "%s[Slot %d] %s (x%d)\n", dialogList, s + 1, itemName, iAmt);
			} else {
				format(dialogList, sizeof(dialogList), "%s[Slot %d] (Kosong)\n", dialogList, s + 1);
			}
		}
		ShowPlayerDialog(playerid, DIALOG_TRUNK_DEPOSIT_ITEM, DIALOG_STYLE_LIST, "PILIH BARANG DARI KANTUNG", dialogList, "Simpan", "Kembali");
		return 1;
	}
	return 1;
}

stock OnTrunkDepositCashResponse(playerid, listitem)
{
	new slot = gSelectedTrunkVeh[playerid];
	if (slot < 0 || slot >= MAX_OWNED_VEHICLES || !gVehicleData[slot][vUsed]) return 0;

	if (listitem == 0) {
		// Simpan Uang ke Bagasi
		ShowPlayerDialog(playerid, DIALOG_TRUNK_DEPOSIT, DIALOG_STYLE_INPUT,
			"SIMPAN UANG KE BAGASI", "Masukkan nominal uang tunai yang ingin disimpan ke bagasi ($1 - $5.000.000):", "Simpan", "Kembali");
	} else if (listitem == 1) {
		// Ambil Uang dari Bagasi
		ShowPlayerDialog(playerid, DIALOG_TRUNK_TAKE_CASH, DIALOG_STYLE_INPUT,
			"AMBIL UANG DARI BAGASI", "Masukkan nominal uang tunai yang ingin diambil dari bagasi ($1 - $5.000.000):", "Ambil", "Kembali");
	}
	return 1;
}

stock OnTrunkSaveCashInput(playerid, const inputtext[])
{
	new slot = gSelectedTrunkVeh[playerid];
	if (slot < 0 || slot >= MAX_OWNED_VEHICLES || !gVehicleData[slot][vUsed]) return 0;

	new amount = strval(inputtext);
	if (amount < 1 || amount > 5000000) {
		SendErr(playerid, "Nominal tidak valid ($1 - $5.000.000).");
		ShowVehicleTrunkDialog(playerid, slot);
		return 0;
	}
	if (gPlayerData[playerid][pMoney] < amount) {
		SendErr(playerid, "Uang tunai Anda tidak mencukupi.");
		ShowVehicleTrunkDialog(playerid, slot);
		return 0;
	}

	TakePlayerMoneyEx(playerid, amount, "simpan bagasi");
	gVehicleData[slot][vTrunk] += amount;
	DB_SaveVehicle(slot);

	SendSucc(playerid, "Berhasil menyimpan $%d ke dalam bagasi kendaraan.", amount);
	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
	ShowVehicleTrunkDialog(playerid, slot);
	return 1;
}

stock OnTrunkTakeCashInput(playerid, const inputtext[])
{
	new slot = gSelectedTrunkVeh[playerid];
	if (slot < 0 || slot >= MAX_OWNED_VEHICLES || !gVehicleData[slot][vUsed]) return 0;

	new amount = strval(inputtext);
	if (amount < 1 || amount > 5000000) {
		SendErr(playerid, "Nominal tidak valid ($1 - $5.000.000).");
		ShowVehicleTrunkDialog(playerid, slot);
		return 0;
	}
	if (gVehicleData[slot][vTrunk] < amount) {
		SendErr(playerid, "Saldo uang di bagasi tidak mencukupi.");
		ShowVehicleTrunkDialog(playerid, slot);
		return 0;
	}

	gVehicleData[slot][vTrunk] -= amount;
	GivePlayerMoneyEx(playerid, amount, "ambil bagasi");
	DB_SaveVehicle(slot);

	SendSucc(playerid, "Berhasil mengambil $%d dari bagasi kendaraan.", amount);
	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
	ShowVehicleTrunkDialog(playerid, slot);
	return 1;
}

stock OnTrunkDepositItemResponse(playerid, listitem)
{
	new slot = gSelectedTrunkVeh[playerid];
	if (slot < 0 || slot >= MAX_OWNED_VEHICLES || !gVehicleData[slot][vUsed]) return 0;

	if (listitem < 0 || listitem >= MAX_INVENTORY_SLOTS) return 0;
	new iType = gPlayerItems[playerid][listitem][itType];
	new iAmt  = gPlayerItems[playerid][listitem][itAmount];

	if (iType == ITEM_NONE || iAmt <= 0) {
		SendErr(playerid, "Slot kantung ini kosong.");
		ShowVehicleTrunkDialog(playerid, slot);
		return 1;
	}

	// Cari slot kosong atau tipe sama di bagasi
	new targetTrunkSlot = -1;
	for (new s = 0; s < MAX_TRUNK_SLOTS; s++) {
		if (gVehicleData[slot][vTrunkItemType][s] == iType) {
			targetTrunkSlot = s;
			break;
		}
	}
	if (targetTrunkSlot == -1) {
		for (new s = 0; s < MAX_TRUNK_SLOTS; s++) {
			if (gVehicleData[slot][vTrunkItemType][s] == ITEM_NONE) {
				targetTrunkSlot = s;
				break;
			}
		}
	}

	if (targetTrunkSlot == -1) {
		SendErr(playerid, "Bagasi kendaraan sudah penuh (Maksimal 6 slot barang).");
		ShowVehicleTrunkDialog(playerid, slot);
		return 1;
	}

	// Pindahkan seluruh stack atau 1..5 unit
	new transferAmt = (iAmt > 5) ? 5 : iAmt;
	TakePlayerItem(playerid, iType, transferAmt);

	gVehicleData[slot][vTrunkItemType][targetTrunkSlot] = iType;
	gVehicleData[slot][vTrunkItemAmount][targetTrunkSlot] += transferAmt;
	DB_SaveVehicle(slot);

	new itemName[32];
	GetItemName(iType, itemName, sizeof(itemName));
	SendSucc(playerid, "Menyimpan %dx %s ke dalam bagasi kendaraan.", transferAmt, itemName);
	PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
	ShowVehicleTrunkDialog(playerid, slot);
	return 1;
}

stock OnTrunkWithdrawItemResponse(playerid, const inputtext[])
{
	new slot = gSelectedTrunkVeh[playerid];
	if (slot < 0 || slot >= MAX_OWNED_VEHICLES || !gVehicleData[slot][vUsed]) return 0;

	new trunkSlot = gSelectedTrunkSlot[playerid];
	if (trunkSlot < 0 || trunkSlot >= MAX_TRUNK_SLOTS) return 0;

	new iType = gVehicleData[slot][vTrunkItemType][trunkSlot];
	new iAmt  = gVehicleData[slot][vTrunkItemAmount][trunkSlot];

	if (iType == ITEM_NONE || iAmt <= 0) {
		SendErr(playerid, "Barang tidak ditemukan di bagasi.");
		ShowVehicleTrunkDialog(playerid, slot);
		return 1;
	}

	new takeAmt = strval(inputtext);
	if (takeAmt <= 0) takeAmt = 1;
	if (takeAmt > iAmt) takeAmt = iAmt;

	gVehicleData[slot][vTrunkItemAmount][trunkSlot] -= takeAmt;
	if (gVehicleData[slot][vTrunkItemAmount][trunkSlot] <= 0) {
		gVehicleData[slot][vTrunkItemType][trunkSlot] = ITEM_NONE;
		gVehicleData[slot][vTrunkItemAmount][trunkSlot] = 0;
	}

	GivePlayerItem(playerid, iType, takeAmt);
	DB_SaveVehicle(slot);

	new itemName[32];
	GetItemName(iType, itemName, sizeof(itemName));
	SendSucc(playerid, "Mengambil %dx %s dari bagasi kendaraan ke dalam kantung.", takeAmt, itemName);
	PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
	ShowVehicleTrunkDialog(playerid, slot);
	return 1;
}

stock OnTrunkWeaponResponse(playerid, listitem)
{
	new slot = gSelectedTrunkVeh[playerid];
	if (slot < 0 || slot >= MAX_OWNED_VEHICLES || !gVehicleData[slot][vUsed]) return 0;

	if (listitem == 0) {
		// Ambil Senjata dari Bagasi
		if (gVehicleData[slot][vTrunkGun] != WEAPON:0 && gVehicleData[slot][vTrunkAmmo] > 0) {
			new WEAPON:gun = gVehicleData[slot][vTrunkGun];
			new ammo = gVehicleData[slot][vTrunkAmmo];
			gVehicleData[slot][vTrunkGun] = WEAPON:0;
			gVehicleData[slot][vTrunkAmmo] = 0;
			DB_SaveVehicle(slot);

			GivePlayerWeapon(playerid, gun, ammo);
			new gunName[32];
			GetWeaponName(gun, gunName, sizeof(gunName));
			SendSucc(playerid, "Mengambil senjata dari bagasi kendaraan.");
			PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
		} else {
			// Simpan Senjata yang sedang dipegang
			new WEAPON:curGun = GetPlayerWeapon(playerid);
			new curAmmo = GetPlayerAmmo(playerid);
			if (curGun == WEAPON_FIST || curAmmo <= 0) {
				SendErr(playerid, "Anda tidak sedang memegang senjata api berpeluru.");
				ShowVehicleTrunkDialog(playerid, slot);
				return 1;
			}
			gVehicleData[slot][vTrunkGun] = curGun;
			gVehicleData[slot][vTrunkAmmo] = curAmmo;
			DB_SaveVehicle(slot);

			ResetPlayerWeapons(playerid);
			SendSucc(playerid, "Menyimpan senjata ke dalam bagasi mobil.");
			PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
		}
	} else if (listitem == 1) {
		// Simpan Senjata yang sedang dipegang
		new WEAPON:curGun = GetPlayerWeapon(playerid);
		new curAmmo = GetPlayerAmmo(playerid);
		if (curGun == WEAPON_FIST || curAmmo <= 0) {
			SendErr(playerid, "Anda tidak sedang memegang senjata api berpeluru.");
			ShowVehicleTrunkDialog(playerid, slot);
			return 1;
		}
		gVehicleData[slot][vTrunkGun] = curGun;
		gVehicleData[slot][vTrunkAmmo] = curAmmo;
		DB_SaveVehicle(slot);

		ResetPlayerWeapons(playerid);
		SendSucc(playerid, "Menyimpan senjata ke dalam bagasi mobil.");
		PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
	}
	ShowVehicleTrunkDialog(playerid, slot);
	return 1;
}
