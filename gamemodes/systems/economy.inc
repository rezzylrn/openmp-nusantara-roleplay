#if defined _INC_rp_economy
	#endinput
#endif
#define _INC_rp_economy

/*
==============================================================================
  NUSANTARA ROLEPLAY - economy.inc
  Cash, bank, ATM, /pay, transfer, gaji (payday) tiap 60 menit
==============================================================================
*/

// ---------------------------------------------------------------------------
// Helpers uang & bank
// ---------------------------------------------------------------------------
stock GetPlayerMoneyEx(playerid)
{
	return gPlayerData[playerid][pMoney];
}

stock GetPlayerBankEx(playerid)
{
	return gPlayerData[playerid][pBank];
}

stock SetPlayerMoneyEx(playerid, amount)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (amount < 0) amount = 0;
	gPlayerData[playerid][pMoney] = amount;
	ResetPlayerMoney(playerid);
	GivePlayerMoney(playerid, amount);
	UpdatePlayerHUD(playerid);
	return 1;
}

stock SetPlayerBankEx(playerid, amount)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (amount < 0) amount = 0;
	gPlayerData[playerid][pBank] = amount;
	UpdatePlayerHUD(playerid);
	return 1;
}

stock GivePlayerMoneyEx(playerid, amount, const reason[] = "")
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (amount <= 0) return 0;
	gPlayerData[playerid][pMoney] += amount;
	ResetPlayerMoney(playerid);
	GivePlayerMoney(playerid, gPlayerData[playerid][pMoney]);
	if (reason[0] != '\0') SendRP(playerid, COLOR_SUCCESS, "Cash +$%d (%s)", amount, reason);
	UpdatePlayerHUD(playerid);
	return 1;
}

stock TakePlayerMoneyEx(playerid, amount, const reason[] = "")
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (amount <= 0) return 0;
	if (gPlayerData[playerid][pMoney] < amount) {
		SendErr(playerid, "Cash tidak mencukupi.");
		return 0;
	}
	gPlayerData[playerid][pMoney] -= amount;
	ResetPlayerMoney(playerid);
	GivePlayerMoney(playerid, gPlayerData[playerid][pMoney]);
	if (reason[0] != '\0') SendRP(playerid, COLOR_ERROR, "Cash -$%d (%s)", amount, reason);
	UpdatePlayerHUD(playerid);
	return 1;
}

stock GivePlayerBankEx(playerid, amount, const reason[] = "")
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (amount <= 0) return 0;
	gPlayerData[playerid][pBank] += amount;
	if (gPlayerData[playerid][pBank] < 0) gPlayerData[playerid][pBank] = 0;
	if (reason[0] != '\0') SendRP(playerid, COLOR_SUCCESS, "Bank +$%d (%s)", amount, reason);
	UpdatePlayerHUD(playerid);
	return 1;
}

stock TakePlayerBankEx(playerid, amount, const reason[] = "")
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (amount <= 0) return 0;
	if (gPlayerData[playerid][pBank] < amount) {
		SendErr(playerid, "Saldo bank tidak mencukupi.");
		return 0;
	}
	gPlayerData[playerid][pBank] -= amount;
	if (reason[0] != '\0') SendRP(playerid, COLOR_ERROR, "Bank -$%d (%s)", amount, reason);
	UpdatePlayerHUD(playerid);
	return 1;
}

// ---------------------------------------------------------------------------
// /pay - transfer cash ke pemain di dekat
// ---------------------------------------------------------------------------
stock ProcessPay(playerid, const targetname[], amount)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (amount < 1) {
		SendErr(playerid, "Jumlah transfer minimal $1.");
		return 0;
	}
	if (gPlayerData[playerid][pMoney] < amount) {
		SendErr(playerid, "Cash tidak mencukupi.");
		return 0;
	}

	new targetid = FindPlayerByName(targetname);
	if (targetid == -1) {
		SendErr(playerid, "Pemain \"%s\" tidak ditemukan.", targetname);
		return 0;
	}
	if (targetid == playerid) {
		SendErr(playerid, "Tidak bisa transfer ke diri sendiri.");
		return 0;
	}
	if (!gPlayerData[targetid][pLoggedIn]) {
		SendErr(playerid, "Target belum login.");
		return 0;
	}

	new Float:x, Float:y, Float:z, Float:tx, Float:ty, Float:tz;
	GetPlayerPos(playerid, x, y, z);
	GetPlayerPos(targetid, tx, ty, tz);
	new Float:dist = VectorSize(x - tx, y - ty, z - tz);
	if (dist > RANGE_PAY) {
		SendErr(playerid, "Target terlalu jauh (maks %d meter).", floatround(RANGE_PAY));
		return 0;
	}

	if (!TakePlayerMoneyEx(playerid, amount, "kepada target")) return 0;
	GivePlayerMoneyEx(targetid, amount, "dari pemain lain");
	SendRP(targetid, COLOR_SUCCESS, "Anda menerima $%d dari %s.", amount, gPlayerData[playerid][pName]);
	SendRP(playerid, COLOR_SUCCESS, "Anda memberikan $%d kepada %s.", amount, gPlayerData[targetid][pName]);
	return 1;
}

// ---------------------------------------------------------------------------
// Dialog bank & ATM
// ---------------------------------------------------------------------------
stock ShowBankDialog(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) {
		SendErr(playerid, "Anda belum login.");
		return 0;
	}
	if (!IsPlayerNearPoint(playerid, BANK_X, BANK_Y, BANK_Z, RANGE_BANK)) {
		SendErr(playerid, "Anda harus berada di depan bank.");
		return 0;
	}
	if (gPlayerData[playerid][pLanguage] == LANG_EN) {
		RP_ShowPlayerDialog(playerid, DIALOG_BANK_MENU, DIALOG_STYLE_LIST,
			"BANK OF NUSANTARA - BANKING SERVICES",
			"Deposit Funds\nWithdraw Funds\nWire Transfer\nBalance Inquiry",
			"Select", "Cancel");
	} else {
		RP_ShowPlayerDialog(playerid, DIALOG_BANK_MENU, DIALOG_STYLE_LIST,
			"LOKAL BANK - NUSANTARA RP",
			"Simpan dana (Deposit)\nTarik dana (Withdraw)\nTransfer rekening\nCek saldo",
			"Pilih", "Batal");
	}
	return 1;
}

stock ShowATMDialog(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) {
		SendErr(playerid, "Anda belum login.");
		return 0;
	}
	if (!IsPlayerNearPoint(playerid, ATM_X, ATM_Y, ATM_Z, RANGE_ATM) && !IsPlayerNearAnyATM(playerid)) {
		if (gPlayerData[playerid][pLanguage] == LANG_EN) SendErr(playerid, "You must be near an ATM.");
		else SendErr(playerid, "Anda harus berada di dekat ATM.");
		return 0;
	}
	if (gPlayerData[playerid][pLanguage] == LANG_EN) {
		RP_ShowPlayerDialog(playerid, DIALOG_ATM_MENU, DIALOG_STYLE_LIST,
			"ATM NUSANTARA - AUTOMATED TELLER MACHINE",
			"Withdraw Cash ($1 - $1,000,000)\nDeposit Cash\nBalance Inquiry",
			"Select", "Cancel");
	} else {
		RP_ShowPlayerDialog(playerid, DIALOG_ATM_MENU, DIALOG_STYLE_LIST,
			"ATM - NUSANTARA RP",
			"Tarik tunai ($1 - $1.000.000)\nSimpan tunai (Deposit)\nCek saldo",
			"Pilih", "Batal");
	}
	return 1;
}

// Handler dialog bank
stock OnBankDialogResponse(playerid, listitem)
{
	if (listitem == 0) {
		RP_ShowPlayerDialog(playerid, DIALOG_BANK_DEPOSIT, DIALOG_STYLE_INPUT,
			"BANK - SIMPAN DANA", "Masukkan jumlah cash yang ingin disimpan:",
			"Simpan", "Batal");
		return 1;
	}
	if (listitem == 1) {
		RP_ShowPlayerDialog(playerid, DIALOG_BANK_WITHDRAW, DIALOG_STYLE_INPUT,
			"BANK - TARIK DANA", "Masukkan jumlah yang ingin ditarik:",
			"Tarik", "Batal");
		return 1;
	}
	if (listitem == 2) {
		RP_ShowPlayerDialog(playerid, DIALOG_BANK_TRANSFER, DIALOG_STYLE_INPUT,
			"BANK - TRANSFER", "Masukkan Nama Target dan Jumlah (format: Nama Jumlah):",
			"Lanjut", "Batal");
		return 1;
	}
	if (listitem == 3) {
		SendRP(playerid, COLOR_INFO, "Saldo rekening Bank Anda: $%d.", gPlayerData[playerid][pBank]);
		return 1;
	}
	return 1;
}

stock OnBankDepositResponse(playerid, const inputtext[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return 0;

	// Guard anti dialog-spoofing: pemain HARUS benar-benar berada di depan bank
	if (!IsPlayerNearPoint(playerid, BANK_X, BANK_Y, BANK_Z, RANGE_BANK)) {
		SendErr(playerid, "Anda harus berada di depan Bank Nusantara untuk bertransaksi.");
		return 0;
	}
	new amount = strval(inputtext);
	if (amount < 1 || amount > 50000000) { SendErr(playerid, "Jumlah setoran tidak valid ($1 - $50.000.000)."); return 0; }
	if (gPlayerData[playerid][pMoney] < amount) { SendErr(playerid, "Uang tunai tidak mencukupi."); return 0; }

	if (!TakePlayerMoneyEx(playerid, amount, "setoran bank")) {
		SendErr(playerid, "Gagal memproses setoran uang tunai.");
		return 0;
	}
	GivePlayerBankEx(playerid, amount, "setoran bank");
	new depmsg[64]; format(depmsg, sizeof(depmsg), "Menyimpan $%d ke bank.", amount);
	ShowPlayerAlert(playerid, depmsg);
	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
	return 1;
}

stock OnBankWithdrawResponse(playerid, const inputtext[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return 0;

	if (!IsPlayerNearPoint(playerid, BANK_X, BANK_Y, BANK_Z, RANGE_BANK)) {
		SendErr(playerid, "Anda harus berada di depan Bank Nusantara untuk bertransaksi.");
		return 0;
	}
	new amount = strval(inputtext);
	if (amount < 1 || amount > 50000000) { SendErr(playerid, "Jumlah penarikan tidak valid ($1 - $50.000.000)."); return 0; }
	if (gPlayerData[playerid][pBank] < amount) { SendErr(playerid, "Saldo bank tidak mencukupi."); return 0; }

	if (!TakePlayerBankEx(playerid, amount, "penarikan bank")) {
		SendErr(playerid, "Gagal memproses penarikan bank.");
		return 0;
	}
	GivePlayerMoneyEx(playerid, amount, "penarikan bank");
	new wmsg[64]; format(wmsg, sizeof(wmsg), "Menarik $%d dari bank.", amount);
	ShowPlayerAlert(playerid, wmsg);
	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
	return 1;
}

stock OnBankTransferResponse(playerid, const inputtext[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return 0;

	if (!IsPlayerNearPoint(playerid, BANK_X, BANK_Y, BANK_Z, RANGE_BANK)) {
		SendErr(playerid, "Anda harus berada di depan Bank Nusantara untuk bertransaksi.");
		return 0;
	}
	new targetid, amount;
	if (sscanf(inputtext, "ud", targetid, amount)) {
		SendSyntax(playerid, "[ID / Nama Pemain] [Jumlah Uang]");
		RP_ShowPlayerDialog(playerid, DIALOG_BANK_TRANSFER, DIALOG_STYLE_INPUT,
			"TRANSFER DANA", "Format: [ID Pemain / Nama] [Jumlah Uang]\nContoh: 0 5000", "Kirim", "Batal");
		return 0;
	}

	if (amount < 1 || amount > 10000000) {
		SendErr(playerid, "Nominal transfer minimal $1 dan maksimal $10.000.000.");
		return 0;
	}

	if (targetid == INVALID_PLAYER_ID || !IsPlayerConnected(targetid) || !gPlayerData[targetid][pLoggedIn]) {
		SendErr(playerid, "Pemain penerima tidak aktif atau belum login.");
		return 0;
	}

	if (targetid == playerid) {
		SendErr(playerid, "Tidak bisa mentransfer dana ke rekening sendiri.");
		return 0;
	}

	if (gPlayerData[playerid][pBank] < amount + TRANSFER_FEE) {
		SendErr(playerid, "Saldo bank tidak cukup (dibutuhkan $%d termasuk biaya admin $%d).", amount + TRANSFER_FEE, TRANSFER_FEE);
		return 0;
	}

	if (!TakePlayerBankEx(playerid, amount + TRANSFER_FEE, "transfer bank")) {
		SendErr(playerid, "Gagal memproses transfer bank.");
		return 0;
	}
	GivePlayerBankEx(targetid, amount, "terima transfer");

	SendRP(targetid, COLOR_SUCCESS, "[BANK] Anda menerima transfer dana $%d dari %s.", amount, gPlayerData[playerid][pName]);
	SendRP(playerid, COLOR_SUCCESS, "[BANK] Transfer $%d ke %s berhasil (Biaya admin: $%d).", amount, gPlayerData[targetid][pName], TRANSFER_FEE);

	new tfmsg[64]; format(tfmsg, sizeof(tfmsg), "Transfer $%d berhasil.", amount);
	ShowPlayerAlert(playerid, tfmsg);

	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
	PlayerPlaySound(targetid, 1052, 0.0, 0.0, 0.0);
	return 1;
}

// Handler dialog ATM
stock OnATMMenuResponse(playerid, listitem)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return 0;

	if (listitem == 0) {
		RP_ShowPlayerDialog(playerid, DIALOG_ATM_WITHDRAW, DIALOG_STYLE_INPUT,
			"ATM - TARIK TUNAI", "Masukkan jumlah penarikan tunai ATM ($1 - $1.000.000):",
			"Tarik", "Batal");
		return 1;
	}
	if (listitem == 1) {
		RP_ShowPlayerDialog(playerid, DIALOG_ATM_DEPOSIT, DIALOG_STYLE_INPUT,
			"ATM - SIMPAN TUNAI", "Masukkan jumlah setor tunai ATM ($1 - $1.000.000):",
			"Setor", "Batal");
		return 1;
	}
	if (listitem == 2) {
		SendRP(playerid, COLOR_INFO, "Saldo rekening Bank Anda: $%d.", gPlayerData[playerid][pBank]);
		return 1;
	}
	return 1;
}

stock OnATMWithdrawResponse(playerid, const inputtext[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return 0;

	if (!IsPlayerNearPoint(playerid, ATM_X, ATM_Y, ATM_Z, RANGE_ATM) && !IsPlayerNearAnyATM(playerid)) {
		SendErr(playerid, "Anda harus berada di depan mesin ATM untuk bertransaksi.");
		return 0;
	}
	new amount = strval(inputtext);
	if (amount < 1 || amount > 1000000) { SendErr(playerid, "Nominal penarikan $1 - $1.000.000."); return 0; }
	if (gPlayerData[playerid][pBank] < amount) { SendErr(playerid, "Saldo bank tidak cukup."); return 0; }

	if (!TakePlayerBankEx(playerid, amount, "tarik ATM")) {
		SendErr(playerid, "Gagal memproses penarikan saldo ATM.");
		return 0;
	}
	GivePlayerMoneyEx(playerid, amount, "tarik tunai ATM");
	new awmsg[64]; format(awmsg, sizeof(awmsg), "ATM: $%d ditarik tunai.", amount);
	ShowPlayerAlert(playerid, awmsg);
	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
	return 1;
}

stock OnATMDepositResponse(playerid, const inputtext[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return 0;

	if (!IsPlayerNearPoint(playerid, ATM_X, ATM_Y, ATM_Z, RANGE_ATM) && !IsPlayerNearAnyATM(playerid)) {
		SendErr(playerid, "Anda harus berada di depan mesin ATM untuk bertransaksi.");
		return 0;
	}
	new amount = strval(inputtext);
	if (amount < 1 || amount > 1000000) { SendErr(playerid, "Nominal setoran $1 - $1.000.000."); return 0; }
	if (gPlayerData[playerid][pMoney] < amount) { SendErr(playerid, "Cash tidak cukup."); return 0; }

	if (!TakePlayerMoneyEx(playerid, amount, "setor ATM")) {
		SendErr(playerid, "Gagal memproses setoran tunai ATM.");
		return 0;
	}
	GivePlayerBankEx(playerid, amount, "setor tunai ATM");
	new admsg[64]; format(admsg, sizeof(admsg), "ATM: $%d disimpan ke bank.", amount);
	ShowPlayerAlert(playerid, admsg);
	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
	return 1;
}

// ---------------------------------------------------------------------------
// Gaji (Payday) - tiap 60 menit
// ---------------------------------------------------------------------------
stock ProcessPaycheck(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn] || !gPlayerData[playerid][pSpawned]) return 0;

	new amount = CalculatePaycheck(playerid);
	if (amount <= 0) return 0;

	new tax = (amount * gCurrentTax) / 100;
	new net = amount - tax;
	if (net < 0) net = 0;

	GivePlayerBankEx(playerid, net, "gaji pokok");
	new pmsg[64]; format(pmsg, sizeof(pmsg), "Gaji masuk bank: $%d.", net);
	ShowPlayerAlert(playerid, pmsg);
	SendRP(playerid, COLOR_PAYDAY, "[PAYDAY] Gaji $%d masuk ke rekening bank Anda (Potongan pajak %d%%: $%d).", net, gCurrentTax, tax);
	GivePlayerExp(playerid, EXP_PAYDAY, "payday");

	gPlayerData[playerid][pPlayHours] += 1;
	gPlayerData[playerid][pPaycheckTimer] = PAYCHECK_INTERVAL;
	gPlayerData[playerid][pPaycheckAmount] = 0;
	return 1;
}

stock CalculatePaycheck(playerid)
{
	new amount = 100; // gaji pokok sipil
	amount += gPlayerData[playerid][pLevel] * 25;

	if (gPlayerData[playerid][pDuty]) {
		amount += FactionSalaryBase(gPlayerData[playerid][pFaction]);
		amount += gPlayerData[playerid][pFactionRank] * SALARY_RANK_BONUS;
	}

	if (gJobActive[playerid]) {
		if (gJobType[playerid] == JOB_COURIER) amount += 150;
		else if (gJobType[playerid] == JOB_SWEEPER) amount += 100;
	}
	return amount;
}

stock FactionSalaryBase(factionid)
{
	switch (factionid) {
		case FACTION_POLICE: return SALARY_POLICE_BASE;
		case FACTION_MEDIC: return SALARY_MEDIC_BASE;
		case FACTION_GOV: return SALARY_GOV_BASE;
		case FACTION_GANG: return SALARY_GANG_BASE;
	}
	return 0;
}

stock TickPaycheck(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn] || !gPlayerData[playerid][pSpawned]) return 0;
	gPlayerData[playerid][pPaycheckTimer]--;
	if (gPlayerData[playerid][pPaycheckTimer] <= 0) {
		ProcessPaycheck(playerid);
	}
	return 1;
}
