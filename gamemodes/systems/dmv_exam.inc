#if defined _INC_rp_dmv_exam
	#endinput
#endif
#define _INC_rp_dmv_exam

/*
==============================================================================
 dmv_exam.inc
 NUSANTARA ROLEPLAY - Driving License Exam & Police Roadside License Inspection
==============================================================================
*/

#define DMV_EXAM_FEE            150

stock DMV_Init()
{
	print("[DMV] Sistem ujian SIM (Surat Izin Mengemudi) & pemeriksaan lisensi aktif.");
	return 1;
}

stock ShowDMVExamDialog(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;

	if (gPlayerData[playerid][pDriveLic]) {
		SendClientMessage(playerid, COLOR_INFO, "INFO: Anda sudah memiliki Surat Izin Mengemudi (SIM) resmi.");
		return 0;
	}

	if (!IsPlayerNearPoint(playerid, GOV_X, GOV_Y, GOV_Z, 10.0)) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus berada di Kantor Pelayanan Warga Balai Kota SAGOV (/gps).");
		return 0;
	}

	if (gPlayerData[playerid][pMoney] < DMV_EXAM_FEE && gPlayerData[playerid][pBank] < DMV_EXAM_FEE) {
		new errFee[128];
		format(errFee, sizeof(errFee), "ERROR: Biaya ujian teori SIM sebesar $%d tidak mencukupi.", DMV_EXAM_FEE);
		SendClientMessage(playerid, COLOR_ERROR, errFee);
		return 0;
	}

	new dialogContent[400];
	format(dialogContent, sizeof(dialogContent),
		"Pilih tindakan pengemudi yang paling tepat:\n\n\
{FFFFFF}1. Menambah laju kecepatan dan mendahului ambulans darurat\n\
{4CD964}2. Memperlambat laju, merapat ke bahu jalan dan memberi prioritas jalan\n\
{FFFFFF}3. Tetap melaju santai di lajur tengah tanpa memberi ruang");

	RP_ShowPlayerDialog(playerid, DIALOG_DMV_THEORY, DIALOG_STYLE_LIST,
		"UJIAN TEORI SIM KOTA LOS SANTOS ($150)", dialogContent, "Pilih", "Batal");
	return 1;
}

stock OnDMVDialogResponse(playerid, listitem)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return 1;
	if (gPlayerData[playerid][pDriveLic]) return 1;
	if (IsPlayerConnected(playerid) && !IsPlayerNearPoint(playerid, GOV_X, GOV_Y, GOV_Z, 10.0)) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda terlalu jauh dari loket Balai Kota.");
		return 1;
	}

	if (listitem == 1) {
		// Jawaban Benar
		// [SECURITY] Cek pembayaran: bila uang tunai & saldo bank tidak cukup,
		// SIM TIDAK boleh diberikan (sebelumnya return TakePlayerBankEx diabaikan
		// sehingga pemain miskin dapat SIM gratis).
		new bool:paid = false;
		if (gPlayerData[playerid][pMoney] >= DMV_EXAM_FEE) {
			paid = (TakePlayerMoneyEx(playerid, DMV_EXAM_FEE, "biaya ujian SIM") == 1);
		} else if (TakePlayerBankEx(playerid, DMV_EXAM_FEE, "biaya ujian SIM") == 1) {
			paid = true;
		}

		if (!paid) {
			PlayerPlaySound(playerid, 1053, 0.0, 0.0, 0.0);
			SendClientMessage(playerid, COLOR_ERROR,
				"[SAGOV DMV] Uang tunai & saldo bank Anda tidak mencukupi untuk biaya ujian SIM ($150).");
			return 1;
		}

		gPlayerData[playerid][pDriveLic] = true;
		DB_SavePlayerData(playerid);

		PlayerPlaySound(playerid, 1058, 0.0, 0.0, 0.0);
		SendClientMessage(playerid, COLOR_SUCCESS,
			"[SAGOV DMV] SELAMAT! Jawaban Anda benar. Anda dinyatakan LULUS dan resmi memperoleh Surat Izin Mengemudi (SIM)!");
	} else {
		// Jawaban Salah
		PlayerPlaySound(playerid, 1053, 0.0, 0.0, 0.0);
		SendClientMessage(playerid, COLOR_ERROR,
			"[SAGOV DMV] Maaf, jawaban Anda salah. Pelajari kembali tata tertib keselamatan lalu lintas dan coba lagi (/dmv).");
	}
	return 1;
}

stock ProcessCheckDriverLicense(playerid, targetid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return 0;

	if (gPlayerData[playerid][pFaction] != FACTION_POLICE || !gPlayerData[playerid][pDuty]) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Pemeriksaan surat kelayakan berkendara khusus petugas Kepolisian SAPD!");
		return 0;
	}

	if (!IsPlayerConnected(targetid) || !gPlayerData[targetid][pLoggedIn]) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Pemain sasaran tidak terhubung.");
		return 0;
	}

	if (!IsPlayerNear(playerid, targetid, 4.0)) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus berada di dekat pengemudi yang diperiksa.");
		return 0;
	}

	new licStatus[32];
	if (gPlayerData[targetid][pDriveLic]) format(licStatus, sizeof(licStatus), "{4CD964}RESMI & AKTIF");
	else format(licStatus, sizeof(licStatus), "{FF3B30}TIDAK MEMILIKI SIM");

	new checkMsg[160];
	format(checkMsg, sizeof(checkMsg),
		"[SAPD RAZIA] Identitas: %s | Status Surat Izin Mengemudi (SIM): %s",
		gPlayerData[targetid][pName], licStatus);
	SendClientMessage(playerid, COLOR_LIGHTBLUE, checkMsg);

	new vicMsg[160];
	format(vicMsg, sizeof(vicMsg),
		"[SAPD RAZIA] Petugas %s sedang memeriksa kelayakan Surat Izin Mengemudi (SIM) Anda.",
		gPlayerData[playerid][pName]);
	SendClientMessage(targetid, COLOR_YELLOW, vicMsg);
	return 1;
}
