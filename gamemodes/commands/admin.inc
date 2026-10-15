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
	AC_SetPlayerPos(playerid, tx + 1.0, ty + 1.0, tz);
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
	AC_SetPlayerPos(targetid, px + 1.0, py + 1.0, pz);
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
	// [SECURITY] Batasi jumlah agar pMoney (int32) tidak overflow / wrap negatif.
	if (amount > 100000000) return SendErr(playerid, "Jumlah maksimum $100,000,000 per pemberian.");

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

	SetTimerEx("RP_KickPlayer", 100, false, "ii", targetid, gPlayerSession[targetid]);
	return 1;
}

forward RP_KickPlayer(playerid, session);
public RP_KickPlayer(playerid, session)
{
	// [SECURITY] Cek session token: cegah kick nyasar ke pemain baru di slot sama.
	if (IsPlayerConnected(playerid) && gPlayerSession[playerid] == session) Kick(playerid);
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

	SetTimerEx("RP_KickPlayer", 100, false, "ii", targetid, gPlayerSession[targetid]);
	return 1;
}

// ---------------------------------------------------------------------------
// ADMIN COMMANDS: DYNAMIC DOORS, ATMS, TRASH BINS
// ---------------------------------------------------------------------------
CMD:createdoor(playerid, params[])
{
	if (!IsPlayerAdminEx(playerid, 3)) return SendErr(playerid, "Diperlukan admin level 3+.");
	new name[48];
	if (sscanf(params, "s[48]", name)) return SendSyntax(playerid, "/createdoor [nama pintu]");
	new Float:x, Float:y, Float:z, Float:a;
	GetPlayerPos(playerid, x, y, z);
	GetPlayerFacingAngle(playerid, a);
	new int = GetPlayerInterior(playerid);
	new vw = GetPlayerVirtualWorld(playerid);
	new slot = Doors_Create(name, x, y, z, a, int, vw, x + 2.0, y, z, a + 180.0, int, vw + 1, false, 0);
	if (slot != -1) {
		SendSucc(playerid, "Pintu '#%d' (%s) berhasil dibuat. Gunakan /setdoorint %d untuk mengatur posisi dalam.", slot + 1, name, slot + 1);
	} else {
		SendErr(playerid, "Batas maksimal pintu tercapai.");
	}
	return 1;
}

CMD:setdoorint(playerid, params[])
{
	if (!IsPlayerAdminEx(playerid, 3)) return SendErr(playerid, "Diperlukan admin level 3+.");
	new doorid;
	if (sscanf(params, "d", doorid)) return SendSyntax(playerid, "/setdoorint [doorid]");
	doorid--;
	if (doorid < 0 || doorid >= MAX_DYNAMIC_DOORS || !gDoors[doorid][dUsed]) return SendErr(playerid, "Pintu tidak valid.");
	new Float:x, Float:y, Float:z, Float:a;
	GetPlayerPos(playerid, x, y, z);
	GetPlayerFacingAngle(playerid, a);
	gDoors[doorid][dIntX] = x;
	gDoors[doorid][dIntY] = y;
	gDoors[doorid][dIntZ] = z;
	gDoors[doorid][dIntA] = a;
	gDoors[doorid][dIntInterior] = GetPlayerInterior(playerid);
	gDoors[doorid][dIntVW] = GetPlayerVirtualWorld(playerid);
	Doors_CreateVisuals(doorid);
	if (gDBReady && gDoors[doorid][dDBID] > 0) {
		new query[350];
		mysql_format(gDB, query, sizeof(query),
			"UPDATE `doors` SET `int_x`=%.4f, `int_y`=%.4f, `int_z`=%.4f, `int_a`=%.4f, `int_interior`=%d, `int_vw`=%d WHERE `id`=%d",
			x, y, z, a, gDoors[doorid][dIntInterior], gDoors[doorid][dIntVW], gDoors[doorid][dDBID]);
		mysql_tquery(gDB, query, "", "");
	}
	SendSucc(playerid, "Posisi interior pintu #%d diperbarui ke lokasi Anda saat ini.", doorid + 1);
	return 1;
}

CMD:destroydoor(playerid, params[])
{
	if (!IsPlayerAdminEx(playerid, 3)) return SendErr(playerid, "Diperlukan admin level 3+.");
	new doorid;
	if (sscanf(params, "d", doorid)) return SendSyntax(playerid, "/destroydoor [doorid]");
	doorid--;
	if (doorid < 0 || doorid >= MAX_DYNAMIC_DOORS || !gDoors[doorid][dUsed]) return SendErr(playerid, "Pintu tidak valid.");
	Doors_Delete(doorid);
	SendSucc(playerid, "Pintu #%d berhasil dihapus.", doorid + 1);
	return 1;
}

CMD:lockdoor(playerid, params[])
{
	if (!IsPlayerAdminEx(playerid, 2)) return SendErr(playerid, "Diperlukan admin level 2+.");
	new doorid;
	if (sscanf(params, "d", doorid)) return SendSyntax(playerid, "/lockdoor [doorid]");
	doorid--;
	if (doorid < 0 || doorid >= MAX_DYNAMIC_DOORS || !gDoors[doorid][dUsed]) return SendErr(playerid, "Pintu tidak valid.");
	gDoors[doorid][dLocked] = !gDoors[doorid][dLocked];
	Doors_CreateVisuals(doorid);
	if (gDBReady && gDoors[doorid][dDBID] > 0) {
		new query[128];
		mysql_format(gDB, query, sizeof(query), "UPDATE `doors` SET `locked`=%d WHERE `id`=%d", gDoors[doorid][dLocked] ? 1 : 0, gDoors[doorid][dDBID]);
		mysql_tquery(gDB, query, "", "");
	}
	SendSucc(playerid, "Pintu #%d sekarang %s.", doorid + 1, gDoors[doorid][dLocked] ? "TERKUNCI" : "TERBUKA");
	return 1;
}

CMD:doors(playerid, params[])
{
	if (!IsPlayerAdminEx(playerid, 1)) return SendErr(playerid, "Perintah khusus admin.");
	SendInfo(playerid, "Total pintu dinamis terpasang: %d pintu.", gTotalDoors);
	return 1;
}

CMD:createatm(playerid, params[])
{
	if (!IsPlayerAdminEx(playerid, 3)) return SendErr(playerid, "Diperlukan admin level 3+.");
	new Float:x, Float:y, Float:z, Float:a;
	GetPlayerPos(playerid, x, y, z);
	GetPlayerFacingAngle(playerid, a);
	new Float:spawnX = x + (1.2 * floatsin(-a, degrees));
	new Float:spawnY = y + (1.2 * floatcos(-a, degrees));
	new slot = ATM_Create(spawnX, spawnY, z, 0.0, 0.0, a, GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid));
	if (slot != -1) {
		SendSucc(playerid, "Mesin ATM #%d berhasil dibuat di depan Anda.", slot + 1);
	} else {
		SendErr(playerid, "Batas maksimal mesin ATM tercapai.");
	}
	return 1;
}

CMD:destroyatm(playerid, params[])
{
	if (!IsPlayerAdminEx(playerid, 3)) return SendErr(playerid, "Diperlukan admin level 3+.");
	new atmid;
	if (sscanf(params, "d", atmid)) return SendSyntax(playerid, "/destroyatm [atmid]");
	atmid--;
	if (atmid < 0 || atmid >= MAX_DYNAMIC_ATMS || !gATMs[atmid][atmUsed]) return SendErr(playerid, "ATM tidak valid.");
	ATM_Delete(atmid);
	SendSucc(playerid, "Mesin ATM #%d berhasil dihapus.", atmid + 1);
	return 1;
}

CMD:atms(playerid, params[])
{
	if (!IsPlayerAdminEx(playerid, 1)) return SendErr(playerid, "Perintah khusus admin.");
	SendInfo(playerid, "Total mesin ATM terpasang: %d ATM.", gTotalATMs);
	return 1;
}

CMD:createtrash(playerid, params[])
{
	if (!IsPlayerAdminEx(playerid, 3)) return SendErr(playerid, "Diperlukan admin level 3+.");
	new Float:x, Float:y, Float:z, Float:a;
	GetPlayerPos(playerid, x, y, z);
	GetPlayerFacingAngle(playerid, a);
	new Float:spawnX = x + (1.2 * floatsin(-a, degrees));
	new Float:spawnY = y + (1.2 * floatcos(-a, degrees));
	new slot = Trash_Create(spawnX, spawnY, z, a, GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid));
	if (slot != -1) {
		SendSucc(playerid, "Tong sampah dinamis #%d berhasil dibuat di depan Anda.", slot + 1);
	} else {
		SendErr(playerid, "Batas maksimal tong sampah tercapai.");
	}
	return 1;
}

CMD:destroytrash(playerid, params[])
{
	if (!IsPlayerAdminEx(playerid, 3)) return SendErr(playerid, "Diperlukan admin level 3+.");
	new trashid;
	if (sscanf(params, "d", trashid)) return SendSyntax(playerid, "/destroytrash [trashid]");
	trashid--;
	if (trashid < 0 || trashid >= MAX_DYNAMIC_TRASH || !gTrashBins[trashid][trUsed]) return SendErr(playerid, "Tong sampah tidak valid.");
	Trash_Delete(trashid);
	SendSucc(playerid, "Tong sampah #%d berhasil dihapus.", trashid + 1);
	return 1;
}

CMD:trashes(playerid, params[])
{
	if (!IsPlayerAdminEx(playerid, 1)) return SendErr(playerid, "Perintah khusus admin.");
	SendInfo(playerid, "Total tong sampah terpasang: %d unit.", gTotalTrash);
	return 1;
}

CMD:createetle(playerid, params[])
{
	if (!IsPlayerAdminEx(playerid, 3)) return SendErr(playerid, "Diperlukan admin level 3+.");
	new Float:limit, fee, name[48];
	if (sscanf(params, "fds[48]", limit, fee, name)) {
		return SendSyntax(playerid, "/createetle [Batas KM/J] [Denda $] [Nama Lokasi]");
	}
	new Float:x, Float:y, Float:z;
	GetPlayerPos(playerid, x, y, z);
	new slot = TrafficCamera_Create(x, y, z, limit, fee, name);
	if (slot != -1) {
		new succMsg[160];
		format(succMsg, sizeof(succMsg), "SUKSES: Kamera ETLE #%d ('%s') berhasil dipasang di titik Anda (Batas: %.0f KM/J, Denda: $%d).",
			slot + 1, name, limit, fee);
		SendClientMessage(playerid, COLOR_SUCCESS, succMsg);
	} else {
		SendErr(playerid, "Batas maksimal kamera ETLE tercapai (%d unit).", MAX_SPEED_CAMERAS);
	}
	return 1;
}

CMD:destroyetle(playerid, params[])
{
	if (!IsPlayerAdminEx(playerid, 3)) return SendErr(playerid, "Diperlukan admin level 3+.");
	new camid;
	if (sscanf(params, "d", camid)) return SendSyntax(playerid, "/destroyetle [camera_id]");
	camid--;
	if (camid < 0 || camid >= MAX_SPEED_CAMERAS || !gSpeedCameras[camid][camUsed]) return SendErr(playerid, "Kamera ETLE tidak valid.");
	TrafficCamera_Destroy(camid);
	new delMsg[80];
	format(delMsg, sizeof(delMsg), "SUKSES: Kamera ETLE #%d berhasil dihapus dari dunia dan database.", camid + 1);
	SendClientMessage(playerid, COLOR_SUCCESS, delMsg);
	return 1;
}

CMD:etles(playerid, params[])
{
	if (!IsPlayerAdminEx(playerid, 1)) return SendErr(playerid, "Perintah khusus admin.");
	SendInfo(playerid, "Total kamera tilang ETLE terpasang: %d unit.", gTotalSpeedCameras);
	return 1;
}
