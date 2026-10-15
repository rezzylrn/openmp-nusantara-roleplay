#if defined _INC_rp_cmd_general
	#endinput
#endif
#define _INC_rp_cmd_general

/*
==============================================================================
 cmd_general.inc (General & Player Utilities)
 NUSANTARA ROLEPLAY - Modular Command Architecture
==============================================================================
*/

stock ProcessFindID(playerid, const query[])
{
	new targetName[MAX_PLAYER_NAME], found = 0;
	SendRP(playerid, COLOR_LIGHTBLUE, "=== PENCARIAN PEMAIN ===");
	for (new i = 0; i < MAX_PLAYERS; i++) {
		if (!IsPlayerConnected(i) || !gPlayerData[i][pLoggedIn]) continue;
		GetPlayerName(i, targetName, sizeof(targetName));
		if (strfind(targetName, query, true) != -1 || i == strval(query)) {
			SendRP(playerid, COLOR_WHITE, "ID: %d | %s | Level: %d | Ping: %d", i, targetName, gPlayerData[i][pLevel], GetPlayerPing(i));
			found++;
		}
	}
	if (!found) SendErr(playerid, "Tidak ada pemain ditemukan.");
	return 1;
}

stock ProcessAdmins(playerid)
{
	SendRP(playerid, COLOR_LIGHTBLUE, "=== ADMINISTRATOR BERTUGAS ===");
	new count = 0;
	for (new i = 0; i < MAX_PLAYERS; i++) {
		if (IsPlayerConnected(i) && gPlayerData[i][pLoggedIn] && gPlayerData[i][pAdmin] > 0) {
			SendRP(playerid, COLOR_WHITE, "ID: %d | %s | Level: %d", i, gPlayerData[i][pName], gPlayerData[i][pAdmin]);
			count++;
		}
	}
	if (!count) SendRP(playerid, COLOR_GREY, "Tidak ada administrator online.");
	return 1;
}

CMD:id(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (isnull(params)) return SendSyntax(playerid, "/id [nama/id]");
	ProcessFindID(playerid, params);
	return 1;
}


CMD:admins(playerid, params[])
{
	ProcessAdmins(playerid);
	return 1;
}


CMD:stats(playerid, params[])
{
	ShowStatsDialog(playerid);
	return 1;
}


CMD:showid(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	new targetid;
	if (sscanf(params, "u", targetid)) {
		return SendSyntax(playerid, "/showid [playerid/nama]");
	}
	if (!IsPlayerConnected(targetid) || !gPlayerData[targetid][pLoggedIn]) {
		return SendErr(playerid, "Pemain tujuan tidak terhubung.");
	}

	new Float:px, Float:py, Float:pz;
	GetPlayerPos(playerid, px, py, pz);
	if (!IsPlayerInRangeOfPoint(targetid, 4.0, px, py, pz)) {
		return SendErr(playerid, "Pemain tujuan terlalu jauh dari Anda.");
	}

	new idCard[256];
	format(idCard, sizeof(idCard),
		"{FFFFFF}=== {33CCFF}KARTU TANDA PENDUDUK (KTP) LS{FFFFFF} ===\n\
Nama: {FFFF00}%s\n\
UCP: {33CCFF}%s\n\
Tanggal Lahir: {FFFF00}%s\n\
Jenis Kelamin: {FFFF00}%s\n\
Level: {FFFF00}%d",
		gPlayerData[playerid][pName],
		gPlayerData[playerid][pUCPName],
		gPlayerData[playerid][pBirthDate],
		(gPlayerData[playerid][pGender] == 1) ? "Laki-Laki" : "Perempuan",
		gPlayerData[playerid][pLevel]);

	RP_ShowPlayerDialog(targetid, DIALOG_HELP, DIALOG_STYLE_MSGBOX, "KARTU IDENTITAS PENDUDUK", idCard, "Tutup", "");

	new rpMsg[128];
	format(rpMsg, sizeof(rpMsg), "* %s memperlihatkan KTP miliknya kepada %s.",
		gPlayerData[playerid][pName], gPlayerData[targetid][pName]);
	SendNearbyPlayer(playerid, COLOR_RP_ME, rpMsg, 15.0);
	SendSucc(playerid, "Anda telah memperlihatkan kartu identitas Anda kepada %s.", gPlayerData[targetid][pName]);
	return 1;
}


CMD:inv(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		return SendErr(playerid, "Anda tidak sadarkan diri dan tidak dapat membuka inventaris.");
	}
	ShowPlayerInventoryDialog(playerid);
	return 1;
}


CMD:inventory(playerid, params[])
{
	return cmd_inv(playerid, params);
}


CMD:items(playerid, params[])
{
	return cmd_inv(playerid, params);
}


CMD:use(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		return SendErr(playerid, "Anda tidak dapat menggunakan barang saat pingsan atau terborgol.");
	}
	new slot;
	if (sscanf(params, "d", slot)) {
		return SendSyntax(playerid, "/use [nomor slot 1-25]");
	}
	if (slot < 1 || slot > MAX_INVENTORY_SLOTS) {
		return SendErr(playerid, "Slot harus antara 1 sampai 25.");
	}
	UsePlayerItemBySlot(playerid, slot - 1);
	if (gInventoryOpen[playerid]) UpdatePlayerInventoryUI(playerid);
	return 1;
}


CMD:phone(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		return SendErr(playerid, "Anda tidak dapat menggunakan ponsel saat pingsan atau terborgol.");
	}
	if (GetPlayerItemCount(playerid, ITEM_PHONE) <= 0) {
		return SendErr(playerid, "Anda tidak memiliki ponsel di dalam inventaris (/inv).");
	}
	if (gPhoneOpen[playerid]) {
		HidePlayerPhone(playerid);
	} else {
		ShowPlayerPhone(playerid);
	}
	return 1;
}


CMD:hp(playerid, params[])
{
	return cmd_phone(playerid, params);
}


CMD:menu(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		return SendErr(playerid, "Anda tidak sadarkan diri dan tidak dapat membuka menu.");
	}
	ShowPlayerActionHub(playerid);
	return 1;
}


CMD:action(playerid, params[])
{
	return cmd_menu(playerid, params);
}


CMD:aksi(playerid, params[])
{
	return cmd_menu(playerid, params);
}


CMD:interaksi(playerid, params[])
{
	return cmd_menu(playerid, params);
}


CMD:give(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		return SendErr(playerid, "Anda tidak dapat memberikan barang/uang saat pingsan atau terborgol.");
	}

	new targetid, item[24], amount;
	if (sscanf(params, "us[24]d", targetid, item, amount)) {
		SendSyntax(playerid, "/give [playerid/nama] [item/cash] [jumlah]");
		SendInfo(playerid, "Contoh: /give 2 cash 500  atau  /give 2 snack 2");
		return 1;
	}

	if (!IsPlayerConnected(targetid) || !gPlayerData[targetid][pLoggedIn]) {
		return SendErr(playerid, "Pemain tujuan tidak terhubung atau belum login.");
	}
	if (targetid == playerid) {
		return SendErr(playerid, "Anda tidak dapat memberikan barang kepada diri sendiri.");
	}
	if (amount <= 0) {
		return SendErr(playerid, "Jumlah harus lebih dari 0.");
	}

	new Float:px, Float:py, Float:pz;
	GetPlayerPos(playerid, px, py, pz);
	if (!IsPlayerInRangeOfPoint(targetid, 4.0, px, py, pz)) {
		return SendErr(playerid, "Pemain tujuan terlalu jauh dari Anda (Maksimal 4 meter).");
	}

	if (strcmp(item, "cash", true) == 0 || strcmp(item, "uang", true) == 0) {
		if (gPlayerData[playerid][pMoney] < amount) {
			return SendErr(playerid, "Uang tunai Anda tidak mencukupi.");
		}
		TakePlayerMoneyEx(playerid, amount);
		GivePlayerMoneyEx(targetid, amount);

		SendRP(playerid, COLOR_SUCCESS, "Anda memberikan uang $%d kepada %s.", amount, gPlayerData[targetid][pName]);
		SendRP(targetid, COLOR_SUCCESS, "%s memberikan uang $%d kepada Anda.", gPlayerData[playerid][pName], amount);

		new rpMsg[128];
		format(rpMsg, sizeof(rpMsg), "* %s mengambil sejumlah uang dan memberikannya kepada %s.",
			gPlayerData[playerid][pName], gPlayerData[targetid][pName]);
		SendNearbyPlayer(playerid, COLOR_RP_ME, rpMsg, 15.0);
		return 1;
	}

	new itemType = GetItemTypeByName(item);
	if (itemType == ITEM_NONE) {
		return SendErr(playerid, "Nama barang tidak dikenali. Ketik /inv untuk melihat daftar barang.");
	}

	if (GetPlayerItemCount(playerid, itemType) < amount) {
		return SendErr(playerid, "Jumlah barang Anda tidak mencukupi di inventaris.");
	}

	if (!TakePlayerItem(playerid, itemType, amount)) {
		return SendErr(playerid, "Gagal memberikan barang: stok tidak mencukupi.");
	}
	GivePlayerItem(targetid, itemType, amount);

	new itemName[32];
	GetItemName(itemType, itemName, sizeof(itemName));

	SendRP(playerid, COLOR_SUCCESS, "Anda memberikan %d %s kepada %s.", amount, itemName, gPlayerData[targetid][pName]);
	SendRP(targetid, COLOR_SUCCESS, "%s memberikan %d %s kepada Anda.", gPlayerData[playerid][pName], amount, itemName);

	new rpMsg[140];
	format(rpMsg, sizeof(rpMsg), "* %s menyerahkan %d %s kepada %s.",
		gPlayerData[playerid][pName], amount, itemName, gPlayerData[targetid][pName]);
	SendNearbyPlayer(playerid, COLOR_RP_ME, rpMsg, 15.0);
	return 1;
}


CMD:gps(playerid, params[])
{
	ShowGPSMenu(playerid);
	return 1;
}


CMD:pay(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		return SendErr(playerid, "Anda tidak dapat memberikan uang saat pingsan atau terborgol.");
	}
	new targetid, amount;
	if (sscanf(params, "ud", targetid, amount)) return SendSyntax(playerid, "/pay [playerid/nama] [jumlah]");
	if (!IsPlayerConnected(targetid)) return SendErr(playerid, "Pemain tidak ditemukan.");
	new targetName[MAX_PLAYER_NAME];
	GetPlayerName(targetid, targetName, sizeof(targetName));
	ProcessPay(playerid, targetName, amount);
	return 1;
}


CMD:bank(playerid, params[])
{
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		return SendErr(playerid, "Anda tidak dapat melakukan transaksi saat pingsan atau terborgol.");
	}
	ShowBankDialog(playerid);
	return 1;
}


CMD:atm(playerid, params[])
{
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		return SendErr(playerid, "Anda tidak dapat menggunakan ATM saat pingsan atau terborgol.");
	}
	ShowATMDialog(playerid);
	return 1;
}


CMD:buy(playerid, params[])
{
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) {
		return SendErr(playerid, "Anda tidak dapat berbelanja saat pingsan atau terborgol.");
	}
	ShowStoreBuyMenu(playerid);
	return 1;
}


CMD:call(playerid, params[])
{
	if (isnull(params)) return SendSyntax(playerid, "/call [nomor telepon]");
	ProcessCall(playerid, params);
	return 1;
}


CMD:p(playerid, params[])
{
	ProcessPickup(playerid);
	return 1;
}


CMD:h(playerid, params[])
{
	ProcessHangup(playerid);
	return 1;
}


CMD:sms(playerid, params[])
{
	new num, text[128];
	if (sscanf(params, "ds[128]", num, text)) return SendSyntax(playerid, "/sms [nomor] [pesan]");
	ProcessSMS(playerid, num, text);
	return 1;
}


CMD:setfreq(playerid, params[])
{
	new f;
	if (sscanf(params, "d", f)) return SendSyntax(playerid, "/setfreq [1-999]");
	if (f < 1 || f > 999) return SendErr(playerid, "Frekuensi harus antara 1 - 999 MHz.");
	gPlayerData[playerid][pRadioFreq] = f;
	SendSucc(playerid, "Frekuensi HT disetel ke %d MHz.", f);
	return 1;
}


