#if defined _INC_traffic_camera
	#endinput
#endif
#define _INC_traffic_camera

/*
==============================================================================
 traffic_camera.inc
 NUSANTARA ROLEPLAY - Dynamic Electronic Traffic Law Enforcement (ETLE)
==============================================================================
*/

#define MAX_SPEED_CAMERAS       30
#define DEFAULT_CAMERA_FEE      150
#define SPEED_TRAP_COOLDOWN     10

enum E_SPEED_CAMERA {
	bool:camUsed,
	camDBID,
	Float:camX,
	Float:camY,
	Float:camZ,
	Float:camLimitKmh,
	camFee,
	camName[48],
	camObject,
	Text3D:camLabel
};

new gSpeedCameras[MAX_SPEED_CAMERAS][E_SPEED_CAMERA];
new gTotalSpeedCameras = 0;
new gPlayerCameraCooldown[MAX_PLAYERS];

forward OnTrafficCamerasLoaded();
forward OnTrafficCameraCreated(slot);

stock TrafficCamera_Init()
{
	for (new i = 0; i < MAX_PLAYERS; i++) {
		gPlayerCameraCooldown[i] = 0;
	}

	for (new i = 0; i < MAX_SPEED_CAMERAS; i++) {
		gSpeedCameras[i][camUsed] = false;
		gSpeedCameras[i][camDBID] = 0;
		gSpeedCameras[i][camObject] = INVALID_OBJECT_ID;
		gSpeedCameras[i][camLabel] = Text3D:INVALID_3DTEXT_ID;
	}
	gTotalSpeedCameras = 0;

	if (gDBReady) {
		mysql_tquery(gDB, "SELECT * FROM `etle_cameras` ORDER BY `id` ASC", "OnTrafficCamerasLoaded", "");
	} else {
		TrafficCamera_CreateDefaults();
	}
	return 1;
}

public OnTrafficCamerasLoaded()
{
	new rows = cache_num_rows();
	if (rows == 0) {
		TrafficCamera_CreateDefaults();
		return 1;
	}

	for (new i = 0; i < rows && i < MAX_SPEED_CAMERAS; i++) {
		gSpeedCameras[i][camUsed] = true;
		cache_get_value_name_int(i, "id", gSpeedCameras[i][camDBID]);
		cache_get_value_name_float(i, "posx", gSpeedCameras[i][camX]);
		cache_get_value_name_float(i, "posy", gSpeedCameras[i][camY]);
		cache_get_value_name_float(i, "posz", gSpeedCameras[i][camZ]);
		cache_get_value_name_float(i, "speed_limit", gSpeedCameras[i][camLimitKmh]);
		cache_get_value_name_int(i, "ticket_fee", gSpeedCameras[i][camFee]);
		cache_get_value_name(i, "name", gSpeedCameras[i][camName], 48);

		TrafficCamera_CreateVisuals(i);
		gTotalSpeedCameras++;
	}

	printf("[ETLE] %d Kamera tilang elektronik dinamis berhasil dimuat dari MySQL.", gTotalSpeedCameras);
	return 1;
}

stock TrafficCamera_CreateVisuals(slot)
{
	if (slot < 0 || slot >= MAX_SPEED_CAMERAS || !gSpeedCameras[slot][camUsed]) return 0;

	// Tiang kamera speed trap (Model 18880)
	gSpeedCameras[slot][camObject] = CreateObject(18880,
		gSpeedCameras[slot][camX],
		gSpeedCameras[slot][camY],
		gSpeedCameras[slot][camZ],
		0.0, 0.0, 0.0, 150.0);

	new lbl[200];
	format(lbl, sizeof(lbl),
		"{FF3B30}[ KAMERA ETLE #%d ]\n{FFFFFF}%s\n{FFFF00}Batas: %.0f KM/J | {FFCC00}Denda: $%d",
		slot + 1, gSpeedCameras[slot][camName], gSpeedCameras[slot][camLimitKmh], gSpeedCameras[slot][camFee]);

	gSpeedCameras[slot][camLabel] = Create3DTextLabel(lbl, 0xFF3B30FF,
		gSpeedCameras[slot][camX],
		gSpeedCameras[slot][camY],
		gSpeedCameras[slot][camZ] + 2.5,
		25.0, 0, true);
	return 1;
}

stock TrafficCamera_Create(Float:x, Float:y, Float:z, Float:limit, fee, const name[])
{
	new slot = -1;
	for (new i = 0; i < MAX_SPEED_CAMERAS; i++) {
		if (!gSpeedCameras[i][camUsed]) {
			slot = i;
			break;
		}
	}
	if (slot == -1) return -1;

	gSpeedCameras[slot][camUsed] = true;
	gSpeedCameras[slot][camX] = x;
	gSpeedCameras[slot][camY] = y;
	gSpeedCameras[slot][camZ] = z;
	gSpeedCameras[slot][camLimitKmh] = limit;
	gSpeedCameras[slot][camFee] = (fee <= 0) ? DEFAULT_CAMERA_FEE : fee;
	format(gSpeedCameras[slot][camName], 48, "%s", name);

	TrafficCamera_CreateVisuals(slot);
	gTotalSpeedCameras++;

	if (gDBReady) {
		new query[320];
		mysql_format(gDB, query, sizeof(query),
			"INSERT INTO `etle_cameras` (`name`, `posx`, `posy`, `posz`, `speed_limit`, `ticket_fee`) \
			VALUES ('%e', %.2f, %.2f, %.2f, %.2f, %d)",
			name, x, y, z, limit, gSpeedCameras[slot][camFee]);
		mysql_tquery(gDB, query, "OnTrafficCameraCreated", "i", slot);
	}
	return slot;
}

public OnTrafficCameraCreated(slot)
{
	if (slot >= 0 && slot < MAX_SPEED_CAMERAS && gSpeedCameras[slot][camUsed]) {
		gSpeedCameras[slot][camDBID] = cache_insert_id();
	}
	return 1;
}

stock TrafficCamera_Destroy(slot)
{
	if (slot < 0 || slot >= MAX_SPEED_CAMERAS || !gSpeedCameras[slot][camUsed]) return 0;

	if (IsValidObject(gSpeedCameras[slot][camObject])) {
		DestroyObject(gSpeedCameras[slot][camObject]);
		gSpeedCameras[slot][camObject] = INVALID_OBJECT_ID;
	}
	if (gSpeedCameras[slot][camLabel] != Text3D:INVALID_3DTEXT_ID) {
		Delete3DTextLabel(gSpeedCameras[slot][camLabel]);
		gSpeedCameras[slot][camLabel] = Text3D:INVALID_3DTEXT_ID;
	}

	if (gDBReady && gSpeedCameras[slot][camDBID] > 0) {
		new query[128];
		mysql_format(gDB, query, sizeof(query), "DELETE FROM `etle_cameras` WHERE `id`=%d", gSpeedCameras[slot][camDBID]);
		mysql_tquery(gDB, query, "", "");
	}

	gSpeedCameras[slot][camUsed] = false;
	gSpeedCameras[slot][camDBID] = 0;
	gTotalSpeedCameras--;
	return 1;
}

stock TrafficCamera_CreateDefaults()
{
	print("[ETLE] Memasang kamera tilang elektronik dinamis bawaan Los Santos...");
	TrafficCamera_Create(1481.0, -1700.0, 13.5, 100.0, 150, "Pershing Square Central");
	TrafficCamera_Create(1650.0, -1120.0, 24.0, 120.0, 200, "Commerce Express Freeway");
	TrafficCamera_Create(1950.0, -1770.0, 13.5, 90.0, 150, "Idlewood Main Boulevard");
	TrafficCamera_Create(390.0, -1880.0, 7.8, 80.0, 100, "Santa Maria Highway");
	return 1;
}

stock ProcessTrafficCameraTick(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;
	if (GetPlayerState(playerid) != PLAYER_STATE_DRIVER) return 0;

	new vehicleid = GetPlayerVehicleID(playerid);
	if (vehicleid < 1) return 0;

	// Petugas kepolisian atau medis on-duty darurat dibebaskan dari tilang
	if (gPlayerData[playerid][pDuty] && (gPlayerData[playerid][pFaction] == FACTION_POLICE || gPlayerData[playerid][pFaction] == FACTION_MEDIC)) {
		return 0;
	}

	new now = gettime();
	if (now < gPlayerCameraCooldown[playerid]) return 0;

	new Float:speed = TD_GetVehicleSpeedKmh(vehicleid);

	for (new i = 0; i < MAX_SPEED_CAMERAS; i++) {
		if (!gSpeedCameras[i][camUsed] || gSpeedCameras[i][camLimitKmh] <= 0.0) continue;

		if (IsPlayerNearPoint(playerid, gSpeedCameras[i][camX], gSpeedCameras[i][camY], gSpeedCameras[i][camZ], 20.0)) {
			if (speed > gSpeedCameras[i][camLimitKmh]) {
				// Terkena tilang kecepatan
				gPlayerCameraCooldown[playerid] = now + SPEED_TRAP_COOLDOWN;

				new fee = gSpeedCameras[i][camFee];
				gPlayerData[playerid][pTickets]++;
				gPlayerData[playerid][pTicketAmount] += fee;
				DB_SavePlayerData(playerid);

				// Efek suara jepretan kamera tilang
				PlayerPlaySound(playerid, 1133, 0.0, 0.0, 0.0);

				new alertMsg[144];
				if (gPlayerData[playerid][pLanguage] == LANG_EN) {
					format(alertMsg, sizeof(alertMsg),
						"[ETLE #%d] Speed violation: %.0f KM/H (Limit: %.0f KM/H)! Fine: $%d (/payticket)",
						i + 1, speed, gSpeedCameras[i][camLimitKmh], fee);
				} else {
					format(alertMsg, sizeof(alertMsg),
						"[ETLE #%d] Pelanggaran kecepatan: %.0f KM/J (Batas: %.0f KM/J)! Denda: $%d (/payticket)",
						i + 1, speed, gSpeedCameras[i][camLimitKmh], fee);
				}
				ShowPlayerAlertEx(playerid, alertMsg, 4000);

				// Broadcast ke radio kepolisian jika melaju sangat kencang (> 135 KM/J)
				if (speed > 135.0) {
					new radioAlert[160];
					new vehName[32];
					GetVehicleModelName(GetVehicleModel(vehicleid), vehName, sizeof(vehName));
					format(radioAlert, sizeof(radioAlert),
						"[ETLE SPEED TRAP] %s melaju kencang (%.0f KM/J) di %s (#%d)!",
						vehName, speed, gSpeedCameras[i][camName], i + 1);

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

stock TrafficCamera_ResetPlayer(playerid)
{
	if (playerid >= 0 && playerid < MAX_PLAYERS) {
		gPlayerCameraCooldown[playerid] = 0;
	}
	return 1;
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

	new bool:paid = false;
	if (gPlayerData[playerid][pBank] >= total) {
		paid = (TakePlayerBankEx(playerid, total, "pelunasan tilang ETLE") == 1);
	}
	if (!paid && gPlayerData[playerid][pMoney] >= total) {
		paid = (TakePlayerMoneyEx(playerid, total, "pelunasan tilang ETLE") == 1);
	}
	if (!paid) {
		new errStr[128];
		if (gPlayerData[playerid][pLanguage] == LANG_EN) format(errStr, sizeof(errStr), "ERROR: Failed to process citation payment. Insufficient funds.");
		else format(errStr, sizeof(errStr), "ERROR: Pembayaran tilang gagal diproses. Dana tidak mencukupi.");
		SendClientMessage(playerid, COLOR_ERROR, errStr);
		return 0;
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
