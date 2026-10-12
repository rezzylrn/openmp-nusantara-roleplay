#if defined _INC_rp_jobs
	#endinput
#endif
#define _INC_rp_jobs

// job menu dialog
stock ShowJobDialog(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendErr(playerid, "Anda tidak dapat mengakses pekerjaan saat pingsan atau terborgol.");
		return 0;
	}

	ShowPlayerDialog(playerid, DIALOG_JOB_MENU, DIALOG_STYLE_LIST,
		"PUSAT BURUH & PEKERJAAN KOTA",
		"1. Kurir Paket (Logistik Bobcat)\n\
2. Pembersih Jalan (Sweeper)\n\
3. Pengangkut Muatan (Trucker Hauler)\n\
4. Mekanik Kendaraan (/service)\n\
5. Pemulung Rongsokan (/searchtrash)\n\
6. Penebang Pohon Hutan (/cutwood)\n\
7. Nelayan Dermaga (/fish)\n\
8. Supir Bus Trans-LS (Bus Rute)\n\
9. Berhenti dari Pekerjaan (/quitjob)",
		"Pilih", "Batal");
	return 1;
}

stock OnJobMenuResponse(playerid, listitem)
{
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendErr(playerid, "Anda tidak dapat mengambil pekerjaan saat pingsan atau terborgol.");
		return 0;
	}

	switch (listitem) {
		case 0: StartCourierJob(playerid);
		case 1: StartSweeperJob(playerid);
		case 2: StartTruckerJob(playerid);
		case 3: StartMechanicJob(playerid);
		case 4: StartScavengerJob(playerid);
		case 5: StartLumberjackJob(playerid);
		case 6: StartFishermanJob(playerid);
		case 7: StartBusDriverJob(playerid);
		case 8: ProcessJobQuit(playerid);
	}
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
	if (gJobActive[playerid]) {
		SendErr(playerid, "Anda masih memiliki tugas pekerjaan aktif.");
		return 0;
	}
	if (!IsPlayerNearPoint(playerid, COURIER_X, COURIER_Y, COURIER_Z, 25.0)) {
		SendErr(playerid, "Anda harus berada di Gudang Logistik Kurir (/gps).");
		return 0;
	}

	gPlayerData[playerid][pJob] = JOB_COURIER;
	gPlayerJobVehicle[playerid] = CreateVehicle(COURIER_VEH_MODEL, COURIER_X + 2.0, COURIER_Y + 2.0, COURIER_Z + 1.0, 90.0, 40, 40, 0);
	gJobCheckpoint[playerid] = 0;
	gJobCash[playerid] = 0;
	gJobActive[playerid] = true;
	gJobType[playerid] = JOB_COURIER;

	AC_PutPlayerInVehicle(playerid, gPlayerJobVehicle[playerid], 0);
	CreateJobCheckpoint(playerid, 0);
	SendInfo(playerid, "Pekerjaan Kurir dimulai. Ikuti tanda merah di peta radar.");
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
	if (gJobActive[playerid]) {
		SendErr(playerid, "Anda masih memiliki tugas pekerjaan aktif.");
		return 0;
	}
	if (!IsPlayerNearPoint(playerid, SWEEPER_X, SWEEPER_Y, SWEEPER_Z, 25.0)) {
		SendErr(playerid, "Anda harus berada di Depo Pembersih Jalan (/gps).");
		return 0;
	}

	gPlayerData[playerid][pJob] = JOB_SWEEPER;
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
	if (gJobActive[playerid]) {
		SendErr(playerid, "Anda masih memiliki tugas pekerjaan aktif.");
		return 0;
	}
	if (!IsPlayerNearPoint(playerid, TRUCKER_X, TRUCKER_Y, TRUCKER_Z, 35.0)) {
		SendErr(playerid, "Anda harus berada di Dermaga Kargo Ocean Docks (/gps).");
		return 0;
	}

	gPlayerData[playerid][pJob] = JOB_TRUCKER;
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
	gPlayerData[playerid][pJob] = JOB_MECHANIC;
	SendSucc(playerid, "Profesi Anda sekarang: Mekanik Bengkel. Gunakan /service [engine/body] [targetid] [harga].");
	return 1;
}

// 5. pemulung
stock StartScavengerJob(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendErr(playerid, "Anda tidak dapat mengambil pekerjaan saat pingsan atau terborgol.");
		return 0;
	}
	gPlayerData[playerid][pJob] = JOB_SCAVENGER;
	SendSucc(playerid, "Profesi Anda sekarang: Pemulung Sampah & Rongsokan.");
	SendInfo(playerid, "Cari tempat sampah di gang kota lalu gunakan /searchtrash. Jual hasil di pengepul (/selltrash).");
	return 1;
}

// 6. penebang pohon
stock StartLumberjackJob(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendErr(playerid, "Anda tidak dapat mengambil pekerjaan saat pingsan atau terborgol.");
		return 0;
	}
	gPlayerData[playerid][pJob] = JOB_LUMBERJACK;
	AC_GiveWeapon(playerid, WEAPON_CHAINSAW, 1);
	SendSucc(playerid, "Profesi Anda sekarang: Penebang Pohon. Anda menerima gergaji mesin.");
	SendInfo(playerid, "Pergi ke Hutan Shady Creeks (/gps) lalu tebang pohon dengan /cutwood. Jual di Sawmill (/sellwood).");
	return 1;
}

// 7. nelayan
stock StartFishermanJob(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendErr(playerid, "Anda tidak dapat mengambil pekerjaan saat pingsan atau terborgol.");
		return 0;
	}
	gPlayerData[playerid][pJob] = JOB_FISHERMAN;
	GivePlayerItem(playerid, ITEM_ROD, 1);
	GivePlayerItem(playerid, ITEM_BAIT, 5);
	SendSucc(playerid, "Profesi Anda sekarang: Nelayan Dermaga. Anda menerima Joran & 5 Umpan di /inv.");
	SendInfo(playerid, "Pergi ke Dermaga Santa Maria Pier (/gps) lalu gunakan /fish. Jual tangkapan dengan /sellfish.");
	return 1;
}

// 8. supir bus
stock StartBusDriverJob(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendErr(playerid, "Anda tidak dapat memulai pekerjaan saat pingsan atau terborgol.");
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

	gPlayerData[playerid][pJob] = JOB_BUSDRIVER;
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
		SendErr(playerid, "Pekerjaan ini khusus untuk Pemulung (/job).");
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
	TakePlayerItem(playerid, ITEM_RECYCLED, count);
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
		SendErr(playerid, "Pekerjaan ini khusus untuk Penebang Pohon (/job).");
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
	TakePlayerItem(playerid, ITEM_WOOD, count);
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
			if (pointindex >= MAX_COURIER_POINTS) return;
			SetPlayerCheckpoint(playerid, gCourierPoints[pointindex][0] * 1.0, gCourierPoints[pointindex][1] * 1.0, gCourierPoints[pointindex][2] * 1.0, 10.0);
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

	// Kurir Paket: Verifikasi Resi Paket (Anti-Bot Captcha)
	if (gJobType[playerid] == JOB_COURIER) {
		TriggerCourierVerification(playerid);
		return 1;
	}

	new next = gJobCheckpoint[playerid] + 1;
	new total = 0, pay = 0;

	switch (gJobType[playerid]) {
		case JOB_COURIER:   { total = MAX_COURIER_POINTS; pay = 150; }
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

	if (gPlayerJobVehicle[playerid] > 0 && IsValidVehicle(gPlayerJobVehicle[playerid])) {
		DestroyVehicle(gPlayerJobVehicle[playerid]);
	}
	gPlayerJobVehicle[playerid] = -1;

	if (gPlayerJobTrailer[playerid] > 0 && IsValidVehicle(gPlayerJobTrailer[playerid])) {
		DestroyVehicle(gPlayerJobTrailer[playerid]);
	}
	gPlayerJobTrailer[playerid] = -1;

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
	gPlayerData[playerid][pJob] = JOB_NONE;

	SendInfo(playerid, "Anda telah membatalkan dan keluar dari pekerjaan.");
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

	// 7. Cek pusat pekerjaan umum
	if (IsPlayerInRangeOfPoint(playerid, 5.0, COURIER_X, COURIER_Y, COURIER_Z)) {
		ShowJobDialog(playerid);
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
