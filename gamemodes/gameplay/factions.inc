#if defined _INC_rp_factions
	#endinput
#endif
#define _INC_rp_factions

/*
==============================================================================
  NUSANTARA ROLEPLAY - factions.inc
  Sistem Faksi Resmi: SAPD (Polisi), SAMD (Medis), SAGOV (Pemerintah), Gang
  Perintah Khusus: /duty, /cuff, /drag, /frisk, /ticket, /arrest, /m, /d,
                   /heal, /revive, /settax, /gov
==============================================================================
*/

stock IsPlayerInFaction(playerid, factionid)
{
	return (gPlayerData[playerid][pFaction] == factionid);
}

stock IsPlayerOnDuty(playerid)
{
	return gPlayerData[playerid][pDuty];
}

stock SetPlayerFaction(playerid, factionid, rank)
{
	gPlayerData[playerid][pFaction] = factionid;
	gPlayerData[playerid][pFactionRank] = rank;
	return 1;
}

stock RemovePlayerFaction(playerid)
{
	gPlayerData[playerid][pFaction] = FACTION_NONE;
	gPlayerData[playerid][pFactionRank] = 0;
	gPlayerData[playerid][pDuty] = false;
	ResetPlayerWeapons(playerid);
	return 1;
}

stock ProcessFactionDuty(playerid, bool:duty)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pFaction] == FACTION_NONE) {
		SendErr(playerid, "Anda tidak terdaftar dalam faksi apapun.");
		return 0;
	}

	gPlayerData[playerid][pDuty] = duty;
	new facname[32], rankname[32];
	GetFactionName(gPlayerData[playerid][pFaction], facname, sizeof(facname));
	GetFactionRankName(gPlayerData[playerid][pFaction], gPlayerData[playerid][pFactionRank], rankname, sizeof(rankname));

	if (duty) {
		SendSucc(playerid, "Anda sekarang ON DUTY sebagai %s (%s).", rankname, facname);
		Faction_ApplyWeapons(playerid);
		SetPlayerArmour(playerid, 100.0);
	} else {
		SendInfo(playerid, "Anda sekarang OFF DUTY. Semua persenjataan dinas disimpan.");
		ResetPlayerWeapons(playerid);
		SetPlayerArmour(playerid, 0.0);
	}
	return 1;
}

stock Faction_ApplyWeapons(playerid)
{
	new fac = gPlayerData[playerid][pFaction];
	ResetPlayerWeapons(playerid);
	if (!gPlayerData[playerid][pDuty]) return 1;

	switch (fac) {
		case FACTION_POLICE: {
			AC_GiveWeapon(playerid, WEAPON_NITESTICK, 1);
			AC_GiveWeapon(playerid, WEAPON_DEAGLE, 60);
			AC_GiveWeapon(playerid, WEAPON_SHOTGUN, 50);
			AC_GiveWeapon(playerid, WEAPON_MP5, 120);
		}
		case FACTION_MEDIC: {
			AC_GiveWeapon(playerid, WEAPON_CAMERA, 50);
		}
		case FACTION_GOV: {
			AC_GiveWeapon(playerid, WEAPON_DEAGLE, 30);
		}
		case FACTION_GANG: {
			AC_GiveWeapon(playerid, WEAPON_BRASSKNUCKLE, 1);
			AC_GiveWeapon(playerid, WEAPON_COLT45, 60);
			AC_GiveWeapon(playerid, WEAPON_UZI, 150);
		}
	}
	return 1;
}

// ---------------------------------------------------------------------------
// OPERASI KEPOLISIAN (SAPD)
// ---------------------------------------------------------------------------
stock ProcessCuff(playerid, targetid)
{
	if (!gPlayerData[playerid][pLoggedIn] || !gPlayerData[playerid][pDuty] || gPlayerData[playerid][pFaction] != FACTION_POLICE) {
		SendErr(playerid, "Perintah ini khusus petugas Kepolisian (SAPD) yang sedang bertugas.");
		return 0;
	}
	if (targetid == -1 || !IsPlayerConnected(targetid) || !gPlayerData[targetid][pLoggedIn]) {
		SendErr(playerid, "Target tidak valid atau offline.");
		return 0;
	}
	if (targetid == playerid) {
		SendErr(playerid, "Anda tidak dapat memborgol diri sendiri.");
		return 0;
	}
	if (gPlayerData[targetid][pCuffed]) {
		SendErr(playerid, "Target sudah dalam keadaan terborgol.");
		return 0;
	}
	if (!IsPlayerNear(playerid, targetid, 3.5)) {
		SendErr(playerid, "Anda harus berada di dekat target untuk memasang borgol.");
		return 0;
	}

	gPlayerData[targetid][pCuffed] = true;
	TogglePlayerControllable(targetid, false);
	SetPlayerSpecialAction(targetid, SPECIAL_ACTION_CUFFED);

	new msg[128];
	format(msg, sizeof(msg), "* %s mengeluarkan borgol baja dan mengunci pergelangan tangan %s.",
		gPlayerData[playerid][pName], gPlayerData[targetid][pName]);
	SendNearbyPlayer(playerid, COLOR_RP_ME, msg, RANGE_ME);

	SendSucc(playerid, "Anda telah memborgol %s.", gPlayerData[targetid][pName]);
	SendErr(targetid, "Anda telah diborgol oleh Petugas %s.", gPlayerData[playerid][pName]);
	return 1;
}

stock ProcessUncuff(playerid, targetid)
{
	if (!gPlayerData[playerid][pLoggedIn] || !gPlayerData[playerid][pDuty] || gPlayerData[playerid][pFaction] != FACTION_POLICE) {
		SendErr(playerid, "Perintah ini khusus petugas Kepolisian (SAPD) yang sedang bertugas.");
		return 0;
	}
	if (targetid == -1 || !IsPlayerConnected(targetid) || !gPlayerData[targetid][pLoggedIn]) {
		SendErr(playerid, "Target tidak valid atau offline.");
		return 0;
	}
	if (!gPlayerData[targetid][pCuffed]) {
		SendErr(playerid, "Target tidak dalam keadaan terborgol.");
		return 0;
	}

	gPlayerData[targetid][pCuffed] = false;
	TogglePlayerControllable(targetid, true);
	SetPlayerSpecialAction(targetid, SPECIAL_ACTION_NONE);

	new msg[128];
	format(msg, sizeof(msg), "* %s memasukkan kunci borgol dan melepaskan borgol dari %s.",
		gPlayerData[playerid][pName], gPlayerData[targetid][pName]);
	SendNearbyPlayer(playerid, COLOR_RP_ME, msg, RANGE_ME);

	SendSucc(playerid, "Anda telah melepaskan borgol %s.", gPlayerData[targetid][pName]);
	SendSucc(targetid, "Borgol Anda telah dilepas oleh Petugas %s.", gPlayerData[playerid][pName]);
	return 1;
}

stock ProcessFrisk(playerid, targetid)
{
	if (!gPlayerData[playerid][pLoggedIn] || !gPlayerData[playerid][pDuty] || gPlayerData[playerid][pFaction] != FACTION_POLICE) {
		SendErr(playerid, "Perintah ini khusus petugas Kepolisian (SAPD) yang sedang bertugas.");
		return 0;
	}
	if (targetid == -1 || !IsPlayerConnected(targetid) || !gPlayerData[targetid][pLoggedIn]) {
		SendErr(playerid, "Target tidak valid atau offline.");
		return 0;
	}
	if (!IsPlayerNear(playerid, targetid, 3.5)) {
		SendErr(playerid, "Anda harus berada di dekat target untuk menggeledah.");
		return 0;
	}

	new wName[32], list[600], itemName[32];
	new WEAPON:wep = GetPlayerWeapon(targetid);
	GetWeaponName(wep, wName, sizeof(wName));

	format(list, sizeof(list), "Uang Tunai: $%d\nSenjata di Tangan: %s\n\nBarang Bawaan (Inventaris):\n",
		gPlayerData[targetid][pMoney], wName);

	new itemCount = 0;
	for (new s = 0; s < MAX_INVENTORY_SLOTS; s++) {
		if (gPlayerItems[targetid][s][itType] != ITEM_NONE) {
			GetItemName(gPlayerItems[targetid][s][itType], itemName, sizeof(itemName));
			format(list, sizeof(list), "%s- %s (%d buah)\n", list, itemName, gPlayerItems[targetid][s][itAmount]);
			itemCount++;
		}
	}
	if (itemCount == 0) {
		format(list, sizeof(list), "%s(Tidak ada barang di kantong)\n", list);
	}

	new header[64];
	format(header, sizeof(header), "HASIL PENGGELEDAHAN - %s", gPlayerData[targetid][pName]);
	ShowPlayerDialog(playerid, DIALOG_SAPD_ACTIONS, DIALOG_STYLE_MSGBOX, header, list, "Tutup", "");

	new msg[128];
	format(msg, sizeof(msg), "* %s memeriksa seluruh pakaian dan kantong %s dengan teliti.",
		gPlayerData[playerid][pName], gPlayerData[targetid][pName]);
	SendNearbyPlayer(playerid, COLOR_RP_ME, msg, RANGE_ME);
	return 1;
}

stock ProcessTicket(playerid, targetid, amount, const reason[])
{
	if (!gPlayerData[playerid][pLoggedIn] || !gPlayerData[playerid][pDuty] || gPlayerData[playerid][pFaction] != FACTION_POLICE) {
		SendErr(playerid, "Perintah ini khusus petugas Kepolisian (SAPD) yang sedang bertugas.");
		return 0;
	}
	if (targetid == -1 || !IsPlayerConnected(targetid) || !gPlayerData[targetid][pLoggedIn]) {
		SendErr(playerid, "Target tidak valid atau offline.");
		return 0;
	}
	if (amount < 5 || amount > 1000) {
		SendErr(playerid, "Nominal tilang harus antara $5 - $1,000.");
		return 0;
	}
	if (gPlayerData[targetid][pMoney] < amount) {
		SendErr(playerid, "Target tidak memiliki cukup uang tunai untuk membayar tilang.");
		return 0;
	}

	TakePlayerMoneyEx(targetid, amount, "denda tilang");
	SendRP(playerid, COLOR_SUCCESS, "SUKSES: Anda menerbitkan surat tilang kepada %s sejumlah $%d. Alasan: %s",
		gPlayerData[targetid][pName], amount, reason);
	SendRP(targetid, COLOR_ERROR, "ERROR: Anda ditilang sebesar $%d oleh Petugas %s. Alasan: %s",
		amount, gPlayerData[playerid][pName], reason);

	new msg[128];
	format(msg, sizeof(msg), "* %s menuliskan surat tilang resmi dan menyerahkannya ke %s.",
		gPlayerData[playerid][pName], gPlayerData[targetid][pName]);
	SendNearbyPlayer(playerid, COLOR_RP_ME, msg, RANGE_ME);
	return 1;
}

stock ProcessArrest(playerid, targetid, timeMinutes, fineAmount)
{
	if (!gPlayerData[playerid][pLoggedIn] || !gPlayerData[playerid][pDuty] || gPlayerData[playerid][pFaction] != FACTION_POLICE) {
		SendErr(playerid, "Perintah ini khusus petugas Kepolisian (SAPD) yang sedang bertugas.");
		return 0;
	}
	if (targetid == -1 || !IsPlayerConnected(targetid) || !gPlayerData[targetid][pLoggedIn]) {
		SendErr(playerid, "Target tidak valid.");
		return 0;
	}
	if (!IsPlayerNearPoint(playerid, POLICE_X, POLICE_Y, POLICE_Z, 25.0)) {
		SendErr(playerid, "Proses penahanan hanya dapat dilakukan di Kantor Polisi LSPD.");
		return 0;
	}

	if (timeMinutes < 1) timeMinutes = 1;
	if (timeMinutes > 60) timeMinutes = 60;

	// Masukkan ke sel penjara LSPD
	AC_SetPlayerPos(targetid, POLICE_JAIL_X, POLICE_JAIL_Y, POLICE_JAIL_Z);
	SetPlayerInterior(targetid, POLICE_JAIL_INT);
	SetPlayerVirtualWorld(targetid, 0);

	gPlayerData[targetid][pCuffed] = false;
	TogglePlayerControllable(targetid, true);
	SetPlayerSpecialAction(targetid, SPECIAL_ACTION_NONE);
	ResetPlayerWeapons(targetid);

	if (fineAmount > 0 && gPlayerData[targetid][pMoney] >= fineAmount) {
		TakePlayerMoneyEx(targetid, fineAmount, "denda penahanan");
	}

	SendRP(playerid, COLOR_SUCCESS, "SUKSES: Tersangka %s resmi ditahan di sel penjara selama %d menit.", gPlayerData[targetid][pName], timeMinutes);
	SendRP(targetid, COLOR_ERROR, "ERROR: Anda resmi dijebloskan ke sel penjara selama %d menit oleh Petugas %s.",
		timeMinutes, gPlayerData[playerid][pName]);
	return 1;
}

stock ProcessMegaphone(playerid, const text[])
{
	if (!gPlayerData[playerid][pLoggedIn] || !gPlayerData[playerid][pDuty] || gPlayerData[playerid][pFaction] != FACTION_POLICE) {
		SendErr(playerid, "Perintah ini khusus petugas Kepolisian (SAPD) yang sedang bertugas.");
		return 0;
	}
	if (!IsPlayerInAnyVehicle(playerid)) {
		SendErr(playerid, "Anda harus berada di dalam kendaraan dinas kepolisian.");
		return 0;
	}
	if (text[0] == '\0') {
		SendSyntax(playerid, "/m [pengumuman pengeras suara]");
		return 0;
	}

	new out[180];
	format(out, sizeof(out), "[MEGAPHONE] %s: %s!", gPlayerData[playerid][pName], text);
	SendNearbyPlayer(playerid, COLOR_MEGAPHONE, out, 50.0);
	return 1;
}

// ---------------------------------------------------------------------------
// OPERASI MEDIS (SAMD)
// ---------------------------------------------------------------------------
stock ProcessHeal(playerid, targetid)
{
	if (!gPlayerData[playerid][pLoggedIn] || !gPlayerData[playerid][pDuty] || gPlayerData[playerid][pFaction] != FACTION_MEDIC) {
		SendErr(playerid, "Perintah ini khusus petugas Medis (SAMD) yang sedang bertugas.");
		return 0;
	}
	if (targetid == -1 || !IsPlayerConnected(targetid) || !gPlayerData[targetid][pLoggedIn]) {
		SendErr(playerid, "Pasien tidak valid.");
		return 0;
	}
	if (!IsPlayerNear(playerid, targetid, 3.5)) {
		SendErr(playerid, "Anda harus berada di dekat pasien untuk memberikan perawatan.");
		return 0;
	}

	SetPlayerHealth(targetid, 100.0);
	SendSucc(playerid, "Anda telah memulihkan kesehatan pasien %s hingga 100%%.", gPlayerData[targetid][pName]);
	SendSucc(targetid, "Kondisi fisik Anda telah dipulihkan oleh Paramedis %s.", gPlayerData[playerid][pName]);

	new msg[128];
	format(msg, sizeof(msg), "* %s memberikan pertolongan medis intensif kepada %s.",
		gPlayerData[playerid][pName], gPlayerData[targetid][pName]);
	SendNearbyPlayer(playerid, COLOR_RP_ME, msg, RANGE_ME);
	return 1;
}

stock ProcessRevive(playerid, targetid)
{
	if (!gPlayerData[playerid][pLoggedIn] || !gPlayerData[playerid][pDuty] || gPlayerData[playerid][pFaction] != FACTION_MEDIC) {
		SendErr(playerid, "Perintah ini khusus petugas Medis (SAMD) yang sedang bertugas.");
		return 0;
	}
	if (targetid == -1 || !IsPlayerConnected(targetid) || !gPlayerData[targetid][pLoggedIn]) {
		SendErr(playerid, "Target tidak valid.");
		return 0;
	}
	if (!gPlayerData[targetid][pInjured]) {
		SendErr(playerid, "Target tidak dalam keadaan sekarat / pingsan.");
		return 0;
	}

	gPlayerData[targetid][pInjured] = false;
	ClearAnimations(targetid);
	TogglePlayerControllable(targetid, true);
	SetPlayerHealth(targetid, 50.0);

	SendSucc(playerid, "Anda berhasil menyadarkan %s dari keadaan kritis.", gPlayerData[targetid][pName]);
	SendSucc(targetid, "Anda telah disadarkan dari kondisi kritis oleh Paramedis %s.", gPlayerData[playerid][pName]);

	new msg[128];
	format(msg, sizeof(msg), "* %s menggunakan alat defibrilator dan obat darurat untuk menyadarkan %s.",
		gPlayerData[playerid][pName], gPlayerData[targetid][pName]);
	SendNearbyPlayer(playerid, COLOR_RP_ME, msg, RANGE_ME);
	return 1;
}

// ---------------------------------------------------------------------------
// OPERASI PEMERINTAHAN (SAGOV)
// ---------------------------------------------------------------------------
stock ProcessGovBroadcast(playerid, const text[])
{
	if (!gPlayerData[playerid][pLoggedIn] || !gPlayerData[playerid][pDuty] || gPlayerData[playerid][pFaction] != FACTION_GOV) {
		SendErr(playerid, "Perintah ini khusus pejabat Pemerintah (SAGOV) bertugas.");
		return 0;
	}
	if (text[0] == '\0') {
		SendSyntax(playerid, "/gov [pengumuman resmi pemerintah]");
		return 0;
	}

	new out[200];
	format(out, sizeof(out), "[PENGUMUMAN PEMERINTAH] %s: %s", gPlayerData[playerid][pName], text);
	SendClientMessageToAll(COLOR_GOLD, out);
	return 1;
}

stock ProcessSetTax(playerid, taxRate)
{
	if (!gPlayerData[playerid][pLoggedIn] || !gPlayerData[playerid][pDuty] || gPlayerData[playerid][pFaction] != FACTION_GOV) {
		SendErr(playerid, "Perintah ini khusus pejabat Pemerintah (SAGOV) bertugas.");
		return 0;
	}
	if (taxRate < 0 || taxRate > 25) {
		SendErr(playerid, "Besaran pajak daerah harus antara 0% - 25%.");
		return 0;
	}

	gCurrentTax = taxRate;
	new out[128];
	format(out, sizeof(out), "[SAGOV] Tarif pajak daerah resmi diubah menjadi %d%% oleh %s.", taxRate, gPlayerData[playerid][pName]);
	SendClientMessageToAll(COLOR_GOLD, out);
	return 1;
}

// ---------------------------------------------------------------------------
// SALURAN RADIO DINAS (/d - Department Radio)
// ---------------------------------------------------------------------------
stock ProcessDepartmentRadio(playerid, const text[])
{
	if (!gPlayerData[playerid][pLoggedIn] || !gPlayerData[playerid][pDuty]) {
		SendErr(playerid, "Anda harus sedang bertugas dinas.");
		return 0;
	}
	new fac = gPlayerData[playerid][pFaction];
	if (fac != FACTION_POLICE && fac != FACTION_MEDIC && fac != FACTION_GOV) {
		SendErr(playerid, "Hanya instansi kedinasan yang dapat mengakses radio departemen.");
		return 0;
	}

	new tag[10];
	if (fac == FACTION_POLICE) format(tag, sizeof(tag), "SAPD");
	else if (fac == FACTION_MEDIC) format(tag, sizeof(tag), "SAMD");
	else format(tag, sizeof(tag), "SAGOV");

	new out[180];
	format(out, sizeof(out), "[DEPT] [%s] %s: %s", tag, gPlayerData[playerid][pName], text);

	for (new i = 0; i < MAX_PLAYERS; i++) {
		if (!IsPlayerConnected(i) || !gPlayerData[i][pLoggedIn] || !gPlayerData[i][pDuty]) continue;
		new targetFac = gPlayerData[i][pFaction];
		if (targetFac == FACTION_POLICE || targetFac == FACTION_MEDIC || targetFac == FACTION_GOV) {
			SendClientMessage(i, COLOR_DEPARTMENT, out);
		}
	}
	return 1;
}
