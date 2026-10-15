#if defined _INC_rp_cmd_vehicle
	#endinput
#endif
#define _INC_rp_cmd_vehicle

/*
==============================================================================
 cmd_vehicle.inc (Manajemen Kendaraan & Transportasi)
 NUSANTARA ROLEPLAY - Modular Command Architecture
==============================================================================
*/


CMD:vehpanel(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	ShowPlayerRadialMenu(playerid);
	return 1;
}


CMD:checkplate(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	return cmd_mdc(playerid, params);
}


CMD:tune(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	ShowWorkshopMenu(playerid);
	return 1;
}


CMD:modveh(playerid, params[])
{
	return cmd_tune(playerid, params);
}


CMD:buyveh(playerid, params[])
{
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return SendErr(playerid, "Anda tidak dapat melakukan tindakan ini saat pingsan atau terborgol.");
	ShowDealershipDialog(playerid);
	return 1;
}


CMD:myveh(playerid, params[])
{
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return SendErr(playerid, "Anda tidak dapat melakukan tindakan ini saat pingsan atau terborgol.");
	ShowMyVehiclesDialog(playerid);
	return 1;
}


CMD:v(playerid, params[])
{
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return SendErr(playerid, "Anda tidak dapat melakukan tindakan ini saat pingsan atau terborgol.");
	ShowMyVehiclesDialog(playerid);
	return 1;
}


CMD:engine(playerid, params[])
{
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return SendErr(playerid, "Anda tidak dapat melakukan tindakan ini saat pingsan atau terborgol.");
	ProcessEngineToggle(playerid);
	return 1;
}


CMD:lights(playerid, params[])
{
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return SendErr(playerid, "Anda tidak dapat melakukan tindakan ini saat pingsan atau terborgol.");
	ProcessLightsToggle(playerid);
	return 1;
}


CMD:lock(playerid, params[])
{
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return SendErr(playerid, "Anda tidak dapat melakukan tindakan ini saat pingsan atau terborgol.");
	new slot = -1;
	if (IsPlayerInAnyVehicle(playerid)) {
		new vid = GetPlayerVehicleID(playerid);
		slot = Vehicle_GetSlot(vid);
	} else {
		slot = Vehicle_FindNearest(playerid, 5.0);
	}

	if (slot != -1) {
		ProcessVehicleLock(playerid, slot);
	} else {
		SendErr(playerid, "Anda harus berada di dalam atau di dekat kendaraan milik Anda (radius 5 meter).");
	}
	return 1;
}


CMD:park(playerid, params[])
{
	new vid = GetPlayerVehicleID(playerid);
	new slot = Vehicle_GetSlot(vid);
	if (slot != -1) ProcessVehiclePark(playerid, slot);
	else SendErr(playerid, "Anda harus berada di dalam kendaraan pribadi.");
	return 1;
}


CMD:hood(playerid, params[])
{
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return SendErr(playerid, "Anda tidak dapat melakukan tindakan ini saat pingsan atau terborgol.");
	new slot = -1, vid = 0;
	if (IsPlayerInAnyVehicle(playerid)) {
		vid = GetPlayerVehicleID(playerid);
		slot = Vehicle_GetSlot(vid);
	} else {
		slot = Vehicle_FindNearest(playerid, 4.0);
		if (slot != -1) vid = gVehicleData[slot][vVehicleHandle];
	}

	if (slot != -1 && vid > 0 && IsValidVehicle(vid)) {
		new bool:newHood = !gVehicleData[slot][vHood];
		Vehicle_SetHood(vid, newHood);
		SendRP(playerid, COLOR_INFO, "Kap mesin kendaraan %s.", newHood ? "dibuka" : "ditutup");
	} else SendErr(playerid, "Anda harus berada di dalam atau di dekat kendaraan.");
	return 1;
}


CMD:trunk(playerid, params[])
{
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return SendErr(playerid, "Anda tidak dapat melakukan tindakan ini saat pingsan atau terborgol.");
	if (IsPlayerInAnyVehicle(playerid)) {
		new vid = GetPlayerVehicleID(playerid);
		new slot = Vehicle_GetSlot(vid);
		if (slot != -1) {
			new bool:newTrunk = !gVehicleData[slot][vTrunkOpen];
			Vehicle_SetTrunk(vid, newTrunk);
			SendRP(playerid, COLOR_INFO, "Pintu bagasi kendaraan %s.", newTrunk ? "dibuka" : "ditutup");
		} else SendErr(playerid, "Ini bukan kendaraan terdaftar.");
		return 1;
	}

	new slot = Vehicle_FindNearest(playerid, 4.0);
	if (slot == -1) {
		SendErr(playerid, "Tidak ada kendaraan di dekat Anda (radius 4.0 meter).");
		return 1;
	}

	if (!gVehicleData[slot][vTrunkOpen]) {
		SendErr(playerid, "Pintu bagasi kendaraan ini masih tertutup.");
		SendInfo(playerid, "Buka kunci pintu dan buka bagasi kendaraan terlebih dahulu.");
		return 1;
	}

	ShowVehicleTrunkDialog(playerid, slot);
	return 1;
}


CMD:refuel(playerid, params[])
{
	ProcessRefuel(playerid);
	return 1;
}


CMD:fill(playerid, params[])
{
	ProcessRefuel(playerid);
	return 1;
}


CMD:customplate(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	if (isnull(params)) return SendSyntax(playerid, "/customplate [plat_baru]");
	ProcessCustomPlate(playerid, params);
	return 1;
}


CMD:setplate(playerid, params[])
{
	return cmd_customplate(playerid, params);
}


CMD:fixpuncture(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	ProcessRepairTire(playerid);
	return 1;
}


CMD:tambalban(playerid, params[])
{
	return cmd_fixpuncture(playerid, params);
}


CMD:pourfuel(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	ProcessPourFuelCanister(playerid);
	return 1;
}


CMD:tuangbensin(playerid, params[])
{
	return cmd_pourfuel(playerid, params);
}


CMD:tow(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	ProcessTowVehicle(playerid);
	return 1;
}


CMD:derek(playerid, params[])
{
	return cmd_tow(playerid, params);
}


CMD:untow(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	ProcessUntowVehicle(playerid);
	return 1;
}


CMD:lepasderek(playerid, params[])
{
	return cmd_untow(playerid, params);
}


CMD:impound(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	ProcessImpoundVehicle(playerid);
	return 1;
}


CMD:sitakendaraan(playerid, params[])
{
	return cmd_impound(playerid, params);
}


CMD:claimveh(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	ProcessClaimImpoundedVehicle(playerid);
	return 1;
}


CMD:tebus(playerid, params[])
{
	return cmd_claimveh(playerid, params);
}


CMD:rentveh(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	ShowRentalMenuDialog(playerid);
	return 1;
}


CMD:sewa(playerid, params[])
{
	return cmd_rentveh(playerid, params);
}


CMD:unrentveh(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	if (gPlayerRentalVeh[playerid] != INVALID_VEHICLE_ID) {
		Vehicle_SafeDestroy(gPlayerRentalVeh[playerid]);
		if (gPlayerData[playerid][pLanguage] == LANG_EN) SendClientMessage(playerid, COLOR_SUCCESS, "SUKSES: Rented vehicle returned successfully.");
		else SendClientMessage(playerid, COLOR_SUCCESS, "SUKSES: Kendaraan sewa berhasil dikembalikan.");
	} else {
		if (gPlayerData[playerid][pLanguage] == LANG_EN) SendClientMessage(playerid, COLOR_ERROR, "ERROR: You do not have any active rented vehicle.");
		else SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda tidak sedang menyewa kendaraan.");
	}
	return 1;
}


CMD:glovebox(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return SendErr(playerid, "Anda tidak dapat membuka laci dashboard saat pingsan atau terborgol.");
	if (!IsPlayerInAnyVehicle(playerid)) {
		SendErr(playerid, "Anda harus berada di dalam kendaraan untuk membuka laci dashboard.");
		return 1;
	}
	new vid = GetPlayerVehicleID(playerid);
	new slot = Vehicle_GetSlot(vid);
	if (slot == -1) {
		SendErr(playerid, "Kendaraan ini tidak memiliki laci dashboard pribadi.");
		return 1;
	}
	ShowVehicleGloveboxDialog(playerid, slot);
	return 1;
}


CMD:stnk(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return SendErr(playerid, "Anda tidak dapat memeriksa STNK saat pingsan atau terborgol.");
	ProcessInspectSTNK(playerid);
	return 1;
}


