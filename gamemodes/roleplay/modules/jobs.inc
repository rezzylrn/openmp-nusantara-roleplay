#if defined _INC_rp_jobs
	#endinput
#endif
#define _INC_rp_jobs

/*
==============================================================================
  NUSANTARA ROLEPLAY - jobs.inc
  Pekerjaan Sampingan: Kurir Paket, Sweeper Jalan, Trucker Hauler, & Mekanik
==============================================================================
*/

stock ShowJobDialog(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) {
		SendErr(playerid, "Anda belum login.");
		return 0;
	}

	ShowPlayerDialog(playerid, DIALOG_JOB_MENU, DIALOG_STYLE_LIST,
		"PUSAT BURUH & PEKERJAAN - NUSANTARA RP",
		"1. Kurir Paket (Bobcat)\n\
2. Pembersih Jalan (Sweeper)\n\
3. Pengangkut Muatan / Trucker (Linerunner)\n\
4. Mekanik Bengkel (/service)\n\
5. Keluar dari Pekerjaan Saat Ini",
		"Pilih", "Batal");
	return 1;
}

stock OnJobMenuResponse(playerid, listitem)
{
	switch (listitem) {
		case 0: StartCourierJob(playerid);
		case 1: StartSweeperJob(playerid);
		case 2: StartTruckerJob(playerid);
		case 3: StartMechanicJob(playerid);
		case 4: ProcessJobQuit(playerid);
	}
	return 1;
}

// ---------------------------------------------------------------------------
// 1. KURIR PAKET
// ---------------------------------------------------------------------------
stock StartCourierJob(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gJobActive[playerid]) {
		SendErr(playerid, "Anda masih memiliki tugas aktif. Selesaikan rute atau ketik /quitjob.");
		return 0;
	}
	if (!IsPlayerNearPoint(playerid, COURIER_X, COURIER_Y, COURIER_Z, RANGE_JOBCENTER)) {
		SendErr(playerid, "Anda harus berada di Pusat Logistik Kurir (/gps).");
		return 0;
	}

	gPlayerJobVehicle[playerid] = CreateVehicle(COURIER_VEH_MODEL, COURIER_X + 2.0, COURIER_Y + 2.0, COURIER_Z + 1.0, 90.0, 40, 40, 0);
	gJobCheckpoint[playerid] = 0;
	gJobCash[playerid] = 0;
	gJobActive[playerid] = true;
	gJobType[playerid] = JOB_COURIER;

	CreateJobCheckpoint(playerid, 0);

	SendInfo(playerid, "Pekerjaan Kurir Paket dimulai. Masuk ke mobil Bobcat dan ikuti checkpoint merah di peta radar.");
	return 1;
}

// ---------------------------------------------------------------------------
// 2. SWEEPER
// ---------------------------------------------------------------------------
stock StartSweeperJob(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gJobActive[playerid]) {
		SendErr(playerid, "Anda masih memiliki tugas aktif. Selesaikan rute atau ketik /quitjob.");
		return 0;
	}
	if (!IsPlayerNearPoint(playerid, SWEEPER_X, SWEEPER_Y, SWEEPER_Z, RANGE_JOBCENTER)) {
		SendErr(playerid, "Anda harus berada di Depo Pembersih Jalan (/gps).");
		return 0;
	}

	gPlayerJobVehicle[playerid] = CreateVehicle(SWEEPER_VEH_MODEL, SWEEPER_X + 2.0, SWEEPER_Y + 2.0, SWEEPER_Z + 1.0, 0.0, 2, 2, 0);
	gJobCheckpoint[playerid] = 0;
	gJobCash[playerid] = 0;
	gJobActive[playerid] = true;
	gJobType[playerid] = JOB_SWEEPER;

	CreateJobCheckpoint(playerid, 0);

	SendInfo(playerid, "Pembersihan jalan raya dimulai. Kemudikan Sweeper menyusuri rute kota Los Santos.");
	return 1;
}

// ---------------------------------------------------------------------------
// 3. TRUCKER HAULER
// ---------------------------------------------------------------------------
stock StartTruckerJob(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gJobActive[playerid]) {
		SendErr(playerid, "Anda masih memiliki tugas aktif.");
		return 0;
	}
	if (!IsPlayerNearPoint(playerid, TRUCKER_X, TRUCKER_Y, TRUCKER_Z, 25.0)) {
		SendErr(playerid, "Anda harus berada di Dermaga Logistik Ocean Docks (/gps).");
		return 0;
	}

	gPlayerJobVehicle[playerid] = CreateVehicle(TRUCKER_VEH_MODEL, TRUCKER_X + 4.0, TRUCKER_Y, TRUCKER_Z + 1.0, 0.0, 1, 1, 0);
	gPlayerJobTrailer[playerid] = CreateVehicle(TRUCKER_TRAILER_MODEL, TRUCKER_X + 4.0, TRUCKER_Y - 10.0, TRUCKER_Z + 1.0, 0.0, 1, 1, 0);
	AttachTrailerToVehicle(gPlayerJobTrailer[playerid], gPlayerJobVehicle[playerid]);

	gJobCheckpoint[playerid] = 0;
	gJobCash[playerid] = 0;
	gJobActive[playerid] = true;
	gJobType[playerid] = JOB_TRUCKER;

	CreateJobCheckpoint(playerid, 0);

	SendInfo(playerid, "Ekspedisi logistik dimulai. Pasang gandengan muatan dan antar ke titik tujuan antar-kota.");
	return 1;
}

// ---------------------------------------------------------------------------
// 4. MEKANIK
// ---------------------------------------------------------------------------
stock StartMechanicJob(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	gPlayerData[playerid][pJob] = JOB_MECHANIC;
	SendSucc(playerid, "Profesi Anda sekarang: Mekanik Bengkel.");
	SendInfo(playerid, "Gunakan /service [engine/body] [targetid] [harga] untuk melayani perbaikan kendaraan pemain lain.");
	return 1;
}

// ---------------------------------------------------------------------------
// PENGELOLA CHECKPOINT
// ---------------------------------------------------------------------------
stock CreateJobCheckpoint(playerid, pointindex)
{
	if (!gJobActive[playerid]) return;

	if (gJobType[playerid] == JOB_COURIER) {
		if (pointindex >= MAX_COURIER_POINTS) return;
		SetPlayerCheckpoint(playerid, gCourierPoints[pointindex][0], gCourierPoints[pointindex][1], gCourierPoints[pointindex][2], 10.0);
	} else if (gJobType[playerid] == JOB_SWEEPER) {
		if (pointindex >= MAX_SWEEPER_POINTS) return;
		SetPlayerCheckpoint(playerid, gSweeperPoints[pointindex][0], gSweeperPoints[pointindex][1], gSweeperPoints[pointindex][2], 8.0);
	} else if (gJobType[playerid] == JOB_TRUCKER) {
		if (pointindex >= MAX_TRUCKER_POINTS) return;
		SetPlayerCheckpoint(playerid, gTruckerPoints[pointindex][0], gTruckerPoints[pointindex][1], gTruckerPoints[pointindex][2], 14.0);
	}
}

stock OnJobCheckpoint(playerid)
{
	if (!gJobActive[playerid]) return 0;

	new next = gJobCheckpoint[playerid] + 1;
	new total = 0;
	new pay = 0;

	switch (gJobType[playerid]) {
		case JOB_COURIER: { total = MAX_COURIER_POINTS; pay = JOB_COURIER_PAY; }
		case JOB_SWEEPER: { total = MAX_SWEEPER_POINTS; pay = JOB_SWEEPER_PAY; }
		case JOB_TRUCKER: { total = MAX_TRUCKER_POINTS; pay = JOB_TRUCKER_PAY; }
		default: return 0;
	}

	if (next >= total) {
		ProcessJobComplete(playerid);
		return 1;
	}

	gJobCash[playerid] += pay;
	gJobCheckpoint[playerid] = next;
	CreateJobCheckpoint(playerid, next);

	SendRP(playerid, COLOR_SUCCESS, "[PEKERJAAN] Checkpoint %d/%d dicapai (+${%d}). Lanjutkan ke titik berikutnya.", next, total, pay);
	return 1;
}

stock ProcessJobComplete(playerid)
{
	new bonus = 0;
	switch (gJobType[playerid]) {
		case JOB_COURIER: bonus = JOB_COURIER_BONUS;
		case JOB_SWEEPER: bonus = JOB_SWEEPER_BONUS;
		case JOB_TRUCKER: bonus = JOB_TRUCKER_BONUS;
	}

	new totalEarned = gJobCash[playerid] + bonus;
	DisablePlayerCheckpoint(playerid);

	if (gPlayerJobVehicle[playerid] > 0 && IsValidVehicle(gPlayerJobVehicle[playerid])) {
		DestroyVehicle(gPlayerJobVehicle[playerid]);
	}
	gPlayerJobVehicle[playerid] = -1;

	if (gPlayerJobTrailer[playerid] > 0 && IsValidVehicle(gPlayerJobTrailer[playerid])) {
		DestroyVehicle(gPlayerJobTrailer[playerid]);
	}
	gPlayerJobTrailer[playerid] = -1;

	GivePlayerMoneyEx(playerid, totalEarned, "gaji rute selesai");
	GivePlayerExp(playerid, EXP_JOB_DONE, "selesai rute pekerjaan");

	SendSucc(playerid, "Rute pekerjaan selesai sepenuhnya! Total upah dan bonus: $%d.", totalEarned);

	gJobActive[playerid] = false;
	gJobCash[playerid] = 0;
	gJobCheckpoint[playerid] = 0;
	gJobType[playerid] = 0;
	return 1;
}

stock ProcessJobQuit(playerid)
{
	if (!gJobActive[playerid]) {
		SendErr(playerid, "Anda tidak sedang menjalankan rute pekerjaan.");
		return 0;
	}

	DisablePlayerCheckpoint(playerid);

	if (gPlayerJobVehicle[playerid] > 0 && IsValidVehicle(gPlayerJobVehicle[playerid])) {
		DestroyVehicle(gPlayerJobVehicle[playerid]);
	}
	gPlayerJobVehicle[playerid] = -1;

	if (gPlayerJobTrailer[playerid] > 0 && IsValidVehicle(gPlayerJobTrailer[playerid])) {
		DestroyVehicle(gPlayerJobTrailer[playerid]);
	}
	gPlayerJobTrailer[playerid] = -1;

	gJobActive[playerid] = false;
	gJobCash[playerid] = 0;
	gJobCheckpoint[playerid] = 0;
	gJobType[playerid] = 0;

	SendInfo(playerid, "Anda telah membatalkan dan keluar dari rute pekerjaan.");
	return 1;
}
