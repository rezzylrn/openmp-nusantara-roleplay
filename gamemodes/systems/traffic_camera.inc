#if defined _INC_traffic_camera
	#endinput
#endif
#define _INC_traffic_camera

/*
==============================================================================
 traffic_camera.inc
 NUSANTARA ROLEPLAY - Electronic Traffic Law Enforcement (ETLE Speed Trap)
==============================================================================
*/

#define MAX_SPEED_CAMERAS       5
#define SPEED_CAMERA_TICKET_FEE 150
#define SPEED_TRAP_COOLDOWN     10

enum E_SPEED_CAMERA {
	Float:camX,
	Float:camY,
	Float:camZ,
	Float:camLimitKmh,
	camName[48],
	camObject,
	Text3D:camLabel
};

new gSpeedCameras[MAX_SPEED_CAMERAS][E_SPEED_CAMERA];
new gPlayerCameraCooldown[MAX_PLAYERS];

stock TrafficCamera_Init()
{
	for (new i = 0; i < MAX_PLAYERS; i++) {
		gPlayerCameraCooldown[i] = 0;
	}

	// 1. Pershing Square Central Boulevard (Batas: 100 KM/J)
	TrafficCamera_Register(0, 1481.0, -1700.0, 13.5, 100.0, "Pershing Square Central");

	// 2. Commerce / Mulholland Express Freeway (Batas: 120 KM/J)
	TrafficCamera_Register(1, 1650.0, -1120.0, 24.0, 120.0, "Commerce Express Freeway");

	// 3. Idlewood Boulevard Avenue (Batas: 90 KM/J)
	TrafficCamera_Register(2, 1950.0, -1770.0, 13.5, 90.0, "Idlewood Main Boulevard");

	print("[TRAFFIC CAMERA] 3 Kamera tilang elektronik ETLE Los Santos aktif.");
	return 1;
}

stock TrafficCamera_Register(slot, Float:x, Float:y, Float:z, Float:limit, const name[])
{
	if (slot < 0 || slot >= MAX_SPEED_CAMERAS) return 0;
	gSpeedCameras[slot][camX] = x;
	gSpeedCameras[slot][camY] = y;
	gSpeedCameras[slot][camZ] = z;
	gSpeedCameras[slot][camLimitKmh] = limit;
	format(gSpeedCameras[slot][camName], 48, "%s", name);

	// Tiang kamera speed trap (Model 18880 / 1224)
	gSpeedCameras[slot][camObject] = CreateObject(18880, x, y, z, 0.0, 0.0, 0.0, 150.0);

	new lbl[160];
	format(lbl, sizeof(lbl),
		"{FF3B30}[ KAMERA SPEED TRAP ETLE ]\n{FFFFFF}%s\n{FFFF00}Batas Maksimal: %.0f KM/J\n{FFCC00}Denda Tilang: $%d",
		name, limit, SPEED_CAMERA_TICKET_FEE);

	gSpeedCameras[slot][camLabel] = Create3DTextLabel(lbl, 0xFF3B30FF, x, y, z + 2.5, 25.0, 0, true);
	return 1;
}

stock ProcessTrafficCameraTick(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;
	if (GetPlayerState(playerid) != PLAYER_STATE_DRIVER) return 0;

	new vehicleid = GetPlayerVehicleID(playerid);
	if (vehicleid < 1) return 0;

	// Petugas kepolisian atau ambulans on-duty yang sedang darurat dibebaskan dari tilang
	if (gPlayerData[playerid][pDuty] && (gPlayerData[playerid][pFaction] == FACTION_POLICE || gPlayerData[playerid][pFaction] == FACTION_MEDIC)) {
		return 0;
	}

	new now = gettime();
	if (now < gPlayerCameraCooldown[playerid]) return 0;

	new Float:speed = TD_GetVehicleSpeedKmh(vehicleid);

	for (new i = 0; i < MAX_SPEED_CAMERAS; i++) {
		if (gSpeedCameras[i][camLimitKmh] <= 0.0) continue;

		if (IsPlayerNearPoint(playerid, gSpeedCameras[i][camX], gSpeedCameras[i][camY], gSpeedCameras[i][camZ], 18.0)) {
			if (speed > gSpeedCameras[i][camLimitKmh]) {
				// Terkena tilang kecepatan
				gPlayerCameraCooldown[playerid] = now + SPEED_TRAP_COOLDOWN;

				gPlayerData[playerid][pTickets]++;
				gPlayerData[playerid][pTicketAmount] += SPEED_CAMERA_TICKET_FEE;
				DB_SavePlayerData(playerid);

				// Efek suara jepretan kamera tilang
				PlayerPlaySound(playerid, 1133, 0.0, 0.0, 0.0);

				new alertMsg[128];
				if (gPlayerData[playerid][pLanguage] == LANG_EN) {
					format(alertMsg, sizeof(alertMsg),
						"[ETLE CAMERA] Speed violation: %.0f KM/H (Limit: %.0f KM/H)! Fine: $%d (/payticket)",
						speed, gSpeedCameras[i][camLimitKmh], SPEED_CAMERA_TICKET_FEE);
				} else {
					format(alertMsg, sizeof(alertMsg),
						"[KAMERA ETLE] Pelanggaran kecepatan: %.0f KM/J (Batas: %.0f KM/J)! Denda: $%d (/payticket)",
						speed, gSpeedCameras[i][camLimitKmh], SPEED_CAMERA_TICKET_FEE);
				}
				ShowPlayerAlertEx(playerid, alertMsg, 4000);

				// Broadcast ke radio kepolisian jika melaju sangat kencang (> 140 KM/J)
				if (speed > 140.0) {
					new radioAlert[160];
					new vehName[32];
					GetVehicleModelName(GetVehicleModel(vehicleid), vehName, sizeof(vehName));
					format(radioAlert, sizeof(radioAlert),
						"[ETLE SPEED TRAP] %s berkecepatan tinggi (%.0f KM/J) terdeteksi di %s!",
						vehName, speed, gSpeedCameras[i][camName]);

					for (new p = 0; p < MAX_PLAYERS; p++) {
						if (IsPlayerConnected(p) && gPlayerData[p][pLoggedIn] && gPlayerData[p][pDuty] && gPlayerData[p][pFaction] == FACTION_POLICE) {
							SendClientMessage(p, COLOR_RADIO, radioAlert);
						}
					}
				}
				return 1;
			}
		}
	}
	return 0;
}

stock ProcessPayTrafficTickets(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;

	if (gPlayerData[playerid][pTickets] <= 0 || gPlayerData[playerid][pTicketAmount] <= 0) {
		if (gPlayerData[playerid][pLanguage] == LANG_EN) SendClientMessage(playerid, COLOR_INFO, "INFO: You have no pending traffic citations.");
		else SendClientMessage(playerid, COLOR_INFO, "INFO: Anda tidak memiliki tunggakan tilang.");
		return 0;
	}

	new total = gPlayerData[playerid][pTicketAmount];
	if (gPlayerData[playerid][pBank] < total && gPlayerData[playerid][pMoney] < total) {
		new errStr[128];
		if (gPlayerData[playerid][pLanguage] == LANG_EN) format(errStr, sizeof(errStr), "ERROR: Insufficient funds to pay traffic citations ($%d required).", total);
		else format(errStr, sizeof(errStr), "ERROR: Dana tidak mencukupi untuk melunasi denda tilang (Dibutuhkan: $%d).", total);
		SendClientMessage(playerid, COLOR_ERROR, errStr);
		return 0;
	}

	if (gPlayerData[playerid][pBank] >= total) {
		TakePlayerBankEx(playerid, total, "pelunasan tilang ETLE");
	} else {
		TakePlayerMoneyEx(playerid, total, "pelunasan tilang ETLE");
	}

	new count = gPlayerData[playerid][pTickets];
	gPlayerData[playerid][pTickets] = 0;
	gPlayerData[playerid][pTicketAmount] = 0;
	DB_SavePlayerData(playerid);

	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
	new succStr[128];
	if (gPlayerData[playerid][pLanguage] == LANG_EN) format(succStr, sizeof(succStr), "SUKSES: Successfully paid %d traffic citation(s) totaling $%d.", count, total);
	else format(succStr, sizeof(succStr), "SUKSES: Berhasil melunasi %d surat tilang sebesar $%d.", count, total);
	SendClientMessage(playerid, COLOR_SUCCESS, succStr);
	return 1;
}
