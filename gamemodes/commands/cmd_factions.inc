#if defined _INC_rp_cmd_factions
	#endinput
#endif
#define _INC_rp_cmd_factions

/*
==============================================================================
 cmd_factions.inc (Faksi Hukum, Medis & Pemerintahan)
 NUSANTARA ROLEPLAY - Modular Command Architecture
==============================================================================
*/


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

	RP_ShowPlayerDialog(playerid, DIALOG_HELP, DIALOG_STYLE_MSGBOX, "DAFTAR DISPATCH 911 AKTIF", listStr, "Tutup", "");
	return 1;
}


CMD:treat(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return SendErr(playerid, "Anda tidak dapat memberikan perawatan saat pingsan atau terborgol.");

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

	if (!TakePlayerItem(playerid, ITEM_BANDAGE, 1)) {
		SendErr(playerid, "Gagal menggunakan perban dari inventaris.");
		return 1;
	}
	ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.1, false, false, false, false, 2000);

	if (gPlayerData[targetid][pInjured]) {
		gPlayerData[targetid][pInjured] = false;
		ClearHospitalTimer(targetid);
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
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return SendErr(playerid, "Anda tidak dapat melakukan resusitasi saat pingsan atau terborgol.");

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
		if (!TakePlayerItem(playerid, ITEM_MEDKIT, 1)) {
			SendErr(playerid, "Gagal menggunakan medkit dari inventaris.");
			return 1;
		}
	}

	ApplyAnimation(playerid, "MEDIC", "CPR", 4.1, false, false, false, false, 3000);
	gPlayerData[targetid][pInjured] = false;
	ClearHospitalTimer(targetid);
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


CMD:armory(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	ShowFactionArmoryDialog(playerid);
	return 1;
}


CMD:locker(playerid, params[])
{
	return cmd_armory(playerid, params);
}


CMD:loker(playerid, params[])
{
	return cmd_armory(playerid, params);
}


CMD:tickets(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	new str[144];
	if (gPlayerData[playerid][pLanguage] == LANG_EN) {
		format(str, sizeof(str), "[ETLE CITATIONS] Pending Violations: %d | Total Fines: $%d | Pay via /payticket",
			gPlayerData[playerid][pTickets], gPlayerData[playerid][pTicketAmount]);
	} else {
		format(str, sizeof(str), "[TILANG ETLE] Surat Pelanggaran: %d | Total Denda: $%d | Bayar via /payticket",
			gPlayerData[playerid][pTickets], gPlayerData[playerid][pTicketAmount]);
	}
	SendClientMessage(playerid, COLOR_YELLOW, str);
	return 1;
}


CMD:payticket(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	ProcessPayTrafficTickets(playerid);
	return 1;
}


CMD:bayartilang(playerid, params[])
{
	return cmd_payticket(playerid, params);
}


CMD:confiscate(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	new targetid;
	if (sscanf(params, "u", targetid)) {
		return SendSyntax(playerid, "/confiscate [playerid/Nama]");
	}
	ProcessConfiscateIllicitItems(playerid, targetid);
	return 1;
}


CMD:sita(playerid, params[])
{
	return cmd_confiscate(playerid, params);
}


CMD:checklic(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	new targetid;
	if (sscanf(params, "u", targetid)) return SendSyntax(playerid, "/checklic [targetid/Nama]");
	ProcessCheckDriverLicense(playerid, targetid);
	return 1;
}


CMD:periksasim(playerid, params[])
{
	return cmd_checklic(playerid, params);
}


CMD:issuegunlic(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	new targetid;
	if (sscanf(params, "u", targetid)) return SendSyntax(playerid, "/issuegunlic [targetid/Nama]");
	ProcessIssueGunLicense(playerid, targetid);
	return 1;
}


CMD:breathalyzer(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	new targetid;
	if (sscanf(params, "u", targetid)) return SendSyntax(playerid, "/breathalyzer [targetid/Nama]");
	ProcessBreathalyzerTest(playerid, targetid);
	return 1;
}


CMD:tesalkohol(playerid, params[])
{
	return cmd_breathalyzer(playerid, params);
}


CMD:checkreg(playerid, params[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return SendErr(playerid, "Anda belum login.");
	if (!gPlayerData[playerid][pDuty] || gPlayerData[playerid][pFaction] != FACTION_POLICE) {
		SendErr(playerid, "Perintah ini khusus untuk petugas Polisi yang sedang bertugas (/duty).");
		return 1;
	}

	new targetid = INVALID_PLAYER_ID;
	if (!sscanf(params, "u", targetid) && IsPlayerConnected(targetid)) {
		ProcessInspectSTNK(playerid, targetid);
	} else {
		ProcessInspectSTNK(playerid);
	}
	return 1;
}

CMD:periksastnk(playerid, params[])
{
	return cmd_checkreg(playerid, params);
}


