#if defined _INC_rp_ac_vehicles
	#endinput
#endif
#define _INC_rp_ac_vehicles

// ---------------------------------------------------------------------------
// Anti-Cheat: Car Slapper, Vehicle Teleport, Speed & Unoccupied Vehicle Security
// ---------------------------------------------------------------------------

stock AC_CheckVehicleSpeed(playerid)
{
	if (!IsPlayerConnected(playerid) || GetPlayerState(playerid) != PLAYER_STATE_DRIVER) return 1;

	new veh = GetPlayerVehicleID(playerid);
	if (veh > 0) {
		new Float:vx, Float:vy, Float:vz;
		GetVehicleVelocity(veh, vx, vy, vz);
		new Float:vehSpeed = floatsqroot(vx*vx + vy*vy + vz*vz) * 180.0;

		if (vehSpeed > 240.0) {
			new model = GetVehicleModel(veh);
			// Kecualikan pesawat jet (Hydra / Shamal)
			if (model != 520 && model != 519 && model != 511) {
				SetVehicleVelocity(veh, vx * 0.4, vy * 0.4, vz * 0.4);
				AC_AlertAdmins(playerid, "Vehicle Speed Hack", "Kecepatan melebihi 240 KM/J");
				return 0;
			}
		}
	}
	return 1;
}

// Menangkal eksploit Car Slapper / Troll Car Crasher (mod_s0beit & CLEO)
stock AC_OnUnoccupiedVehicleUpdate(vehicleid, playerid, Float:new_x, Float:new_y, Float:new_z)
{
	if (!IsValidVehicle(vehicleid)) return 0;

	new Float:vx, Float:vy, Float:vz;
	GetVehiclePos(vehicleid, vx, vy, vz);

	// Hitung lompatan jarak posisi kendaraan kosong yang dilaporkan client
	new Float:deltaDist = floatsqroot((new_x-vx)*(new_x-vx) + (new_y-vy)*(new_y-vy) + (new_z-vz)*(new_z-vz));

	// Jika lompatan posisi > 15 meter tanpa pengemudi di dalamnya, tolak paket!
	if (deltaDist > 15.0) {
		if (IsPlayerConnected(playerid)) {
			new Float:px, Float:py, Float:pz;
			GetPlayerPos(playerid, px, py, pz);
			new Float:playerDist = floatsqroot((px-vx)*(px-vx) + (py-vy)*(py-vy) + (pz-vz)*(pz-vz));

			if (playerDist > 75.0) {
				AC_AlertAdmins(playerid, "Car Slapper / Crasher Hack", "Menggerakkan mobil kosong dari jarak jauh");
				return 0; // Tolak sinkronisasi
			}
		}
		return 0; // Tolak paket exploit
	}
	return 1;
}
