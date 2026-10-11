#if defined _INC_rp_house_sharing
	#endinput
#endif
#define _INC_rp_house_sharing

/*
==============================================================================
 house_sharing.inc
 NUSANTARA ROLEPLAY - Roommate Key Sharing & Tenant Permission System
==============================================================================
*/

new gPlayerSharedHouse[MAX_PLAYERS] = {-1, ...};

stock HouseSharing_Init()
{
	for (new i = 0; i < MAX_PLAYERS; i++) {
		gPlayerSharedHouse[i] = -1;
	}
	print("[HOUSE SHARING] Sistem pembagian kunci rumah & roommate aktif.");
	return 1;
}

stock ProcessShareHouseKey(playerid, targetid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;

	if (!gPlayerData[targetid][pLoggedIn]) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Pemain sasaran belum login.");
		return 0;
	}

	if (targetid == playerid) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda tidak dapat membagikan kunci ke diri sendiri.");
		return 0;
	}

	if (!IsPlayerNear(playerid, targetid, 4.0)) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus berada di dekat teman Anda untuk memberikan kunci.");
		return 0;
	}

	// Cari rumah milik playerid
	new houseSlot = -1;
	for (new i = 0; i < MAX_HOUSES; i++) {
		if (gHouseData[i][hUsed] && gHouseData[i][hOwnerID] == gPlayerData[playerid][pID]) {
			if (IsPlayerNearPoint(playerid, gHouseData[i][hEnterX], gHouseData[i][hEnterY], gHouseData[i][hEnterZ], 8.0)) {
				houseSlot = i;
				break;
			}
		}
	}

	if (houseSlot == -1) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus berdiri di dekat rumah milik pribadi Anda.");
		return 0;
	}

	gPlayerSharedHouse[targetid] = houseSlot;
	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
	PlayerPlaySound(targetid, 1052, 0.0, 0.0, 0.0);

	new msgOwner[160], msgFriend[160];
	format(msgOwner, sizeof(msgOwner),
		"SUKSES: Anda telah menyerahkan kunci cadangan rumah di %s kepada %s.",
		gHouseData[houseSlot][hAddress], gPlayerData[targetid][pName]);
	format(msgFriend, sizeof(msgFriend),
		"SUKSES: Anda menerima kunci cadangan rumah di %s dari %s! Anda sekarang dapat keluar-masuk rumah ini.",
		gHouseData[houseSlot][hAddress], gPlayerData[playerid][pName]);

	SendClientMessage(playerid, COLOR_SUCCESS, msgOwner);
	SendClientMessage(targetid, COLOR_SUCCESS, msgFriend);
	return 1;
}

stock ProcessRevokeHouseKey(playerid, targetid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;

	if (gPlayerSharedHouse[targetid] == -1) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Pemain tersebut tidak sedang memegang kunci rumah Anda.");
		return 0;
	}

	gPlayerSharedHouse[targetid] = -1;
	SendClientMessage(playerid, COLOR_INFO, "INFO: Kunci cadangan rumah telah ditarik kembali.");
	return 1;
}
