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
forward CallTimeout(playerid, targetid, sessionCaller, sessionTarget);

stock ShowPhoneMenu(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendErr(playerid, "Anda tidak dapat menggunakan handphone saat pingsan atau terborgol.");
		return 0;
	}
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

	RP_ShowPlayerDialog(playerid, DIALOG_PHONE_MENU, DIALOG_STYLE_LIST,
		"PONSEL PINTAR", "1. Panggil Nomor\n2. Kirim Pesan (SMS)\n3. Alihkan Mode Dering", "Pilih", "Tutup");
	return 1;
}

stock ProcessCall(playerid, const numberStr[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendErr(playerid, "Anda tidak dapat menggunakan handphone saat pingsan atau terborgol.");
		return 0;
	}
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
		SendSyntax(playerid, "/call [nomor telepon / 911]");
		return 0;
	}

	// Layanan Darurat 911
	if (targetNumber == 911) {
		RP_ShowPlayerDialog(playerid, DIALOG_911_SERVICE, DIALOG_STYLE_LIST,
			"LAYANAN DARURAT 911 LOS SANTOS",
			"1. Kepolisian (SAPD) - Laporkan Tindak Kriminalitas & Situasi Darurat\n\
2. Medis & Ambulans (SAMD) - Laporkan Korban Terluka & Kecelakaan",
			"Pilih", "Batal");
		return 1;
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

	SetTimerEx("CallTimeout", 25000, false, "iiii", playerid, targetid, gPlayerSession[playerid], gPlayerSession[targetid]);
	return 1;
}

public CallTimeout(playerid, targetid, sessionCaller, sessionTarget)
{
	if (!IsPlayerConnected(playerid) || gPlayerSession[playerid] != sessionCaller) return 1;
	if (gPlayerData[playerid][pCallState] == 1 && gPlayerData[playerid][pCallTarget] == targetid) {
		SendErr(playerid, "Tidak ada jawaban dari penerima.");
		gPlayerData[playerid][pCallState] = 0;
		gPlayerData[playerid][pCallTarget] = -1;

		if (IsPlayerConnected(targetid) && gPlayerSession[targetid] == sessionTarget && gPlayerData[targetid][pCallTarget] == playerid) {
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
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendErr(playerid, "Anda tidak dapat mengangkat telepon saat pingsan atau terborgol.");
		return 0;
	}
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
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendErr(playerid, "Anda tidak dapat menggunakan handphone saat pingsan atau terborgol.");
		return 0;
	}
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
	if (gPlayerData[playerid][pMoney] < 1) {
		SendErr(playerid, "Uang tunai Anda tidak mencukupi untuk pulsa SMS ($1).");
		return 0;
	}
	if (!TakePlayerMoneyEx(playerid, 1, "biaya kirim sms")) {
		SendErr(playerid, "Gagal membayar pulsa SMS ($1).");
		return 0;
	}

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
	RP_ShowPlayerDialog(playerid, DIALOG_RADIO_FREQ, DIALOG_STYLE_INPUT,
		"PENGATURAN FREKUENSI HT", str, "Atur", "Tutup");
	return 1;
}

stock ProcessRadio(playerid, const message[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendErr(playerid, "Anda tidak dapat menggunakan radio HT saat pingsan atau terborgol.");
		return 0;
	}
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
// Sistem Panggilan Darurat 911 (Emergency Dispatch)
// ---------------------------------------------------------------------------
stock CreateDispatchReport(playerid, service, const message[])
{
	new slot = -1;
	for (new i = 0; i < MAX_DISPATCHES; i++) {
		if (!gDispatches[i][dActive]) {
			slot = i;
			break;
		}
	}
	if (slot == -1) slot = 0; // Timpa yang terlama jika penuh

	new Float:px, Float:py, Float:pz;
	GetPlayerPos(playerid, px, py, pz);

	gDispatches[slot][dActive] = true;
	gDispatches[slot][dCallerID] = playerid;
	format(gDispatches[slot][dCallerName], MAX_PLAYER_NAME, "%s", gPlayerData[playerid][pName]);
	gDispatches[slot][dCallerPhone] = gPlayerData[playerid][pPhone];
	gDispatches[slot][dService] = service;
	format(gDispatches[slot][dMessage], 128, "%s", message);
	gDispatches[slot][dX] = px;
	gDispatches[slot][dY] = py;
	gDispatches[slot][dZ] = pz;
	gDispatches[slot][dTime] = gettime();
	gDispatches[slot][dResponderID] = -1;

	SendSucc(playerid, "[911] Laporan darurat Anda berhasil dikirim ke unit terkait.");
	SendInfo(playerid, "Harap tetap berada di lokasi atau cari perlindungan yang aman.");
	PlayerPlaySound(playerid, 1058, 0.0, 0.0, 0.0);

	new broadcastMsg[192];
	new serviceName[16];
	format(serviceName, sizeof(serviceName), "%s", (service == 1) ? "SAPD POLISI" : "SAMD MEDIS");

	format(broadcastMsg, sizeof(broadcastMsg),
		"[DISPATCH 911 - %s] #%d dari %s (Telp: %d): \"%s\" | Ketik /acceptdispatch %d",
		serviceName, slot + 1, gPlayerData[playerid][pName], gPlayerData[playerid][pPhone], message, slot + 1);

	for (new i = 0; i < MAX_PLAYERS; i++) {
		if (!IsPlayerConnected(i) || !gPlayerData[i][pLoggedIn] || !gPlayerData[i][pDuty]) continue;
		if (service == 1 && gPlayerData[i][pFaction] == FACTION_POLICE) {
			SendClientMessage(i, COLOR_YELLOW, broadcastMsg);
			PlayerPlaySound(i, 1057, 0.0, 0.0, 0.0);
		} else if (service == 2 && gPlayerData[i][pFaction] == FACTION_MEDIC) {
			SendClientMessage(i, COLOR_YELLOW, broadcastMsg);
			PlayerPlaySound(i, 1057, 0.0, 0.0, 0.0);
		}
	}
	return 1;
}

// ---------------------------------------------------------------------------
// GPS NAVIGASI
// ---------------------------------------------------------------------------
stock ShowGPSMenu(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;

	new dialogContent[1150];
	format(dialogContent, sizeof(dialogContent),
		"Lokasi Tujuan	Jarak Dari Posisi\n\
{FFFF00}1. Bank Nusantara (Pusat Finansial)	{4CD964}%.0f meter\n\
{FFFF00}2. Kantor Kepolisian (SAPD LSPD)	{4CD964}%.0f meter\n\
{FFFF00}3. RS All Saints General (SAMD ASGH)	{4CD964}%.0f meter\n\
{FFFF00}4. Balai Kota Los Santos (SAGOV)	{4CD964}%.0f meter\n\
{FFFF00}5. SPBU Bahan Bakar Idlewood	{4CD964}%.0f meter\n\
{FFFF00}6. Toko Serba Ada 24/7 Idlewood	{4CD964}%.0f meter\n\
{FFFF00}7. Bengkel Modifikasi Mekanik (Workshop)	{4CD964}%.0f meter\n\
{FFFF00}8. Showroom Dealership Kendaraan	{4CD964}%.0f meter\n\
{33CCFF}9. Job: Logistik Pengiriman Kurir Paket	{4CD964}%.0f meter\n\
{33CCFF}10. Job: Depot Pembersih Jalan Sweeper	{4CD964}%.0f meter\n\
{33CCFF}11. Job: Depot Truk Angkutan Hauler	{4CD964}%.0f meter\n\
{33CCFF}12. Job: Terminal Bus Umum Los Santos	{4CD964}%.0f meter\n\
{33CCFF}13. Job: Dermaga Mancing Santa Maria Pier	{4CD964}%.0f meter\n\
{33CCFF}14. Job: Hutan Penebangan Shady Creeks	{4CD964}%.0f meter\n\
{33CCFF}15. Job: Tempat Pengolahan Kayu (Sawmill)	{4CD964}%.0f meter\n\
{33CCFF}16. Job: Tempat Daur Ulang Pemulung	{4CD964}%.0f meter\n\
{FF6666}X. [ MATIKAN TANDA NAVIGASI / GPS ]	{FF6666}-",
		GetPlayerDistanceFromPoint(playerid, BANK_X, BANK_Y, BANK_Z),
		GetPlayerDistanceFromPoint(playerid, POLICE_X, POLICE_Y, POLICE_Z),
		GetPlayerDistanceFromPoint(playerid, MEDIC_X, MEDIC_Y, MEDIC_Z),
		GetPlayerDistanceFromPoint(playerid, GOV_X, GOV_Y, GOV_Z),
		GetPlayerDistanceFromPoint(playerid, FUEL_STATION_X, FUEL_STATION_Y, FUEL_STATION_Z),
		GetPlayerDistanceFromPoint(playerid, STORE_IDLEWOOD_X, STORE_IDLEWOOD_Y, STORE_IDLEWOOD_Z),
		GetPlayerDistanceFromPoint(playerid, WORKSHOP_X, WORKSHOP_Y, WORKSHOP_Z),
		GetPlayerDistanceFromPoint(playerid, DEALER_X, DEALER_Y, DEALER_Z),
		GetPlayerDistanceFromPoint(playerid, COURIER_X, COURIER_Y, COURIER_Z),
		GetPlayerDistanceFromPoint(playerid, SWEEPER_X, SWEEPER_Y, SWEEPER_Z),
		GetPlayerDistanceFromPoint(playerid, TRUCKER_X, TRUCKER_Y, TRUCKER_Z),
		GetPlayerDistanceFromPoint(playerid, BUS_DEPOT_X, BUS_DEPOT_Y, BUS_DEPOT_Z),
		GetPlayerDistanceFromPoint(playerid, FISH_PIER_X, FISH_PIER_Y, FISH_PIER_Z),
		GetPlayerDistanceFromPoint(playerid, LUMBER_X, LUMBER_Y, LUMBER_Z),
		GetPlayerDistanceFromPoint(playerid, SAWMILL_X, SAWMILL_Y, SAWMILL_Z),
		GetPlayerDistanceFromPoint(playerid, TRASH_BUYER_X, TRASH_BUYER_Y, TRASH_BUYER_Z)
	);

	RP_ShowPlayerDialog(playerid, DIALOG_GPS_MENU, DIALOG_STYLE_TABLIST_HEADERS,
		"SISTEM GPS NAVIGASI SMARTPHONE", dialogContent, "Tandai", "Batal");
	return 1;
}
