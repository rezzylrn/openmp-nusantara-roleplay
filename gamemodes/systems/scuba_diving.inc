#if defined _INC_rp_scuba_diving
	#endinput
#endif
#define _INC_rp_scuba_diving

/*
==============================================================================
 scuba_diving.inc
 NUSANTARA ROLEPLAY - Santa Maria Beach Scuba Diving & Deep Sea Pearl Harvest
==============================================================================
*/

#define SCUBA_SHOP_X        383.5
#define SCUBA_SHOP_Y        -2088.0
#define SCUBA_SHOP_Z        7.8

#define SCUBA_REEF_X        375.0
#define SCUBA_REEF_Y        -2160.0
#define SCUBA_REEF_Z        -2.0

static gScubaShopPickup;
static Text3D:gScubaShopLabel;
static Text3D:gScubaReefLabel;

static gScubaTargetReef[MAX_PLAYERS];

stock Scuba_Init()
{
	gScubaShopPickup = CreatePickup(1239, 1, SCUBA_SHOP_X, SCUBA_SHOP_Y, SCUBA_SHOP_Z);
	gScubaShopLabel = Create3DTextLabel("{3498DB}[ POS PENYEWAAN SCUBA ]\n{FFFFFF}/rentscuba untuk sewa tabung ($75)\n/sellpearl untuk jual mutiara ($175)",
		COLOR_LIGHTBLUE, SCUBA_SHOP_X, SCUBA_SHOP_Y, SCUBA_SHOP_Z + 0.8, 15.0, 0, false);

	gScubaReefLabel = Create3DTextLabel("{3498DB}[ AREA TERUMBU MUTIARA ]\n{FFFFFF}Gunakan /dive untuk menyelam mencari mutiara dasar laut",
		COLOR_LIGHTBLUE, SCUBA_REEF_X, SCUBA_REEF_Y, SCUBA_REEF_Z + 1.5, 30.0, 0, false);

	#pragma unused gScubaShopPickup, gScubaShopLabel, gScubaReefLabel
	print("[SCUBA] Sistem penyelaman laut dalam & perburuan mutiara aktif.");
	return 1;
}

stock ProcessRentScuba(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (!IsPlayerNearPoint(playerid, SCUBA_SHOP_X, SCUBA_SHOP_Y, SCUBA_SHOP_Z, 4.0)) {
		SendErr(playerid, "Anda harus berada di Pos Penyewaan Scuba Dermaga Santa Maria.");
		return 0;
	}

	if (gPlayerData[playerid][pMoney] < 75) {
		SendErr(playerid, "Uang tunai Anda tidak mencukupi ($75 diperlukan).");
		return 0;
	}

	TakePlayerMoneyEx(playerid, 75, "sewa tabung selam scuba");
	GivePlayerItem(playerid, ITEM_SCUBA, 1);
	SendSucc(playerid, "Anda menyewa 1x Tabung Selam Scuba seharga $75. Pergi ke laut Santa Maria untuk /dive.");
	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
	return 1;
}

forward OnPlayerFinishDiving(playerid, extra);
public OnPlayerFinishDiving(playerid, extra)
{
	#pragma unused extra
	if (!gPlayerData[playerid][pLoggedIn]) return 0;

	if (IsPlayerConnected(playerid)) {
		ClearAnimations(playerid);
		TogglePlayerControllable(playerid, true);
	}

	gScubaTargetReef[playerid] = random(3);

	new body[350];
	format(body, sizeof(body),
		"{FFFFFF}Anda mengamati dasar terumbu karang! Pilih celah tiram yang ingin diperiksa:\n\n\
{3498DB}1. Celah rongga terumbu karang bercabang\n\
{3498DB}2. Tumpukan pasir bawah cekungan batuan karang\n\
{3498DB}3. Formasi kerang hitam di sisi dinding karang laut");

	Dialog_Show(playerid, DialogScubaSearch, DIALOG_STYLE_LIST,
		"{3498DB}PENYELAMAN - TITIK KARANG TIRAM", body, "Buka Tiram", "Batal");
	return 1;
}

Dialog:DialogScubaSearch(playerid, response, listitem, inputtext[])
{
	if (!response) {
		SendInfo(playerid, "Anda naik kembali ke permukaan tanpa mengambil tiram.");
		return 1;
	}

	if (listitem == gScubaTargetReef[playerid]) {
		new pearlGain = 1 + random(2);
		GivePlayerItem(playerid, ITEM_PEARL, pearlGain);
		GivePlayerExp(playerid, 14, "menemukan mutiara laut");

		new msg[144];
		format(msg, sizeof(msg), "[SUKSES] Tiram terbuka! Anda menemukan %dx Mutiara Laut Alami (/inv). Jual di /sellpearl.", pearlGain);
		SendClientMessage(playerid, COLOR_SUCCESS, msg);
		PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
	} else {
		GivePlayerExp(playerid, 4, "menyelam terumbu karang");
		SendClientMessage(playerid, COLOR_INFO, "[INFO] Cangkang tiram kosong, hanya berisi endapan pasir laut.");
	}
	return 1;
}

stock ProcessStartDiving(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendErr(playerid, "Anda tidak dapat menyelam saat pingsan atau terborgol.");
		return 0;
	}
	if (!IsPlayerNearPoint(playerid, SCUBA_REEF_X, SCUBA_REEF_Y, SCUBA_REEF_Z, 60.0)) {
		SendErr(playerid, "Anda harus berada di zona perairan terumbu karang Santa Maria (/gps).");
		return 0;
	}

	if (GetPlayerItemCount(playerid, ITEM_SCUBA) <= 0) {
		SendErr(playerid, "Anda membutuhkan Tabung Selam Scuba di inventaris. Sewa di dermaga /rentscuba.");
		return 0;
	}

	ApplyAnimation(playerid, "SWIM", "SWIM_under", 4.1, true, false, false, false, 0);
	ApplyProgressBar(playerid, "MENELUSURI DASAR TERUMBU LAUT...", 6, "OnPlayerFinishDiving", 0);
	SendRP(playerid, COLOR_ME, "* %s memasang regulator tabung scuba dan menyelam menelusuri dasar karang laut.", gPlayerData[playerid][pName]);
	return 1;
}

stock ProcessSellPearls(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (!IsPlayerNearPoint(playerid, SCUBA_SHOP_X, SCUBA_SHOP_Y, SCUBA_SHOP_Z, 5.0)) {
		SendErr(playerid, "Anda harus berada di Pos Dermaga Santa Maria.");
		return 0;
	}

	new count = GetPlayerItemCount(playerid, ITEM_PEARL);
	if (count <= 0) {
		SendErr(playerid, "Anda tidak membawa Mutiara Laut Alami di dalam inventaris.");
		return 0;
	}

	TakePlayerItem(playerid, ITEM_PEARL, count);
	new payout = count * 175;
	GivePlayerMoneyEx(playerid, payout, "jual mutiara laut");

	new msg[144];
	format(msg, sizeof(msg), "[SUKSES] Anda menjual %dx Mutiara Laut Alami ke kolektor dermaga seharga $%d.", count, payout);
	SendClientMessage(playerid, COLOR_SUCCESS, msg);
	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
	return 1;
}
