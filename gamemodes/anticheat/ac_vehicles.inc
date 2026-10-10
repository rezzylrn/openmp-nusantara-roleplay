#if defined _INC_rp_ac_vehicles
	#endinput
#endif
#define _INC_rp_ac_vehicles

// ---------------------------------------------------------------------------
// Anti-Cheat: Car Jacker, Speedhack, Car Fly, Car Slapper & Crasher
// Countering: mod_sa Car Warp/CarJacker, UltraFuck Vehicle Multiplier, CLEO Slapper
// ---------------------------------------------------------------------------

// Deteksi Car Jacker / Warp Masuk Kendaraan Seketika Tanpa Animasi
stock AC_ValidateVehicleEntry(playerid, vehicleid)
{
	if (!IsPlayerConnected(playerid) || !IsValidVehicle(vehicleid)) return 1;

	new Float:vx, Float:vy, Float:vz;
	GetVehiclePos(vehicleid, vx, vy, vz);

	new Float:dx = gACData[playerid][acPreEnterX] - vx;
	new Float:dy = gACData[playerid][acPreEnterY] - vy;
	new Float:dz = gACData[playerid][acPreEnterZ] - vz;
	new Float:dist = floatsqroot(dx*dx + dy*dy + dz*dz);

	// Pada interaksi normal GTA, pemain harus berdiri di dekat pintu mobil (< 5.0m)
	// Jika jarak > 7.5m, pemain menggunakan eksploit Car Jacker / Warp Vehicle
	if (dist > 7.5) {
		new detail[64];
		format(detail, sizeof(detail), "Jarak masuk: %.1fm (Maks toleransi: 7.5m)", dist);
		AC_AlertAdmins(playerid, "Car Jacker / Remote Vehicle Warp", detail);

		// Keluarkan paksa dari kendaraan dan kembalikan ke posisi jalan kaki
		RemovePlayerFromVehicle(playerid);
		SetPlayerPos(playerid, gACData[playerid][acPreEnterX], gACData[playerid][acPreEnterY], gACData[playerid][acPreEnterZ]);
		return 0;
	}
	return 1;
}

// Deteksi Kecepatan Kendaraan & Car Fly (Airbreak Mobil)
stock AC_CheckVehicleSpeed(playerid)
{
	if (!IsPlayerConnected(playerid) || GetPlayerState(playerid) != PLAYER_STATE_DRIVER) return 1;

	new veh = GetPlayerVehicleID(playerid);
	if (veh > 0 && IsValidVehicle(veh)) {
		new Float:vx, Float:vy, Float:vz;
		GetVehicleVelocity(veh, vx, vy, vz);
		new Float:vehSpeed = floatsqroot(vx*vx + vy*vy + vz*vz) * 180.0;
		new model = GetVehicleModel(veh);

		// 1. Tangkal Speedhack pada kendaraan darat (> 230 KM/J)
		if (vehSpeed > 230.0) {
			// Kecualikan pesawat dan helikopter
			if (model != 520 && model != 519 && model != 511 && model != 592 && model != 487 && model != 497) {
				SetVehicleVelocity(veh, vx * 0.35, vy * 0.35, vz * 0.35);
				new detail[48];
				format(detail, sizeof(detail), "Kecepatan: %.1f KM/J (Maks: 230)", vehSpeed);
				AC_AlertAdmins(playerid, "Vehicle Speed Hack", detail);
				return 0;
			}
		}

		// 2. Tangkal Car Fly / Mobil Terbang (Kendaraan darat naik di udara tanpa tanjakan)
		if (model != 520 && model != 519 && model != 511 && model != 592 && model != 487 && model != 497) {
			new Float:px, Float:py, Float:pz;
			GetPlayerPos(playerid, px, py, pz);
			if (vz > 0.45 && pz > 30.0) {
				SetVehicleVelocity(veh, vx * 0.5, vy * 0.5, 0.0);
				AC_AlertAdmins(playerid, "Car Fly / Vehicle Airbreak", "Mobil darat terbang di ketinggian");
				return 0;
			}
		}
	}
	return 1;
}

// Menangkal eksploit Car Slapper / Troll Car Crasher (mod_s0beit & CLEO Slapper)
stock AC_OnUnoccupiedVehicleUpdate(vehicleid, playerid, Float:new_x, Float:new_y, Float:new_z)
{
	if (!IsValidVehicle(vehicleid)) return 0;

	new Float:vx, Float:vy, Float:vz;
	GetVehiclePos(vehicleid, vx, vy, vz);

	// Hitung lompatan jarak posisi kendaraan kosong yang dilaporkan client
	new Float:deltaDist = floatsqroot((new_x-vx)*(new_x-vx) + (new_y-vy)*(new_y-vy) + (new_z-vz)*(new_z-vz));

	// Jika lompatan posisi > 12 meter tanpa pengemudi di dalamnya, tolak paket!
	if (deltaDist > 12.0) {
		if (IsPlayerConnected(playerid)) {
			new Float:px, Float:py, Float:pz;
			GetPlayerPos(playerid, px, py, pz);
			new Float:playerDist = floatsqroot((px-vx)*(px-vx) + (py-vy)*(py-vy) + (pz-vz)*(pz-vz));

			if (playerDist > 55.0) {
				AC_AlertAdmins(playerid, "Car Slapper / Crasher Hack", "Menggerakkan mobil kosong dari jarak jauh");
				return 0; // Tolak sinkronisasi
			}
		}
		return 0; // Tolak paket exploit
	}
	return 1;
}
