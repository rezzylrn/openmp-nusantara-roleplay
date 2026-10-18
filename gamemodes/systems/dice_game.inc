#if defined _INC_rp_dice
	#endinput
#endif
#define _INC_rp_dice

/*
==============================================================================
 dice_game.inc
 NUSANTARA ROLEPLAY - Street Dice Gambling & Casino Roll System
==============================================================================
*/

#define MIN_DICE_BET            5
#define MAX_DICE_BET            5000

stock Dice_Init()
{
	print("[DICE] Sistem permainan dadu taruhan jalanan & kasino aktif.");
	return 1;
}

stock ProcessPlayDice(playerid, targetid, betAmount)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda tidak dapat bertaruh dadu saat pingsan atau terborgol.");
		return 0;
	}

	if (!IsPlayerConnected(targetid) || !gPlayerData[targetid][pLoggedIn]) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Pemain sasaran tidak terhubung.");
		return 0;
	}

	if (gPlayerData[targetid][pInjured] || gPlayerData[targetid][pCuffed]) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Lawan tidak sadarkan diri atau sedang terborgol.");
		return 0;
	}

	if (targetid == playerid) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda tidak dapat bertaruh dadu melawan diri sendiri.");
		return 0;
	}

	if (GetPlayerItemCount(playerid, ITEM_DICE) <= 0) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda memerlukan Dadu Kasino di inventaris (/inv). Beli di toko 24/7.");
		return 0;
	}

	if (!IsPlayerNear(playerid, targetid, 4.0)) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus berada di dekat lawan taruhan (maks. 4 meter).");
		return 0;
	}

	if (betAmount < MIN_DICE_BET || betAmount > MAX_DICE_BET) {
		new errBet[128];
		format(errBet, sizeof(errBet), "ERROR: Nominal taruhan dadu harus antara $%d hingga $%d.", MIN_DICE_BET, MAX_DICE_BET);
		SendClientMessage(playerid, COLOR_ERROR, errBet);
		return 0;
	}

	if (gPlayerData[playerid][pMoney] < betAmount) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Uang tunai Anda tidak mencukupi untuk nominal taruhan ini.");
		return 0;
	}

	if (gPlayerData[targetid][pMoney] < betAmount) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Uang tunai lawan tidak mencukupi untuk nominal taruhan ini.");
		return 0;
	}

	// Lempar dadu (1 - 6)
	new roll1 = 1 + random(6);
	new roll2 = 1 + random(6);

	// Animasi lempar dadu
	ApplyAnimation(playerid, "CASINO", "cards_in", 4.1, false, false, false, false, 0);
	ApplyAnimation(targetid, "CASINO", "cards_in", 4.1, false, false, false, false, 0);

	PlayerPlaySound(playerid, 1058, 0.0, 0.0, 0.0);
	PlayerPlaySound(targetid, 1058, 0.0, 0.0, 0.0);

	new rpMsg[180];
	if (roll1 > roll2) {
		// Player 1 Menang. [FIX] Ambil dulu uang pecundang; jika gagal (dana
		// berubah), batalkan agar uang pemenang tidak diciptakan dari udara.
		if (!TakePlayerMoneyEx(targetid, betAmount, "kalah judi dadu")) {
			SendClientMessage(playerid, COLOR_ERROR, "ERROR: Taruhan dibatalkan, dana lawan tidak mencukupi.");
			SendClientMessage(targetid, COLOR_ERROR, "ERROR: Taruhan dibatalkan, dana Anda tidak mencukupi.");
			return 0;
		}
		GivePlayerMoneyEx(playerid, betAmount, "menang judi dadu");

		format(rpMsg, sizeof(rpMsg),
			"* [DADU] %s melempar [%d] vs %s melempar [%d]! Pemenang: %s (+ $%d)",
			gPlayerData[playerid][pName], roll1,
			gPlayerData[targetid][pName], roll2,
			gPlayerData[playerid][pName], betAmount);
	} else if (roll2 > roll1) {
		// Target Menang. [FIX] Ambil dulu uang pecundang; batalkan bila gagal.
		if (!TakePlayerMoneyEx(playerid, betAmount, "kalah judi dadu")) {
			SendClientMessage(playerid, COLOR_ERROR, "ERROR: Taruhan dibatalkan, dana Anda tidak mencukupi.");
			SendClientMessage(targetid, COLOR_ERROR, "ERROR: Taruhan dibatalkan, dana lawan tidak mencukupi.");
			return 0;
		}
		GivePlayerMoneyEx(targetid, betAmount, "menang judi dadu");

		format(rpMsg, sizeof(rpMsg),
			"* [DADU] %s melempar [%d] vs %s melempar [%d]! Pemenang: %s (+ $%d)",
			gPlayerData[playerid][pName], roll1,
			gPlayerData[targetid][pName], roll2,
			gPlayerData[targetid][pName], betAmount);
	} else {
		// Seri
		format(rpMsg, sizeof(rpMsg),
			"* [DADU SERI] %s melempar [%d] vs %s melempar [%d]! Skor seimbang, taruhan dikembalikan.",
			gPlayerData[playerid][pName], roll1,
			gPlayerData[targetid][pName], roll2);
	}

	// Broadcast hasil ke sekitar radius 12.0m
	for (new i = 0; i < MAX_PLAYERS; i++) {
		if (IsPlayerConnected(i) && gPlayerData[i][pLoggedIn] && IsPlayerNear(playerid, i, 12.0)) {
			SendClientMessage(i, COLOR_YELLOW, rpMsg);
		}
	}
	return 1;
}
