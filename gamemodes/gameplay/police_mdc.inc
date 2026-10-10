#if defined _INC_rp_police_mdc
	#endinput
#endif
#define _INC_rp_police_mdc

// ---------------------------------------------------------------------------
// Police MDC (Mobile Data Computer) & Speed Radar System
// ---------------------------------------------------------------------------

#define POLICE_RADAR_RANGE      40.0
#define POLICE_SPEED_LIMIT      100.0 // KM/J speed limit batas tilang

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

stock ShowPoliceMDC(playerid)
{
	if (gPlayerData[playerid][pFaction] != FACTION_POLICE) {
		SendErr(playerid, "Akses MDC hanya untuk petugas Kepolisian SAPD.");
		return 0;
	}

	new str[400];
	format(str, sizeof(str),
		"1. Cek Plat Nomor Kendaraan (/checkplate)\n\
2. Daftar Buronan Aktif (Wanted List / APB)\n\
3. Catatan Kriminal Warga (/record)\n\
4. Pasang Status Tilang Kendaraan\n\
5. Laporan Panggilan Darurat 911");

	ShowPlayerDialog(playerid, DIALOG_SAPD_ACTIONS, DIALOG_STYLE_LIST,
		"SAPD - MOBILE DATA COMPUTER (MDC)", str, "Pilih", "Tutup");
	return 1;
}

stock ProcessCheckPlate(playerid, const plate[])
{
	if (gPlayerData[playerid][pFaction] != FACTION_POLICE) {
		SendErr(playerid, "Hanya staf kepolisian yang dapat mengakses database plat kendaraan.");
		return 0;
	}

	new found = -1;
	for (new i = 0; i < MAX_OWNED_VEHICLES; i++) {
		if (gVehicleData[i][vUsed] && strcmp(gVehicleData[i][vPlate], plate, true) == 0) {
			found = i;
			break;
		}
	}

	if (found == -1) {
		SendInfo(playerid, "MDC: Plat nomor '%s' tidak ditemukan dalam arsip kendaraan sipil.", plate);
		return 1;
	}

	new vName[32];
	GetVehicleModelName(gVehicleData[found][vModel], vName, sizeof(vName));

	SendRP(playerid, COLOR_LIGHTBLUE, "====== [ MDC SAPD: DATA KENDARAAN ] ======");
	SendRP(playerid, COLOR_WHITE, "Plat Nomor : {FFFF00}%s", gVehicleData[found][vPlate]);
	SendRP(playerid, COLOR_WHITE, "Model Mobil: {33CCFF}%s", vName);
	SendRP(playerid, COLOR_WHITE, "ID Pemilik : %d", gVehicleData[found][vOwnerID]);
	SendRP(playerid, COLOR_WHITE, "Bahan Bakar: %.1f L", gVehicleData[found][vFuel]);
	SendRP(playerid, COLOR_LIGHTBLUE, "===========================================");
	return 1;
}
