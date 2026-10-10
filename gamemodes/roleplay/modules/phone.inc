#if defined _INC_rp_phone
	#endinput
#endif
#define _INC_rp_phone

/*
==============================================================================
  NUSANTARA ROLEPLAY - phone.inc
  Sistem Telekomunikasi: Panggilan Suara, SMS, Handy Talkie (HT), & GPS
==============================================================================
*/

// Forward declarations
forward CallTimeout(playerid, targetid);

stock ShowPhoneMenu(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (GetPlayerItemCount(playerid, ITEM_PHONE) <= 0) {
		SendErr(playerid, "Anda tidak memiliki ponsel. Beli di toko 24/7.");
		return 0;
	}

	new statusStr[16];
	format(statusStr, sizeof(statusStr), "%s", gPlayerData[playerid][pPhoneStatus] ? "Aktif" : "Hening");

	new body[256];
	format(body, sizeof(body),
		"Nomor Telepon: %d\nStatus Mode: %s\n\n1. Lakukan Panggilan (/call)\n2. Kirim Pesan Singkat (/sms)\n3. Ganti Mode (Aktif / Hening)",
		gPlayerData[playerid][pPhone], statusStr);

	ShowPlayerDialog(playerid, DIALOG_PHONE_MENU, DIALOG_STYLE_LIST,
		"PONSEL PINTAR", "1. Panggil Nomor\n2. Kirim Pesan (SMS)\n3. Alihkan Mode Dering", "Pilih", "Tutup");
	return 1;
}

stock ProcessCall(playerid, const numberStr[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (GetPlayerItemCount(playerid, ITEM_PHONE) <= 0) {
		SendErr(playerid, "Anda tidak memiliki ponsel. Beli di toko 24/7.");
		return 0;
	}
	if (gPlayerData[playerid][pCallState] != 0) {
		SendErr(playerid, "Anda sedang dalam panggilan aktif. Gunakan /h untuk memutus sambungan.");
		return 0;
	}

	new targetNumber = strval(numberStr);
	if (targetNumber <= 0) {
		SendSyntax(playerid, "/call [nomor telepon]");
		return 0;
	}

	if (targetNumber == gPlayerData[playerid][pPhone]) {
		SendErr(playerid, "Nomor yang Anda tuju sedang sibuk (panggilan ke nomor sendiri).");
		return 0;
	}

	// Cari pemain yang memiliki nomor ini
	new targetid = -1;
	for (new i = 0; i < MAX_PLAYERS; i++) {
		if (IsPlayerConnected(i) && gPlayerData[i][pLoggedIn] && gPlayerData[i][pPhone] == targetNumber) {
			targetid = i;
			break;
		}
	}

	if (targetid == -1 || GetPlayerItemCount(targetid, ITEM_PHONE) <= 0) {
		SendErr(playerid, "Nomor yang Anda tuju tidak sedang aktif atau berada di luar jangkauan.");
		return 0;
	}

	if (gPlayerData[targetid][pCallState] != 0) {
		SendErr(playerid, "Nomor yang Anda tuju sedang dalam panggilan lain.");
		return 0;
	}

	// Mulai proses dering
	gPlayerData[playerid][pCallTarget] = targetid;
	gPlayerData[playerid][pCallState] = 1; // Memanggil

	gPlayerData[targetid][pCallTarget] = playerid;
	gPlayerData[targetid][pCallState] = 1; // Berdering

	SendInfo(playerid, "Menghubungi nomor %d... Menunggu jawaban.", targetNumber);

	if (gPlayerData[targetid][pPhoneStatus]) {
		// Ponsel berdering
		PlayerPlaySound(targetid, 1056, 0.0, 0.0, 0.0);
		SendRP(targetid, COLOR_YELLOW, "[TELEPON] Panggilan masuk dari %d (%s). Ketik /p untuk mengangkat atau /h untuk menolak.",
			gPlayerData[playerid][pPhone], gPlayerData[playerid][pName]);

		new ringNearby[128];
		format(ringNearby, sizeof(ringNearby), "* Ponsel milik %s berdering.", gPlayerData[targetid][pName]);
		SendNearbyPlayer(targetid, COLOR_RP_ME, ringNearby, 10.0);
	} else {
		SendInfo(targetid, "[MODE HENING] Panggilan masuk tak bersuara dari nomor %d.", gPlayerData[playerid][pPhone]);
	}

	SetTimerEx("CallTimeout", 25000, false, "ii", playerid, targetid);
	return 1;
}

public CallTimeout(playerid, targetid)
{
	if (IsPlayerConnected(playerid) && gPlayerData[playerid][pCallState] == 1 && gPlayerData[playerid][pCallTarget] == targetid) {
		SendErr(playerid, "Tidak ada jawaban dari penerima.");
		gPlayerData[playerid][pCallState] = 0;
		gPlayerData[playerid][pCallTarget] = -1;

		if (IsPlayerConnected(targetid) && gPlayerData[targetid][pCallTarget] == playerid) {
			gPlayerData[targetid][pCallState] = 0;
			gPlayerData[targetid][pCallTarget] = -1;
			SendInfo(targetid, "Panggilan tak terjawab.");
		}
	}
	return 1;
}

stock ProcessPickup(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pCallState] != 1) {
		SendErr(playerid, "Tidak ada panggilan masuk yang sedang berdering.");
		return 0;
	}

	new callerid = gPlayerData[playerid][pCallTarget];
	if (callerid == -1 || !IsPlayerConnected(callerid) || gPlayerData[callerid][pCallTarget] != playerid) {
		SendErr(playerid, "Panggilan telah terputus.");
		gPlayerData[playerid][pCallState] = 0;
		gPlayerData[playerid][pCallTarget] = -1;
		return 0;
	}

	gPlayerData[playerid][pCallState] = 2; // Terhubung
	gPlayerData[callerid][pCallState] = 2; // Terhubung

	SendSucc(playerid, "Panggilan terhubung. Berbicara langsung melalui chat biasa atau /call [pesan]. Ketik /h untuk menutup.");
	SendSucc(callerid, "Panggilan diangkat. Sambungan aktif. Ketik /h untuk memutus sambungan.");

	new msg[128];
	format(msg, sizeof(msg), "* %s mengangkat panggilan telepon.", gPlayerData[playerid][pName]);
	SendNearbyPlayer(playerid, COLOR_RP_ME, msg, RANGE_ME);
	return 1;
}

stock ProcessHangup(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pCallState] == 0) {
		SendErr(playerid, "Anda tidak sedang dalam panggilan telepon.");
		return 0;
	}

	new otherid = gPlayerData[playerid][pCallTarget];
	gPlayerData[playerid][pCallState] = 0;
	gPlayerData[playerid][pCallTarget] = -1;

	SendInfo(playerid, "Sambungan telepon diputus.");

	if (otherid != -1 && IsPlayerConnected(otherid) && gPlayerData[otherid][pCallTarget] == playerid) {
		gPlayerData[otherid][pCallState] = 0;
		gPlayerData[otherid][pCallTarget] = -1;
		SendInfo(otherid, "Pihak lawan telah memutus sambungan telepon.");
	}

	new msg[128];
	format(msg, sizeof(msg), "* %s menutup panggilan telepon.", gPlayerData[playerid][pName]);
	SendNearbyPlayer(playerid, COLOR_RP_ME, msg, RANGE_ME);
	return 1;
}

stock ProcessSMS(playerid, targetNumber, const message[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (GetPlayerItemCount(playerid, ITEM_PHONE) <= 0) {
		SendErr(playerid, "Anda tidak memiliki ponsel.");
		return 0;
	}
	if (targetNumber <= 0 || message[0] == '\0') {
		SendSyntax(playerid, "/sms [nomor tujuan] [isi pesan]");
		return 0;
	}

	new targetid = -1;
	for (new i = 0; i < MAX_PLAYERS; i++) {
		if (IsPlayerConnected(i) && gPlayerData[i][pLoggedIn] && gPlayerData[i][pPhone] == targetNumber) {
			targetid = i;
			break;
		}
	}

	if (targetid == -1 || GetPlayerItemCount(targetid, ITEM_PHONE) <= 0) {
		SendErr(playerid, "Pesan gagal terkirim. Nomor tujuan tidak aktif.");
		return 0;
	}

	// Biaya SMS $1
	TakePlayerMoneyEx(playerid, 1);

	SendRP(playerid, COLOR_YELLOW, "[SMS TERKIRIM] Ke: %d | Pesan: %s", targetNumber, message);
	SendRP(targetid, COLOR_YELLOW, "[SMS MASUK] Dari: %d (%s) | Pesan: %s",
		gPlayerData[playerid][pPhone], gPlayerData[playerid][pName], message);
	PlayerPlaySound(targetid, 1057, 0.0, 0.0, 0.0);
	return 1;
}

// ---------------------------------------------------------------------------
// HANDY TALKIE (HT / RADIO)
// ---------------------------------------------------------------------------
stock ShowRadioDialog(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (GetPlayerItemCount(playerid, ITEM_RADIO) <= 0) {
		SendErr(playerid, "Anda tidak memiliki Handy Talkie (HT). Beli di toko 24/7.");
		return 0;
	}

	new str[128];
	format(str, sizeof(str), "Frekuensi saat ini: %d MHz\n\nMasukkan nomor frekuensi baru (1 - 999):", gPlayerData[playerid][pRadioFreq]);
	ShowPlayerDialog(playerid, DIALOG_RADIO_FREQ, DIALOG_STYLE_INPUT,
		"PENGATURAN FREKUENSI HT", str, "Atur", "Tutup");
	return 1;
}

stock ProcessRadio(playerid, const message[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (GetPlayerItemCount(playerid, ITEM_RADIO) <= 0) {
		SendErr(playerid, "Anda tidak memiliki Handy Talkie (HT).");
		return 0;
	}
	if (gPlayerData[playerid][pRadioFreq] <= 0) {
		SendErr(playerid, "Frekuensi HT belum diatur. Gunakan /setfreq [1-999].");
		return 0;
	}
	if (message[0] == '\0') {
		SendSyntax(playerid, "/r [pesan radio]");
		return 0;
	}

	new freq = gPlayerData[playerid][pRadioFreq];
	new out[180];
	format(out, sizeof(out), "[RADIO CH: %d] %s: %s, ganti.", freq, gPlayerData[playerid][pName], message);

	for (new i = 0; i < MAX_PLAYERS; i++) {
		if (!IsPlayerConnected(i) || !gPlayerData[i][pLoggedIn]) continue;
		if (GetPlayerItemCount(i, ITEM_RADIO) > 0 && gPlayerData[i][pRadioFreq] == freq) {
			SendClientMessage(i, COLOR_RADIO, out);
		}
	}

	// Narasi lokal
	new localMsg[128];
	format(localMsg, sizeof(localMsg), "* %s berbicara melalui radio HT.", gPlayerData[playerid][pName]);
	SendNearbyPlayer(playerid, COLOR_RP_ME, localMsg, 8.0);
	return 1;
}

// ---------------------------------------------------------------------------
// GPS NAVIGASI
// ---------------------------------------------------------------------------
stock ShowGPSMenu(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	ShowPlayerDialog(playerid, DIALOG_GPS_MENU, DIALOG_STYLE_LIST,
		"SISTEM GPS NAVIGASI",
		"1. Bank Nusantara (Pusat Keuangan)\n\
2. Kantor Balai Kota (Pemerintah)\n\
3. Rumah Sakit All Saints (SAMD)\n\
4. Kantor Kepolisian (SAPD LSPD)\n\
5. Pusat Pekerjaan Umum (Job Center)\n\
6. SPBU Bahan Bakar Utama\n\
7. Toko Serba Ada 24/7 (Idlewood)\n\
8. Dealer Kendaraan Los Santos\n\
9. Matikan Tanda Navigasi",
		"Tandai", "Batal");
	return 1;
}
