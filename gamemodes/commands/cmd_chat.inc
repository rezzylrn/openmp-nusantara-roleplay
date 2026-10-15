#if defined _INC_rp_cmd_chat
	#endinput
#endif
#define _INC_rp_cmd_chat

/*
==============================================================================
 cmd_chat.inc (Komunikasi & Chat IC/OOC)
 NUSANTARA ROLEPLAY - Modular Command Architecture
==============================================================================
*/

stock ProcessWhisper(playerid, targetid, const text[])
{
	if (targetid == -1 || text[0] == '\0') return SendSyntax(playerid, "/w [targetid] [pesan]");
	if (!IsPlayerConnected(targetid) || !gPlayerData[targetid][pLoggedIn]) return SendErr(playerid, "Target tidak aktif.");
	if (targetid == playerid) return SendErr(playerid, "Tidak bisa berbisik ke diri sendiri.");
	if (!IsPlayerNear(playerid, targetid, RANGE_WHISPER)) return SendErr(playerid, "Target terlalu jauh.");
	new msg[160];
	format(msg, sizeof(msg), "(bisik ke %s) %s", gPlayerData[targetid][pName], text);
	SendClientMessage(playerid, COLOR_YELLOW, msg);
	format(msg, sizeof(msg), "(bisik dari %s) %s", gPlayerData[playerid][pName], text);
	SendClientMessage(targetid, COLOR_YELLOW, msg);
	return 1;
}

stock ProcessShout(playerid, const text[])
{
	new msg[160];
	format(msg, sizeof(msg), "%s berteriak: %s!", gPlayerData[playerid][pName], text);
	SendNearbyPlayer(playerid, COLOR_WHITE, msg, RANGE_SHOUT);
	return 1;
}

stock ProcessLow(playerid, const text[])
{
	new msg[160];
	format(msg, sizeof(msg), "(pelan) %s: %s", gPlayerData[playerid][pName], text);
	SendNearbyPlayer(playerid, COLOR_DARKGREY, msg, RANGE_LOW);
	return 1;
}

stock ProcessPM(playerid, targetid, const message[])
{
	if (!IsPlayerConnected(targetid) || !gPlayerData[targetid][pLoggedIn]) return SendErr(playerid, "Target tidak aktif.");
	if (targetid == playerid) return SendErr(playerid, "Tidak bisa kirim PM ke diri sendiri.");
	SendRP(playerid, COLOR_YELLOW, "PM Terkirim ke %s (ID %d): %s", gPlayerData[targetid][pName], targetid, message);
	SendRP(targetid, COLOR_YELLOW, "PM Dari %s (ID %d): %s", gPlayerData[playerid][pName], playerid, message);
	PlayerPlaySound(targetid, 1057, 0.0, 0.0, 0.0);
	return 1;
}

stock ProcessReport(playerid, const text[])
{
	new msg[160];
	format(msg, sizeof(msg), "[REPORT] %s (ID %d): %s", gPlayerData[playerid][pName], playerid, text);
	SendSucc(playerid, "Laporan telah diteruskan ke admin.");
	for (new i = 0; i < MAX_PLAYERS; i++) {
		if (IsPlayerConnected(i) && gPlayerData[i][pAdmin] > 0) SendClientMessage(i, COLOR_ADMIN_CHAT, msg);
	}
	return 1;
}

stock ProcessAsk(playerid, const text[])
{
	new msg[160];
	format(msg, sizeof(msg), "[ASK] %s (ID %d): %s", gPlayerData[playerid][pName], playerid, text);
	SendSucc(playerid, "Pertanyaan telah diteruskan ke staf Helper.");
	for (new i = 0; i < MAX_PLAYERS; i++) {
		if (IsPlayerConnected(i) && gPlayerData[i][pAdmin] > 0) SendClientMessage(i, COLOR_LIGHTBLUE, msg);
	}
	return 1;
}

stock ShowHelpDialog(playerid)
{
	new buf[800];
	format(buf, sizeof(buf),
		"PANDUAN PERINTAH UTAMA:\n\n\
1. Roleplay Chat:\n\
   /me, /do, /b, /w [id], /s, /l, /pm [id], /id [nama], /admins\n\n\
2. Inventaris & Toko:\n\
   /inv, /buy (di toko 24/7), /gps\n\n\
3. Telekomunikasi:\n\
   /call, /p, /h, /sms, /setfreq, /r, /d, /m\n\n\
4. Kendaraan:\n\
   /v, /engine (N), /lights (H), /lock, /hood, /trunk, /refuel, /buyveh\n\n\
5. Keuangan:\n\
   /stats, /pay, /bank, /atm\n\n\
6. Pekerjaan:\n\
   /job, /quitjob, /searchtrash, /selltrash, /cutwood, /sellwood, /fish, /sellfish, /service");
	RP_ShowPlayerDialog(playerid, DIALOG_HELP, DIALOG_STYLE_MSGBOX,
		"PANDUAN SERVER - NUSANTARA RP", buf, "Tutup", "");
	return 1;
}

CMD:me(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured]) return SendErr(playerid, "Anda sedang pingsan tidak sadarkan diri.");
	if (isnull(params)) return SendSyntax(playerid, "/me [tindakan]");
	new msg[160];
	format(msg, sizeof(msg), "* %s %s", gPlayerData[playerid][pName], params);
	SendNearbyPlayer(playerid, COLOR_RP_ME, msg, RANGE_ME);
	return 1;
}


CMD:do(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (isnull(params)) return SendSyntax(playerid, "/do [keadaan sekitar]");
	new msg[160];
	format(msg, sizeof(msg), "* %s (( %s ))", params, gPlayerData[playerid][pName]);
	SendNearbyPlayer(playerid, COLOR_RP_DO, msg, RANGE_ME);
	return 1;
}


CMD:b(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (isnull(params)) return SendSyntax(playerid, "/b [pesan ooc]");
	new msg[160];
	format(msg, sizeof(msg), "(( [OOC] %s: %s ))", gPlayerData[playerid][pName], params);
	SendNearbyPlayer(playerid, COLOR_OOC, msg, RANGE_OOC);
	return 1;
}


CMD:o(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (isnull(params)) return SendSyntax(playerid, "/o [pesan ooc global]");
	new msg[160];
	format(msg, sizeof(msg), "[OOC] %s: %s", gPlayerData[playerid][pName], params);
	SendClientMessageToAll(COLOR_OOC, msg);
	return 1;
}


CMD:w(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured]) return SendErr(playerid, "Anda sedang pingsan tidak sadarkan diri.");
	new targetid, text[128];
	if (sscanf(params, "us[128]", targetid, text)) return SendSyntax(playerid, "/w [playerid/nama] [pesan]");
	ProcessWhisper(playerid, targetid, text);
	return 1;
}


CMD:s(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured]) return SendErr(playerid, "Anda sedang pingsan tidak sadarkan diri.");
	if (isnull(params)) return SendSyntax(playerid, "/s [pesan teriakan]");
	ProcessShout(playerid, params);
	return 1;
}


CMD:l(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured]) return SendErr(playerid, "Anda sedang pingsan tidak sadarkan diri.");
	if (isnull(params)) return SendSyntax(playerid, "/l [pesan pelan]");
	ProcessLow(playerid, params);
	return 1;
}


CMD:pm(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	new targetid, text[128];
	if (sscanf(params, "us[128]", targetid, text)) return SendSyntax(playerid, "/pm [playerid/nama] [pesan]");
	ProcessPM(playerid, targetid, text);
	return 1;
}


CMD:report(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (isnull(params)) return SendSyntax(playerid, "/report [laporan]");
	ProcessReport(playerid, params);
	return 1;
}


CMD:ask(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (isnull(params)) return SendSyntax(playerid, "/ask [pertanyaan]");
	ProcessAsk(playerid, params);
	return 1;
}


CMD:help(playerid, params[])
{
	ShowHelpDialog(playerid);
	return 1;
}


CMD:lang(playerid, params[])
{
	return cmd_language(playerid, params);
}


CMD:language(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	ShowLanguageSelectionDialog(playerid);
	return 1;
}


