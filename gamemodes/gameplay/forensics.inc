#if defined _INC_rp_forensics
	#endinput
#endif
#define _INC_rp_forensics

/*
==============================================================================
 forensics.inc
 NUSANTARA ROLEPLAY - Ballistics Forensics & Crime Scene Investigation
 Sistem Forensik Balistik Kepolisian SAPD:
 - Mencatat selongsong peluru di lokasi penembakan secara real-time.
 - Detektif kepolisian dapat memeriksa TKP (/casings /forensik) untuk mengetahui
   jenis kaliber peluru, waktu penembakan, dan arah balistik.
 - Mengamankan barang bukti selongsong peluru (/collectcasing) ke tas bukti.
==============================================================================
*/

#define MAX_BULLET_CASINGS      80
#define CASING_EXPIRY_SECONDS   900 // 15 menit bertahan di TKP

enum E_BULLET_CASING {
	bool:bcUsed,
	WEAPON:bcWeapon,
	Float:bcX,
	Float:bcY,
	Float:bcZ,
	bcTime,
	bcShooterID
};

new gBulletCasings[MAX_BULLET_CASINGS][E_BULLET_CASING];
static gNextCasingSlot = 0;

forward RP_OnFinishCollectCasing(playerid, casingSlot);

stock Forensics_Init()
{
	for (new i = 0; i < MAX_BULLET_CASINGS; i++) {
		gBulletCasings[i][bcUsed] = false;
	}
	return 1;
}

stock Forensics_RecordShot(playerid, WEAPON:weaponid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;
	// Hanya catat senjata api berpeluru
	if (weaponid < WEAPON_COLT45 || weaponid > WEAPON_SNIPER) return 0;

	new Float:x, Float:y, Float:z;
	GetPlayerPos(playerid, x, y, z);

	new slot = gNextCasingSlot;
	gBulletCasings[slot][bcUsed] = true;
	gBulletCasings[slot][bcWeapon] = weaponid;
	gBulletCasings[slot][bcX] = x;
	gBulletCasings[slot][bcY] = y;
	gBulletCasings[slot][bcZ] = z - 0.9;
	gBulletCasings[slot][bcTime] = gettime();
	gBulletCasings[slot][bcShooterID] = gPlayerData[playerid][pID];

	gNextCasingSlot = (gNextCasingSlot + 1) % MAX_BULLET_CASINGS;
	return 1;
}

CMD:casings(playerid, params[])
{
	return cmd_forensik(playerid, params);
}

CMD:forensik(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda belum login.");
	if (gPlayerData[playerid][pFaction] != FACTION_POLICE || !gPlayerData[playerid][pDuty]) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Hanya petugas kepolisian SAPD on-duty yang dapat melakukan investigasi forensik.");
		return 1;
	}

	new foundCount = 0;
	new dialogContent[1024];
	format(dialogContent, sizeof(dialogContent), "Jarak\tKaliber Senjata\tWaktu Penembakan\n");

	new now = gettime();
	for (new i = 0; i < MAX_BULLET_CASINGS; i++) {
		if (gBulletCasings[i][bcUsed]) {
			if ((now - gBulletCasings[i][bcTime]) > CASING_EXPIRY_SECONDS) {
				gBulletCasings[i][bcUsed] = false;
				continue;
			}

			new Float:dist = GetPlayerDistanceFromPoint(playerid, gBulletCasings[i][bcX], gBulletCasings[i][bcY], gBulletCasings[i][bcZ]);
			if (dist <= 12.0) {
				new weapName[32];
				GetWeaponName(gBulletCasings[i][bcWeapon], weapName, sizeof(weapName));

				new elapsed = now - gBulletCasings[i][bcTime];
				new timeDesc[32];
				if (elapsed < 60) format(timeDesc, sizeof(timeDesc), "%d detik lalu", elapsed);
				else format(timeDesc, sizeof(timeDesc), "%d menit lalu", elapsed / 60);

				new row[128];
				format(row, sizeof(row), "{FFFF00}%.1fm\t{3498DB}%s\t{FFFFFF}%s\n", dist, weapName, timeDesc);
				strcat(dialogContent, row);
				foundCount++;
			}
		}
	}

	if (foundCount == 0) {
		Dialog_Show(playerid, DialogForensicsEmpty, DIALOG_STYLE_MSGBOX,
			"{3498DB}INVESTIGASI BALISTIK SAPD",
			"{FFFFFF}Tidak ditemukan selongsong peluru di radius 12 meter dari posisi Anda.", "Tutup", "");
		return 1;
	}

	Dialog_Show(playerid, DialogForensicsList, DIALOG_STYLE_TABLIST_HEADERS,
		"{3498DB}TEMUAN SELONGSONG PELURU DI TKP", dialogContent, "Amankan Bukti", "Tutup");
	return 1;
}

Dialog:DialogForensicsEmpty(playerid, response, listitem, inputtext[])
{
	return 1;
}

Dialog:DialogForensicsList(playerid, response, listitem, inputtext[])
{
	if (!response) return 1;
	return ProcessCollectCasing(playerid);
}

CMD:collectcasing(playerid, params[])
{
	return ProcessCollectCasing(playerid);
}

stock ProcessCollectCasing(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda belum login.");
	if (gPlayerData[playerid][pFaction] != FACTION_POLICE || !gPlayerData[playerid][pDuty]) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Hanya petugas kepolisian SAPD yang dapat mengamankan barang bukti.");
		return 1;
	}

	new nearSlot = -1;
	new Float:closest = 3.5;
	new now = gettime();

	for (new i = 0; i < MAX_BULLET_CASINGS; i++) {
		if (gBulletCasings[i][bcUsed]) {
			if ((now - gBulletCasings[i][bcTime]) > CASING_EXPIRY_SECONDS) {
				gBulletCasings[i][bcUsed] = false;
				continue;
			}

			new Float:d = GetPlayerDistanceFromPoint(playerid, gBulletCasings[i][bcX], gBulletCasings[i][bcY], gBulletCasings[i][bcZ]);
			if (d < closest) {
				closest = d;
				nearSlot = i;
			}
		}
	}

	if (nearSlot == -1) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Tidak ada selongsong peluru dalam jangkauan dekat Anda.");
		return 1;
	}

	ApplyProgressBar(playerid, "MENGAMANKAN SELONGSONG BUKTI...", 3, "RP_OnFinishCollectCasing", nearSlot);
	ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.1, false, false, false, false, 0, 1);
	return 1;
}

public RP_OnFinishCollectCasing(playerid, casingSlot)
{
	if (casingSlot < 0 || casingSlot >= MAX_BULLET_CASINGS || !gBulletCasings[casingSlot][bcUsed]) return 0;

	new weapName[32];
	GetWeaponName(gBulletCasings[casingSlot][bcWeapon], weapName, sizeof(weapName));

	gBulletCasings[casingSlot][bcUsed] = false;

	new msg[144];
	format(msg, sizeof(msg), "[FORENSIK SAPD] Berhasil mengamankan 1x selongsong peluru kaliber %s ke kantong barang bukti.", weapName);
	SendClientMessage(playerid, COLOR_SUCCESS, msg);
	return 1;
}
