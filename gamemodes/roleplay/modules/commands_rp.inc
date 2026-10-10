#if defined _INC_rp_commands_rp
	#endinput
#endif
#define _INC_rp_commands_rp

/*
==============================================================================
  NUSANTARA ROLEPLAY - commands_rp.inc
  Perintah Roleplay Standar Komunitas:
  /me, /do, /b, /o, /w, /s, /l, /id, /pm, /admins, /report, /ask, /help
==============================================================================
*/

// ---------------------------------------------------------------------------
// /me - Aksi Karakter In-Character
// ---------------------------------------------------------------------------
stock ProcessMe(playerid, const text[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (text[0] == '\0') {
		SendSyntax(playerid, "/me [tindakan karakter]");
		return 0;
	}

	new msg[160];
	format(msg, sizeof(msg), "* %s %s", gPlayerData[playerid][pName], text);
	SendNearbyPlayer(playerid, COLOR_RP_ME, msg, RANGE_ME);
	return 1;
}

// ---------------------------------------------------------------------------
// /do - Narasi Keadaan Lingkungan / Jawaban Situasi
// ---------------------------------------------------------------------------
stock ProcessDo(playerid, const text[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (text[0] == '\0') {
		SendSyntax(playerid, "/do [keadaan sekitar / jawaban]");
		return 0;
	}

	new msg[160];
	format(msg, sizeof(msg), "* %s (( %s ))", text, gPlayerData[playerid][pName]);
	SendNearbyPlayer(playerid, COLOR_RP_DO, msg, RANGE_ME);
	return 1;
}

// ---------------------------------------------------------------------------
// /b - Local Out-Of-Character (OOC) Chat
// ---------------------------------------------------------------------------
stock ProcessB(playerid, const text[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (text[0] == '\0') {
		SendSyntax(playerid, "/b [pesan OOC]");
		return 0;
	}

	new msg[160];
	format(msg, sizeof(msg), "(( [OOC] %s: %s ))", gPlayerData[playerid][pName], text);
	SendNearbyPlayer(playerid, COLOR_LOCAL_OOC, msg, RANGE_OOC);
	return 1;
}

// ---------------------------------------------------------------------------
// /o - Global Out-Of-Character (OOC) Chat
// ---------------------------------------------------------------------------
stock ProcessO(playerid, const text[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (text[0] == '\0') {
		SendSyntax(playerid, "/o [pesan OOC global]");
		return 0;
	}

	new msg[160];
	format(msg, sizeof(msg), "[OOC] %s: %s", gPlayerData[playerid][pName], text);
	SendClientMessageToAll(COLOR_OOC, msg);
	return 1;
}

// ---------------------------------------------------------------------------
// /w - Whisper (Bisik-bisik Pribadi)
// ---------------------------------------------------------------------------
stock ProcessWhisper(playerid, targetid, const text[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (targetid == -1 || text[0] == '\0') {
		SendSyntax(playerid, "/w [playerid] [pesan]");
		return 0;
	}
	if (!IsPlayerConnected(targetid) || !gPlayerData[targetid][pLoggedIn]) {
		SendErr(playerid, "Pemain tidak ditemukan atau belum login.");
		return 0;
	}
	if (targetid == playerid) {
		SendErr(playerid, "Anda tidak dapat berbisik kepada diri sendiri.");
		return 0;
	}
	if (!IsPlayerNear(playerid, targetid, RANGE_WHISPER)) {
		SendErr(playerid, "Pemain berada terlalu jauh untuk berbisik.");
		return 0;
	}

	new msg[160];
	format(msg, sizeof(msg), "(bisik ke %s) %s", gPlayerData[targetid][pName], text);
	SendClientMessage(playerid, COLOR_YELLOW, msg);

	format(msg, sizeof(msg), "(bisik dari %s) %s", gPlayerData[playerid][pName], text);
	SendClientMessage(targetid, COLOR_YELLOW, msg);
	return 1;
}

// ---------------------------------------------------------------------------
// /s - Shout (Berteriak Kencang)
// ---------------------------------------------------------------------------
stock ProcessShout(playerid, const text[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (text[0] == '\0') {
		SendSyntax(playerid, "/s [pesan teriakan]");
		return 0;
	}

	new msg[160];
	format(msg, sizeof(msg), "%s berteriak: %s!", gPlayerData[playerid][pName], text);
	SendNearbyPlayer(playerid, COLOR_WHITE, msg, RANGE_SHOUT);
	return 1;
}

// ---------------------------------------------------------------------------
// /l - Low (Bicara Pelan)
// ---------------------------------------------------------------------------
stock ProcessLow(playerid, const text[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (text[0] == '\0') {
		SendSyntax(playerid, "/l [pesan pelan]");
		return 0;
	}

	new msg[160];
	format(msg, sizeof(msg), "(pelan) %s: %s", gPlayerData[playerid][pName], text);
	SendNearbyPlayer(playerid, COLOR_DARKGREY, msg, RANGE_LOW);
	return 1;
}

// ---------------------------------------------------------------------------
// /id - Pencarian ID Pemain
// ---------------------------------------------------------------------------
stock ProcessFindID(playerid, const query[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (query[0] == '\0') {
		SendSyntax(playerid, "/id [nama / id pemain]");
		return 0;
	}

	new targetName[MAX_PLAYER_NAME];
	new found = 0;

	SendRP(playerid, COLOR_LIGHTBLUE, "================== HASIL PENCARIAN PEMAIN ==================");
	for (new i = 0; i < MAX_PLAYERS; i++) {
		if (!IsPlayerConnected(i) || !gPlayerData[i][pLoggedIn]) continue;
		GetPlayerName(i, targetName, sizeof(targetName));

		if (strfind(targetName, query, true) != -1 || i == strval(query)) {
			SendRP(playerid, COLOR_WHITE, "ID: %d | Nama: %s | Level: %d | Ping: %d",
				i, targetName, gPlayerData[i][pLevel], GetPlayerPing(i));
			found++;
		}
	}

	if (found == 0) {
		SendErr(playerid, "Tidak ada pemain yang cocok dengan '%s'.", query);
	}
	return 1;
}

// ---------------------------------------------------------------------------
// /admins - Daftar Staf Administrator yang Sedang Online
// ---------------------------------------------------------------------------
stock ProcessAdmins(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;

	SendRP(playerid, COLOR_LIGHTBLUE, "================== ADMINISTRATOR ONLINE ==================");
	new count = 0;
	for (new i = 0; i < MAX_PLAYERS; i++) {
		if (IsPlayerConnected(i) && gPlayerData[i][pLoggedIn] && gPlayerData[i][pAdmin] > 0) {
			SendRP(playerid, COLOR_WHITE, "ID: %d | %s | Level Admin: %d",
				i, gPlayerData[i][pName], gPlayerData[i][pAdmin]);
			count++;
		}
	}
	if (count == 0) {
		SendRP(playerid, COLOR_GREY, "Saat ini tidak ada administrator yang sedang bertugas.");
	}
	return 1;
}

// ---------------------------------------------------------------------------
// /pm - Pesan Pribadi OOC
// ---------------------------------------------------------------------------
stock ProcessPM(playerid, targetid, const message[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (targetid < 0 || targetid >= MAX_PLAYERS || !IsPlayerConnected(targetid) || !gPlayerData[targetid][pLoggedIn]) {
		SendErr(playerid, "Pemain tujuan tidak valid atau offline.");
		return 0;
	}
	if (targetid == playerid) {
		SendErr(playerid, "Anda tidak dapat mengirim PM ke diri sendiri.");
		return 0;
	}
	if (message[0] == '\0') {
		SendSyntax(playerid, "/pm [playerid] [pesan]");
		return 0;
	}

	SendRP(playerid, COLOR_YELLOW, "PM Terkirim ke %s (ID %d): %s", gPlayerData[targetid][pName], targetid, message);
	SendRP(targetid, COLOR_YELLOW, "PM Masuk dari %s (ID %d): %s", gPlayerData[playerid][pName], playerid, message);
	PlayerPlaySound(targetid, 1057, 0.0, 0.0, 0.0);
	return 1;
}

// ---------------------------------------------------------------------------
// /report & /ask
// ---------------------------------------------------------------------------
stock ProcessReport(playerid, const text[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (text[0] == '\0') {
		SendSyntax(playerid, "/report [laporan pelanggaran / bantuan]");
		return 0;
	}

	new msg[160];
	format(msg, sizeof(msg), "[REPORT] Dari %s (ID %d): %s", gPlayerData[playerid][pName], playerid, text);
	SendSucc(playerid, "Laporan Anda telah diteruskan kepada staf admin yang online.");

	for (new i = 0; i < MAX_PLAYERS; i++) {
		if (IsPlayerConnected(i) && gPlayerData[i][pAdmin] > 0) {
			SendClientMessage(i, COLOR_ADMIN_CHAT, msg);
		}
	}
	return 1;
}

stock ProcessAsk(playerid, const text[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (text[0] == '\0') {
		SendSyntax(playerid, "/ask [pertanyaan seputar roleplay]");
		return 0;
	}

	new msg[160];
	format(msg, sizeof(msg), "[ASK] Dari %s (ID %d): %s", gPlayerData[playerid][pName], playerid, text);
	SendSucc(playerid, "Pertanyaan Anda telah diteruskan kepada Helper dan Admin.");

	for (new i = 0; i < MAX_PLAYERS; i++) {
		if (IsPlayerConnected(i) && gPlayerData[i][pAdmin] > 0) {
			SendClientMessage(i, COLOR_LIGHTBLUE, msg);
		}
	}
	return 1;
}

// ---------------------------------------------------------------------------
// /help - Buku Panduan Server
// ---------------------------------------------------------------------------
stock ShowHelpDialog(playerid)
{
	new buf[800];
	format(buf, sizeof(buf),
		"Kategori Perintah Utama Nusantara Roleplay:\n\n\
1. Roleplay Chat:\n\
   /me [aksi], /do [narasi], /b [ooc lokal], /w [id] [pesan], /s [teriak], /l [pelan], /pm [id] [pesan]\n\n\
2. Karakter & Inventaris:\n\
   /stats, /inv (atau /items), /pay [id] [jumlah], /id [nama], /admins\n\n\
3. Telekomunikasi & Navigasi:\n\
   /call [nomor], /p (angkat), /h (tutup), /sms [nomor] [pesan], /setfreq [1-999], /r [pesan], /gps\n\n\
4. Kendaraan:\n\
   /v (menu kendaraan), /engine (tombol N), /lights (tombol H), /lock, /hood, /trunk, /refuel, /buyveh\n\n\
5. Bisnis & Toko:\n\
   /buy (di toko 24/7), /bank, /atm, /transfer, /withdraw, /deposit\n\n\
6. Pekerjaan & Faksi:\n\
   /job, /quitjob, /duty, /cuff, /uncuff, /frisk, /ticket, /arrest, /heal, /revive, /m (megaphone)");

	ShowPlayerDialog(playerid, DIALOG_HELP, DIALOG_STYLE_MSGBOX,
		"PANDUAN LENGKAP SERVER - NUSANTARA RP", buf, "Tutup", "");
	return 1;
}

stock IsPlayerNear(playerid, targetid, Float:range)
{
	new Float:px, Float:py, Float:pz, Float:tx, Float:ty, Float:tz;
	GetPlayerPos(playerid, px, py, pz);
	GetPlayerPos(targetid, tx, ty, tz);
	return (VectorSize(px - tx, py - ty, pz - tz) <= range);
}
