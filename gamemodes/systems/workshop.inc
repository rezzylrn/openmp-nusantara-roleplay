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

	RP_ShowPlayerDialog(playerid, DIALOG_WORKSHOP_MENU, DIALOG_STYLE_LIST,
		"BENGKEL MODIFIKASI WORKSHOP", str, "Pasang", "Batal");
	return 1;
}

stock ApplyVehicleUpgrade(vehid, componentid)
{
	if (vehid <= 0 || !IsValidVehicle(vehid)) return 0;
	AddVehicleComponent(vehid, componentid);
	return 1;
}

// ---------------------------------------------------------------------------
// Handler tombol "Pasang" pada menu workshop. Sebelumnya dialog memakai id
// DIALOG_HELP yang tidak punya handler -> seluruh menu modifikasi mati.
// ---------------------------------------------------------------------------
stock OnWorkshopMenuResponse(playerid, listitem)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return 0;
	if (!IsPlayerNearWorkshop(playerid)) {
		SendErr(playerid, "Anda harus berada di Bengkel Modifikasi Idlewood.");
		return 0;
	}

	// Harga & komponen tiap baris menu
	static const wsPrice[] = { 1500, 2500, 3000, 1200, 900, 500 };
	static const wsComp[]  = { 1025, 1087, 1010, 1000, 1018, 0 }; // 0 = cat ulang (tanpa komponen)

	if (listitem < 0 || listitem >= sizeof(wsPrice)) return 0;

	new veh = GetPlayerVehicleID(playerid);
	if (veh == 0 || !IsValidVehicle(veh) || GetPlayerState(playerid) != PLAYER_STATE_DRIVER) {
		SendErr(playerid, "Anda harus tetap berada di kursi pengemudi kendaraan.");
		return 0;
	}

	new cost = wsPrice[listitem];
	if (gPlayerData[playerid][pMoney] < cost) {
		SendErr(playerid, "Uang tunai tidak mencukupi ($%d).", cost);
		return 0;
	}

	if (!TakePlayerMoneyEx(playerid, cost, "modifikasi workshop")) {
		SendErr(playerid, "Pembayaran modifikasi gagal. Uang tunai tidak mencukupi.");
		return 0;
	}

	if (listitem == 5) {
		// Cat ulang bodi: warna acak
		ChangeVehicleColor(veh, random(126), random(126));
	} else {
		if (wsComp[listitem] == 0) return 0;
		AddVehicleComponent(veh, wsComp[listitem]);
	}

	PlayerPlaySound(playerid, 1133, 0.0, 0.0, 0.0);
	SendSucc(playerid, "Modifikasi berhasil dipasang! Biaya: $%d.", cost);

	// Simpan bila kendaraan milik pemain
	new slot = Vehicle_GetSlot(veh);
	if (slot != -1) DB_SaveVehicle(slot);
	return 1;
}
