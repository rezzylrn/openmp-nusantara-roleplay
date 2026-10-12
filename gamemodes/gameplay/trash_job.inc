#if defined _INC_rp_trash_job
	#endinput
#endif
#define _INC_rp_trash_job

/*
==============================================================================
 trash_job.inc
 NUSANTARA ROLEPLAY - Sanitation Worker / Trashmaster Route Job
 Pekerjaan Pengangkutan Sampah Kota:
 - Mengemudikan truk sampah Trashmaster (Model 408).
 - Mengangkut muatan sampah dari perumahan warga ke belakang truk dengan progress bar.
 - Menyetorkan muatan penuh ke Tempat Pembuangan Akhir (TPA) Los Santos ($450).
==============================================================================
*/

#define TRASH_DEPOT_X           2185.0
#define TRASH_DEPOT_Y           -1980.0
#define TRASH_DEPOT_Z           13.5

#define MAX_TRASH_STOPS         5

static const Float:gTrashStops[MAX_TRASH_STOPS][3] = {
	{2105.0, -1815.0, 13.5}, // Gang Belakang Idlewood
	{2315.0, -1650.0, 14.5}, // Gang Ganton Cul-de-sac
	{2450.0, -1950.0, 13.5}, // Willowfield Industrial
	{1350.0, -1750.0, 13.5}, // Belakang Mall Commerce
	{1140.0, -1450.0, 15.5}  // Pemukiman Market
};

new bool:gTrashDuty[MAX_PLAYERS];
new gTrashVeh[MAX_PLAYERS];
new gTrashCurrentStop[MAX_PLAYERS];
new gTrashCollected[MAX_PLAYERS];
new bool:gHoldingTrashBag[MAX_PLAYERS];

forward RP_OnFinishPickupTrashBag(playerid, stopIndex);
forward RP_OnFinishThrowTrashBag(playerid, veh);

stock TrashJob_Init()
{
	Create3DTextLabel("{2ECC71}[ DINAS KEBERSIHAN KOTA ]\n{FFFFFF}Gunakan {FFFF00}/starttrash {FFFFFF}untuk mengambil rute truk sampah",
		0xFFFFFFFF, TRASH_DEPOT_X, TRASH_DEPOT_Y, TRASH_DEPOT_Z + 0.5, 15.0, 0, false);
	return 1;
}

CMD:starttrash(playerid, params[])
{
	return ProcessStartTrash(playerid);
}

stock ProcessStartTrash(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda belum login.");
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda tidak dapat memulai pekerjaan saat pingsan atau terborgol.");
		return 1;
	}
	if (gTrashDuty[playerid]) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda sudah sedang dalam rute pengangkutan sampah.");
		return 1;
	}

	if (!IsPlayerNearPoint(playerid, TRASH_DEPOT_X, TRASH_DEPOT_Y, TRASH_DEPOT_Z, 6.0)) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda harus berada di Depot Dinas Kebersihan Los Santos.");
		return 1;
	}

	// Spawn Truk Trashmaster
	new veh = CreateVehicle(408, 2195.0, -1975.0, 13.8, 0.0, 1, 1, 120);
	LinkVehicleToInterior(veh, 0);
	SetVehicleVirtualWorld(veh, 0);

	gTrashDuty[playerid] = true;
	gTrashVeh[playerid] = veh;
	gTrashCurrentStop[playerid] = 0;
	gTrashCollected[playerid] = 0;
	gHoldingTrashBag[playerid] = false;

	AC_PutPlayerInVehicle(playerid, veh, 0);
	SetPlayerCheckpoint(playerid, gTrashStops[0][0], gTrashStops[0][1], gTrashStops[0][2], 4.5);

	SendClientMessage(playerid, COLOR_SUCCESS, "[KEBERSIHAN] Rute dinas kebersihan dimulai! Menuju checkpoint tong sampah pertama.");
	SendClientMessage(playerid, COLOR_INFO, "[PETUNJUK] Parkir dekat tong sampah -> /pickuptrash -> bawa ke belakang truk -> /throwtrash.");
	return 1;
}

CMD:pickuptrash(playerid, params[])
{
	if (!gTrashDuty[playerid]) return SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda tidak sedang bekerja di dinas kebersihan.");
	if (gHoldingTrashBag[playerid]) return SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda sudah membawa kantong sampah di tangan.");

	new cur = gTrashCurrentStop[playerid];
	if (cur >= MAX_TRASH_STOPS) return SendClientMessage(playerid, COLOR_INFO, "[INFO] Muatan truk sudah penuh. Antarkan ke TPA (/unloaddump).");

	if (!IsPlayerNearPoint(playerid, gTrashStops[cur][0], gTrashStops[cur][1], gTrashStops[cur][2], 5.0)) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda tidak berada di titik lokasi tong sampah saat ini.");
		return 1;
	}

	ApplyProgressBar(playerid, "MENGAMBIL KANTONG SAMPAH...", 3, "RP_OnFinishPickupTrashBag", cur);
	ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.1, false, false, false, false, 0, 1);
	return 1;
}

public RP_OnFinishPickupTrashBag(playerid, stopIndex)
{
	#pragma unused stopIndex
	gHoldingTrashBag[playerid] = true;
	SetPlayerSpecialAction(playerid, SPECIAL_ACTION_CARRY);
	SendClientMessage(playerid, COLOR_SUCCESS, "[SUKSES] Kantong sampah berhasil diangkat. Bawa ke bagian belakang truk Anda dan ketik /throwtrash.");
	return 1;
}

CMD:throwtrash(playerid, params[])
{
	if (!gTrashDuty[playerid]) return SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda tidak sedang bekerja di dinas kebersihan.");
	if (!gHoldingTrashBag[playerid]) return SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda tidak sedang membawa kantong sampah.");

	new veh = gTrashVeh[playerid];
	if (!IsValidVehicle(veh)) return SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Truk sampah Anda tidak ditemukan.");

	new Float:vx, Float:vy, Float:vz;
	GetVehiclePos(veh, vx, vy, vz);

	if (GetPlayerDistanceFromPoint(playerid, vx, vy, vz) > 6.0) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Dekati bagian belakang truk sampah Anda.");
		return 1;
	}

	ApplyProgressBar(playerid, "MEMASUKKAN KE MESIN TRUK...", 3, "RP_OnFinishThrowTrashBag", veh);
	ApplyAnimation(playerid, "GRENADE", "WEAPON_throwu", 4.1, false, false, false, false, 0, 1);
	return 1;
}

public RP_OnFinishThrowTrashBag(playerid, veh)
{
	#pragma unused veh
	gHoldingTrashBag[playerid] = false;
	SetPlayerSpecialAction(playerid, SPECIAL_ACTION_NONE);
	gTrashCollected[playerid]++;
	gTrashCurrentStop[playerid]++;

	new cur = gTrashCurrentStop[playerid];
	if (cur < MAX_TRASH_STOPS) {
		SetPlayerCheckpoint(playerid, gTrashStops[cur][0], gTrashStops[cur][1], gTrashStops[cur][2], 4.5);
		new msg[144];
		format(msg, sizeof(msg), "[KEBERSIHAN] Muatan terkumpul: %d/%d kantong. Menuju checkpoint berikutnya di radar.",
			gTrashCollected[playerid], MAX_TRASH_STOPS);
		SendClientMessage(playerid, COLOR_SUCCESS, msg);
	} else {
		SetPlayerCheckpoint(playerid, TRASH_DEPOT_X, TRASH_DEPOT_Y, TRASH_DEPOT_Z, 6.0);
		SendClientMessage(playerid, COLOR_SUCCESS, "[KEBERSIHAN] Muatan truk penuh! Bawa truk kembali ke Depot TPA untuk pencairan upah (/unloaddump).");
	}
	return 1;
}

CMD:unloaddump(playerid, params[])
{
	if (!gTrashDuty[playerid]) return SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda tidak sedang bertugas di dinas kebersihan.");
	if (gTrashCollected[playerid] < MAX_TRASH_STOPS) {
		new msg[128];
		format(msg, sizeof(msg), "[ERROR] Muatan truk belum penuh (%d/%d kantong sampah).",
			gTrashCollected[playerid], MAX_TRASH_STOPS);
		SendClientMessage(playerid, COLOR_ERROR, msg);
		return 1;
	}

	if (!IsPlayerNearPoint(playerid, TRASH_DEPOT_X, TRASH_DEPOT_Y, TRASH_DEPOT_Z, 12.0)) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda harus berada di area Depot TPA Los Santos.");
		return 1;
	}

	DisablePlayerCheckpoint(playerid);
	DestroyVehicle(gTrashVeh[playerid]);

	gTrashDuty[playerid] = false;
	gTrashVeh[playerid] = INVALID_VEHICLE_ID;
	gTrashCollected[playerid] = 0;
	gHoldingTrashBag[playerid] = false;

	new wage = 450;
	GivePlayerMoneyEx(playerid, wage);

	new msg[144];
	format(msg, sizeof(msg), "[DINAS KEBERSIHAN] Seluruh sampah berhasil disetor! Anda menerima upah kerja sebesar $%d tunai.", wage);
	SendClientMessage(playerid, COLOR_SUCCESS, msg);
	return 1;
}
