#if defined _INC_rp_admin
	#endinput
#endif
#define _INC_rp_admin

// admin command handlers

stock IsPlayerAdminEx(playerid, requiredlevel = 1)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	return (gPlayerData[playerid][pAdmin] >= requiredlevel);
}

// admin chat
CMD:a(playerid, params[])
{
	if (!IsPlayerAdminEx(playerid, 1)) return SendErr(playerid, "Anda bukan staf admin.");
	if (isnull(params)) return SendSyntax(playerid, "/a [pesan]");

	new msg[180];
	format(msg, sizeof(msg), "[ADMIN CHAT] (Lvl %d) %s: %s",
		gPlayerData[playerid][pAdmin], gPlayerData[playerid][pName], params);

	for (new i = 0; i < MAX_PLAYERS; i++) {
		if (IsPlayerConnected(i) && gPlayerData[i][pAdmin] > 0) {
			SendClientMessage(i, COLOR_ADMIN_CHAT, msg);
		}
	}
	return 1;
}

// teleport commands
CMD:goto(playerid, params[])
{
	if (!IsPlayerAdminEx(playerid, 1)) return SendErr(playerid, "Perintah khusus admin.");
	new targetid;
	if (sscanf(params, "u", targetid)) return SendSyntax(playerid, "/goto [playerid/nama]");
	if (!IsPlayerConnected(targetid)) return SendErr(playerid, "Target tidak ditemukan.");

	new Float:tx, Float:ty, Float:tz;
	GetPlayerPos(targetid, tx, ty, tz);
	SetPlayerPos(playerid, tx + 1.0, ty + 1.0, tz);
	SetPlayerInterior(playerid, GetPlayerInterior(targetid));
	SetPlayerVirtualWorld(playerid, GetPlayerVirtualWorld(targetid));

	SendSucc(playerid, "Berpindah ke lokasi %s.", gPlayerData[targetid][pName]);
	return 1;
}

CMD:gethere(playerid, params[])
{
	if (!IsPlayerAdminEx(playerid, 2)) return SendErr(playerid, "Diperlukan admin level 2+.");
	new targetid;
	if (sscanf(params, "u", targetid)) return SendSyntax(playerid, "/gethere [playerid/nama]");
	if (!IsPlayerConnected(targetid)) return SendErr(playerid, "Target tidak ditemukan.");

	new Float:px, Float:py, Float:pz;
	GetPlayerPos(playerid, px, py, pz);
	SetPlayerPos(targetid, px + 1.0, py + 1.0, pz);
	SetPlayerInterior(targetid, GetPlayerInterior(playerid));
	SetPlayerVirtualWorld(targetid, GetPlayerVirtualWorld(playerid));

	SendSucc(playerid, "%s ditarik ke posisi Anda.", gPlayerData[targetid][pName]);
	SendInfo(targetid, "Anda dipindahkan oleh Admin %s.", gPlayerData[playerid][pName]);
	return 1;
}

// player configuration
CMD:setskin(playerid, params[])
{
	if (!IsPlayerAdminEx(playerid, 1)) return SendErr(playerid, "Perintah khusus admin.");
	new targetid, skinid;
	if (sscanf(params, "ud", targetid, skinid)) return SendSyntax(playerid, "/setskin [targetid] [skin id]");
	if (!IsPlayerConnected(targetid)) return SendErr(playerid, "Target tidak ditemukan.");
	if (skinid < 0 || skinid > 311) return SendErr(playerid, "ID Skin tidak valid (0-311).");

	SetPlayerSkin(targetid, skinid);
	gPlayerData[targetid][pSkin] = skinid;
	DB_SavePlayerData(targetid);

	SendSucc(playerid, "Skin %s diubah ke ID %d.", gPlayerData[targetid][pName], skinid);
	SendInfo(targetid, "Skin Anda diubah oleh Admin %s.", gPlayerData[playerid][pName]);
	return 1;
}

CMD:setfaction(playerid, params[])
{
	if (!IsPlayerAdminEx(playerid, 3)) return SendErr(playerid, "Diperlukan admin level 3+.");
	new targetid, factionid, rank;
	if (sscanf(params, "udd", targetid, factionid, rank)) return SendSyntax(playerid, "/setfaction [targetid] [faksi (0-4)] [rank (1-10)]");
	if (!IsPlayerConnected(targetid)) return SendErr(playerid, "Target tidak ditemukan.");
	if (factionid < 0 || factionid > 4) return SendErr(playerid, "Faksi: 0=Sipil, 1=SAPD, 2=SAMD, 3=SAGOV, 4=Gang.");

	gPlayerData[targetid][pFaction] = factionid;
	gPlayerData[targetid][pFactionRank] = rank;
	gPlayerData[targetid][pDuty] = false;
	DB_SavePlayerData(targetid);

	new fname[32];
	GetFactionName(factionid, fname, sizeof(fname));
	SendSucc(playerid, "Faksi %s disetel ke %s (Pangkat %d).", gPlayerData[targetid][pName], fname, rank);
	SendInfo(targetid, "Anda diangkat ke faksi %s oleh Admin %s.", fname, gPlayerData[playerid][pName]);
	return 1;
}

CMD:givemoney(playerid, params[])
{
	if (!IsPlayerAdminEx(playerid, 4)) return SendErr(playerid, "Diperlukan admin level 4+.");
	new targetid, amount;
	if (sscanf(params, "ud", targetid, amount)) return SendSyntax(playerid, "/givemoney [targetid] [jumlah]");
	if (!IsPlayerConnected(targetid)) return SendErr(playerid, "Target tidak ditemukan.");
	if (amount <= 0) return SendErr(playerid, "Jumlah uang harus bernilai positif.");

	GivePlayerMoneyEx(targetid, amount, "admin reward");
	DB_SavePlayerData(targetid);

	SendSucc(playerid, "Memberikan $%d kepada %s.", amount, gPlayerData[targetid][pName]);
	SendSucc(targetid, "Admin %s memberikan Anda $%d tunai.", gPlayerData[playerid][pName], amount);
	return 1;
}

// moderation
CMD:freeze(playerid, params[])
{
	if (!IsPlayerAdminEx(playerid, 1)) return SendErr(playerid, "Perintah khusus admin.");
	new targetid;
	if (sscanf(params, "u", targetid)) return SendSyntax(playerid, "/freeze [targetid/nama]");
	if (!IsPlayerConnected(targetid)) return SendErr(playerid, "Target tidak ditemukan.");

	gPlayerData[targetid][pFrozen] = true;
	TogglePlayerControllable(targetid, false);

	SendSucc(playerid, "%s berhasil dibekukan.", gPlayerData[targetid][pName]);
	SendRP(targetid, COLOR_WARNING, "Karakter Anda dibekukan oleh Admin %s.", gPlayerData[playerid][pName]);
	return 1;
}

CMD:unfreeze(playerid, params[])
{
	if (!IsPlayerAdminEx(playerid, 1)) return SendErr(playerid, "Perintah khusus admin.");
	new targetid;
	if (sscanf(params, "u", targetid)) return SendSyntax(playerid, "/unfreeze [targetid/nama]");
	if (!IsPlayerConnected(targetid)) return SendErr(playerid, "Target tidak ditemukan.");

	gPlayerData[targetid][pFrozen] = false;
	TogglePlayerControllable(targetid, true);

	SendSucc(playerid, "%s berhasil dicairkan.", gPlayerData[targetid][pName]);
	SendSucc(targetid, "Karakter Anda dicairkan oleh Admin %s.", gPlayerData[playerid][pName]);
	return 1;
}

CMD:kick(playerid, params[])
{
	if (!IsPlayerAdminEx(playerid, 1)) return SendErr(playerid, "Perintah khusus admin.");
	new targetid, reason[64];
	if (sscanf(params, "us[64]", targetid, reason)) return SendSyntax(playerid, "/kick [targetid] [alasan]");
	if (!IsPlayerConnected(targetid)) return SendErr(playerid, "Target tidak ditemukan.");

	new msg[180];
	format(msg, sizeof(msg), "[KICK] %s ditendang oleh Admin %s. Alasan: %s",
		gPlayerData[targetid][pName], gPlayerData[playerid][pName], reason);
	SendClientMessageToAll(COLOR_RED, msg);

	SetTimerEx("RP_KickPlayer", 100, false, "i", targetid);
	return 1;
}

forward RP_KickPlayer(playerid);
public RP_KickPlayer(playerid)
{
	if (IsPlayerConnected(playerid)) Kick(playerid);
	return 1;
}

CMD:ban(playerid, params[])
{
	if (!IsPlayerAdminEx(playerid, 3)) return SendErr(playerid, "Diperlukan admin level 3+.");
	new targetid, reason[64];
	if (sscanf(params, "us[64]", targetid, reason)) return SendSyntax(playerid, "/ban [targetid] [alasan]");
	if (!IsPlayerConnected(targetid)) return SendErr(playerid, "Target tidak ditemukan.");

	new ip[20];
	GetPlayerIp(targetid, ip, sizeof(ip));
	DB_AddBan(gPlayerData[targetid][pName], ip, reason, gPlayerData[playerid][pName]);

	new msg[180];
	format(msg, sizeof(msg), "[BANNED] %s diblokir permanen oleh Admin %s. Alasan: %s",
		gPlayerData[targetid][pName], gPlayerData[playerid][pName], reason);
	SendClientMessageToAll(COLOR_RED, msg);

	SetTimerEx("RP_KickPlayer", 100, false, "i", targetid);
	return 1;
}
