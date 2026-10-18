#if defined _INC_rp_jobs
	#endinput
#endif
#define _INC_rp_jobs

// job menu dialog (Pusat Buruh & Balai Kota)
stock ShowJobDialog(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendErr(playerid, "Anda tidak dapat mengakses pekerjaan saat pingsan atau terborgol.");
		return 0;
	}

	new currentJob[96];
	if (gPlayerData[playerid][pJob] == JOB_NONE) {
		format(currentJob, sizeof(currentJob), "{2ECC71}Menganggur (Belum memiliki pekerjaan)");
	} else {
		new jName[32];
		GetJobName(gPlayerData[playerid][pJob], jName, sizeof(jName));
		format(currentJob, sizeof(currentJob), "{FFFF00}%s {AFAFAF}(Gunakan /quitjob untuk resign)", jName);
	}

	new body[950];
	format(body, sizeof(body),
		"{FFFFFF}Pekerjaan Utama Anda Saat Ini: %s\n\n\
{FFFFFF}1. Kurir Paket Logistik (Gudang Willowfield/Ocean Docks)\n\
{FFFFFF}2. Pembersih Jalan Raya (Depo Sweeper Unity)\n\
{FFFFFF}3. Supir Truk Kargo Ekspedisi (Dermaga Ocean Docks)\n\
{FFFFFF}4. Mekanik Bengkel Mesin & Bodi (/service)\n\
{FFFFFF}5. Pemulung Barang Rongsokan Gang Kota (/searchtrash)\n\
{FFFFFF}6. Penebang Pohon Hutan Shady Creeks (/cutwood)\n\
{FFFFFF}7. Nelayan Dermaga Santa Maria Pier (/fish)\n\
{FFFFFF}8. Supir Bus Kota Trans-LS (Terminal Bus)\n\
{FFFFFF}9. Petugas Dinas Truk Sampah (/starttrash)\n\
{FFFFFF}10. Penambang Batu Hunter Quarry (/mine)\n\
{E74C3C}11. Mengundurkan Diri dari Pekerjaan Utama (/quitjob)", currentJob);

	RP_ShowPlayerDialog(playerid, DIALOG_JOB_MENU, DIALOG_STYLE_LIST,
		"PUSAT BURUH & PEKERJAAN UTAMA KOTA", body, "Pilih", "Tutup");
	return 1;
}

stock OnJobMenuResponse(playerid, listitem)
{
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendErr(playerid, "Anda tidak dapat mengambil pekerjaan saat pingsan atau terborgol.");
		return 0;
	}

	if (listitem == 10) {
		return ProcessJobQuit(playerid);
	}

	new targetJob = JOB_NONE;
	switch (listitem) {
		case 0: targetJob = JOB_COURIER;
		case 1: targetJob = JOB_SWEEPER;
		case 2: targetJob = JOB_TRUCKER;
		case 3: targetJob = JOB_MECHANIC;
		case 4: targetJob = JOB_SCAVENGER;
		case 5: targetJob = JOB_LUMBERJACK;
		case 6: targetJob = JOB_FISHERMAN;
		case 7: targetJob = JOB_BUSDRIVER;
		case 8: targetJob = JOB_TRASHMAN;
		case 9: targetJob = JOB_MINER;
		default: return 1;
	}

	// 1. Cek jika sudah memiliki pekerjaan tetap (hanya boleh 1 main job)
	if (gPlayerData[playerid][pJob] != JOB_NONE) {
		if (gPlayerData[playerid][pJob] == targetJob) {
			new jName[32];
			GetJobName(targetJob, jName, sizeof(jName));
			SendInfo(playerid, "Anda sudah bekerja sebagai %s! Silakan menuju tempat kerja Anda (/gps).", jName);
			return 1;
		}
		new currName[32];
		GetJobName(gPlayerData[playerid][pJob], currName, sizeof(currName));
		SendErr(playerid, "Anda masih terikat pekerjaan sebagai %s! Setiap warga hanya boleh memiliki 1 pekerjaan utama.", currName);
		SendInfo(playerid, "Gunakan /quitjob terlebih dahulu jika Anda ingin mengundurkan diri dan berganti profesi.");
		return 1;
	}

	// 2. Cek jeda timing quitjob (cooldown 10 menit)
	if (gPlayerData[playerid][pJobCooldown] > gettime()) {
		new sisa = gPlayerData[playerid][pJobCooldown] - gettime();
		new menit = sisa / 60;
		new detik = sisa % 60;
		SendErr(playerid, "Anda masih dalam masa jeda resign. Tunggu %d menit %d detik lagi sebelum melamar pekerjaan baru.", menit, detik);
		return 1;
	}

	// 3. Terima pekerjaan
	gPlayerData[playerid][pJob] = targetJob;
	new newJobName[32];
	GetJobName(targetJob, newJobName, sizeof(newJobName));

	SendSucc(playerid, "Selamat! Anda resmi diterima bekerja sebagai: %s.", newJobName);
	SendInfo(playerid, "Buka peta atau gunakan /gps untuk menuju ke lokasi tempat pekerjaan Anda.");
	PlayerPlaySound(playerid, 1058, 0.0, 0.0, 0.0);
	return 1;
}

// 1. kurir paket
stock StartCourierJob(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendErr(playerid, "Anda tidak dapat memulai pekerjaan saat pingsan atau terborgol.");
		return 0;
	}
	if (gPlayerData[playerid][pJob] != JOB_COURIER) {
		SendErr(playerid, "Pekerjaan ini khusus untuk pegawai Kurir Paket (/job di Balai Kota).");
		return 0;
	}
	if (gJobActive[playerid]) {
		SendErr(playerid, "Anda masih memiliki tugas pengantaran paket yang sedang aktif.");
		return 0;
	}
	if (!IsPlayerNearPoint(playerid, COURIER_X, COURIER_Y, COURIER_Z, 30.0)) {
		SendErr(playerid, "Anda harus berada di Gudang Logistik Kurir (/gps).");
		return 0;
	}

	gPlayerJobVehicle[playerid] = CreateVehicle(COURIER_VEH_MODEL, COURIER_X + 2.0, COURIER_Y + 2.0, COURIER_Z + 1.0, 90.0, 40, 40, 0);
	gJobCheckpoint[playerid] = 0;
	gJobCash[playerid] = 0;
	gJobActive[playerid] = true;
	gJobType[playerid] = JOB_COURIER;

	// Pilih alamat pengiriman
	new dest = random(sizeof(gCourierDestList));
	gCourierTargetDest[playerid] = dest;

	AC_PutPlayerInVehicle(playerid, gPlayerJobVehicle[playerid], 0);
	SetPlayerCheckpoint(playerid, gCourierDestList[dest][cdX], gCourierDestList[dest][cdY], gCourierDestList[dest][cdZ], 6.0);

	SendSucc(playerid, "[KURIR] Paket diambil di Gudang! Antarkan ke alamat: %s (Tanda merah di radar).", gCourierDestList[dest][cdAddress]);
	return 1;
}

// 2. sweeper
stock StartSweeperJob(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendErr(playerid, "Anda tidak dapat memulai pekerjaan saat pingsan atau terborgol.");
		return 0;
	}
	if (gPlayerData[playerid][pJob] != JOB_SWEEPER) {
		SendErr(playerid, "Pekerjaan ini khusus untuk petugas Pembersih Jalan (/job di Balai Kota).");
		return 0;
	}
	if (gJobActive[playerid]) {
		SendErr(playerid, "Anda masih memiliki tugas pekerjaan aktif.");
		return 0;
	}
	if (!IsPlayerNearPoint(playerid, SWEEPER_X, SWEEPER_Y, SWEEPER_Z, 25.0)) {
		SendErr(playerid, "Anda harus berada di Depo Pembersih Jalan (/gps).");
		return 0;
	}

	gPlayerJobVehicle[playerid] = CreateVehicle(SWEEPER_VEH_MODEL, SWEEPER_X + 2.0, SWEEPER_Y + 2.0, SWEEPER_Z + 1.0, 0.0, 2, 2, 0);
	gJobCheckpoint[playerid] = 0;
	gJobCash[playerid] = 0;
	gJobActive[playerid] = true;
	gJobType[playerid] = JOB_SWEEPER;

	AC_PutPlayerInVehicle(playerid, gPlayerJobVehicle[playerid], 0);
	CreateJobCheckpoint(playerid, 0);
	SendInfo(playerid, "Pembersihan jalan dimulai. Sapu rute jalanan kota.");
	return 1;
}

// 3. trucker
stock StartTruckerJob(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendErr(playerid, "Anda tidak dapat memulai pekerjaan saat pingsan atau terborgol.");
		return 0;
	}
	if (gPlayerData[playerid][pJob] != JOB_TRUCKER) {
		SendErr(playerid, "Pekerjaan ini khusus untuk Supir Truk Kargo (/job di Balai Kota).");
		return 0;
	}
	if (gJobActive[playerid]) {
		SendErr(playerid, "Anda masih memiliki tugas pekerjaan aktif.");
		return 0;
	}
	if (!IsPlayerNearPoint(playerid, TRUCKER_X, TRUCKER_Y, TRUCKER_Z, 35.0)) {
		SendErr(playerid, "Anda harus berada di Dermaga Kargo Ocean Docks (/gps).");
		return 0;
	}

	gPlayerJobVehicle[playerid] = CreateVehicle(TRUCKER_VEH_MODEL, TRUCKER_X + 4.0, TRUCKER_Y, TRUCKER_Z + 1.0, 0.0, 1, 1, 0);
	gPlayerJobTrailer[playerid] = CreateVehicle(TRUCKER_TRAILER_MODEL, TRUCKER_X + 4.0, TRUCKER_Y - 10.0, TRUCKER_Z + 1.0, 0.0, 1, 1, 0);
	AttachTrailerToVehicle(gPlayerJobTrailer[playerid], gPlayerJobVehicle[playerid]);

	gJobCheckpoint[playerid] = 0;
	gJobCash[playerid] = 0;
	gJobActive[playerid] = true;
	gJobType[playerid] = JOB_TRUCKER;

	AC_PutPlayerInVehicle(playerid, gPlayerJobVehicle[playerid], 0);
	CreateJobCheckpoint(playerid, 0);
	SendInfo(playerid, "Tugas ekspedisi truk dimulai. Pasang gandengan dan antar ke checkpoint.");
	return 1;
}

// 4. mekanik
stock StartMechanicJob(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendErr(playerid, "Anda tidak dapat mengambil pekerjaan saat pingsan atau terborgol.");
		return 0;
	}
	return ShowJobDialog(playerid);
}

// 5. pemulung
stock StartScavengerJob(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendErr(playerid, "Anda tidak dapat mengambil pekerjaan saat pingsan atau terborgol.");
		return 0;
	}
	return ShowJobDialog(playerid);
}

// 6. penebang pohon
stock StartLumberjackJob(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendErr(playerid, "Anda tidak dapat mengambil pekerjaan saat pingsan atau terborgol.");
		return 0;
	}
	return ShowJobDialog(playerid);
}

// 7. nelayan
stock StartFishermanJob(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendErr(playerid, "Anda tidak dapat mengambil pekerjaan saat pingsan atau terborgol.");
		return 0;
	}
	return ShowJobDialog(playerid);
}

// 8. supir bus
stock StartBusDriverJob(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendErr(playerid, "Anda tidak dapat memulai pekerjaan saat pingsan atau terborgol.");
		return 0;
	}
	if (gPlayerData[playerid][pJob] != JOB_BUSDRIVER) {
		SendErr(playerid, "Pekerjaan ini khusus untuk Supir Bus Kota (/job di Balai Kota).");
		return 0;
	}
	if (gJobActive[playerid]) {
		SendErr(playerid, "Anda masih memiliki tugas pekerjaan aktif.");
		return 0;
	}
	if (!IsPlayerNearPoint(playerid, BUS_DEPOT_X, BUS_DEPOT_Y, BUS_DEPOT_Z, 25.0)) {
		SendErr(playerid, "Anda harus berada di Terminal Bus Trans-LS (/gps).");
		return 0;
	}

	gPlayerJobVehicle[playerid] = CreateVehicle(BUS_VEH_MODEL, BUS_DEPOT_X + 2.0, BUS_DEPOT_Y + 2.0, BUS_DEPOT_Z + 1.0, 90.0, 3, 3, 0);
	gJobCheckpoint[playerid] = 0;
	gJobCash[playerid] = 0;
	gJobActive[playerid] = true;
	gJobType[playerid] = JOB_BUSDRIVER;

	AC_PutPlayerInVehicle(playerid, gPlayerJobVehicle[playerid], 0);
	CreateJobCheckpoint(playerid, 0);
	SendInfo(playerid, "Trayek Bus Kota Trans-LS dimulai. Singgahi setiap halte halte kota.");
	return 1;
}

// job actions & commands
stock ProcessSearchTrash(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendErr(playerid, "Anda tidak dapat memulung saat pingsan atau terborgol.");
		return 0;
	}
	if (gPlayerData[playerid][pJob] != JOB_SCAVENGER) {
		SendErr(playerid, "Pekerjaan ini khusus untuk Pemulung (/job di Balai Kota).");
		return 0;
	}

	new nearTrash = 0;
	for (new i = 0; i < MAX_TRASH_POINTS; i++) {
		if (IsPlayerNearPoint(playerid, gTrashPoints[i][0] * 1.0, gTrashPoints[i][1] * 1.0, gTrashPoints[i][2] * 1.0, 4.0)) {
			nearTrash = 1;
			break;
		}
	}
	if (!nearTrash) {
		SendErr(playerid, "Anda tidak berada di dekat tong sampah kota.");
		return 0;
	}

	return TriggerScavengerMinigame(playerid);
}

stock ProcessSellTrash(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendErr(playerid, "Anda tidak dapat menjual barang saat pingsan atau terborgol.");
		return 0;
	}
	if (!IsPlayerNearPoint(playerid, TRASH_BUYER_X, TRASH_BUYER_Y, TRASH_BUYER_Z, 6.0)) {
		SendErr(playerid, "Anda harus berada di Pengepul Rongsokan Willowfield (/gps).");
		return 0;
	}

	new count = GetPlayerItemCount(playerid, ITEM_RECYCLED);
	if (count <= 0) {
		SendErr(playerid, "Anda tidak memiliki barang rongsokan untuk dijual.");
		return 0;
	}

	new totalPay = count * 35;
	if (!TakePlayerItem(playerid, ITEM_RECYCLED, count)) {
		SendErr(playerid, "Gagal menjual rongsokan: jumlah barang di inventaris tidak mencukupi.");
		return 0;
	}
	GivePlayerMoneyEx(playerid, totalPay, "jual rongsokan");
	GivePlayerExp(playerid, 15, "jual barang rongsokan");

	SendSucc(playerid, "Berhasil menjual %d rongsokan dengan total bayaran $%d.", count, totalPay);
	return 1;
}

stock ProcessCutWood(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendErr(playerid, "Anda tidak dapat menebang pohon saat pingsan atau terborgol.");
		return 0;
	}
	if (gPlayerData[playerid][pJob] != JOB_LUMBERJACK) {
		SendErr(playerid, "Pekerjaan ini khusus untuk Penebang Pohon (/job di Balai Kota).");
		return 0;
	}
	return StartLumberjackMinigameProcess(playerid);
}

stock ProcessSellWood(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendErr(playerid, "Anda tidak dapat menjual kayu saat pingsan atau terborgol.");
		return 0;
	}
	if (!IsPlayerNearPoint(playerid, SAWMILL_X, SAWMILL_Y, SAWMILL_Z, 10.0)) {
		SendErr(playerid, "Anda harus berada di Pabrik Pengolahan Kayu Sawmill (/gps).");
		return 0;
	}

	new count = GetPlayerItemCount(playerid, ITEM_WOOD);
	if (count <= 0) {
		SendErr(playerid, "Anda tidak membawa balok kayu di inventaris.");
		return 0;
	}

	new totalPay = count * 180;
	if (!TakePlayerItem(playerid, ITEM_WOOD, count)) {
		SendErr(playerid, "Gagal menjual kayu: jumlah barang di inventaris tidak mencukupi.");
		return 0;
	}
	GivePlayerMoneyEx(playerid, totalPay, "jual kayu");
	GivePlayerExp(playerid, 20, "jual kayu sawmill");

	SendSucc(playerid, "Berhasil menjual %d balok kayu ke Sawmill seharga $%d.", count, totalPay);
	return 1;
}

stock ProcessFish(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendErr(playerid, "Anda tidak dapat memancing saat pingsan atau terborgol.");
		return 0;
	}
	if (gPlayerData[playerid][pJob] != JOB_FISHERMAN) {
		SendErr(playerid, "Pekerjaan ini khusus untuk Nelayan Dermaga (/job di Balai Kota).");
		return 0;
	}
	return StartFishingMinigameProcess(playerid);
}

stock ProcessSellFish(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendErr(playerid, "Anda tidak dapat menjual ikan saat pingsan atau terborgol.");
		return 0;
	}
	if (!IsPlayerNearPoint(playerid, FISH_PIER_X, FISH_PIER_Y, FISH_PIER_Z, 25.0)) {
		SendErr(playerid, "Anda harus berada di Pasar Ikan Dermaga (/gps).");
		return 0;
	}
	if (gPlayerData[playerid][pFishWeight] <= 0) {
		SendErr(playerid, "Anda tidak memiliki ikan hasil tangkapan.");
		return 0;
	}

	new totalPay = gPlayerData[playerid][pFishWeight] * 25;
	new weight = gPlayerData[playerid][pFishWeight];
	gPlayerData[playerid][pFishWeight] = 0;

	GivePlayerMoneyEx(playerid, totalPay, "jual hasil ikan");
	SendSucc(playerid, "Menjual %d Kg ikan segar dengan total perolehan $%d.", weight, totalPay);
	return 1;
}

// checkpoint router
stock CreateJobCheckpoint(playerid, pointindex)
{
	if (!gJobActive[playerid]) return;

	switch (gJobType[playerid]) {
		case JOB_COURIER: {
			new dest = gCourierTargetDest[playerid];
			SetPlayerCheckpoint(playerid, gCourierDestList[dest][cdX], gCourierDestList[dest][cdY], gCourierDestList[dest][cdZ], 6.0);
		}
		case JOB_SWEEPER: {
			if (pointindex >= MAX_SWEEPER_POINTS) return;
			SetPlayerCheckpoint(playerid, gSweeperPoints[pointindex][0] * 1.0, gSweeperPoints[pointindex][1] * 1.0, gSweeperPoints[pointindex][2] * 1.0, 8.0);
		}
		case JOB_TRUCKER: {
			if (pointindex >= MAX_TRUCKER_POINTS) return;
			SetPlayerCheckpoint(playerid, gTruckerPoints[pointindex][0] * 1.0, gTruckerPoints[pointindex][1] * 1.0, gTruckerPoints[pointindex][2] * 1.0, 14.0);
		}
		case JOB_BUSDRIVER: {
			if (pointindex >= MAX_BUS_POINTS) return;
			SetPlayerCheckpoint(playerid, gBusPoints[pointindex][0] * 1.0, gBusPoints[pointindex][1] * 1.0, gBusPoints[pointindex][2] * 1.0, 8.0);
		}
	}
}

stock OnJobCheckpoint(playerid)
{
	if (!gJobActive[playerid]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return 0;

	// Kurir Paket: Tiba di alamat rumah pemesan yang sebenarnya!
	if (gJobType[playerid] == JOB_COURIER) {
		new dest = gCourierTargetDest[playerid];
		DisablePlayerCheckpoint(playerid);
		TriggerCourierVerification(playerid, gCourierDestList[dest][cdAddress]);
		return 1;
	}

	new next = gJobCheckpoint[playerid] + 1;
	new total = 0, pay = 0;

	switch (gJobType[playerid]) {
		case JOB_SWEEPER:   { total = MAX_SWEEPER_POINTS; pay = 100; }
		case JOB_TRUCKER:   { total = MAX_TRUCKER_POINTS; pay = 350; }
		case JOB_BUSDRIVER: { total = MAX_BUS_POINTS;     pay = 80;  }
		default: return 0;
	}

	if (next >= total) {
		ProcessJobComplete(playerid);
		return 1;
	}

	gJobCash[playerid] += pay;
	gJobCheckpoint[playerid] = next;
	CreateJobCheckpoint(playerid, next);

	// Bus & Sweeper: Peringatan Keselamatan Rambu Lalu Lintas QTE
	if ((gJobType[playerid] == JOB_BUSDRIVER || gJobType[playerid] == JOB_SWEEPER) && (next % 2 == 0)) {
		TriggerTrafficSafetyAlert(playerid);
	}

	SendRP(playerid, COLOR_SUCCESS, "[RUTE TUGAS] Checkpoint %d/%d dicapai (+${%d}). Lanjutkan perjalanan.", next, total, pay);
	return 1;
}

stock ProcessJobComplete(playerid)
{
	new bonus = 500;
	new totalEarned = gJobCash[playerid] + bonus;
	DisablePlayerCheckpoint(playerid);

	if (IsPlayerInAnyVehicle(playerid) && GetPlayerVehicleID(playerid) == gPlayerJobVehicle[playerid]) {
		RemovePlayerFromVehicle(playerid);
	}
	if (IsPlayerInAnyVehicle(playerid) && GetPlayerVehicleID(playerid) == gPlayerJobTrailer[playerid]) {
		RemovePlayerFromVehicle(playerid);
	}

	Vehicle_SafeDestroy(gPlayerJobVehicle[playerid]);
	Vehicle_SafeDestroy(gPlayerJobTrailer[playerid]);
	gPlayerJobVehicle[playerid] = INVALID_VEHICLE_ID;
	gPlayerJobTrailer[playerid] = INVALID_VEHICLE_ID;

	GivePlayerMoneyEx(playerid, totalEarned, "gaji rute kerja");
	GivePlayerExp(playerid, EXP_JOB_DONE, "selesai rute pekerjaan");

	SendSucc(playerid, "Rute pekerjaan selesai! Upah dan bonus masuk ke dompet: $%d.", totalEarned);

	gJobActive[playerid] = false;
	gJobCash[playerid] = 0;
	gJobCheckpoint[playerid] = 0;
	gJobType[playerid] = 0;
	return 1;
}

stock ProcessJobQuit(playerid)
{
	if (!gJobActive[playerid] && gPlayerData[playerid][pJob] == JOB_NONE) {
		SendErr(playerid, "Anda tidak memiliki pekerjaan tetap.");
		return 0;
	}

	new oldJob = gPlayerData[playerid][pJob];
	new jName[32];
	GetJobName(oldJob, jName, sizeof(jName));

	DisablePlayerCheckpoint(playerid);

	if (IsPlayerInAnyVehicle(playerid) && GetPlayerVehicleID(playerid) == gPlayerJobVehicle[playerid]) {
		RemovePlayerFromVehicle(playerid);
	}
	if (IsPlayerInAnyVehicle(playerid) && GetPlayerVehicleID(playerid) == gPlayerJobTrailer[playerid]) {
		RemovePlayerFromVehicle(playerid);
	}

	Vehicle_SafeDestroy(gPlayerJobVehicle[playerid]);
	Vehicle_SafeDestroy(gPlayerJobTrailer[playerid]);
	gPlayerJobVehicle[playerid] = INVALID_VEHICLE_ID;
	gPlayerJobTrailer[playerid] = INVALID_VEHICLE_ID;

	if (gTrashDuty[playerid] && gTrashVeh[playerid] > 0 && IsValidVehicle(gTrashVeh[playerid])) {
		Vehicle_SafeDestroy(gTrashVeh[playerid]);
		gTrashDuty[playerid] = false;
		gTrashVeh[playerid] = 0;
	}

	gJobActive[playerid] = false;
	gJobCash[playerid] = 0;
	gJobCheckpoint[playerid] = 0;
	gJobType[playerid] = 0;
	gPlayerData[playerid][pJob] = JOB_NONE;

	// [SECURITY] Tutup dialog QTE/verifikasi yang masih terbuka. Tanpa ini,
	// pemain bisa membuka QTE lalu /quitjob dan tetap menjawab dialog untuk
	// menerima upah padahal job sudah berakhir.
	HidePlayerDialog(playerid);
	Dialog_Close(playerid);

	// Cooldown 10 menit (600 detik)
	gPlayerData[playerid][pJobCooldown] = gettime() + 600;

	SendSucc(playerid, "Anda telah resmi mengundurkan diri dari pekerjaan (%s).", jName);
	SendInfo(playerid, "Masa jeda berlaku: Anda harus menunggu 10 menit sebelum dapat melamar pekerjaan baru.");
	return 1;
}

// ---------------------------------------------------------------------------
// Interaksi Tombol Y (KEY_YES) untuk Aktivitas Pekerjaan & Fasilitas Publik
// ---------------------------------------------------------------------------
stock ProcessKeyActivity(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;

	// 1. Cek pemulung tempat sampah
	for (new i = 0; i < sizeof(gTrashPoints); i++) {
		if (IsPlayerInRangeOfPoint(playerid, 3.5, float(gTrashPoints[i][0]), float(gTrashPoints[i][1]), float(gTrashPoints[i][2]))) {
			ProcessSearchTrash(playerid);
			return 1;
		}
	}

	// 2. Cek penebang kayu di Hutan Shady Creeks
	if (IsPlayerInRangeOfPoint(playerid, 35.0, LUMBER_X, LUMBER_Y, LUMBER_Z)) {
		ProcessCutWood(playerid);
		return 1;
	}

	// 3. Cek memancing di Dermaga Santa Maria Pier
	if (IsPlayerInRangeOfPoint(playerid, 25.0, FISH_PIER_X, FISH_PIER_Y, FISH_PIER_Z)) {
		ProcessFish(playerid);
		return 1;
	}

	// 4. Cek jual rongsokan di Pengepul
	if (IsPlayerInRangeOfPoint(playerid, 5.0, TRASH_BUYER_X, TRASH_BUYER_Y, TRASH_BUYER_Z)) {
		ProcessSellTrash(playerid);
		return 1;
	}

	// 5. Cek jual kayu di Sawmill
	if (IsPlayerInRangeOfPoint(playerid, 8.0, SAWMILL_X, SAWMILL_Y, SAWMILL_Z)) {
		ProcessSellWood(playerid);
		return 1;
	}

	// 6. Cek jual ikan di Dermaga
	if (IsPlayerInRangeOfPoint(playerid, 6.0, FISH_PIER_X, FISH_PIER_Y, FISH_PIER_Z)) {
		ProcessSellFish(playerid);
		return 1;
	}

	// 7. Cek pusat pekerjaan umum (Balai Kota SAGOV Pershing Square)
	if (IsPlayerInRangeOfPoint(playerid, 6.0, JOB_CENTER_X, JOB_CENTER_Y, JOB_CENTER_Z)) {
		ShowJobDialog(playerid);
		return 1;
	}

	// 7a. Cek gudang kurir paket
	if (IsPlayerInRangeOfPoint(playerid, 10.0, COURIER_X, COURIER_Y, COURIER_Z)) {
		StartCourierJob(playerid);
		return 1;
	}

	// 7b. Cek depo pembersih jalan sweeper
	if (IsPlayerInRangeOfPoint(playerid, 10.0, SWEEPER_X, SWEEPER_Y, SWEEPER_Z)) {
		StartSweeperJob(playerid);
		return 1;
	}

	// 7c. Cek terminal bus kota
	if (IsPlayerInRangeOfPoint(playerid, 10.0, BUS_DEPOT_X, BUS_DEPOT_Y, BUS_DEPOT_Z)) {
		StartBusDriverJob(playerid);
		return 1;
	}

	// 7d. Cek dermaga kargo truk
	if (IsPlayerInRangeOfPoint(playerid, 15.0, TRUCKER_X, TRUCKER_Y, TRUCKER_Z)) {
		StartTruckerJob(playerid);
		return 1;
	}

	// 8. Cek ATM
	if (IsPlayerInRangeOfPoint(playerid, 4.0, ATM_X, ATM_Y, ATM_Z)) {
		ShowPlayerBankUI(playerid);
		return 1;
	}

	// 9. Cek Bank
	if (IsPlayerInRangeOfPoint(playerid, 6.0, BANK_X, BANK_Y, BANK_Z)) {
		ShowPlayerBankUI(playerid);
		return 1;
	}

	// 10. Cek Toko 24/7
	if (IsPlayerInRangeOfPoint(playerid, 6.0, STORE_IDLEWOOD_X, STORE_IDLEWOOD_Y, STORE_IDLEWOOD_Z)) {
		ShowStoreBuyMenu(playerid);
		return 1;
	}

	// 11. Cek Dealership
	if (IsPlayerInRangeOfPoint(playerid, 6.0, DEALER_X, DEALER_Y, DEALER_Z)) {
		ShowDealershipDialog(playerid);
		return 1;
	}

	// 12. Cek SPBU
	if (IsPlayerInRangeOfPoint(playerid, 8.0, FUEL_STATION_X, FUEL_STATION_Y, FUEL_STATION_Z)) {
		ProcessRefuel(playerid);
		return 1;
	}

	// 13. Cek Pertanian Flint County
	if (IsPlayerNearPoint(playerid, FARM_SHOP_X, FARM_SHOP_Y, FARM_SHOP_Z, 4.5)) {
		ShowFarmShopDialog(playerid);
		return 1;
	}
	if (IsPlayerNearPoint(playerid, FARM_SILO_X, FARM_SILO_Y, FARM_SILO_Z, 6.0)) {
		ProcessSellCrops(playerid);
		return 1;
	}
	if (IsPlayerInFlintFarm(playerid)) {
		if (GetPlayerItemCount(playerid, ITEM_WHEATSEED) > 0 || GetPlayerItemCount(playerid, ITEM_CORNSEED) > 0) {
			ProcessPlantCrop(playerid);
			return 1;
		}
		ProcessHarvestCrop(playerid);
		return 1;
	}

	// 14. Cek Pertambangan Hunter Quarry
	if (IsPlayerNearPoint(playerid, MINING_SHOP_X, MINING_SHOP_Y, MINING_SHOP_Z, 4.5)) {
		ProcessBuyPickaxe(playerid);
		return 1;
	}
	if (IsPlayerNearPoint(playerid, MINING_SMELTER_X, MINING_SMELTER_Y, MINING_SMELTER_Z, 5.0)) {
		ProcessSmeltMetals(playerid);
		return 1;
	}
	if (IsPlayerNearPoint(playerid, MINING_BUYER_X, MINING_BUYER_Y, MINING_BUYER_Z, 5.0)) {
		ProcessSellOre(playerid);
		return 1;
	}
	if (IsPlayerNearPoint(playerid, MINING_SITE_X, MINING_SITE_Y, MINING_SITE_Z, 25.0)) {
		ProcessStartMining(playerid);
		return 1;
	}

	// 15. Cek Penyelaman Laut Santa Maria
	if (IsPlayerNearPoint(playerid, SCUBA_SHOP_X, SCUBA_SHOP_Y, SCUBA_SHOP_Z, 4.5)) {
		if (GetPlayerItemCount(playerid, ITEM_PEARL) > 0) {
			ProcessSellPearls(playerid);
			return 1;
		}
		ProcessRentScuba(playerid);
		return 1;
	}
	if (IsPlayerNearPoint(playerid, SCUBA_REEF_X, SCUBA_REEF_Y, SCUBA_REEF_Z, 60.0)) {
		ProcessStartDiving(playerid);
		return 1;
	}

	// 16. Cek Depot Truk Sampah
	if (IsPlayerNearPoint(playerid, TRASH_DEPOT_X, TRASH_DEPOT_Y, TRASH_DEPOT_Z, 8.0)) {
		ProcessStartTrash(playerid);
		return 1;
	}

	SendInfo(playerid, "Tidak ada objek interaksi di sekitar Anda. Dekati fasilitas, toko, atau pekerjaan lalu tekan Y.");
	return 0;
}
