#if defined _INC_rp_boombox
	#endinput
#endif
#define _INC_rp_boombox

/*
==============================================================================
 boombox.inc
 NUSANTARA ROLEPLAY - Portable Live Audio Stream Boombox System
 Sistem Speaker Musik Portabel:
 - Meletakkan speaker boombox di tanah dan memutar URL audio stream bersama.
 - Pemain dalam radius 30 meter dapat mendengarkan siaran musik secara real-time.
 - Mengambil kembali speaker boombox ke inventaris kapan saja.
==============================================================================
*/

#define MAX_BOOMBOXES           20
#define BOOMBOX_AUDIO_RADIUS    35.0

enum E_BOOMBOX_DATA {
	bool:bbUsed,
	bbOwnerID,
	bbObject,
	Float:bbX,
	Float:bbY,
	Float:bbZ,
	bbUrl[128],
	bbStation[48],
	Text3D:bbLabel
};

new gBoomboxes[MAX_BOOMBOXES][E_BOOMBOX_DATA];

stock Boombox_Init()
{
	for (new i = 0; i < MAX_BOOMBOXES; i++) {
		gBoomboxes[i][bbUsed] = false;
		gBoomboxes[i][bbObject] = INVALID_OBJECT_ID;
		gBoomboxes[i][bbLabel] = Text3D:INVALID_3DTEXT_ID;
	}
	return 1;
}

CMD:boombox(playerid, params[])
{
	return ProcessBoombox(playerid);
}

stock ProcessBoombox(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda belum login.");
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda tidak dapat menggunakan Boombox saat pingsan atau terborgol.");
		return 1;
	}
	if (IsPlayerInAnyVehicle(playerid)) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda harus turun dari kendaraan untuk meletakkan Boombox.");
		return 1;
	}

	// Cek jika pemain berada di dekat boombox miliknya untuk mengambilnya
	for (new i = 0; i < MAX_BOOMBOXES; i++) {
		if (gBoomboxes[i][bbUsed] && gBoomboxes[i][bbOwnerID] == gPlayerData[playerid][pID]) {
			if (IsPlayerNearPoint(playerid, gBoomboxes[i][bbX], gBoomboxes[i][bbY], gBoomboxes[i][bbZ], 3.5)) {
				// Ambil boombox
				return ProcessPickupBoombox(playerid);
			}
		}
	}

	if (GetPlayerItemCount(playerid, ITEM_BOOMBOX) <= 0) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda tidak memiliki speaker Boombox (Beli di Toko Elektronik/Pasar Gelap).");
		return 1;
	}

	Dialog_Show(playerid, DialogBoomboxStation, DIALOG_STYLE_LIST,
		"{3498DB}PILIH SALURAN MUSIK BOOMBOX",
		"{FFFFFF}1. Nusantara Lofi Chill Beats\n\
{FFFFFF}2. Los Santos Hiphop & R&B Hits\n\
{FFFFFF}3. Classic Rock & Oldies Anthems\n\
{FFFFFF}4. Pop Dance & Electronic Beats\n\
{FFFFFF}5. Masukkan Custom Audio Stream URL",
		"Nyalakan", "Batal");
	return 1;
}

Dialog:DialogBoomboxStation(playerid, response, listitem, inputtext[])
{
	if (!response) return 1;

	new chosenUrl[128], chosenStation[48];
	switch (listitem) {
		case 0: {
			format(chosenUrl, sizeof(chosenUrl), "https://stream.zeno.fm/f3wvbbqmdg8uv");
			format(chosenStation, sizeof(chosenStation), "Lofi Chill Beats");
		}
		case 1: {
			format(chosenUrl, sizeof(chosenUrl), "https://stream.zeno.fm/0r0xa792kwzuv");
			format(chosenStation, sizeof(chosenStation), "Los Santos Hiphop");
		}
		case 2: {
			format(chosenUrl, sizeof(chosenUrl), "https://stream.zeno.fm/h2v84803vzwtv");
			format(chosenStation, sizeof(chosenStation), "Classic Rock Hits");
		}
		case 3: {
			format(chosenUrl, sizeof(chosenUrl), "https://stream.zeno.fm/4v704w80szwtv");
			format(chosenStation, sizeof(chosenStation), "Pop Dance Energy");
		}
		case 4: {
			Dialog_Show(playerid, DialogBoomboxCustomURL, DIALOG_STYLE_INPUT,
				"{3498DB}CUSTOM AUDIO STREAM URL",
				"{FFFFFF}Masukkan URL siaran audio streaming langsung (format http/https mp3):",
				"Nyalakan", "Batal");
			return 1;
		}
	}

	DeployPlayerBoombox(playerid, chosenUrl, chosenStation);
	return 1;
}

Dialog:DialogBoomboxCustomURL(playerid, response, listitem, inputtext[])
{
	if (!response || strlen(inputtext) == 0) return 1;
	DeployPlayerBoombox(playerid, inputtext, "Custom Stream URL");
	return 1;
}

stock DeployPlayerBoombox(playerid, const url[], const stationName[])
{
	new slot = -1;
	for (new i = 0; i < MAX_BOOMBOXES; i++) {
		if (!gBoomboxes[i][bbUsed]) {
			slot = i;
			break;
		}
	}

	if (slot == -1) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Batas maksimal boombox aktif di kota telah tercapai.");
		return 0;
	}

	// [FIX] Pastikan item benar-benar terambil (atomic) sebelum deploy.
	if (!TakePlayerItem(playerid, ITEM_BOOMBOX, 1)) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda tidak memiliki Boombox untuk diletakkan.");
		return 0;
	}

	new Float:x, Float:y, Float:z, Float:a;
	if (IsPlayerConnected(playerid)) {
		GetPlayerPos(playerid, x, y, z);
		GetPlayerFacingAngle(playerid, a);
	} else {
		x = gPlayerData[playerid][pLastX];
		y = gPlayerData[playerid][pLastY];
		z = gPlayerData[playerid][pLastZ];
		a = gPlayerData[playerid][pLastA];
	}

	gBoomboxes[slot][bbUsed] = true;
	gBoomboxes[slot][bbOwnerID] = gPlayerData[playerid][pID];
	gBoomboxes[slot][bbX] = x;
	gBoomboxes[slot][bbY] = y;
	gBoomboxes[slot][bbZ] = z - 0.7;
	format(gBoomboxes[slot][bbUrl], 128, "%s", url);
	format(gBoomboxes[slot][bbStation], 48, "%s", stationName);

	gBoomboxes[slot][bbObject] = CreateObject(2226, x, y, z - 0.7, 0.0, 0.0, a, 100.0);

	new label[144];
	format(label, sizeof(label), "{3498DB}[ BOOMBOX ]\n{FFFFFF}Stasiun: {FFFF00}%s\n{AFAFAF}Gunakan /boombox untuk mengambil",
		stationName);
	gBoomboxes[slot][bbLabel] = Create3DTextLabel(label, 0xFFFFFFFF, x, y, z, 15.0, 0, false);

	// Putar audio untuk semua pemain terdekat
	for (new p = 0; p < MAX_PLAYERS; p++) {
		if (IsPlayerConnected(p) && IsPlayerNearPoint(p, x, y, z, BOOMBOX_AUDIO_RADIUS)) {
			PlayAudioStreamForPlayer(p, url, x, y, z, BOOMBOX_AUDIO_RADIUS, true);
		}
	}

	SendClientMessage(playerid, COLOR_SUCCESS, "[SUKSES] Boombox berhasil diletakkan di tanah dan memutar siaran musik.");
	return 1;
}

CMD:pickupboombox(playerid, params[])
{
	return ProcessPickupBoombox(playerid);
}

stock ProcessPickupBoombox(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda belum login.");
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda tidak dapat mengambil Boombox saat pingsan atau terborgol.");
		return 1;
	}

	new foundSlot = -1;
	for (new i = 0; i < MAX_BOOMBOXES; i++) {
		if (gBoomboxes[i][bbUsed] && IsPlayerNearPoint(playerid, gBoomboxes[i][bbX], gBoomboxes[i][bbY], gBoomboxes[i][bbZ], 3.5)) {
			if (gBoomboxes[i][bbOwnerID] == gPlayerData[playerid][pID] || gPlayerData[playerid][pAdmin] > 0) {
				foundSlot = i;
				break;
			}
		}
	}

	if (foundSlot == -1) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda tidak berada di dekat Boombox milik Anda.");
		return 1;
	}

	// [FIX] Cek kapasitas inventaris sebelum mematikan & menghancurkan boombox
	// agar boombox tidak terhapus ke void bila kantong pemain penuh.
	if (!GivePlayerItem(playerid, ITEM_BOOMBOX, 1)) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Kantung inventaris Anda penuh! Kosongkan slot terlebih dahulu.");
		return 1;
	}

	new Float:bx = gBoomboxes[foundSlot][bbX];
	new Float:by = gBoomboxes[foundSlot][bbY];
	new Float:bz = gBoomboxes[foundSlot][bbZ];

	// Stop audio stream untuk semua pemain terdekat
	for (new p = 0; p < MAX_PLAYERS; p++) {
		if (IsPlayerConnected(p) && IsPlayerNearPoint(p, bx, by, bz, BOOMBOX_AUDIO_RADIUS + 5.0)) {
			StopAudioStreamForPlayer(p);
		}
	}

	if (gBoomboxes[foundSlot][bbObject] != INVALID_OBJECT_ID) {
		DestroyObject(gBoomboxes[foundSlot][bbObject]);
		gBoomboxes[foundSlot][bbObject] = INVALID_OBJECT_ID;
	}
	if (gBoomboxes[foundSlot][bbLabel] != Text3D:INVALID_3DTEXT_ID) {
		Delete3DTextLabel(gBoomboxes[foundSlot][bbLabel]);
		gBoomboxes[foundSlot][bbLabel] = Text3D:INVALID_3DTEXT_ID;
	}
	gBoomboxes[foundSlot][bbUsed] = false;

	SendClientMessage(playerid, COLOR_SUCCESS, "[SUKSES] Speaker Boombox berhasil dimatikan dan dimasukkan kembali ke tas Anda.");
	return 1;
}
