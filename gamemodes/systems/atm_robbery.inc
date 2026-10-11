#if defined _INC_rp_atm_robbery
	#endinput
#endif
#define _INC_rp_atm_robbery

/*
==============================================================================
 atm_robbery.inc
 NUSANTARA ROLEPLAY - ATM Skimming & Safecracking Drill System
 Pembobolan Mesin ATM Jalanan:
 - Membutuhkan Bor Listrik Baja (ITEM_DRILL) atau Lockpick dari Pasar Gelap.
 - Memicu alarm darurat sensor getaran ke radio frekuensi kepolisian SAPD.
 - Menghasilkan jarahan uang tunai $700 - $1.400 dengan cooldown per mesin.
==============================================================================
*/

#define ATM_ROB_COOLDOWN        1200 // 20 Menit cooldown
new gATMCooldown[MAX_DYNAMIC_ATMS];

forward RP_OnFinishDrillATM(playerid, atmIndex);

CMD:drillatm(playerid, params[])
{
	return ProcessDrillATM(playerid);
}

CMD:bobolatm(playerid, params[])
{
	return ProcessDrillATM(playerid);
}

stock ProcessDrillATM(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda belum login.");

	new nearATM = -1;
	for (new i = 0; i < MAX_DYNAMIC_ATMS; i++) {
		if (gATMs[i][atmUsed] && IsPlayerNearPoint(playerid, gATMs[i][atmX], gATMs[i][atmY], gATMs[i][atmZ], 2.8)) {
			nearATM = i;
			break;
		}
	}

	if (nearATM == -1) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda tidak berada di depan mesin ATM mana pun.");
		return 1;
	}

	if (gettime() < gATMCooldown[nearATM]) {
		new sisa = (gATMCooldown[nearATM] - gettime()) / 60;
		new msg[128];
		format(msg, sizeof(msg), "[INFO] Brankas ATM ini baru saja dibobol atau dalam perbaikan keamanan (Tunggu %d menit).", sisa + 1);
		SendClientMessage(playerid, COLOR_INFO, msg);
		return 1;
	}

	if (GetPlayerItemCount(playerid, ITEM_DRILL) <= 0 && GetPlayerItemCount(playerid, ITEM_LOCKPICK) <= 0) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda membutuhkan Bor Listrik (Drill) atau Obeng Lockpick dari Pasar Gelap.");
		return 1;
	}

	// Trigger alarm ke kepolisian
	new zone[48];
	GetPlayerZoneName(playerid, zone, sizeof(zone));

	new policeAlert[144];
	format(policeAlert, sizeof(policeAlert), "[RADIO 911] Sensor keamanan mendeteksi pembobolan paksa mesin ATM di %s!", zone);
	for (new p = 0; p < MAX_PLAYERS; p++) {
		if (IsPlayerConnected(p) && gPlayerData[p][pFaction] == FACTION_POLICE && gPlayerData[p][pDuty]) {
			SendClientMessage(p, COLOR_RED, policeAlert);
		}
	}

	ApplyProgressBar(playerid, "MEMBOBOL BRANKAS ATM...", 7, "RP_OnFinishDrillATM", nearATM);
	ApplyAnimation(playerid, "COP_AMBIENT", "Coplook_loop", 4.1, false, false, false, false, 0, 1);
	return 1;
}

public RP_OnFinishDrillATM(playerid, atmIndex)
{
	if (atmIndex < 0 || atmIndex >= MAX_DYNAMIC_ATMS || !gATMs[atmIndex][atmUsed]) return 0;

	gATMCooldown[atmIndex] = gettime() + ATM_ROB_COOLDOWN;

	// Peluang bor aus
	if (GetPlayerItemCount(playerid, ITEM_DRILL) > 0) {
		if (random(100) < 35) {
			TakePlayerItem(playerid, ITEM_DRILL, 1);
			SendClientMessage(playerid, COLOR_INFO, "[INFO] Mata bor listrik Anda patah akibat panas gesekan baja brankas.");
		}
	} else {
		TakePlayerItem(playerid, ITEM_LOCKPICK, 1);
	}

	new loot = 700 + random(750); // $700 - $1.450
	GivePlayerMoneyEx(playerid, loot);

	new msg[144];
	format(msg, sizeof(msg), "[SUKSES] Brankas ATM terbuka! Anda berhasil menggasak uang tunai sebesar $%d.", loot);
	SendClientMessage(playerid, COLOR_SUCCESS, msg);
	return 1;
}
