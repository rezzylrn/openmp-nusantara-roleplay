#if defined _INC_admin_panel
	#endinput
#endif
#define _INC_admin_panel

/*
==============================================================================
 admin_panel.inc
 NUSANTARA ROLEPLAY - Dialog-Based Admin Control Panel
 Antarmuka panel admin lengkap untuk staf server.
==============================================================================
*/

static gAdminSelectedTarget[MAX_PLAYERS];

CMD:admin(playerid, params[])
{
	if (gPlayerData[playerid][pAdmin] < 1) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda tidak memiliki izin untuk mengakses perintah ini.");
		return 1;
	}
	ShowAdminPanelDialog(playerid);
	return 1;
}

CMD:adminpanel(playerid, params[])
{
	return cmd_admin(playerid, params);
}

CMD:admindialog(playerid, params[])
{
	return cmd_admin(playerid, params);
}

stock ShowAdminPanelDialog(playerid)
{
	if (gPlayerData[playerid][pAdmin] < 1) return 0;

	new body[512];
	format(body, sizeof(body),
		"{FFFFFF}1. Teleportasi Lokasi Penting Kota\n\
{FFFFFF}2. Teleportasi ke Pemain (Goto Player)\n\
{FFFFFF}3. Panggil Pemain ke Sini (Get Player)\n\
{FFFFFF}4. Pulihkan Nyawa & Armor (Heal Player)\n\
{FFFFFF}5. Bekukan / Lepas Freeze Pemain\n\
{FFFFFF}6. Spawn Kendaraan Sementara (/veh)\n\
{FFFFFF}7. Atur Cuaca & Jam Dunia (/weather /time)\n\
{FFFFFF}8. Tendang Pemain dari Server (Kick)\n\
{FFFFFF}9. Blokir Akun Pemain (Ban Permanent)");

	Dialog_Show(playerid, DialogAdminMain, DIALOG_STYLE_LIST,
		"{FFFFFF}PANEL ADMINISTRATOR SERVER", body, "Pilih", "Tutup");
	return 1;
}

Dialog:DialogAdminMain(playerid, response, listitem, inputtext[])
{
	if (!response || gPlayerData[playerid][pAdmin] < 1) return 1;

	switch (listitem) {
		case 0: { // 1. Teleportasi Lokasi
			new locs[512];
			format(locs, sizeof(locs),
				"{FFFFFF}1. Spawn Point (Unity Station)\n\
{FFFFFF}2. Kantor Polisi LSPD\n\
{FFFFFF}3. Rumah Sakit ASGH\n\
{FFFFFF}4. Bank Nusantara (Pershing Square)\n\
{FFFFFF}5. Balai Kota DMV SAGOV\n\
{FFFFFF}6. Santa Maria Pier\n\
{FFFFFF}7. Bandara Internasional LS\n\
{FFFFFF}8. Dermaga Ocean Docks\n\
{FFFFFF}9. Puncak Gunung Chiliad");
			Dialog_Show(playerid, DialogAdminTeleport, DIALOG_STYLE_LIST,
				"{FFFFFF}TELEPORTASI LOKASI", locs, "Teleport", "Kembali");
		}
		case 1: { // 2. Goto Player
			Dialog_Show(playerid, DialogAdminGoto, DIALOG_STYLE_INPUT,
				"{FFFFFF}TELEPORT KE PEMAIN",
				"{FFFFFF}Masukkan ID pemain yang ingin Anda tuju:", "Teleport", "Batal");
		}
		case 2: { // 3. Get Player
			Dialog_Show(playerid, DialogAdminGet, DIALOG_STYLE_INPUT,
				"{FFFFFF}PANGGIL PEMAIN",
				"{FFFFFF}Masukkan ID pemain yang ingin dipanggil ke lokasi Anda:", "Panggil", "Batal");
		}
		case 3: { // 4. Heal Player
			Dialog_Show(playerid, DialogAdminHeal, DIALOG_STYLE_INPUT,
				"{FFFFFF}PULIHKAN PEMAIN (HEAL)",
				"{FFFFFF}Masukkan ID pemain (atau ID Anda sendiri) untuk memulihkan darah & armor:", "Pulihkan", "Batal");
		}
		case 4: { // 5. Freeze / Unfreeze
			Dialog_Show(playerid, DialogAdminFreeze, DIALOG_STYLE_INPUT,
				"{FFFFFF}FREEZE / UNFREEZE PEMAIN",
				"{FFFFFF}Masukkan ID pemain yang ingin dibekukan atau dilepas freeze:", "Ubah", "Batal");
		}
		case 5: { // 6. Spawn Veh
			Dialog_Show(playerid, DialogAdminVeh, DIALOG_STYLE_INPUT,
				"{FFFFFF}SPAWN KENDARAAN",
				"{FFFFFF}Masukkan ID Model Kendaraan (contoh: 411 = Infernus, 560 = Sultan, 522 = NRG-500):", "Spawn", "Batal");
		}
		case 6: { // 7. Cuaca & Jam
			Dialog_Show(playerid, DialogAdminWeather, DIALOG_STYLE_LIST,
				"{FFFFFF}PENGATURAN CUACA DUNIA",
				"{FFFFFF}1. Cerah Bersih (Sunny)\n\
{FFFFFF}2. Panas Terik (Hot)\n\
{FFFFFF}3. Hujan Gerimis (Rain)\n\
{FFFFFF}4. Kabut Tebal (Foggy)\n\
{FFFFFF}5. Badai Petir (Thunderstorm)", "Terapkan", "Batal");
		}
		case 7: { // 8. Kick
			Dialog_Show(playerid, DialogAdminKick, DIALOG_STYLE_INPUT,
				"{FFFFFF}KICK PEMAIN",
				"{FFFFFF}Masukkan ID pemain yang ingin dikeluarkan dari server:", "Kick", "Batal");
		}
		case 8: { // 9. Ban
			Dialog_Show(playerid, DialogAdminBan, DIALOG_STYLE_INPUT,
				"{FFFFFF}BAN PERMANEN PEMAIN",
				"{FFFFFF}Masukkan ID pemain yang ingin diblokir permanen:", "Banned", "Batal");
		}
	}
	return 1;
}

Dialog:DialogAdminTeleport(playerid, response, listitem, inputtext[])
{
	if (!response) {
		ShowAdminPanelDialog(playerid);
		return 1;
	}

	switch (listitem) {
		case 0: AC_SetPlayerPos(playerid, 1745.0, -1865.0, 13.5);
		case 1: AC_SetPlayerPos(playerid, 1555.0, -1675.0, 16.0);
		case 2: AC_SetPlayerPos(playerid, 1172.0, -1323.0, 15.0);
		case 3: AC_SetPlayerPos(playerid, 1481.0, -1748.0, 15.5);
		case 4: AC_SetPlayerPos(playerid, 1480.0, -1785.0, 15.5);
		case 5: AC_SetPlayerPos(playerid, 380.0, -2020.0, 7.8);
		case 6: AC_SetPlayerPos(playerid, 1685.0, -2238.0, 13.5);
		case 7: AC_SetPlayerPos(playerid, 2760.0, -2455.0, 13.5);
		case 8: AC_SetPlayerPos(playerid, -2310.0, -1640.0, 483.5);
	}
	SetPlayerInterior(playerid, 0);
	SetPlayerVirtualWorld(playerid, 0);
	SendClientMessage(playerid, COLOR_SUCCESS, "[SUKSES] Anda telah diteleportasi ke lokasi tujuan.");
	return 1;
}

Dialog:DialogAdminGoto(playerid, response, listitem, inputtext[])
{
	if (!response) return 1;
	new targetid = strval(inputtext);
	if (!IsPlayerConnected(targetid) || targetid == playerid) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] ID pemain tidak valid.");
		return 1;
	}

	new Float:tx, Float:ty, Float:tz;
	GetPlayerPos(targetid, tx, ty, tz);
	AC_SetPlayerPos(playerid, tx + 1.0, ty + 1.0, tz);
	SetPlayerInterior(playerid, GetPlayerInterior(targetid));
	SetPlayerVirtualWorld(playerid, GetPlayerVirtualWorld(targetid));

	new msg[144];
	format(msg, sizeof(msg), "[ADMIN] Anda telah teleportasi ke %s (ID %d).", gPlayerData[targetid][pName], targetid);
	SendClientMessage(playerid, COLOR_INFO, msg);
	return 1;
}

Dialog:DialogAdminGet(playerid, response, listitem, inputtext[])
{
	if (!response) return 1;
	new targetid = strval(inputtext);
	if (!IsPlayerConnected(targetid) || targetid == playerid) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] ID pemain tidak valid.");
		return 1;
	}

	new Float:px, Float:py, Float:pz;
	GetPlayerPos(playerid, px, py, pz);
	AC_SetPlayerPos(targetid, px + 1.0, py + 1.0, pz);
	SetPlayerInterior(targetid, GetPlayerInterior(playerid));
	SetPlayerVirtualWorld(targetid, GetPlayerVirtualWorld(playerid));

	new msg[144];
	format(msg, sizeof(msg), "[ADMIN] Anda telah memanggil %s (ID %d) ke lokasi Anda.", gPlayerData[targetid][pName], targetid);
	SendClientMessage(playerid, COLOR_INFO, msg);
	return 1;
}

Dialog:DialogAdminHeal(playerid, response, listitem, inputtext[])
{
	if (!response) return 1;
	new targetid = strval(inputtext);
	if (!IsPlayerConnected(targetid)) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] ID pemain tidak valid.");
		return 1;
	}

	SetPlayerHealth(targetid, 100.0);
	SetPlayerArmour(targetid, 100.0);
	gPlayerData[targetid][pHunger] = 100.0;
	gPlayerData[targetid][pThirst] = 100.0;
	gPlayerData[targetid][pInjured] = false;

	SendClientMessage(targetid, COLOR_SUCCESS, "[ADMIN] Darah, armor, dan stamina Anda telah dipulihkan oleh admin.");
	SendClientMessage(playerid, COLOR_SUCCESS, "[ADMIN] Pemain berhasil dipulihkan secara penuh.");
	return 1;
}

Dialog:DialogAdminFreeze(playerid, response, listitem, inputtext[])
{
	if (!response) return 1;
	new targetid = strval(inputtext);
	if (!IsPlayerConnected(targetid)) return 1;

	TogglePlayerControllable(targetid, true);
	SendClientMessage(targetid, COLOR_INFO, "[ADMIN] Status kontrol pergerakan karakter Anda telah diubah oleh staf.");
	SendClientMessage(playerid, COLOR_SUCCESS, "[ADMIN] Freeze pemain berhasil di-toggle.");
	return 1;
}

Dialog:DialogAdminVeh(playerid, response, listitem, inputtext[])
{
	if (!response) return 1;
	new model = strval(inputtext);
	if (model < 400 || model > 611) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Model ID kendaraan harus antara 400 hingga 611.");
		return 1;
	}

	new Float:x, Float:y, Float:z, Float:a;
	GetPlayerPos(playerid, x, y, z);
	GetPlayerFacingAngle(playerid, a);

	new veh = CreateVehicle(model, x + 2.0, y + 2.0, z + 0.5, a, 1, 1, 60);
	SetVehicleVirtualWorld(veh, GetPlayerVirtualWorld(playerid));
	LinkVehicleToInterior(veh, GetPlayerInterior(playerid));

	new msg[144];
	format(msg, sizeof(msg), "[ADMIN] Kendaraan model %d berhasil di-spawn.", model);
	SendClientMessage(playerid, COLOR_SUCCESS, msg);
	return 1;
}

Dialog:DialogAdminWeather(playerid, response, listitem, inputtext[])
{
	if (!response) return 1;
	new weatherId = 1;
	switch (listitem) {
		case 0: weatherId = 1;  // Sunny
		case 1: weatherId = 11; // Hot
		case 2: weatherId = 8;  // Rain
		case 3: weatherId = 9;  // Foggy
		case 4: weatherId = 19; // Thunderstorm
	}
	SetWeather(weatherId);
	SendClientMessage(playerid, COLOR_SUCCESS, "[ADMIN] Cuaca server berhasil diperbarui.");
	return 1;
}

Dialog:DialogAdminKick(playerid, response, listitem, inputtext[])
{
	if (!response) return 1;
	new targetid = strval(inputtext);
	if (!IsPlayerConnected(targetid) || targetid == playerid) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] ID pemain tidak valid.");
		return 1;
	}

	gAdminSelectedTarget[playerid] = targetid;
	Dialog_Show(playerid, DialogAdminKickReason, DIALOG_STYLE_INPUT,
		"{FFFFFF}ALASAN KICK",
		"{FFFFFF}Masukkan alasan mengeluarkan pemain ini dari server:", "Keluarkan", "Batal");
	return 1;
}

Dialog:DialogAdminKickReason(playerid, response, listitem, inputtext[])
{
	if (!response || gPlayerData[playerid][pAdmin] < 1) return 1;
	new targetid = gAdminSelectedTarget[playerid];
	if (!IsPlayerConnected(targetid)) return 1;

	new reason[64];
	format(reason, sizeof(reason), "%s", strlen(inputtext) > 0 ? inputtext : "Pelanggaran aturan roleplay");

	new broadcast[144];
	format(broadcast, sizeof(broadcast), "[ADM-KICK] %s telah dikeluarkan oleh Admin %s. Alasan: %s",
		gPlayerData[targetid][pName], gPlayerData[playerid][pName], reason);
	SendClientMessageToAll(COLOR_RED, broadcast);

	SetTimerEx("RP_DelayedKick", 350, false, "i", targetid);
	return 1;
}

Dialog:DialogAdminBan(playerid, response, listitem, inputtext[])
{
	if (!response || gPlayerData[playerid][pAdmin] < 2) return 1;
	new targetid = strval(inputtext);
	if (!IsPlayerConnected(targetid) || targetid == playerid) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] ID pemain tidak valid.");
		return 1;
	}

	gAdminSelectedTarget[playerid] = targetid;
	Dialog_Show(playerid, DialogAdminBanReason, DIALOG_STYLE_INPUT,
		"{FFFFFF}ALASAN BANNED PERMANEN",
		"{FFFFFF}Masukkan alasan memblokir akun pemain ini:", "Ban", "Batal");
	return 1;
}

Dialog:DialogAdminBanReason(playerid, response, listitem, inputtext[])
{
	if (!response || gPlayerData[playerid][pAdmin] < 2) return 1;
	new targetid = gAdminSelectedTarget[playerid];
	if (!IsPlayerConnected(targetid)) return 1;

	new reason[64];
	format(reason, sizeof(reason), "%s", strlen(inputtext) > 0 ? inputtext : "Penggunaan program ilegal / perusuh");

	new broadcast[144];
	format(broadcast, sizeof(broadcast), "[ADM-BAN] %s telah diblokir permanen oleh Admin %s. Alasan: %s",
		gPlayerData[targetid][pName], gPlayerData[playerid][pName], reason);
	SendClientMessageToAll(COLOR_RED, broadcast);

	if (gDBReady) {
		new tip[24];
		GetPlayerIp(targetid, tip, sizeof(tip));
		DB_AddBan(gPlayerData[targetid][pName], tip, reason, gPlayerData[playerid][pName]);
	}

	SetTimerEx("RP_DelayedKick", 350, false, "i", targetid);
	return 1;
}
