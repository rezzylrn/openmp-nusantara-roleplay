#if defined _INC_rp_vehicle_dealership
	#endinput
#endif
#define _INC_rp_vehicle_dealership

/*
==============================================================================
 vehicle_dealership.inc
 NUSANTARA ROLEPLAY - Dealership Showroom & Owned Vehicles Management
==============================================================================
*/

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

	RP_ShowPlayerDialog(playerid, DIALOG_DEALERSHIP, DIALOG_STYLE_TABLIST_HEADERS,
		"DEALERSHIP LOS SANTOS", list, "Beli", "Batal");
	return 1;
}

stock OnDealershipResponse(playerid, listitem)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return 0;
	if (!IsPlayerNearPoint(playerid, DEALER_X, DEALER_Y, DEALER_Z, RANGE_DEALER)) {
		SendErr(playerid, "Anda harus berada di showroom Dealership (/gps).");
		return 0;
	}

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

	if (!TakePlayerMoneyEx(playerid, price, "beli kendaraan")) {
		SendErr(playerid, "Pembayaran gagal. Uang tunai Anda tidak mencukupi.");
		return 0;
	}

	new Float:px, Float:py, Float:pz, Float:pa;
	GetPlayerPos(playerid, px, py, pz);
	GetPlayerFacingAngle(playerid, pa);

	// Generate plat nomor acak
	new plate[16];
	format(plate, sizeof(plate), "LS-%04d", 1000 + random(8999));

	gVehicleData[slot][vUsed] = true;
	gVehicleData[slot][vDBID] = 0;
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

	// Simpan baris kendaraan baru; vDBID diisi asinkron dari AUTO_INCREMENT id
	DB_CreateVehicleRow(gPlayerData[playerid][pID], model, px + 2.5, py + 2.5, pz, pa,
		gDealerStock[listitem][2], gDealerStock[listitem][3], plate, slot);

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
			gPlayerVehListSlot[playerid][count] = i;
			GetVehicleModelName(gVehicleData[i][vModel], vname, sizeof(vname));
			format(list, sizeof(list), "%s#%d	%s	%.0f Liter	%s\n",
				list, count + 1, vname, gVehicleData[i][vFuel], gVehicleData[i][vPlate]);
			count++;
		}
	}

	if (count == 0) {
		SendInfo(playerid, "Anda belum memiliki kendaraan pribadi. Kunjungi dealership (/gps).");
		return 0;
	}

	RP_ShowPlayerDialog(playerid, DIALOG_MYVEH, DIALOG_STYLE_TABLIST_HEADERS,
		"KENDARAAN PRIBADI SAYA", list, "Pilih", "Batal");
	return 1;
}

stock ShowVehicleActionDialog(playerid, slot)
{
	if (slot < 0 || slot >= MAX_OWNED_VEHICLES || !gVehicleData[slot][vUsed] || gVehicleData[slot][vOwnerID] != gPlayerData[playerid][pID]) return 0;
	gPlayerVehicleSlot[playerid] = slot;
	new vname[32];
	GetVehicleModelName(gVehicleData[slot][vModel], vname, sizeof(vname));

	new header[64];
	format(header, sizeof(header), "PILIHAN - %s (%s)", vname, gVehicleData[slot][vPlate]);

	RP_ShowPlayerDialog(playerid, DIALOG_MYVEH_ACTION, DIALOG_STYLE_LIST,
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

