#if defined _INC_rp_cmd_activities
	#endinput
#endif
#define _INC_rp_cmd_activities

/*
==============================================================================
 cmd_activities.inc (Gaya Hidup & Aktivitas Kota)
 NUSANTARA ROLEPLAY - Modular Command Architecture
==============================================================================
*/


CMD:detect(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return SendErr(playerid, "Anda tidak dapat melakukan aktivitas saat pingsan atau terborgol.");
	ProcessMetalDetect(playerid);
	return 1;
}


CMD:deteksi(playerid, params[])
{
	return cmd_detect(playerid, params);
}


CMD:dig(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return SendErr(playerid, "Anda tidak dapat melakukan aktivitas saat pingsan atau terborgol.");
	ProcessDigTreasure(playerid);
	return 1;
}


CMD:gali(playerid, params[])
{
	return cmd_dig(playerid, params);
}


CMD:vending(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return SendErr(playerid, "Anda tidak dapat menggunakan vending machine saat pingsan atau terborgol.");
	ShowVendingDialog(playerid);
	return 1;
}


CMD:tag(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	if (isnull(params)) return SendSyntax(playerid, "/tag [teks_grafiti]");
	ProcessTagGraffiti(playerid, params);
	return 1;
}


CMD:cleantag(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	ProcessCleanGraffiti(playerid);
	return 1;
}


CMD:campfire(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	ProcessCreateCampfire(playerid);
	return 1;
}


CMD:apiunggun(playerid, params[])
{
	return cmd_campfire(playerid, params);
}


CMD:cook(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	ProcessCookFish(playerid);
	return 1;
}


CMD:bakar(playerid, params[])
{
	return cmd_cook(playerid, params);
}


CMD:makanikan(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	ProcessEatCookedFish(playerid);
	return 1;
}


CMD:dice(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	new targetid, bet;
	if (sscanf(params, "ud", targetid, bet)) return SendSyntax(playerid, "/dice [targetid/Nama] [nominal_taruhan]");
	ProcessPlayDice(playerid, targetid, bet);
	return 1;
}


CMD:dadu(playerid, params[])
{
	return cmd_dice(playerid, params);
}


CMD:dmv(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	ShowDMVExamDialog(playerid);
	return 1;
}


CMD:examsim(playerid, params[])
{
	return cmd_dmv(playerid, params);
}


CMD:buygun(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	ShowAmmuNationMenu(playerid);
	return 1;
}


CMD:ammunation(playerid, params[])
{
	return cmd_buygun(playerid, params);
}


CMD:ad(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	if (isnull(params)) return SendSyntax(playerid, "/ad [teks_iklan_sanews]");
	ProcessSendAdvertisement(playerid, params);
	return 1;
}


CMD:iklan(playerid, params[])
{
	return cmd_ad(playerid, params);
}


CMD:weather(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	new wName[48];
	GetCurrentWeatherName(wName, sizeof(wName));
	new msg[128];
	format(msg, sizeof(msg), "[BMKG] Kondisi cuaca langit saat ini: '%s'.", wName);
	SendClientMessage(playerid, COLOR_LIGHTBLUE, msg);
	return 1;
}


CMD:cuaca(playerid, params[])
{
	return cmd_weather(playerid, params);
}


CMD:bar(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return SendErr(playerid, "Anda tidak dapat melakukan aktivitas saat pingsan atau terborgol.");
	ShowBarDrinksMenu(playerid);
	return 1;
}


CMD:orderdrink(playerid, params[])
{
	return cmd_bar(playerid, params);
}


CMD:gym(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return SendErr(playerid, "Anda tidak dapat melakukan aktivitas saat pingsan atau terborgol.");
	ShowGymStyleDialog(playerid);
	return 1;
}


CMD:fightstyle(playerid, params[])
{
	return cmd_gym(playerid, params);
}


CMD:workout(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return SendErr(playerid, "Anda tidak dapat berolahraga saat pingsan atau terborgol.");
	ProcessGymWorkout(playerid);
	return 1;
}


CMD:angkatbesi(playerid, params[])
{
	return cmd_workout(playerid, params);
}


CMD:orderfood(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return SendErr(playerid, "Anda tidak dapat memesan makanan saat pingsan atau terborgol.");
	ShowFastFoodMenu(playerid);
	return 1;
}


CMD:pesanmakan(playerid, params[])
{
	return cmd_orderfood(playerid, params);
}


CMD:accessories(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return SendErr(playerid, "Anda tidak dapat memakai aksesoris saat pingsan atau terborgol.");
	ShowAccessoriesMenu(playerid);
	return 1;
}


CMD:aksesoris(playerid, params[])
{
	return cmd_accessories(playerid, params);
}


