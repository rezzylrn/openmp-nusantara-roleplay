#if defined _INC_rp_police_mdc
	#endinput
#endif
#define _INC_rp_police_mdc

/*
==============================================================================
 police_mdc.inc
 NUSANTARA ROLEPLAY - Police Mobile Data Computer (MDC) & Speed Radar
 Menggunakan easyDialog untuk mempermudah akses kepolisian SAPD.
==============================================================================
*/

#define POLICE_RADAR_RANGE      40.0
#define POLICE_SPEED_LIMIT      100.0 // KM/J speed limit batas tilang

static gSelectedDispatchIndex[MAX_PLAYERS];

stock IsPoliceVehicleModel(m)
{
	if (m == 596 || m == 597 || m == 598 || m == 599 || m == 528 || m == 490 || m == 427) {
		return 1;
	}
	return 0;
}

stock IsPoliceVehicle(vehid)
{
	if (vehid <= 0 || !IsValidVehicle(vehid)) return 0;
	return IsPoliceVehicleModel(GetVehicleModel(vehid));
}

stock ProcessPoliceRadarTick(playerid)
{
	if (!IsPlayerConnected(playerid) || gPlayerData[playerid][pFaction] != FACTION_POLICE || !gPlayerData[playerid][pDuty]) {
		return 0;
	}

	new veh = GetPlayerVehicleID(playerid);
	if (!IsPoliceVehicle(veh) || GetPlayerState(playerid) != PLAYER_STATE_DRIVER) {
		return 0;
	}

	new Float:px, Float:py, Float:pz;
	GetPlayerPos(playerid, px, py, pz);

	// Scan kendaraan terdekat di depan mobil patroli
	new closestVeh = 0;
	new Float:closestDist = POLICE_RADAR_RANGE;

	for (new v = 1; v < MAX_VEHICLES; v++) {
		if (!IsValidVehicle(v) || v == veh) continue;

		new Float:vx, Float:vy, Float:vz;
		GetVehiclePos(v, vx, vy, vz);

		new Float:d = floatsqroot((px-vx)*(px-vx) + (py-vy)*(py-vy) + (pz-vz)*(pz-vz));
		if (d < closestDist) {
			closestDist = d;
			closestVeh = v;
		}
	}

	if (closestVeh != 0) {
		new Float:vx, Float:vy, Float:vz;
		GetVehicleVelocity(closestVeh, vx, vy, vz);
		new Float:targetSpeed = floatsqroot(vx*vx + vy*vy + vz*vz) * 180.0;

		if (targetSpeed > 20.0) {
			new carModel = GetVehicleModel(closestVeh);
			new vName[32];
			GetVehicleModelName(carModel, vName, sizeof(vName));

			new vSlot = Vehicle_GetSlot(closestVeh);
			new plate[16];
			if (vSlot != -1) {
				format(plate, sizeof(plate), "%s", gVehicleData[vSlot][vPlate]);
			} else {
				format(plate, sizeof(plate), "POLICE/GOV");
			}

			if (targetSpeed > POLICE_SPEED_LIMIT) {
				new radarAlert[64];
				format(radarAlert, sizeof(radarAlert), "~r~RADAR: %s [%.0f KM/J] ~w~Plat: %s", vName, targetSpeed, plate);
				ShowPlayerSubtitle(playerid, radarAlert, 1500);
			}
		}
	}
	return 1;
}

CMD:mdc(playerid, params[])
{
	if (gPlayerData[playerid][pFaction] != FACTION_POLICE || !gPlayerData[playerid][pDuty]) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Hanya petugas kepolisian on-duty yang dapat mengakses MDC.");
		return 1;
	}
	ShowPoliceMDCDialog(playerid);
	return 1;
}

stock ShowPoliceMDC(playerid)
{
	return ShowPoliceMDCDialog(playerid);
}

stock ShowPoliceMDCDialog(playerid)
{
	if (gPlayerData[playerid][pFaction] != FACTION_POLICE || !gPlayerData[playerid][pDuty]) return 0;

	new body[512];
	format(body, sizeof(body),
		"{FFFFFF}1. [Pencarian] Database Catatan Warga (Citizen Record)\n\
{FFFFFF}2. [Pencarian] Database Kendaraan (Plate Lookup)\n\
{FFFFFF}3. [Dispatch] Antrean Panggilan Darurat 911 (Dispatches)\n\
{FFFFFF}4. [Tilang] Daftar Surat Tilang ETLE Belum Dibayar\n\
{FFFFFF}5. [Buronan] Daftar Buronan Aktif Kota Los Santos");

	Dialog_Show(playerid, DialogPoliceMDCMain, DIALOG_STYLE_LIST,
		"{3498DB}SAPD - MOBILE DATA COMPUTER (MDC)", body, "Buka", "Tutup");
	return 1;
}

Dialog:DialogPoliceMDCMain(playerid, response, listitem, inputtext[])
{
	if (!response || gPlayerData[playerid][pFaction] != FACTION_POLICE || !gPlayerData[playerid][pDuty]) return 1;

	switch (listitem) {
		case 0: { // 1. Citizen Lookup
			Dialog_Show(playerid, DialogMDCCitizenInput, DIALOG_STYLE_INPUT,
				"{3498DB}SAPD MDC - PENCARIAN WARGA",
				"{FFFFFF}Masukkan ID atau Nama Lengkap warga yang ingin diperiksa:", "Cari", "Kembali");
		}
		case 1: { // 2. Plate Lookup
			Dialog_Show(playerid, DialogMDCPlateInput, DIALOG_STYLE_INPUT,
				"{3498DB}SAPD MDC - PENCARIAN KENDARAAN",
				"{FFFFFF}Masukkan Nomor Plat Kendaraan yang ingin diperiksa (contoh: NR-1002):", "Cari", "Kembali");
		}
		case 2: { // 3. 911 Dispatches
			new list[1024];
			list[0] = '\0';
			new count = 0;

			for (new d = 0; d < MAX_DISPATCHES; d++) {
				if (gDispatches[d][dActive] && gDispatches[d][dService] == FACTION_POLICE) {
					new row[128];
					format(row, sizeof(row), "{FFFF00}#%d {FFFFFF}| %s (Telp: %d) | {2ECC71}%s\n",
						d + 1, gDispatches[d][dCallerName], gDispatches[d][dCallerPhone], gDispatches[d][dMessage]);
					strcat(list, row);
					count++;
				}
			}

			if (count == 0) {
				Dialog_Show(playerid, DialogMDCEmptyDispatch, DIALOG_STYLE_MSGBOX,
					"{3498DB}SAPD MDC - PANGGILAN 911",
					"{FFFFFF}Tidak ada panggilan darurat kepolisian yang aktif saat ini.", "Kembali", "");
				return 1;
			}

			Dialog_Show(playerid, DialogMDCDispatchList, DIALOG_STYLE_LIST,
				"{3498DB}SAPD MDC - ANTREAN PANGGILAN 911", list, "Respons", "Kembali");
		}
		case 3: { // 4. Unpaid ETLE Citations
			new list[1024];
			list[0] = '\0';
			new count = 0;

			for (new p = 0; p < MAX_PLAYERS; p++) {
				if (IsPlayerConnected(p) && gPlayerData[p][pLoggedIn] && gPlayerData[p][pTickets] > 0) {
					new row[128];
					format(row, sizeof(row), "{FFFFFF}%s (ID %d) | {E74C3C}%d Lembar Tilang | Tunggakan: ${FFFF00}%d\n",
						gPlayerData[p][pName], p, gPlayerData[p][pTickets], gPlayerData[p][pTicketAmount]);
					strcat(list, row);
					count++;
				}
			}

			if (count == 0) {
				Dialog_Show(playerid, DialogMDCEmptyTickets, DIALOG_STYLE_MSGBOX,
					"{3498DB}SAPD MDC - SURAT TILANG",
					"{FFFFFF}Semua warga yang terhubung saat ini bebas dari tunggakan surat tilang ETLE.", "Kembali", "");
				return 1;
			}

			Dialog_Show(playerid, DialogMDCTicketList, DIALOG_STYLE_LIST,
				"{3498DB}SAPD MDC - DAFTAR PELANGGAR TILANG ETLE", list, "Tutup", "Kembali");
		}
		case 4: { // 5. Wanted List
			new list[1024];
			list[0] = '\0';
			new count = 0;

			for (new p = 0; p < MAX_PLAYERS; p++) {
				if (IsPlayerConnected(p) && gPlayerData[p][pLoggedIn]) {
					new wl = GetPlayerWantedLevel(p);
					if (wl > 0) {
						new row[128];
						format(row, sizeof(row), "{FFFFFF}%s (ID %d) | {E74C3C}Buronan: Level %d\n",
							gPlayerData[p][pName], p, wl);
						strcat(list, row);
						count++;
					}
				}
			}

			if (count == 0) {
				Dialog_Show(playerid, DialogMDCEmptyWanted, DIALOG_STYLE_MSGBOX,
					"{3498DB}SAPD MDC - DAFTAR BURONAN",
					"{FFFFFF}Tidak ada buronan aktif (wanted level) di kota saat ini.", "Kembali", "");
				return 1;
			}

			Dialog_Show(playerid, DialogMDCWantedList, DIALOG_STYLE_LIST,
				"{3498DB}SAPD MDC - BURONAN AKTIF", list, "Tutup", "Kembali");
		}
	}
	return 1;
}

// 1. Citizen Lookup Response
Dialog:DialogMDCCitizenInput(playerid, response, listitem, inputtext[])
{
	if (!response) {
		ShowPoliceMDCDialog(playerid);
		return 1;
	}

	new targetid = strval(inputtext);
	if (!IsPlayerConnected(targetid) && strlen(inputtext) > 0) {
		for (new p = 0; p < MAX_PLAYERS; p++) {
			if (IsPlayerConnected(p) && strfind(gPlayerData[p][pName], inputtext, true) != -1) {
				targetid = p;
				break;
			}
		}
	}

	if (!IsPlayerConnected(targetid) || !gPlayerData[targetid][pLoggedIn]) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Data warga tidak ditemukan dalam basis data kota.");
		return 1;
	}

	new rec[512];
	format(rec, sizeof(rec),
		"{FFFFFF}Nama Lengkap: {3498DB}%s (ID %d)\n\
{FFFFFF}Tanggal Lahir: {FFFF00}%s\n\
{FFFFFF}Lisensi Mengemudi: %s\n\
{FFFFFF}Lisensi Senjata Api: %s\n\
{FFFFFF}Surat Tilang Aktif: {E74C3C}%d lembar (Denda: $%d)\n\
{FFFFFF}Status Buronan: {E74C3C}%d Tingkat Kejahatan\n\
{FFFFFF}Nomor Ponsel: {FFFF00}%d",
		gPlayerData[targetid][pName], targetid,
		gPlayerData[targetid][pBirthDate],
		gPlayerData[targetid][pDriveLic] ? ("{2ECC71}AKTIF") : ("{E74C3C}TIDAK MEMILIKI"),
		gPlayerData[targetid][pGunLic] ? ("{2ECC71}RESMI") : ("{E74C3C}TIDAK ADA"),
		gPlayerData[targetid][pTickets], gPlayerData[targetid][pTicketAmount],
		GetPlayerWantedLevel(targetid),
		gPlayerData[targetid][pPhone]);

	Dialog_Show(playerid, DialogMDCCitizenResult, DIALOG_STYLE_MSGBOX,
		"{3498DB}SAPD MDC - CATATAN KEWARGANEGARAAN", rec, "Kembali", "");
	return 1;
}

Dialog:DialogMDCCitizenResult(playerid, response, listitem, inputtext[])
{
	ShowPoliceMDCDialog(playerid);
	return 1;
}

// 2. Vehicle Plate Lookup Response
Dialog:DialogMDCPlateInput(playerid, response, listitem, inputtext[])
{
	if (!response) {
		ShowPoliceMDCDialog(playerid);
		return 1;
	}

	new foundSlot = -1;
	for (new i = 0; i < MAX_OWNED_VEHICLES; i++) {
		if (gVehicleData[i][vUsed] && strcmp(gVehicleData[i][vPlate], inputtext, true) == 0) {
			foundSlot = i;
			break;
		}
	}

	if (foundSlot == -1) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Plat nomor tidak terdaftar dalam database kepolisian.");
		return 1;
	}

	new vehInfo[512];
	new vehName[32];
	GetVehicleModelName(gVehicleData[foundSlot][vModel], vehName, sizeof(vehName));

	format(vehInfo, sizeof(vehInfo),
		"{FFFFFF}Nomor Plat: {FFFF00}%s\n\
{FFFFFF}Model Kendaraan: {3498DB}%s (Model ID: %d)\n\
{FFFFFF}ID Pemilik Akun: {FFFFFF}#%d\n\
{FFFFFF}Status Sitaan: %s\n\
{FFFFFF}Status Kunci: %s",
		gVehicleData[foundSlot][vPlate], vehName, gVehicleData[foundSlot][vModel],
		gVehicleData[foundSlot][vOwnerID],
		gVehicleData[foundSlot][vImpounded] ? ("{E74C3C}DISITA DI DEPO LSPD") : ("{2ECC71}BEBAS / AKTIF"),
		gVehicleData[foundSlot][vLocked] ? ("{FFFF00}TERKUNCI") : ("{2ECC71}TERBUKA"));

	Dialog_Show(playerid, DialogMDCPlateResult, DIALOG_STYLE_MSGBOX,
		"{3498DB}SAPD MDC - REGISTRASI KENDARAAN", vehInfo, "Kembali", "");
	return 1;
}

Dialog:DialogMDCPlateResult(playerid, response, listitem, inputtext[])
{
	ShowPoliceMDCDialog(playerid);
	return 1;
}

// 3. Dispatch Responses
Dialog:DialogMDCEmptyDispatch(playerid, response, listitem, inputtext[])
{
	ShowPoliceMDCDialog(playerid);
	return 1;
}

Dialog:DialogMDCDispatchList(playerid, response, listitem, inputtext[])
{
	if (!response) {
		ShowPoliceMDCDialog(playerid);
		return 1;
	}

	new cur = 0;
	for (new d = 0; d < MAX_DISPATCHES; d++) {
		if (gDispatches[d][dActive] && gDispatches[d][dService] == FACTION_POLICE) {
			if (cur == listitem) {
				gSelectedDispatchIndex[playerid] = d;
				SetPlayerCheckpoint(playerid, gDispatches[d][dX], gDispatches[d][dY], gDispatches[d][dZ], 4.5);

				new msg[144];
				format(msg, sizeof(msg), "[MDC-GPS] Merespons panggilan dari %s. Rute GPS telah ditandai di minimap radar Anda.",
					gDispatches[d][dCallerName]);
				SendClientMessage(playerid, COLOR_SUCCESS, msg);
				return 1;
			}
			cur++;
		}
	}
	return 1;
}

Dialog:DialogMDCEmptyTickets(playerid, response, listitem, inputtext[])
{
	ShowPoliceMDCDialog(playerid);
	return 1;
}

Dialog:DialogMDCTicketList(playerid, response, listitem, inputtext[])
{
	ShowPoliceMDCDialog(playerid);
	return 1;
}

Dialog:DialogMDCEmptyWanted(playerid, response, listitem, inputtext[])
{
	ShowPoliceMDCDialog(playerid);
	return 1;
}

Dialog:DialogMDCWantedList(playerid, response, listitem, inputtext[])
{
	ShowPoliceMDCDialog(playerid);
	return 1;
}
