#if defined _INC_rp_cmd_illegal
	#endinput
#endif
#define _INC_rp_cmd_illegal

/*
==============================================================================
 cmd_illegal.inc (Aktivitas Bawah Tanah & Kriminal)
 NUSANTARA ROLEPLAY - Modular Command Architecture
==============================================================================
*/


CMD:smuggle(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	ProcessStartSmuggle(playerid);
	return 1;
}


CMD:selundup(playerid, params[])
{
	return cmd_smuggle(playerid, params);
}


CMD:chopveh(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	ProcessChopVehicle(playerid);
	return 1;
}


CMD:chopshop(playerid, params[])
{
	return cmd_chopveh(playerid, params);
}


CMD:lockpick(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	ProcessLockpickVehicle(playerid);
	return 1;
}


CMD:bobol(playerid, params[])
{
	return cmd_lockpick(playerid, params);
}


CMD:blackmarket(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	ShowBlackMarketDialog(playerid);
	return 1;
}


CMD:bm(playerid, params[])
{
	return cmd_blackmarket(playerid, params);
}


CMD:pasargelap(playerid, params[])
{
	return cmd_blackmarket(playerid, params);
}


CMD:robstore(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	ProcessRobStore(playerid);
	return 1;
}


CMD:rampok(playerid, params[])
{
	return cmd_robstore(playerid, params);
}


CMD:plantweed(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	ProcessPlantWeed(playerid);
	return 1;
}


CMD:tanam(playerid, params[])
{
	return cmd_plantweed(playerid, params);
}


CMD:harvestweed(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	ProcessHarvestWeed(playerid);
	return 1;
}


CMD:panen(playerid, params[])
{
	return cmd_harvestweed(playerid, params);
}


CMD:burnweed(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	ProcessBurnWeed(playerid);
	return 1;
}


CMD:hanguskan(playerid, params[])
{
	return cmd_burnweed(playerid, params);
}


