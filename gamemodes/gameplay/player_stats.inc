#if defined _INC_rp_player_stats
	#endinput
#endif
#define _INC_rp_player_stats

/*
==============================================================================
  NUSANTARA ROLEPLAY - player_stats.inc
  /stats dialog, sistem level/EXP, kondisi lapar/haus, kematian & rumah sakit
==============================================================================
*/

forward OnPlayerLevelUp(playerid);
forward OnEmergencyHospitalRespawn(playerid, session);
new gHospitalTimer[MAX_PLAYERS] = {-1, ...};

stock ShowStatsDialog(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) {
		SendErr(playerid, "Anda belum login.");
		return 0;
	}

	new name[MAX_PLAYER_NAME], fac[32], rank[32], job[32], buf[700];
	GetPlayerName(playerid, name, sizeof(name));
	GetFactionName(gPlayerData[playerid][pFaction], fac, sizeof(fac));
	GetFactionRankName(gPlayerData[playerid][pFaction], gPlayerData[playerid][pFactionRank], rank, sizeof(rank));
	GetJobName(gPlayerData[playerid][pJob], job, sizeof(job));

	new phoneStr[24];
	if (gPlayerData[playerid][pPhone] > 0) format(phoneStr, sizeof(phoneStr), "%d", gPlayerData[playerid][pPhone]);
	else format(phoneStr, sizeof(phoneStr), "Tidak Ada");

	format(buf, sizeof(buf),
		"{33CCFF}[ BIODATA KARAKTER ]\n\
{FFFFFF}Nama Lengkap:\t%s (ID: %d)\n\
Level / EXP:\tLevel %d (%d / %d EXP)\n\
Nomor Ponsel:\t%s\n\
Waktu Bermain:\t%d Jam\n\n\
{33CCFF}[ KEUANGAN & SALDO ]\n\
{FFFFFF}Uang Tunai:\t{2ECC71}$%d\n\
{FFFFFF}Tabungan Bank:\t{3498DB}$%d\n\n\
{33CCFF}[ KONDISI SURVIVAL ]\n\
{FFFFFF}Tingkat Lapar:\t%.0f / 100\n\
Tingkat Haus:\t%.0f / 100\n\n\
{33CCFF}[ KARIR & ORGANISASI ]\n\
{FFFFFF}Faksi:\t%s (%s)\n\
Pekerjaan:\t%s",
		name, playerid,
		gPlayerData[playerid][pLevel], gPlayerData[playerid][pExp],
		EXP_PER_LEVEL * gPlayerData[playerid][pLevel],
		phoneStr, gPlayerData[playerid][pPlayHours],
		gPlayerData[playerid][pMoney], gPlayerData[playerid][pBank],
		gPlayerData[playerid][pHunger], gPlayerData[playerid][pThirst],
		fac, rank, job);

	RP_ShowPlayerDialog(playerid, DIALOG_STATS, DIALOG_STYLE_MSGBOX,
		"STATISTIK KARAKTER - NUSANTARA ROLEPLAY", buf, "Tutup", "");
	return 1;
}

stock GivePlayerExp(playerid, amount, const reason[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (amount <= 0) return 0;
	gPlayerData[playerid][pExp] += amount;

	while (gPlayerData[playerid][pExp] >= EXP_PER_LEVEL * gPlayerData[playerid][pLevel]) {
		gPlayerData[playerid][pExp] -= EXP_PER_LEVEL * gPlayerData[playerid][pLevel];
		gPlayerData[playerid][pLevel]++;
		OnPlayerLevelUp(playerid);
	}

	if (reason[0] != '\0') {
		SendRP(playerid, COLOR_SUCCESS, "+%d EXP (%s)", amount, reason);
	}
	UpdatePlayerHUD(playerid);
	return 1;
}

public OnPlayerLevelUp(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;
	SendRP(playerid, COLOR_PAYDAY, "[LEVEL UP] Selamat! Karakter Anda naik ke Level %d.", gPlayerData[playerid][pLevel]);
	PlayerPlaySound(playerid, 1058, 0.0, 0.0, 0.0);
	return 1;
}

// ---------------------------------------------------------------------------
// Loop Status Lapar / Haus / Darurat
// ---------------------------------------------------------------------------
stock UpdateHungerThirst(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn] || !gPlayerData[playerid][pSpawned]) return 0;

	// Pengurangan wajar
	gPlayerData[playerid][pHunger] -= HUNGER_DECAY;
	gPlayerData[playerid][pThirst] -= THIRST_DECAY;

	if (gPlayerData[playerid][pHunger] < 0.0) gPlayerData[playerid][pHunger] = 0.0;
	if (gPlayerData[playerid][pThirst] < 0.0) gPlayerData[playerid][pThirst] = 0.0;

	// Jika lapar atau haus 0, kurangi HP secara bertahap (setiap 15 detik)
	new Float:hp;
	GetPlayerHealth(playerid, hp);
	if ((gPlayerData[playerid][pHunger] <= 0.0 || gPlayerData[playerid][pThirst] <= 0.0) && (gPlayerData[playerid][pPaycheckTimer] % 15 == 0)) {
		if (hp > 10.0) {
			SetPlayerHealth(playerid, hp - 2.0);
			SendClientMessage(playerid, COLOR_ERROR, "[PERINGATAN] Anda mengalami dehidrasi/kelaparan berat! Segera makan atau minum (/use /inv).");
		} else if (hp <= 10.0 && !gPlayerData[playerid][pInjured]) {
			// Masuk ke fase pingsan kelaparan
			ProcessPlayerKnockout(playerid, "dehidrasi / kelaparan");
		}
	}

	// Update HUD periodik
	if (gPlayerData[playerid][pPaycheckTimer] % 5 == 0) {
		UpdatePlayerHUD(playerid);
	}
	return 1;
}

stock ProcessPlayerKnockout(playerid, const reason[])
{
	if (gPlayerData[playerid][pInjured]) return 0;
	gPlayerData[playerid][pInjured] = true;
	gPlayerData[playerid][pInjuredTime] = 90; // 90 detik countdown darurat

	if (gJobActive[playerid]) ProcessJobQuit(playerid);
	if (IsPlayerInAnyVehicle(playerid)) {
		RemovePlayerFromVehicle(playerid);
	}
	if (gHoldingTrashBag[playerid]) {
		gHoldingTrashBag[playerid] = false;
		RemovePlayerAttachedObject(playerid, 1);
	}
	StopProgressBar(playerid, false);
	HidePlayerDialog(playerid);
	Dialog_Close(playerid);

	SetPlayerHealth(playerid, 100.0); // Tahan HP agar tidak langsung mati engine GTA
	TogglePlayerControllable(playerid, false);
	ApplyAnimation(playerid, "CRACK", "crckdeth2", 4.1, true, false, false, false, 0);

	SendErr(playerid, "[KONDISI KRITIS] Anda pingsan / sekarat akibat %s.", reason);
	SendInfo(playerid, "Tunggu tim medis SAMD memberikan pertolongan (/revive) atau tunggu 90 detik menuju UGD Rumah Sakit.");

	new msg[128];
	format(msg, sizeof(msg), "* %s ambruk ke tanah dan tidak sadarkan diri.", gPlayerData[playerid][pName]);
	SendNearbyPlayer(playerid, COLOR_RP_ME, msg, RANGE_ME);

	// Timer UGD Rumah Sakit. Simpan handle agar bisa dibatalkan (revive / mati
	// ulang / disconnect) sehingga tidak ada dua timer yang masing-masing
	// menagih $50 pada pemain yang sama.
	if (gHospitalTimer[playerid] != -1) KillTimer(gHospitalTimer[playerid]);
	gHospitalTimer[playerid] = SetTimerEx("OnEmergencyHospitalRespawn", 90000, false, "ii", playerid, gPlayerSession[playerid]);
	return 1;
}

public OnEmergencyHospitalRespawn(playerid, session)
{
	gHospitalTimer[playerid] = -1;
	// [SECURITY] Cek session token + kondisi pingsan masih aktif. Mencegah
	// tagihan ganda / respawn nyasar ke pemain baru di slot playerid sama.
	if (!IsPlayerConnected(playerid) || gPlayerSession[playerid] != session) return 0;
	if (!gPlayerData[playerid][pInjured]) return 0;

	gPlayerData[playerid][pInjured] = false;
	gPlayerData[playerid][pHunger] = 80.0;
	gPlayerData[playerid][pThirst] = 80.0;
	gPlayerData[playerid][pDeaths]++;

	// Biaya rawat inap $50
	TakePlayerMoneyEx(playerid, 50, "biaya perawatan rumah sakit");

	ClearAnimations(playerid);
	AC_SetPlayerPos(playerid, HOSPITAL_X, HOSPITAL_Y, HOSPITAL_Z);
	SetPlayerFacingAngle(playerid, HOSPITAL_A);
	SetPlayerInterior(playerid, 0);
	SetPlayerVirtualWorld(playerid, 0);
	SetPlayerHealth(playerid, 100.0);
	TogglePlayerControllable(playerid, true);

	SendInfo(playerid, "[RUMAH SAKIT] Anda telah dirawat oleh tim medis UGD All Saints Hospital (Biaya: $50).");
	return 1;
}

// Batalkan timer UGD bila pemain di-revive lebih awal oleh staf medis/admin
// atau pulih dengan cara lain. Tanpa ini timer 90 detik tetap berjalan dan
// menagih $50 / memindahkan pemain setelah dia sudah sadar.
stock ClearHospitalTimer(playerid)
{
	if (gHospitalTimer[playerid] != -1) {
		KillTimer(gHospitalTimer[playerid]);
		gHospitalTimer[playerid] = -1;
	}
	return 1;
}
