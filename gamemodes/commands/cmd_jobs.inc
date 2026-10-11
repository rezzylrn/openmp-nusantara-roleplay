#if defined _INC_rp_cmd_jobs
	#endinput
#endif
#define _INC_rp_cmd_jobs

/*
==============================================================================
 cmd_jobs.inc (Pekerjaan Kota & Mata Pencaharian)
 NUSANTARA ROLEPLAY - Modular Command Architecture
==============================================================================
*/


CMD:scavenge(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	ProcessPlayerScavenge(playerid);
	return 1;
}


CMD:rongsok(playerid, params[])
{
	return cmd_scavenge(playerid, params);
}


CMD:ngais(playerid, params[])
{
	return cmd_scavenge(playerid, params);
}


CMD:trash(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	if (!IsPlayerNearAnyTrash(playerid)) {
		if (gPlayerData[playerid][pLanguage] == LANG_EN) return SendErr(playerid, "You are not near a trash bin.");
		else return SendErr(playerid, "Anda tidak berada di dekat tong sampah.");
	}
	if (gPlayerData[playerid][pLanguage] == LANG_EN) {
		SendInfo(playerid, "You are at a trash bin. To discard an item, open /inv, select the item, and choose Drop.");
	} else {
		SendInfo(playerid, "Anda berada di dekat tong sampah. Untuk membuang barang, buka /inv, pilih barang, lalu pilih Drop/Buang.");
	}
	return 1;
}


CMD:buang(playerid, params[])
{
	return cmd_trash(playerid, params);
}


CMD:searchtrash(playerid, params[])
{
	ProcessSearchTrash(playerid);
	return 1;
}


CMD:selltrash(playerid, params[])
{
	ProcessSellTrash(playerid);
	return 1;
}


CMD:job(playerid, params[])
{
	ShowJobDialog(playerid);
	return 1;
}


CMD:quitjob(playerid, params[])
{
	ProcessJobQuit(playerid);
	return 1;
}


CMD:cutwood(playerid, params[])
{
	ProcessCutWood(playerid);
	return 1;
}


CMD:sellwood(playerid, params[])
{
	ProcessSellWood(playerid);
	return 1;
}


CMD:fish(playerid, params[])
{
	ProcessFish(playerid);
	return 1;
}


CMD:sellfish(playerid, params[])
{
	ProcessSellFish(playerid);
	return 1;
}


CMD:service(playerid, params[])
{
	if (gPlayerData[playerid][pJob] != JOB_MECHANIC) return SendErr(playerid, "Perintah ini khusus untuk Mekanik Bengkel.");
	new type[16], targetid, price;
	if (sscanf(params, "s[16]ud", type, targetid, price)) return SendSyntax(playerid, "/service [engine/body] [targetid] [harga]");
	if (!IsPlayerConnected(targetid)) return SendErr(playerid, "Pemain tidak ditemukan.");
	if (!IsPlayerInAnyVehicle(targetid)) return SendErr(playerid, "Target tidak berada di dalam kendaraan.");

	new tVeh = GetPlayerVehicleID(targetid);
	if (strcmp(type, "engine", true) == 0) {
		TakePlayerMoneyEx(targetid, price, "service mesin");
		GivePlayerMoneyEx(playerid, price, "upah service mekanik");
		TriggerMechanicDiagnostic(playerid, tVeh);
		SendSucc(targetid, "Mesin kendaraan Anda sedang diperiksa oleh Mekanik %s ($%d).", gPlayerData[playerid][pName], price);
	} else if (strcmp(type, "body", true) == 0) {
		RepairVehicle(tVeh);
		TakePlayerMoneyEx(targetid, price, "service bodi");
		GivePlayerMoneyEx(playerid, price, "upah service mekanik");
		SendSucc(playerid, "Bodi kendaraan berhasil diperbaiki. Menerima bayaran $%d.", price);
		SendSucc(targetid, "Bodi kendaraan Anda diperbaiki oleh Mekanik %s ($%d).", gPlayerData[playerid][pName], price);
	} else {
		SendSyntax(playerid, "/service [engine/body] [targetid] [harga]");
	}
	return 1;
}


CMD:taximeter(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	new fare = DEFAULT_TAXI_FARE;
	if (!isnull(params)) fare = strval(params);
	ProcessToggleTaxiMeter(playerid, fare);
	return 1;
}


CMD:argometer(playerid, params[])
{
	return cmd_taximeter(playerid, params);
}


CMD:calltaxi(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	ProcessCallTaxi(playerid);
	return 1;
}


CMD:pesantaksi(playerid, params[])
{
	return cmd_calltaxi(playerid, params);
}


