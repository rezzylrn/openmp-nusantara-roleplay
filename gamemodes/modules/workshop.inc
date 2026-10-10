#if defined _INC_rp_workshop
	#endinput
#endif
#define _INC_rp_workshop

// ---------------------------------------------------------------------------
// Workshop & Vehicle Tuning System
// ---------------------------------------------------------------------------

#define WORKSHOP_IDLEWOOD_X     1995.0
#define WORKSHOP_IDLEWOOD_Y     -2060.0
#define WORKSHOP_IDLEWOOD_Z     13.5

stock IsPlayerNearWorkshop(playerid)
{
	if (!IsPlayerConnected(playerid)) return 0;
	return IsPlayerInRangeOfPoint(playerid, 12.0, WORKSHOP_IDLEWOOD_X, WORKSHOP_IDLEWOOD_Y, WORKSHOP_IDLEWOOD_Z);
}

stock ShowWorkshopMenu(playerid)
{
	new veh = GetPlayerVehicleID(playerid);
	if (veh == 0 || GetPlayerState(playerid) != PLAYER_STATE_DRIVER) {
		SendErr(playerid, "Anda harus berada di kursi pengemudi kendaraan di dalam bengkel.");
		return 0;
	}

	new str[400];
	format(str, sizeof(str),
		"1. Velg Mobil Sport (Rimshine/Mega/Shadow) - $1,500\n\
2. Sistem Hidrolik (Hydraulics) - $2,500\n\
3. Tabung Nitro 10x (10x Nitrous Oxide) - $3,000\n\
4. Spoiler Belakang Aerodinamis - $1,200\n\
5. Modifikasi Knalpot Racing - $900\n\
6. Cat Ulang Bodi Kendaraan - $500");

	ShowPlayerDialog(playerid, DIALOG_HELP, DIALOG_STYLE_LIST,
		"BENGKEL MODIFIKASI WORKSHOP", str, "Pasang", "Batal");
	return 1;
}

stock ApplyVehicleUpgrade(vehid, componentid)
{
	if (vehid <= 0 || !IsValidVehicle(vehid)) return 0;
	AddVehicleComponent(vehid, componentid);
	return 1;
}
