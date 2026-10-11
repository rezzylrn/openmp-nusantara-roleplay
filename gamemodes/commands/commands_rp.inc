#if defined _INC_rp_commands_rp
	#endinput
#endif
#define _INC_rp_commands_rp

// cmd loader & rp commands

// chat rp
CMD:me(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (isnull(params)) return SendSyntax(playerid, "/me [tindakan]");
	new msg[160];
	format(msg, sizeof(msg), "* %s %s", gPlayerData[playerid][pName], params);
	SendNearbyPlayer(playerid, COLOR_RP_ME, msg, RANGE_ME);
	return 1;
}

CMD:do(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (isnull(params)) return SendSyntax(playerid, "/do [keadaan sekitar]");
	new msg[160];
	format(msg, sizeof(msg), "* %s (( %s ))", params, gPlayerData[playerid][pName]);
	SendNearbyPlayer(playerid, COLOR_RP_DO, msg, RANGE_ME);
	return 1;
}

CMD:b(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (isnull(params)) return SendSyntax(playerid, "/b [pesan ooc]");
	new msg[160];
	format(msg, sizeof(msg), "(( [OOC] %s: %s ))", gPlayerData[playerid][pName], params);
	SendNearbyPlayer(playerid, COLOR_OOC, msg, RANGE_OOC);
	return 1;
}

CMD:o(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (isnull(params)) return SendSyntax(playerid, "/o [pesan ooc global]");
	new msg[160];
	format(msg, sizeof(msg), "[OOC] %s: %s", gPlayerData[playerid][pName], params);
	SendClientMessageToAll(COLOR_OOC, msg);
	return 1;
}

CMD:w(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	new targetid, text[128];
	if (sscanf(params, "us[128]", targetid, text)) return SendSyntax(playerid, "/w [playerid/nama] [pesan]");
	ProcessWhisper(playerid, targetid, text);
	return 1;
}

CMD:s(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (isnull(params)) return SendSyntax(playerid, "/s [pesan teriakan]");
	ProcessShout(playerid, params);
	return 1;
}

CMD:l(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (isnull(params)) return SendSyntax(playerid, "/l [pesan pelan]");
	ProcessLow(playerid, params);
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

CMD:pm(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	new targetid, text[128];
	if (sscanf(params, "us[128]", targetid, text)) return SendSyntax(playerid, "/pm [playerid/nama] [pesan]");
	ProcessPM(playerid, targetid, text);
	return 1;
}

CMD:report(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (isnull(params)) return SendSyntax(playerid, "/report [laporan]");
	ProcessReport(playerid, params);
	return 1;
}

CMD:ask(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (isnull(params)) return SendSyntax(playerid, "/ask [pertanyaan]");
	ProcessAsk(playerid, params);
	return 1;
}

CMD:help(playerid, params[])
{
	ShowHelpDialog(playerid);
	return 1;
}

// player stats & items
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

	ShowPlayerDialog(targetid, DIALOG_HELP, DIALOG_STYLE_MSGBOX, "KARTU IDENTITAS PENDUDUK", idCard, "Tutup", "");

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
	if (gInventoryOpen[playerid]) {
		HidePlayerInventoryUI(playerid);
	} else {
		ShowPlayerInventoryUI(playerid);
	}
	return 1;
}

CMD:inventory(playerid, params[])
{
	return cmd_inv(playerid, params);
}

CMD:items(playerid, params[])
{
	ShowPlayerInventory(playerid);
	return 1;
}

CMD:use(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	new slot;
	if (sscanf(params, "d", slot)) {
		return SendSyntax(playerid, "/use [nomor slot 1-12]");
	}
	if (slot < 1 || slot > 12) {
		return SendErr(playerid, "Slot harus antara 1 sampai 12.");
	}
	UsePlayerItemBySlot(playerid, slot - 1);
	if (gInventoryOpen[playerid]) UpdatePlayerInventoryUI(playerid);
	return 1;
}

CMD:phone(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
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
	ShowPlayerActMenu(playerid);
	return 1;
}

CMD:vehpanel(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	ShowPlayerVehDash(playerid);
	return 1;
}

CMD:mdc(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	ShowPoliceMDC(playerid);
	return 1;
}

CMD:checkplate(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	if (isnull(params)) return SendSyntax(playerid, "/checkplate [plat nomor]");
	ProcessCheckPlate(playerid, params);
	return 1;
}

CMD:tune(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	ShowWorkshopMenu(playerid);
	return 1;
}

CMD:modveh(playerid, params[])
{
	return cmd_tune(playerid, params);
}

CMD:give(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");

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

	TakePlayerItem(playerid, itemType, amount);
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

// economy
CMD:pay(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
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
	ShowBankDialog(playerid);
	return 1;
}

CMD:atm(playerid, params[])
{
	ShowATMDialog(playerid);
	return 1;
}

CMD:buy(playerid, params[])
{
	ShowStoreBuyMenu(playerid);
	return 1;
}

// comms
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

CMD:r(playerid, params[])
{
	if (isnull(params)) return SendSyntax(playerid, "/r [pesan radio]");
	ProcessRadio(playerid, params);
	return 1;
}

CMD:d(playerid, params[])
{
	if (isnull(params)) return SendSyntax(playerid, "/d [pesan departemen]");
	ProcessDepartmentRadio(playerid, params);
	return 1;
}

CMD:m(playerid, params[])
{
	if (isnull(params)) return SendSyntax(playerid, "/m [pengumuman pengeras suara]");
	ProcessMegaphone(playerid, params);
	return 1;
}

// vehicles
CMD:buyveh(playerid, params[])
{
	ShowDealershipDialog(playerid);
	return 1;
}

CMD:myveh(playerid, params[])
{
	ShowMyVehiclesDialog(playerid);
	return 1;
}

CMD:v(playerid, params[])
{
	ShowMyVehiclesDialog(playerid);
	return 1;
}

CMD:engine(playerid, params[])
{
	ProcessEngineToggle(playerid);
	return 1;
}

CMD:lights(playerid, params[])
{
	ProcessLightsToggle(playerid);
	return 1;
}

CMD:lock(playerid, params[])
{
	new vid = GetPlayerVehicleID(playerid);
	new slot = Vehicle_GetSlot(vid);
	if (slot != -1) ProcessVehicleLock(playerid, slot);
	else SendErr(playerid, "Anda harus berada di dalam kendaraan pribadi.");
	return 1;
}

CMD:park(playerid, params[])
{
	new vid = GetPlayerVehicleID(playerid);
	new slot = Vehicle_GetSlot(vid);
	if (slot != -1) ProcessVehiclePark(playerid, slot);
	else SendErr(playerid, "Anda harus berada di dalam kendaraan pribadi.");
	return 1;
}

CMD:hood(playerid, params[])
{
	new vid = GetPlayerVehicleID(playerid);
	new slot = Vehicle_GetSlot(vid);
	if (slot != -1) {
		new bool:newHood = !gVehicleData[slot][vHood];
		Vehicle_SetHood(vid, newHood);
		SendRP(playerid, COLOR_INFO, "Kap mesin kendaraan %s.", newHood ? "dibuka" : "ditutup");
	} else SendErr(playerid, "Anda harus berada di dalam kendaraan.");
	return 1;
}

CMD:trunk(playerid, params[])
{
	if (IsPlayerInAnyVehicle(playerid)) {
		new vid = GetPlayerVehicleID(playerid);
		new slot = Vehicle_GetSlot(vid);
		if (slot != -1) {
			new bool:newTrunk = !gVehicleData[slot][vTrunkOpen];
			Vehicle_SetTrunk(vid, newTrunk);
			SendRP(playerid, COLOR_INFO, "Pintu bagasi kendaraan %s.", newTrunk ? "dibuka" : "ditutup");
		} else SendErr(playerid, "Ini bukan kendaraan terdaftar.");
		return 1;
	}

	new slot = Vehicle_FindNearest(playerid, 4.0);
	if (slot == -1) {
		SendErr(playerid, "Tidak ada kendaraan di dekat Anda (radius 4.0 meter).");
		return 1;
	}

	if (!gVehicleData[slot][vTrunkOpen]) {
		SendErr(playerid, "Pintu bagasi kendaraan ini masih tertutup.");
		SendInfo(playerid, "Buka kunci pintu dan buka bagasi kendaraan terlebih dahulu.");
		return 1;
	}

	ShowVehicleTrunkDialog(playerid, slot);
	return 1;
}

// ---------------------------------------------------------------------------
// Perintah Layanan Darurat 911 & Medis Trauma
// ---------------------------------------------------------------------------
CMD:acceptdispatch(playerid, params[])
{
	if (!gPlayerData[playerid][pDuty]) {
		SendErr(playerid, "Anda harus sedang bertugas dinas (/duty).");
		return 1;
	}
	if (gPlayerData[playerid][pFaction] != FACTION_POLICE && gPlayerData[playerid][pFaction] != FACTION_MEDIC) {
		SendErr(playerid, "Perintah ini khusus untuk petugas Polisi atau Medis.");
		return 1;
	}

	new dispId;
	if (sscanf(params, "d", dispId)) {
		SendSyntax(playerid, "/acceptdispatch [ID Panggilan]");
		return 1;
	}

	new idx = dispId - 1;
	if (idx < 0 || idx >= MAX_DISPATCHES || !gDispatches[idx][dActive]) {
		SendErr(playerid, "Panggilan darurat #%d tidak aktif atau sudah kadaluarsa.", dispId);
		return 1;
	}

	gDispatches[idx][dResponderID] = playerid;
	SetPlayerCheckpoint(playerid, gDispatches[idx][dX], gDispatches[idx][dY], gDispatches[idx][dZ], 6.0);

	new Float:dist = GetPlayerDistanceFromPoint(playerid, gDispatches[idx][dX], gDispatches[idx][dY], gDispatches[idx][dZ]);
	SendSucc(playerid, "[DISPATCH] Anda merespons panggilan #%d. Rute GPS ditandai ke lokasi (%.0fm).", dispId, dist);

	new callerId = gDispatches[idx][dCallerID];
	if (callerId != INVALID_PLAYER_ID && IsPlayerConnected(callerId)) {
		SendRP(callerId, COLOR_INFO, "[911] Petugas %s sedang menuju ke lokasi Anda (Jarak: %.0fm).",
			gPlayerData[playerid][pName], dist);
		PlayerPlaySound(callerId, 1057, 0.0, 0.0, 0.0);
	}
	return 1;
}

CMD:dispatches(playerid, params[])
{
	if (!gPlayerData[playerid][pDuty] || (gPlayerData[playerid][pFaction] != FACTION_POLICE && gPlayerData[playerid][pFaction] != FACTION_MEDIC)) {
		SendErr(playerid, "Perintah ini khusus petugas on duty (/duty).");
		return 1;
	}

	new listStr[1024];
	listStr[0] = '\0';
	new activeCount = 0;

	for (new i = 0; i < MAX_DISPATCHES; i++) {
		if (gDispatches[i][dActive]) {
			new Float:dist = GetPlayerDistanceFromPoint(playerid, gDispatches[i][dX], gDispatches[i][dY], gDispatches[i][dZ]);
			format(listStr, sizeof(listStr), "%s#%d | %s | %s (%.0fm) - \"%s\"\n",
				listStr, i + 1, (gDispatches[i][dService] == 1) ? "POLISI" : "MEDIS",
				gDispatches[i][dCallerName], dist, gDispatches[i][dMessage]);
			activeCount++;
		}
	}

	if (activeCount == 0) {
		SendInfo(playerid, "Tidak ada panggilan darurat 911 aktif saat ini.");
		return 1;
	}

	ShowPlayerDialog(playerid, DIALOG_HELP, DIALOG_STYLE_MSGBOX, "DAFTAR DISPATCH 911 AKTIF", listStr, "Tutup", "");
	return 1;
}

CMD:treat(playerid, params[])
{
	new targetid;
	if (sscanf(params, "u", targetid)) return SendSyntax(playerid, "/treat [ID / Nama Pemain]");
	if (!IsPlayerConnected(targetid) || !gPlayerData[targetid][pLoggedIn]) return SendErr(playerid, "Pemain tidak ditemukan.");
	if (targetid == playerid) return SendErr(playerid, "Gunakan inventaris untuk merawat diri sendiri.");

	if (GetPlayerItemCount(playerid, ITEM_BANDAGE) <= 0) {
		SendErr(playerid, "Anda tidak memiliki Perban (Bandage). Beli di Toko 24/7.");
		return 1;
	}

	new Float:px, Float:py, Float:pz;
	new Float:tx, Float:ty, Float:tz;
	GetPlayerPos(playerid, px, py, pz);
	GetPlayerPos(targetid, tx, ty, tz);
	new Float:dist = floatsqroot((px-tx)*(px-tx) + (py-ty)*(py-ty) + (pz-tz)*(pz-tz));
	if (dist > 3.5) return SendErr(playerid, "Anda terlalu jauh dari target (maks. 3.5 meter).");

	TakePlayerItem(playerid, ITEM_BANDAGE, 1);
	ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.1, false, false, false, false, 2000);

	if (gPlayerData[targetid][pInjured]) {
		gPlayerData[targetid][pInjured] = false;
		ClearAnimations(targetid);
		TogglePlayerControllable(targetid, true);
		SetPlayerHealth(targetid, 35.0);
		SendRP(targetid, COLOR_SUCCESS, "Anda sadar kembali setelah mendapatkan pertolongan pertama dari %s (+35 HP).", gPlayerData[playerid][pName]);
		SendRP(playerid, COLOR_SUCCESS, "Anda berhasil memberikan pertolongan pertama ke %s sehingga ia sadar.", gPlayerData[targetid][pName]);
	} else {
		new Float:hp;
		GetPlayerHealth(targetid, hp);
		hp += 25.0;
		if (hp > 100.0) hp = 100.0;
		SetPlayerHealth(targetid, hp);
		SendRP(targetid, COLOR_SUCCESS, "%s membalut luka Anda dengan perban (+25 HP).", gPlayerData[playerid][pName]);
		SendRP(playerid, COLOR_SUCCESS, "Anda membalut luka %s dengan perban.", gPlayerData[targetid][pName]);
	}

	new meMsg[128];
	format(meMsg, sizeof(meMsg), "* %s membalut perban medis pada tubuh %s.", gPlayerData[playerid][pName], gPlayerData[targetid][pName]);
	SendNearbyPlayer(playerid, COLOR_RP_ME, meMsg, RANGE_ME);
	return 1;
}

CMD:revive(playerid, params[])
{
	if (!IsPlayerAdminEx(playerid, 1) && (!gPlayerData[playerid][pDuty] || gPlayerData[playerid][pFaction] != FACTION_MEDIC)) {
		SendErr(playerid, "Perintah ini khusus untuk petugas medis SAMD atau Admin.");
		return 1;
	}

	new targetid;
	if (sscanf(params, "u", targetid)) return SendSyntax(playerid, "/revive [ID / Nama Pemain]");
	if (!IsPlayerConnected(targetid) || !gPlayerData[targetid][pLoggedIn]) return SendErr(playerid, "Pemain tidak ditemukan.");

	new Float:px, Float:py, Float:pz;
	new Float:tx, Float:ty, Float:tz;
	GetPlayerPos(playerid, px, py, pz);
	GetPlayerPos(targetid, tx, ty, tz);
	new Float:dist = floatsqroot((px-tx)*(px-tx) + (py-ty)*(py-ty) + (pz-tz)*(pz-tz));
	if (dist > 4.0) return SendErr(playerid, "Anda terlalu jauh dari target (maks. 4 meter).");

	if (!gPlayerData[targetid][pInjured]) {
		SendErr(playerid, "Pemain ini tidak dalam kondisi pingsan / sekarat.");
		return 1;
	}

	if (!IsPlayerAdminEx(playerid, 1)) {
		if (GetPlayerItemCount(playerid, ITEM_MEDKIT) <= 0) {
			SendErr(playerid, "Anda membutuhkan Kotak P3K (Medkit).");
			return 1;
		}
		TakePlayerItem(playerid, ITEM_MEDKIT, 1);
	}

	ApplyAnimation(playerid, "MEDIC", "CPR", 4.1, false, false, false, false, 3000);
	gPlayerData[targetid][pInjured] = false;
	ClearAnimations(targetid);
	TogglePlayerControllable(targetid, true);
	SetPlayerHealth(targetid, 100.0);

	SendRP(targetid, COLOR_SUCCESS, "[MEDIS] Petugas medis %s telah memulihkan kondisi vital Anda (100%% HP).", gPlayerData[playerid][pName]);
	SendRP(playerid, COLOR_SUCCESS, "[MEDIS] Anda berhasil memulihkan kondisi vital %s.", gPlayerData[targetid][pName]);

	new meMsg[128];
	format(meMsg, sizeof(meMsg), "* %s melakukan prosedur resusitasi dan memulihkan detak jantung %s.", gPlayerData[playerid][pName], gPlayerData[targetid][pName]);
	SendNearbyPlayer(playerid, COLOR_RP_ME, meMsg, RANGE_ME);

	PlayerPlaySound(targetid, 1058, 0.0, 0.0, 0.0);
	PlayerPlaySound(playerid, 1058, 0.0, 0.0, 0.0);
	return 1;
}

CMD:refuel(playerid, params[])
{
	ProcessRefuel(playerid);
	return 1;
}

CMD:fill(playerid, params[])
{
	ProcessRefuel(playerid);
	return 1;
}

// jobs
CMD:job(playerid, params[])
{
	ShowJobDialog(playerid);
	return 1;
}

CMD:quitjob(playerid, params[])
{
	ProcessJobQuit(playerid);
	return 1;
}

CMD:searchtrash(playerid, params[])
{
	ProcessSearchTrash(playerid);
	return 1;
}

CMD:selltrash(playerid, params[])
{
	ProcessSellTrash(playerid);
	return 1;
}

CMD:cutwood(playerid, params[])
{
	ProcessCutWood(playerid);
	return 1;
}

CMD:sellwood(playerid, params[])
{
	ProcessSellWood(playerid);
	return 1;
}

CMD:fish(playerid, params[])
{
	ProcessFish(playerid);
	return 1;
}

CMD:sellfish(playerid, params[])
{
	ProcessSellFish(playerid);
	return 1;
}

CMD:service(playerid, params[])
{
	if (gPlayerData[playerid][pJob] != JOB_MECHANIC) return SendErr(playerid, "Perintah ini khusus untuk Mekanik Bengkel.");
	new type[16], targetid, price;
	if (sscanf(params, "s[16]ud", type, targetid, price)) return SendSyntax(playerid, "/service [engine/body] [targetid] [harga]");
	if (!IsPlayerConnected(targetid)) return SendErr(playerid, "Pemain tidak ditemukan.");
	if (!IsPlayerInAnyVehicle(targetid)) return SendErr(playerid, "Target tidak berada di dalam kendaraan.");

	new tVeh = GetPlayerVehicleID(targetid);
	if (strcmp(type, "engine", true) == 0) {
		SetVehicleHealth(tVeh, 1000.0);
		TakePlayerMoneyEx(targetid, price, "service mesin");
		GivePlayerMoneyEx(playerid, price, "upah service mekanik");
		SendSucc(playerid, "Mesin kendaraan berhasil diperbaiki. Menerima bayaran $%d.", price);
		SendSucc(targetid, "Mesin kendaraan Anda diperbaiki oleh Mekanik %s ($%d).", gPlayerData[playerid][pName], price);
	} else if (strcmp(type, "body", true) == 0) {
		RepairVehicle(tVeh);
		TakePlayerMoneyEx(targetid, price, "service bodi");
		GivePlayerMoneyEx(playerid, price, "upah service mekanik");
		SendSucc(playerid, "Bodi kendaraan berhasil diperbaiki. Menerima bayaran $%d.", price);
		SendSucc(targetid, "Bodi kendaraan Anda diperbaiki oleh Mekanik %s ($%d).", gPlayerData[playerid][pName], price);
	} else {
		SendSyntax(playerid, "/service [engine/body] [targetid] [harga]");
	}
	return 1;
}

// factions
CMD:duty(playerid, params[])
{
	new bool:newduty = !gPlayerData[playerid][pDuty];
	ProcessFactionDuty(playerid, newduty);
	return 1;
}

CMD:cuff(playerid, params[])
{
	new targetid;
	if (sscanf(params, "u", targetid)) return SendSyntax(playerid, "/cuff [targetid/nama]");
	ProcessCuff(playerid, targetid);
	return 1;
}

CMD:uncuff(playerid, params[])
{
	new targetid;
	if (sscanf(params, "u", targetid)) return SendSyntax(playerid, "/uncuff [targetid/nama]");
	ProcessUncuff(playerid, targetid);
	return 1;
}

CMD:frisk(playerid, params[])
{
	new targetid;
	if (sscanf(params, "u", targetid)) return SendSyntax(playerid, "/frisk [targetid/nama]");
	ProcessFrisk(playerid, targetid);
	return 1;
}

CMD:ticket(playerid, params[])
{
	new targetid, amount, reason[64];
	if (sscanf(params, "uds[64]", targetid, amount, reason)) return SendSyntax(playerid, "/ticket [targetid] [denda] [alasan]");
	ProcessTicket(playerid, targetid, amount, reason);
	return 1;
}

CMD:arrest(playerid, params[])
{
	new targetid, timeMinutes;
	if (sscanf(params, "ud", targetid, timeMinutes)) return SendSyntax(playerid, "/arrest [targetid] [menit]");
	ProcessArrest(playerid, targetid, timeMinutes, 100);
	return 1;
}

CMD:heal(playerid, params[])
{
	new targetid;
	if (sscanf(params, "u", targetid)) return SendSyntax(playerid, "/heal [targetid]");
	ProcessHeal(playerid, targetid);
	return 1;
}

CMD:gov(playerid, params[])
{
	if (isnull(params)) return SendSyntax(playerid, "/gov [pengumuman resmi]");
	ProcessGovBroadcast(playerid, params);
	return 1;
}

CMD:settax(playerid, params[])
{
	new rate;
	if (sscanf(params, "d", rate)) return SendSyntax(playerid, "/settax [0-25%]");
	ProcessSetTax(playerid, rate);
	return 1;
}

// helper procedures
stock ProcessWhisper(playerid, targetid, const text[])
{
	if (targetid == -1 || text[0] == '\0') return SendSyntax(playerid, "/w [targetid] [pesan]");
	if (!IsPlayerConnected(targetid) || !gPlayerData[targetid][pLoggedIn]) return SendErr(playerid, "Target tidak aktif.");
	if (targetid == playerid) return SendErr(playerid, "Tidak bisa berbisik ke diri sendiri.");
	if (!IsPlayerNear(playerid, targetid, RANGE_WHISPER)) return SendErr(playerid, "Target terlalu jauh.");

	new msg[160];
	format(msg, sizeof(msg), "(bisik ke %s) %s", gPlayerData[targetid][pName], text);
	SendClientMessage(playerid, COLOR_YELLOW, msg);
	format(msg, sizeof(msg), "(bisik dari %s) %s", gPlayerData[playerid][pName], text);
	SendClientMessage(targetid, COLOR_YELLOW, msg);
	return 1;
}

stock ProcessShout(playerid, const text[])
{
	new msg[160];
	format(msg, sizeof(msg), "%s berteriak: %s!", gPlayerData[playerid][pName], text);
	SendNearbyPlayer(playerid, COLOR_WHITE, msg, RANGE_SHOUT);
	return 1;
}

stock ProcessLow(playerid, const text[])
{
	new msg[160];
	format(msg, sizeof(msg), "(pelan) %s: %s", gPlayerData[playerid][pName], text);
	SendNearbyPlayer(playerid, COLOR_DARKGREY, msg, RANGE_LOW);
	return 1;
}

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

stock ProcessPM(playerid, targetid, const message[])
{
	if (!IsPlayerConnected(targetid) || !gPlayerData[targetid][pLoggedIn]) return SendErr(playerid, "Target tidak aktif.");
	if (targetid == playerid) return SendErr(playerid, "Tidak bisa kirim PM ke diri sendiri.");
	SendRP(playerid, COLOR_YELLOW, "PM Terkirim ke %s (ID %d): %s", gPlayerData[targetid][pName], targetid, message);
	SendRP(targetid, COLOR_YELLOW, "PM Dari %s (ID %d): %s", gPlayerData[playerid][pName], playerid, message);
	PlayerPlaySound(targetid, 1057, 0.0, 0.0, 0.0);
	return 1;
}

stock ProcessReport(playerid, const text[])
{
	new msg[160];
	format(msg, sizeof(msg), "[REPORT] %s (ID %d): %s", gPlayerData[playerid][pName], playerid, text);
	SendSucc(playerid, "Laporan telah diteruskan ke admin.");
	for (new i = 0; i < MAX_PLAYERS; i++) {
		if (IsPlayerConnected(i) && gPlayerData[i][pAdmin] > 0) SendClientMessage(i, COLOR_ADMIN_CHAT, msg);
	}
	return 1;
}

stock ProcessAsk(playerid, const text[])
{
	new msg[160];
	format(msg, sizeof(msg), "[ASK] %s (ID %d): %s", gPlayerData[playerid][pName], playerid, text);
	SendSucc(playerid, "Pertanyaan telah diteruskan ke staf Helper.");
	for (new i = 0; i < MAX_PLAYERS; i++) {
		if (IsPlayerConnected(i) && gPlayerData[i][pAdmin] > 0) SendClientMessage(i, COLOR_LIGHTBLUE, msg);
	}
	return 1;
}

stock ShowHelpDialog(playerid)
{
	new buf[800];
	format(buf, sizeof(buf),
		"PANDUAN PERINTAH UTAMA:\n\n\
1. Roleplay Chat:\n\
   /me, /do, /b, /w [id], /s, /l, /pm [id], /id [nama], /admins\n\n\
2. Inventaris & Toko:\n\
   /inv, /buy (di toko 24/7), /gps\n\n\
3. Telekomunikasi:\n\
   /call, /p, /h, /sms, /setfreq, /r, /d, /m\n\n\
4. Kendaraan:\n\
   /v, /engine (N), /lights (H), /lock, /hood, /trunk, /refuel, /buyveh\n\n\
5. Keuangan:\n\
   /stats, /pay, /bank, /atm\n\n\
6. Pekerjaan:\n\
   /job, /quitjob, /searchtrash, /selltrash, /cutwood, /sellwood, /fish, /sellfish, /service");

	ShowPlayerDialog(playerid, DIALOG_HELP, DIALOG_STYLE_MSGBOX,
		"PANDUAN SERVER - NUSANTARA RP", buf, "Tutup", "");
	return 1;
}

stock IsPlayerNear(playerid, targetid, Float:range)
{
	new Float:px, Float:py, Float:pz, Float:tx, Float:ty, Float:tz;
	GetPlayerPos(playerid, px, py, pz);
	GetPlayerPos(targetid, tx, ty, tz);
	return (VectorSize(px - tx, py - ty, pz - tz) <= range);
}
