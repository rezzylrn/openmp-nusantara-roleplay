#if defined _INC_rp_mining
	#endinput
#endif
#define _INC_rp_mining

/*
==============================================================================
 mining.inc
 NUSANTARA ROLEPLAY - Hunter Quarry Mining, Mineral Excavation & Metal Smelting
==============================================================================
*/

#define MINING_SHOP_X       815.5
#define MINING_SHOP_Y       856.0
#define MINING_SHOP_Z       12.0

#define MINING_SITE_X       592.5
#define MINING_SITE_Y       870.0
#define MINING_SITE_Z       -42.5

#define MINING_SMELTER_X    822.0
#define MINING_SMELTER_Y    862.0
#define MINING_SMELTER_Z    12.0

#define MINING_BUYER_X      825.0
#define MINING_BUYER_Y      850.0
#define MINING_BUYER_Z      12.0

static gMiningPickaxePickup;
static Text3D:gMiningShopLabel;
static Text3D:gMiningSiteLabel;
static Text3D:gMiningSmelterLabel;
static Text3D:gMiningBuyerLabel;

static gMiningTargetCrack[MAX_PLAYERS];

stock Mining_Init()
{
	gMiningPickaxePickup = CreatePickup(1239, 1, MINING_SHOP_X, MINING_SHOP_Y, MINING_SHOP_Z);
	gMiningShopLabel = Create3DTextLabel("{E67E22}[ TOKO ALAT TAMBANG ]\n{FFFFFF}Gunakan /buypickaxe untuk membeli beliung ($45)",
		COLOR_ORANGE, MINING_SHOP_X, MINING_SHOP_Y, MINING_SHOP_Z + 0.8, 15.0, 0, false);

	gMiningSiteLabel = Create3DTextLabel("{E67E22}[ URAT MINERAL QUARRY ]\n{FFFFFF}Gunakan /mine untuk menambang bijih besi & batu bara",
		COLOR_ORANGE, MINING_SITE_X, MINING_SITE_Y, MINING_SITE_Z + 0.8, 20.0, 0, false);

	gMiningSmelterLabel = Create3DTextLabel("{E74C3C}[ TUNGKU PELEBURAN BESI ]\n{FFFFFF}Gunakan /smelt untuk melebur bijih besi jadi batangan",
		COLOR_RED, MINING_SMELTER_X, MINING_SMELTER_Y, MINING_SMELTER_Z + 0.8, 15.0, 0, false);

	gMiningBuyerLabel = Create3DTextLabel("{2ECC71}[ PENGEPUL HASIL TAMBANG ]\n{FFFFFF}Gunakan /sellore untuk menjual hasil tambang Anda",
		COLOR_GREEN, MINING_BUYER_X, MINING_BUYER_Y, MINING_BUYER_Z + 0.8, 15.0, 0, false);

	#pragma unused gMiningPickaxePickup, gMiningShopLabel, gMiningSiteLabel, gMiningSmelterLabel, gMiningBuyerLabel
	print("[MINING] Sistem penambangan Hunter Quarry & peleburan logam aktif.");
	return 1;
}

stock ProcessBuyPickaxe(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (!IsPlayerNearPoint(playerid, MINING_SHOP_X, MINING_SHOP_Y, MINING_SHOP_Z, 4.0)) {
		SendErr(playerid, "Anda harus berada di Toko Alat Tambang Hunter Quarry.");
		return 0;
	}

	if (gPlayerData[playerid][pMoney] < 45) {
		SendErr(playerid, "Uang tunai Anda tidak mencukupi ($45 diperlukan).");
		return 0;
	}

	TakePlayerMoneyEx(playerid, 45, "beli beliung tambang");
	GivePlayerItem(playerid, ITEM_PICKAXE, 1);
	SendSucc(playerid, "Anda berhasil membeli 1x Beliung Tambang (Pickaxe) seharga $45.");
	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
	return 1;
}

forward OnPlayerFinishMiningProgress(playerid, extra);
public OnPlayerFinishMiningProgress(playerid, extra)
{
	#pragma unused extra
	if (!gPlayerData[playerid][pLoggedIn]) return 0;

	// [FIX] Validasi ulang: pemain harus masih penambang, tidak pingsan/
	// terborgol, dan masih di area tambang. Sebelumnya bisa quit job / kabur
	// dan tetap lanjut ke tahap ayunan akhir.
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return 0;
	if (gPlayerData[playerid][pJob] != JOB_MINER) return 0;
	if (IsPlayerConnected(playerid) && !IsPlayerNearPoint(playerid, MINING_SITE_X, MINING_SITE_Y, MINING_SITE_Z, 25.0)) return 0;

	if (IsPlayerConnected(playerid)) {
		ClearAnimations(playerid);
		TogglePlayerControllable(playerid, true);
	}

	// Tentukan retakan sasaran acak (0, 1, 2)
	gMiningTargetCrack[playerid] = random(3);

	new body[350];
	format(body, sizeof(body),
		"{FFFFFF}Urat batu terbelah sebagian! Pilih teknik ayunan beliung akhir:\n\n\
{FFFF00}1. Hantam tepat di retakan tengah batu (Konsentrasi Inti)\n\
{FFFF00}2. Tebas celah lapisan atas dinding batu\n\
{FFFF00}3. Congkel urat mineral di rekahan bawah batu");

	Dialog_Show(playerid, DialogMiningStrike, DIALOG_STYLE_LIST,
		"{E67E22}MINING - TEKNIK AYUNAN AKHIR", body, "Hantam", "Batal");
	return 1;
}

Dialog:DialogMiningStrike(playerid, response, listitem, inputtext[])
{
	if (!response) {
		SendInfo(playerid, "Anda menghentikan proses penambangan batu.");
		return 1;
	}

	if (listitem == gMiningTargetCrack[playerid]) {
		// Pukulan presisi sempurna
		new ironGain = 2 + random(2);
		new coalGain = 1 + random(2);

		GivePlayerItem(playerid, ITEM_IRON_ORE, ironGain);
		GivePlayerItem(playerid, ITEM_COAL, coalGain);
		GivePlayerExp(playerid, 12, "menambang mineral");

		new msg[144];
		format(msg, sizeof(msg), "[SUKSES] Ayunan presisi! Anda memperoleh %dx Bijih Besi dan %dx Batu Bara.", ironGain, coalGain);
		SendClientMessage(playerid, COLOR_SUCCESS, msg);
		PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
	} else {
		// Pukulan meleset namun tetap dapat sedikit serpihan batu bara
		GivePlayerItem(playerid, ITEM_COAL, 1);
		GivePlayerExp(playerid, 4, "menambang mineral");
		SendClientMessage(playerid, COLOR_INFO, "[INFO] Ayunan kurang pas. Anda hanya mendapatkan 1x Batu Bara serpihan.");
	}
	return 1;
}

stock ProcessStartMining(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendErr(playerid, "Anda tidak dapat menambang saat pingsan atau terborgol.");
		return 0;
	}
	if (gPlayerData[playerid][pJob] != JOB_MINER) {
		SendErr(playerid, "Pekerjaan ini khusus untuk Penambang Batu Quarry (/job di Balai Kota).");
		return 0;
	}
	if (!IsPlayerNearPoint(playerid, MINING_SITE_X, MINING_SITE_Y, MINING_SITE_Z, 25.0)) {
		SendErr(playerid, "Anda harus berada di area urat mineral dasar Hunter Quarry.");
		return 0;
	}

	if (GetPlayerItemCount(playerid, ITEM_PICKAXE) <= 0) {
		SendErr(playerid, "Anda memerlukan Beliung Tambang (Pickaxe) di inventaris (/inv). Beli di /buypickaxe.");
		return 0;
	}

	ApplyAnimation(playerid, "BASEBALL", "Bat_4", 4.1, true, false, false, false, 0);
	ApplyProgressBar(playerid, "MEMECAHKAN URAT BATU...", 6, "OnPlayerFinishMiningProgress", 0);
	SendRP(playerid, COLOR_ME, "* %s mengayunkan beliung dan menghantam dinding batu mineral berkali-kali.", gPlayerData[playerid][pName]);
	return 1;
}

forward OnPlayerFinishSmelting(playerid, extra);
public OnPlayerFinishSmelting(playerid, extra)
{
	#pragma unused extra
	if (!gPlayerData[playerid][pLoggedIn]) return 0;

	if (IsPlayerConnected(playerid)) {
		ClearAnimations(playerid);
		TogglePlayerControllable(playerid, true);
	}

	// [SECURITY] Verifikasi bahan baku masih ada SEBELUM memberi hasil. Tanpa
	// cek ini, pemain bisa membuang ore saat progress bar berjalan dan tetap
	// menerima Batangan Besi gratis.
	if (GetPlayerItemCount(playerid, ITEM_IRON_ORE) < 2 || GetPlayerItemCount(playerid, ITEM_COAL) < 1) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Peleburan gagal: bahan baku (2 Bijih Besi + 1 Batubara) tidak lagi mencukupi.");
		return 0;
	}

	TakePlayerItem(playerid, ITEM_IRON_ORE, 2);
	TakePlayerItem(playerid, ITEM_COAL, 1);
	GivePlayerItem(playerid, ITEM_IRON_BAR, 1);
	GivePlayerExp(playerid, 15, "melebur logam");

	SendClientMessage(playerid, COLOR_SUCCESS, "[SUKSES] Proses peleburan selesai! 1x Batangan Besi Murni berhasil dicetak ke inventaris.");
	if (IsPlayerConnected(playerid)) PlayerPlaySound(playerid, 1058, 0.0, 0.0, 0.0);
	return 1;
}

stock ProcessSmeltMetals(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendErr(playerid, "Anda tidak dapat melebur logam saat pingsan atau terborgol.");
		return 0;
	}
	if (!IsPlayerNearPoint(playerid, MINING_SMELTER_X, MINING_SMELTER_Y, MINING_SMELTER_Z, 5.0)) {
		SendErr(playerid, "Anda harus berada di depan Tungku Peleburan Besi Hunter Quarry.");
		return 0;
	}

	if (GetPlayerItemCount(playerid, ITEM_IRON_ORE) < 2) {
		SendErr(playerid, "Dibutuhkan minimal 2x Bongkahan Bijih Besi untuk peleburan.");
		return 0;
	}

	if (GetPlayerItemCount(playerid, ITEM_COAL) < 1) {
		SendErr(playerid, "Dibutuhkan 1x Bongkahan Batu Bara sebagai bahan bakar pembakaran tungku.");
		return 0;
	}

	ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.1, true, false, false, false, 0);
	ApplyProgressBar(playerid, "MELEBUR BIJIH BESI KE CETAKAN...", 5, "OnPlayerFinishSmelting", 0);
	SendRP(playerid, COLOR_ME, "* %s memasukkan bijih besi dan batu bara ke dalam tungku bersuhu tinggi.", gPlayerData[playerid][pName]);
	return 1;
}

stock ProcessSellOre(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendErr(playerid, "Anda tidak dapat menjual hasil tambang saat pingsan atau terborgol.");
		return 0;
	}
	if (!IsPlayerNearPoint(playerid, MINING_BUYER_X, MINING_BUYER_Y, MINING_BUYER_Z, 5.0)) {
		SendErr(playerid, "Anda harus berada di Pengepul Hasil Tambang Hunter Quarry.");
		return 0;
	}

	new ironCount = GetPlayerItemCount(playerid, ITEM_IRON_ORE);
	new coalCount = GetPlayerItemCount(playerid, ITEM_COAL);
	new barCount  = GetPlayerItemCount(playerid, ITEM_IRON_BAR);

	if (ironCount <= 0 && coalCount <= 0 && barCount <= 0) {
		SendErr(playerid, "Anda tidak membawa hasil tambang apa pun (Bijih Besi, Batu Bara, atau Batangan Besi).");
		return 0;
	}

	new totalCash = 0;
	if (ironCount > 0) {
		TakePlayerItem(playerid, ITEM_IRON_ORE, ironCount);
		totalCash += (ironCount * 45);
	}
	if (coalCount > 0) {
		TakePlayerItem(playerid, ITEM_COAL, coalCount);
		totalCash += (coalCount * 30);
	}
	if (barCount > 0) {
		TakePlayerItem(playerid, ITEM_IRON_BAR, barCount);
		totalCash += (barCount * 140);
	}

	GivePlayerMoneyEx(playerid, totalCash, "penjualan hasil tambang");
	new msg[144];
	format(msg, sizeof(msg), "[SUKSES] Anda menjual hasil tambang (%dx Besi, %dx Bara, %dx Batang) seharga $%d.",
		ironCount, coalCount, barCount, totalCash);
	SendClientMessage(playerid, COLOR_SUCCESS, msg);
	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
	return 1;
}
