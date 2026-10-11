#if defined _INC_rp_job_minigames
	#endinput
#endif
#define _INC_rp_job_minigames

/*
==============================================================================
 job_minigames.inc
 NUSANTARA ROLEPLAY - Interactive Job Minigames & Anti-Bot Farming Suite
 Sistem Mini-Game Interaktif Seluruh Pekerjaan:
 - Mencegah AFK bot farming script secara otomatis.
 - Memberikan tantangan menyenangkan & bonus upah tambahan bagi pemain aktif.
 - Nelayan (Fishing QTE), Penebang Kayu (Lumber Angle), Kurir (Receipt Captcha),
   Mekanik (Diagnostic Puzzle), Pemulung (Scavenge Sorting), Bus/Sweeper (Traffic QTE).
==============================================================================
*/

// State per player
new gFishingTargetAction[MAX_PLAYERS];
new gLumberTargetCut[MAX_PLAYERS];
new gCourierCode[MAX_PLAYERS][8];
new gMechanicTargetPart[MAX_PLAYERS];
new gMechanicTargetVeh[MAX_PLAYERS];
new gTrafficTargetAnswer[MAX_PLAYERS];

// Forward declarations
forward RP_OnFinishCastRod(playerid, extra);
forward RP_OnFinishChainsaw(playerid, extra);
forward RP_OnFinishMechanicInspection(playerid, vehid);

// ===========================================================================
// 1. NELAYAN (FISHING MINIGAME)
// ===========================================================================
stock StartFishingMinigameProcess(playerid)
{
	if (!IsPlayerNearPoint(playerid, FISH_PIER_X, FISH_PIER_Y, FISH_PIER_Z, 25.0)) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda harus berada di Dermaga Santa Maria Pier (/gps).");
		return 0;
	}
	if (GetPlayerItemCount(playerid, ITEM_ROD) <= 0) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda memerlukan Joran Pancing di inventaris.");
		return 0;
	}
	if (GetPlayerItemCount(playerid, ITEM_BAIT) <= 0) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Umpan pancing Anda habis. Beli di toko 24/7.");
		return 0;
	}

	TakePlayerItem(playerid, ITEM_BAIT, 1);
	ApplyAnimation(playerid, "SWAT", "swt_lkt__01", 4.1, false, false, false, false, 0, 1);

	ApplyProgressBar(playerid, "MEMASANG UMPAN & MELEMPAR KAIL...", 4, "RP_OnFinishCastRod", 0);
	return 1;
}

public RP_OnFinishCastRod(playerid, extra)
{
	#pragma unused extra
	// Ikan menyambar umpan! Tentukan gerakan acak ikan
	new fishMove = random(3);
	gFishingTargetAction[playerid] = fishMove;

	new prompt[300];
	if (fishMove == 0) {
		format(prompt, sizeof(prompt),
			"{FFFFFF}Umpan disambar ikan besar!\n\
Ikan berenang liar menarik kail ke {FFFF00}ARAH KIRI{FFFFFF}!\n\n\
Pilih tindakan yang tepat untuk menyeimbangkan tarikan:\n\
1. [ TARIK JORAN KE KANAN ] (Seimbangkan)\n\
2. [ IKUTI TARIKAN KE KIRI ]\n\
3. [ GULUNG TALI SEKUAT TENAGA ]");
	} else if (fishMove == 1) {
		format(prompt, sizeof(prompt),
			"{FFFFFF}Umpan disambar ikan besar!\n\
Ikan melompat ke permukaan dan berenang ke {FFFF00}ARAH KANAN{FFFFFF}!\n\n\
Pilih tindakan yang tepat untuk menyeimbangkan tarikan:\n\
1. [ IKUTI TARIKAN KE KANAN ]\n\
2. [ TARIK JORAN KE KIRI ] (Seimbangkan)\n\
3. [ KENDURKAN SENAR PANCING ]");
	} else {
		format(prompt, sizeof(prompt),
			"{FFFFFF}Umpan disambar ikan monster laut!\n\
Ikan menyelam ke {FFFF00}KEDALAMAN DASAR LAUT{FFFFFF}!\n\n\
Pilih tindakan yang tepat untuk menyeimbangkan tarikan:\n\
1. [ LEPAS REM REEL JORAN ]\n\
2. [ TARIK JORAN KE ATAS ] (Sentak Kuat)\n\
3. [ DIAMKAN JORAN ]");
	}

	Dialog_Show(playerid, DialogFishingQTE, DIALOG_STYLE_LIST,
		"{3498DB}TARIKAN JORAN PANCING - REAKSI CEPAT!", prompt, "Tarik!", "Lepas");
	return 1;
}

Dialog:DialogFishingQTE(playerid, response, listitem, inputtext[])
{
	if (!response) {
		SendClientMessage(playerid, COLOR_INFO, "[INFO] Anda melepas tarikan. Ikan berhasil kabur ke laut lepas.");
		return 1;
	}

	new target = gFishingTargetAction[playerid];
	new bool:success = false;

	if (target == 0 && listitem == 0) success = true;       // Kiri -> Tarik Kanan
	else if (target == 1 && listitem == 1) success = true;  // Kanan -> Tarik Kiri
	else if (target == 2 && listitem == 1) success = true;  // Dasar -> Sentak ke Atas

	if (success) {
		new kg = 4 + random(10); // 4 - 13 Kg
		gPlayerData[playerid][pFishWeight] += kg;
		GivePlayerExp(playerid, 12, "berhasil memancing ikan besar");

		new bonusCash = 15 + random(25);
		GivePlayerMoneyEx(playerid, bonusCash);

		new msg[144];
		format(msg, sizeof(msg), "[STRIKE SUKSES] Tarikan joran sempurna! Mendapatkan ikan segar %d Kg & bonus kecakapan $%d (/sellfish).",
			kg, bonusCash);
		SendClientMessage(playerid, COLOR_SUCCESS, msg);
		PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
	} else {
		SendClientMessage(playerid, COLOR_ERROR, "[FAILED] Tarikan joran tidak seimbang! Senar pancing putus dan ikan lepas.");
	}
	return 1;
}

// ===========================================================================
// 2. PENEBANG POHON (LUMBERJACK MINIGAME)
// ===========================================================================
stock StartLumberjackMinigameProcess(playerid)
{
	if (!IsPlayerNearPoint(playerid, LUMBER_X, LUMBER_Y, LUMBER_Z, 25.0)) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda harus berada di Hutan Shady Creeks (/gps).");
		return 0;
	}

	ApplyProgressBar(playerid, "MENGOPERASIKAN GERGAJI MESIN...", 4, "RP_OnFinishChainsaw", 0);
	ApplyAnimation(playerid, "CHAINSAW", "CSW_1", 4.1, false, false, false, false, 0, 1);
	return 1;
}

public RP_OnFinishChainsaw(playerid, extra)
{
	#pragma unused extra
	new treeLean = random(3);
	gLumberTargetCut[playerid] = treeLean;

	new prompt[300];
	if (treeLean == 0) {
		format(prompt, sizeof(prompt),
			"{FFFFFF}Batang pohon jati condong ke {FFFF00}ARAH BARAT{FFFFFF}.\n\
Tentukan potongan undercut gergaji agar kayu tidak retak:\n\n\
1. [ Tebasan Bawah Sudut 45 Derajat ] (Presisi Seimbang)\n\
2. [ Tebasan Datar Sudut 90 Derajat ]\n\
3. [ Tebasan Silang Dua Sisi ]");
	} else if (treeLean == 1) {
		format(prompt, sizeof(prompt),
			"{FFFFFF}Batang pohon pinus berdiri {FFFF00}TEGAK LURUS SEMPURNA{FFFFFF}.\n\
Tentukan potongan takik gergaji agar pohon rebah ke jalan:\n\n\
1. [ Tebasan Potong Atas Datar ]\n\
2. [ Potongan Takik Felling Cut 90 Derajat ] (Presisi Seimbang)\n\
3. [ Tebasan Serong Sembarang ]");
	} else {
		format(prompt, sizeof(prompt),
			"{FFFFFF}Batang pohon tua berurat tebal dan condong ke {FFFF00}ARAH TIMUR{FFFFFF}.\n\
Tentukan teknik tebangan agar pohon tumbang sempurna:\n\n\
1. [ Potongan Tarik Belakang ]\n\
2. [ Tebasan Rata Bawah ]\n\
3. [ Teknik Tebasan Silang Ganda ] (Presisi Seimbang)");
	}

	Dialog_Show(playerid, DialogLumberQTE, DIALOG_STYLE_LIST,
		"{2ECC71}TEKNIK TEBASAN POHON HUTAN", prompt, "Tebas", "Batal");
	return 1;
}

Dialog:DialogLumberQTE(playerid, response, listitem, inputtext[])
{
	if (!response) {
		SendClientMessage(playerid, COLOR_INFO, "[INFO] Anda membatalkan tebangan pohon.");
		return 1;
	}

	new target = gLumberTargetCut[playerid];
	new bool:success = false;

	if (target == 0 && listitem == 0) success = true;
	else if (target == 1 && listitem == 1) success = true;
	else if (target == 2 && listitem == 2) success = true;

	if (success) {
		GivePlayerItem(playerid, ITEM_WOOD, 2); // 2 Balok kayu kualitas super
		GivePlayerExp(playerid, 15, "tebangan pohon presisi");

		SendClientMessage(playerid, COLOR_SUCCESS,
			"[SUKSES] Tebasan pohon sempurna! Memperoleh 2 Balok Kayu Kualitas Super (/inv). Jual di Sawmill (/sellwood).");
		PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
	} else {
		GivePlayerItem(playerid, ITEM_WOOD, 1);
		SendClientMessage(playerid, COLOR_INFO,
			"[INFO] Tebasan agak retak. Hanya memperoleh 1 balok kayu standar.");
	}
	return 1;
}

// ===========================================================================
// 3. KURIR PAKET (COURIER CAPTCHA VERIFICATION)
// ===========================================================================
stock TriggerCourierVerification(playerid)
{
	// Generate 4-digit security code
	new c1 = 'A' + random(26);
	new c2 = '0' + random(10);
	new c3 = 'A' + random(26);
	new c4 = '0' + random(10);

	format(gCourierCode[playerid], 8, "%c%c%c%c", c1, c2, c3, c4);

	new prompt[300];
	format(prompt, sizeof(prompt),
		"{FFFFFF}Paket telah tiba di depan rumah pemesan.\n\
Untuk konfirmasi serah terima dan memverifikasi keaslian pengantaran,\n\
masukkan {FFFF00}Kode Resi Penerima{FFFFFF} berikut ke kolom di bawah:\n\n\
Kode Resi: {2ECC71}%s", gCourierCode[playerid]);

	Dialog_Show(playerid, DialogCourierReceipt, DIALOG_STYLE_INPUT,
		"{3498DB}VERIFIKASI SERAH TERIMA RESI PAKET", prompt, "Konfirmasi", "Batal");
	return 1;
}

Dialog:DialogCourierReceipt(playerid, response, listitem, inputtext[])
{
	if (!response) {
		SendClientMessage(playerid, COLOR_ERROR, "[INFO] Serah terima paket ditunda.");
		return 1;
	}

	if (strcmp(inputtext, gCourierCode[playerid], true) == 0) {
		new pay = 150;
		new bonus = 35; // Bonus ketelitian
		gJobCash[playerid] += (pay + bonus);

		new msg[144];
		format(msg, sizeof(msg), "[RESI VALID] Paket sukses diserahkan! Mendapatkan upah rute $%d + bonus ketelitian $%d.", pay, bonus);
		SendClientMessage(playerid, COLOR_SUCCESS, msg);

		new next = gJobCheckpoint[playerid] + 1;
		if (next >= MAX_COURIER_POINTS) {
			ProcessJobComplete(playerid);
		} else {
			gJobCheckpoint[playerid] = next;
			CreateJobCheckpoint(playerid, next);
			SendClientMessage(playerid, COLOR_INFO, "[INFO] Menuju alamat pengiriman berikutnya di radar minimap.");
		}
	} else {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Kode resi yang Anda masukkan salah! Coba periksa kembali.");
		TriggerCourierVerification(playerid);
	}
	return 1;
}

// ===========================================================================
// 4. MEKANIK KENDARAAN (MECHANIC DIAGNOSTIC PUZZLE)
// ===========================================================================
stock TriggerMechanicDiagnostic(playerid, vehid)
{
	new partBug = random(3);
	gMechanicTargetPart[playerid] = partBug;
	gMechanicTargetVeh[playerid] = vehid;

	ApplyProgressBar(playerid, "INSPEKSI KELAYAKAN MESIN...", 3, "RP_OnFinishMechanicInspection", vehid);
	ApplyAnimation(playerid, "CAR", "Fixn_Car_Loop", 4.1, false, false, false, false, 0, 1);
	return 1;
}

public RP_OnFinishMechanicInspection(playerid, vehid)
{
	#pragma unused vehid
	new bug = gMechanicTargetPart[playerid];
	new prompt[300];

	if (bug == 0) {
		format(prompt, sizeof(prompt),
			"{FFFFFF}Hasil cek mekanik: Mesin tersendat dan pembakaran pincang.\n\
Tentukan penanganan servis mekanik yang paling tepat:\n\n\
1. [ Ganti Busi & Kalibrasi Injektor ] (Penanganan Tepat)\n\
2. [ Kuras Air Radiator ]\n\
3. [ Kencangkan Baut Knalpot ]");
	} else if (bug == 1) {
		format(prompt, sizeof(prompt),
			"{FFFFFF}Hasil cek mekanik: Indikator suhu radiator merah panas tinggi.\n\
Tentukan penanganan servis mekanik yang paling tepat:\n\n\
1. [ Setel Ulang Karburator ]\n\
2. [ Tambal Kebocoran Selang Radiator & Kipas ] (Penanganan Tepat)\n\
3. [ Ganti Kampas Rem Depan ]");
	} else {
		format(prompt, sizeof(prompt),
			"{FFFFFF}Hasil cek mekanik: Setir kemudi bergetar keras saat berbelok.\n\
Tentukan penanganan servis mekanik yang paling tepat:\n\n\
1. [ Ganti Busi Pengapian ]\n\
2. [ Isi Angin Ban Cadangan ]\n\
3. [ Spooring Balancing & Tie Rod Suspensi ] (Penanganan Tepat)");
	}

	Dialog_Show(playerid, DialogMechanicDiagnostic, DIALOG_STYLE_LIST,
		"{FFFF00}DIAGNOSTIK MEKANIK KENDARAAN", prompt, "Servis", "Batal");
	return 1;
}

Dialog:DialogMechanicDiagnostic(playerid, response, listitem, inputtext[])
{
	if (!response) {
		SendClientMessage(playerid, COLOR_INFO, "[INFO] Perbaikan dibatalkan.");
		return 1;
	}

	new target = gMechanicTargetPart[playerid];
	new veh = gMechanicTargetVeh[playerid];
	new bool:success = false;

	if (target == 0 && listitem == 0) success = true;
	else if (target == 1 && listitem == 1) success = true;
	else if (target == 2 && listitem == 2) success = true;

	if (success) {
		if (IsValidVehicle(veh)) {
			RepairVehicle(veh);
			SetVehicleHealth(veh, 1000.0);
		}
		new tip = 35 + random(25);
		GivePlayerMoneyEx(playerid, tip);
		GivePlayerExp(playerid, 15, "servis mekanik presisi");

		new msg[144];
		format(msg, sizeof(msg), "[SERVIS SEMPURNA] Analisa mekanik tepat! Kendaraan pulih 100%% & Anda mendapat tip $%d.", tip);
		SendClientMessage(playerid, COLOR_SUCCESS, msg);
		PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
	} else {
		if (IsValidVehicle(veh)) {
			SetVehicleHealth(veh, 700.0);
		}
		SendClientMessage(playerid, COLOR_INFO, "[INFO] Servis selesai namun komponen masih butuh penyesuaian lanjutan.");
	}
	return 1;
}

// ===========================================================================
// 5. PEMULUNG (SCAVENGER RECYCLE SORTING)
// ===========================================================================
stock TriggerScavengerMinigame(playerid)
{
	new prompt[300];
	format(prompt, sizeof(prompt),
		"{FFFFFF}Pilih sudut tumpukan tong sampah yang ingin Anda aduk teliti:\n\n\
1. [ Sisi Kiri ] - Tumpukan Kardus & Wadah Plastik Minuman\n\
2. [ Sisi Tengah ] - Kantong Hitam Terikat Rapat\n\
3. [ Sisi Bawah ] - Dasar Keranjang Logam Berkarat");

	Dialog_Show(playerid, DialogScavengerSort, DIALOG_STYLE_LIST,
		"{FFFF00}MEMILAH TONG SAMPAH KOTA", prompt, "Aduk", "Batal");
	return 1;
}

Dialog:DialogScavengerSort(playerid, response, listitem, inputtext[])
{
	if (!response) return 1;

	new roll = random(100);
	if (roll < 45) {
		new cash = 10 + random(25);
		GivePlayerMoneyEx(playerid, cash);
		GivePlayerItem(playerid, ITEM_RECYCLED, 2);
		new msg[144];
		format(msg, sizeof(msg), "[SUKSES] Menemukan 2x Barang Rongsokan dan dompet tercecer berisi uang $%d!", cash);
		SendClientMessage(playerid, COLOR_SUCCESS, msg);
	} else if (roll < 80) {
		GivePlayerItem(playerid, ITEM_RECYCLED, 3);
		SendClientMessage(playerid, COLOR_SUCCESS, "[SUKSES] Menemukan 3x Logam Daur Ulang bernilai jual tinggi (/inv).");
	} else {
		// Pecahan kaca / sampah basah
		new Float:hp;
		GetPlayerHealth(playerid, hp);
		if (hp > 15.0) SetPlayerHealth(playerid, hp - 5.0);
		SendClientMessage(playerid, COLOR_INFO, "[INFO] Tergores pecahan botol beling saat mengaduk sampah (-5 HP). Tetap berhati-hati!");
	}
	return 1;
}

// ===========================================================================
// 6. BUS DRIVER & SWEEPER (TRAFFIC SAFETY QTE)
// ===========================================================================
stock TriggerTrafficSafetyAlert(playerid)
{
	new lightColor = random(2);
	gTrafficTargetAnswer[playerid] = lightColor;

	new prompt[300];
	if (lightColor == 0) {
		format(prompt, sizeof(prompt),
			"{FFFFFF}PERINGATAN LALU LINTAS DI PEREMPATAN JALAN!\n\
Lampu lalu lintas di depan Anda menyala {2ECC71}HIJAU{FFFFFF}!\n\n\
Tindakan pengemudi profesional:\n\
1. [ Lanjutkan Laju Kendaraan dengan Waspada ] (Tindakan Tepat)\n\
2. [ Injak Rem Mendadak di Tengah Jalan ]");
	} else {
		format(prompt, sizeof(prompt),
			"{FFFFFF}PERINGATAN LALU LINTAS DI PEREMPATAN JALAN!\n\
Lampu lalu lintas di depan Anda menyala {E74C3C}MERAH{FFFFFF} dan pejalan kaki menyeberang!\n\n\
Tindakan pengemudi profesional:\n\
1. [ Terobos Lampu Merah dengan Cepat ]\n\
2. [ Hentikan Kendaraan Sebelum Garis Marka ] (Tindakan Tepat)");
	}

	Dialog_Show(playerid, DialogTrafficQTE, DIALOG_STYLE_LIST,
		"{3498DB}PERINGATAN KESELAMATAN LALU LINTAS", prompt, "Respon", "Batal");
	return 1;
}

Dialog:DialogTrafficQTE(playerid, response, listitem, inputtext[])
{
	if (!response) return 1;

	new target = gTrafficTargetAnswer[playerid];
	new bool:success = false;

	if (target == 0 && listitem == 0) success = true;
	else if (target == 1 && listitem == 1) success = true;

	if (success) {
		new bonus = 25;
		gJobCash[playerid] += bonus;
		new msg[144];
		format(msg, sizeof(msg), "[TERTIB LALU LINTAS] Mengemudi aman & mematuhi rambu jalan! Bonus ketertiban dinas +$%d.", bonus);
		SendClientMessage(playerid, COLOR_SUCCESS, msg);
	} else {
		SendClientMessage(playerid, COLOR_ERROR, "[PELANGGARAN] Anda tidak mematuhi etika lalu lintas kota!");
	}
	return 1;
}
