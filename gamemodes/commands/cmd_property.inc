#if defined _INC_rp_cmd_property
	#endinput
#endif
#define _INC_rp_cmd_property

/*
==============================================================================
 cmd_property.inc (Properti, Pintu & Kunci Rumah)
 NUSANTARA ROLEPLAY - Modular Command Architecture
==============================================================================
*/


CMD:enter(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	if (!ProcessDoorEnter(playerid)) {
		if (gPlayerData[playerid][pLanguage] == LANG_EN) SendErr(playerid, "You are not near any entrance door.");
		else SendErr(playerid, "Anda tidak berada di dekat pintu masuk.");
	}
	return 1;
}


CMD:masuk(playerid, params[])
{
	return cmd_enter(playerid, params);
}


CMD:exit(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	if (!ProcessDoorExit(playerid)) {
		if (gPlayerData[playerid][pLanguage] == LANG_EN) SendErr(playerid, "You are not near any exit door.");
		else SendErr(playerid, "Anda tidak berada di dekat pintu keluar.");
	}
	return 1;
}


CMD:keluar(playerid, params[])
{
	return cmd_exit(playerid, params);
}


CMD:buyhouse(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	new h = House_CheckNearby(playerid);
	if (h == -1) return SendErr(playerid, "Anda tidak berada di depan rumah yang dijual.");
	if (gHouseData[h][hOwnerID] != 0) return SendErr(playerid, "Rumah ini sudah ada pemiliknya.");
	return ProcessBuyHouse(playerid, h);
}


CMD:lockhouse(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	new h = House_CheckNearby(playerid);
	if (h == -1) h = House_CheckInside(playerid);
	if (h == -1) return SendErr(playerid, "Anda harus berada di dekat pintu rumah.");
	if (gHouseData[h][hOwnerID] != gPlayerData[playerid][pID] && gPlayerSharedHouse[playerid] != h) {
		return SendErr(playerid, "Anda bukan pemilik atau pemegang kunci rumah ini.");
	}
	gHouseData[h][hLocked] = !gHouseData[h][hLocked];
	House_RefreshDisplay(h);
	if (gHouseData[h][hLocked]) {
		SendSucc(playerid, "Pintu rumah telah dikunci.");
	} else {
		SendInfo(playerid, "Pintu rumah telah dibuka.");
	}
	return 1;
}


CMD:sharekey(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	new targetid;
	if (sscanf(params, "u", targetid)) return SendSyntax(playerid, "/sharekey [targetid/Nama]");
	ProcessShareHouseKey(playerid, targetid);
	return 1;
}


CMD:bagikunci(playerid, params[])
{
	return cmd_sharekey(playerid, params);
}


CMD:unsharekey(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	new targetid;
	if (sscanf(params, "u", targetid)) return SendSyntax(playerid, "/unsharekey [targetid/Nama]");
	ProcessRevokeHouseKey(playerid, targetid);
	return 1;
}


