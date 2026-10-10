#if defined _INC_rp_admin
	#endinput
#endif
#define _INC_rp_admin

/*
==============================================================================
  NUSANTARA ROLEPLAY - admin.inc
  Perintah admin: /a, /goto, /gethere, /setskin, /setfaction, /givemoney,
  /freeze, /unfreeze, /kick, /ban
==============================================================================
*/

// ---------------------------------------------------------------------------
// Cek level admin
// ---------------------------------------------------------------------------
stock IsPlayerAdminEx(playerid, requiredlevel = 1)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	return (gPlayerData[playerid][pAdmin] >= requiredlevel);
}

// ---------------------------------------------------------------------------
// /a - Admin chat
// ---------------------------------------------------------------------------
stock ProcessAdminChat(playerid, const text[])
{
	if (!IsPlayerAdminEx(playerid, 1)) {
		SendErr(playerid, "Anda bukan staf admin.");
		return 0;
	}
	if (text[0] == '\0') {
		SendErr(playerid, "Gunakan: /a [pesan]");
		return 0;
	}

	new msg[180];
	format(msg, sizeof(msg), "[ADMIN CHAT] (Lvl %d) %s: %s",
		gPlayerData[playerid][pAdmin], gPlayerData[playerid][pName], text);

	for (new i = 0; i < MAX_PLAYERS; i++) {
		if (IsPlayerConnected(i) && gPlayerData[i][pAdmin] > 0) {
			SendClientMessage(i, COLOR_ADMIN_CHAT, msg);
		}
	}
	return 1;
}

// ---------------------------------------------------------------------------
// /goto - Teleport ke pemain
// ---------------------------------------------------------------------------
stock ProcessGoto(playerid, const targetname[])
{
	if (!IsPlayerAdminEx(playerid, 1)) {
		SendErr(playerid, "Perintah khusus admin.");
		return 0;
	}
	new targetid = FindPlayerByName(targetname);
	if (targetid == -1) {
		SendErr(playerid, "Target tidak ditemukan: %s", targetname);
		return 0;
	}

	new Float:tx, Float:ty, Float:tz;
	GetPlayerPos(targetid, tx, ty, tz);
	SetPlayerPos(playerid, tx + 1.0, ty + 1.0, tz);
	SetPlayerInterior(playerid, GetPlayerInterior(targetid));
	SetPlayerVirtualWorld(playerid, GetPlayerVirtualWorld(targetid));

	SendRP(playerid, COLOR_SUCCESS, "Anda berpindah ke %s.", gPlayerData[targetid][pName]);
	return 1;
}

// ---------------------------------------------------------------------------
// /gethere - Tarik pemain ke posisi admin
// ---------------------------------------------------------------------------
stock ProcessGethere(playerid, const targetname[])
{
	if (!IsPlayerAdminEx(playerid, 2)) {
		SendErr(playerid, "Diperlukan admin level 2+.");
		return 0;
	}
	new targetid = FindPlayerByName(targetname);
	if (targetid == -1) {
		SendErr(playerid, "Target tidak ditemukan: %s", targetname);
		return 0;
	}

	new Float:px, Float:py, Float:pz;
	GetPlayerPos(playerid, px, py, pz);
	SetPlayerPos(targetid, px + 1.0, py + 1.0, pz);
	SetPlayerInterior(targetid, GetPlayerInterior(playerid));
	SetPlayerVirtualWorld(targetid, GetPlayerVirtualWorld(playerid));

	SendRP(playerid, COLOR_SUCCESS, "%s ditarik ke posisi Anda.", gPlayerData[targetid][pName]);
	SendRP(targetid, COLOR_INFO, "Anda ditarik oleh Admin %s.", gPlayerData[playerid][pName]);
	return 1;
}

// ---------------------------------------------------------------------------
// /setskin - Ganti skin pemain
// ---------------------------------------------------------------------------
stock ProcessSetSkin(playerid, const targetname[], skinid)
{
	if (!IsPlayerAdminEx(playerid, 1)) {
		SendErr(playerid, "Perintah khusus admin.");
		return 0;
	}
	new targetid = FindPlayerByName(targetname);
	if (targetid == -1) {
		SendErr(playerid, "Target tidak ditemukan.");
		return 0;
	}
	if (skinid < 0 || skinid > 311) {
		SendErr(playerid, "ID Skin tidak valid (0-311).");
		return 0;
	}

	SetPlayerSkin(targetid, skinid);
	gPlayerData[targetid][pSkin] = skinid;
	DB_SavePlayerData(targetid);

	SendRP(playerid, COLOR_SUCCESS, "Skin %s diubah ke %d.", gPlayerData[targetid][pName], skinid);
	SendRP(targetid, COLOR_INFO, "Skin Anda diubah oleh Admin %s.", gPlayerData[playerid][pName]);
	return 1;
}

// ---------------------------------------------------------------------------
// /setfaction - Ubah faksi & pangkat pemain
// ---------------------------------------------------------------------------
stock ProcessSetFaction(playerid, const targetname[], factionid, rank)
{
	if (!IsPlayerAdminEx(playerid, 3)) {
		SendErr(playerid, "Diperlukan admin level 3+.");
		return 0;
	}
	new targetid = FindPlayerByName(targetname);
	if (targetid == -1) {
		SendErr(playerid, "Target tidak ditemukan.");
		return 0;
	}
	if (factionid < 0 || factionid > 4) {
		SendErr(playerid, "ID Faksi tidak valid (0=Sipil, 1=SAPD, 2=SAMD, 3=SAGOV, 4=Gang).");
		return 0;
	}
	if (rank < 0 || rank > 10) {
		SendErr(playerid, "Pangkat harus antara 0-10.");
		return 0;
	}

	gPlayerData[targetid][pFaction] = factionid;
	gPlayerData[targetid][pFactionRank] = rank;
	gPlayerData[targetid][pDuty] = false;
	DB_SavePlayerData(targetid);

	new fname[32];
	GetFactionName(factionid, fname, sizeof(fname));
	SendRP(playerid, COLOR_SUCCESS, "Faksi %s diubah menjadi %s (Pangkat %d).",
		gPlayerData[targetid][pName], fname, rank);
	SendRP(targetid, COLOR_INFO, "Anda dijadikan anggota %s (Pangkat %d) oleh Admin %s.",
		fname, rank, gPlayerData[playerid][pName]);
	return 1;
}

// ---------------------------------------------------------------------------
// /givemoney - Beri uang cash ke pemain
// ---------------------------------------------------------------------------
stock ProcessGiveMoney(playerid, const targetname[], amount)
{
	if (!IsPlayerAdminEx(playerid, 4)) {
		SendErr(playerid, "Diperlukan admin level 4+.");
		return 0;
	}
	new targetid = FindPlayerByName(targetname);
	if (targetid == -1) {
		SendErr(playerid, "Target tidak ditemukan.");
		return 0;
	}
	if (amount <= 0) {
		SendErr(playerid, "Jumlah uang harus positif.");
		return 0;
	}

	GivePlayerMoneyEx(targetid, amount, "admin spawned");
	DB_SavePlayerData(targetid);

	SendRP(playerid, COLOR_SUCCESS, "Memberikan $%d ke %s.", amount, gPlayerData[targetid][pName]);
	SendRP(targetid, COLOR_SUCCESS, "Admin %s memberi Anda $%d.", gPlayerData[playerid][pName], amount);
	return 1;
}

// ---------------------------------------------------------------------------
// /freeze & /unfreeze
// ---------------------------------------------------------------------------
stock ProcessFreeze(playerid, const targetname[])
{
	if (!IsPlayerAdminEx(playerid, 1)) {
		SendErr(playerid, "Perintah khusus admin.");
		return 0;
	}
	new targetid = FindPlayerByName(targetname);
	if (targetid == -1) {
		SendErr(playerid, "Target tidak ditemukan.");
		return 0;
	}

	gPlayerData[targetid][pFrozen] = true;
	TogglePlayerControllable(targetid, false);

	SendRP(playerid, COLOR_SUCCESS, "%s telah dibekukan.", gPlayerData[targetid][pName]);
	SendRP(targetid, COLOR_WARNING, "Anda dibekukan oleh Admin %s.", gPlayerData[playerid][pName]);
	return 1;
}

stock ProcessUnfreeze(playerid, const targetname[])
{
	if (!IsPlayerAdminEx(playerid, 1)) {
		SendErr(playerid, "Perintah khusus admin.");
		return 0;
	}
	new targetid = FindPlayerByName(targetname);
	if (targetid == -1) {
		SendErr(playerid, "Target tidak ditemukan.");
		return 0;
	}

	gPlayerData[targetid][pFrozen] = false;
	TogglePlayerControllable(targetid, true);

	SendRP(playerid, COLOR_SUCCESS, "%s telah dicairkan.", gPlayerData[targetid][pName]);
	SendRP(targetid, COLOR_SUCCESS, "Anda dicairkan oleh Admin %s.", gPlayerData[playerid][pName]);
	return 1;
}

// ---------------------------------------------------------------------------
// /kick
// ---------------------------------------------------------------------------
stock ProcessKick(playerid, const targetname[], const reason[])
{
	if (!IsPlayerAdminEx(playerid, 1)) {
		SendErr(playerid, "Perintah khusus admin.");
		return 0;
	}
	new targetid = FindPlayerByName(targetname);
	if (targetid == -1) {
		SendErr(playerid, "Target tidak ditemukan.");
		return 0;
	}

	new msg[180];
	format(msg, sizeof(msg), "[KICK] %s telah ditendang oleh Admin %s. Alasan: %s",
		gPlayerData[targetid][pName], gPlayerData[playerid][pName], reason[0] ? reason : "tanpa alasan");
	SendClientMessageToAll(COLOR_RED, msg);

	// Delay kick agar pesan sampai
	SetTimerEx("RP_KickPlayer", 100, false, "i", targetid);
	return 1;
}

forward RP_KickPlayer(playerid);
public RP_KickPlayer(playerid)
{
	if (IsPlayerConnected(playerid)) Kick(playerid);
	return 1;
}

// ---------------------------------------------------------------------------
// /ban
// ---------------------------------------------------------------------------
stock ProcessBan(playerid, const targetname[], const reason[])
{
	if (!IsPlayerAdminEx(playerid, 3)) {
		SendErr(playerid, "Diperlukan admin level 3+.");
		return 0;
	}
	new targetid = FindPlayerByName(targetname);
	if (targetid == -1) {
		SendErr(playerid, "Target tidak ditemukan.");
		return 0;
	}

	new ip[20];
	GetPlayerIp(targetid, ip, sizeof(ip));
	DB_AddBan(gPlayerData[targetid][pName], ip, reason, gPlayerData[playerid][pName]);

	new msg[180];
	format(msg, sizeof(msg), "[BANNED] %s telah diblokir permanen oleh Admin %s. Alasan: %s",
		gPlayerData[targetid][pName], gPlayerData[playerid][pName], reason[0] ? reason : "tanpa alasan");
	SendClientMessageToAll(COLOR_RED, msg);

	SetTimerEx("RP_KickPlayer", 100, false, "i", targetid);
	return 1;
}

