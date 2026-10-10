#if defined _INC_rp_ac_troll
	#endinput
#endif
#define _INC_rp_ac_troll

// ---------------------------------------------------------------------------
// Anti-Cheat: Troll Exploits, Pickup Teleport & Vehicle Component Crasher
// Countering: CLEO Pickup Teleport, mod_sa Invalid Tuning Crasher
// ---------------------------------------------------------------------------

// Validasi pengambilan pickup (Anti-Pickup Teleport / Remote Money/Weapon Pickup)
stock AC_ValidatePickup(playerid, pickupid)
{
	if (!IsPlayerConnected(playerid)) return 0;

	new Float:pkX, Float:pkY, Float:pkZ;
	if (GetPickupPos(pickupid, pkX, pkY, pkZ)) {
		new Float:px, Float:py, Float:pz;
		GetPlayerPos(playerid, px, py, pz);

		new Float:dx = px - pkX;
		new Float:dy = py - pkY;
		new Float:dz = pz - pkZ;
		new Float:dist = floatsqroot(dx*dx + dy*dy + dz*dz);

		// Toleransi pengambilan pickup sah adalah dalam radius 5.5 meter
		if (dist > 5.5) {
			new detail[64];
			format(detail, sizeof(detail), "Jarak ke pickup ID %d: %.1fm (Maks: 5.5m)", pickupid, dist);
			AC_AlertAdmins(playerid, "Pickup Teleport Hack", detail);
			return 0; // Batalkan pengambilan pickup
		}
	}
	return 1;
}

// Validasi modifikasi komponen kendaraan (Anti-Vehicle Crasher / Invalid Tuning)
stock AC_ValidateVehicleMod(playerid, vehicleid, componentid)
{
	if (!IsPlayerConnected(playerid) || !IsValidVehicle(vehicleid)) return 0;

	// 1. Cek tipe komponen sah via open.mp CARMODTYPE
	new CARMODTYPE:modType = GetVehicleComponentType(componentid);
	if (_:modType < 0 || modType == CARMODTYPE_NONE) {
		new detail[48];
		format(detail, sizeof(detail), "Komponen crash ID: %d", componentid);
		AC_AlertAdmins(playerid, "Invalid Tuning Component Crasher", detail);
		return 0;
	}

	// 2. Pemain harus berada di dalam kendaraan yang dimodifikasi
	if (GetPlayerVehicleID(playerid) != vehicleid) {
		AC_AlertAdmins(playerid, "Remote Vehicle Tuning", "Memodifikasi mobil dari luar kendaraan");
		return 0;
	}

	return 1;
}
