#if defined _INC_rp_rental
	#endinput
#endif
#define _INC_rp_rental

/*
==============================================================================
 rental.inc
 NUSANTARA ROLEPLAY - Public Bicycle & Starter Vehicle Rental Stations
==============================================================================
*/

#define MAX_RENTAL_STATIONS     4
#define RANGE_RENTAL_STATION    3.5

enum E_RENTAL_STATION {
	Float:renX,
	Float:renY,
	Float:renZ,
	Float:renSpawnX,
	Float:renSpawnY,
	Float:renSpawnZ,
	Float:renSpawnA,
	renName[48],
	renPickup,
	Text3D:renLabel
};

new gRentalStations[MAX_RENTAL_STATIONS][E_RENTAL_STATION];
new gPlayerRentalVeh[MAX_PLAYERS] = {INVALID_VEHICLE_ID, ...};

stock Rental_Init()
{
	for (new i = 0; i < MAX_PLAYERS; i++) {
		gPlayerRentalVeh[i] = INVALID_VEHICLE_ID;
	}

	// 1. Stasiun Kereta Unity Station (Pintu Kedatangan Warga Baru)
	Rental_RegisterStation(0, 1745.0, -1865.0, 13.5,
		1748.0, -1862.0, 13.5, 90.0, "Stasiun Kereta Unity Station");

	// 2. Pershing Square Civic Center (Tengah Kota)
	Rental_RegisterStation(1, 1465.0, -1725.0, 13.5,
		1460.0, -1725.0, 13.5, 270.0, "Pershing Square Civic Center");

	// 3. Dermaga Pantai Santa Maria Beach
	Rental_RegisterStation(2, 380.0, -2035.0, 8.0,
		385.0, -2035.0, 8.0, 180.0, "Santa Maria Beach Promenade");

	print("[RENTAL] 3 Stasiun rental sepeda & kendaraan umum Nusantara aktif.");
	return 1;
}

stock Rental_RegisterStation(slot, Float:x, Float:y, Float:z, Float:sx, Float:sy, Float:sz, Float:sa, const name[])
{
	if (slot < 0 || slot >= MAX_RENTAL_STATIONS) return 0;
	gRentalStations[slot][renX] = x;
	gRentalStations[slot][renY] = y;
	gRentalStations[slot][renZ] = z;
	gRentalStations[slot][renSpawnX] = sx;
	gRentalStations[slot][renSpawnY] = sy;
	gRentalStations[slot][renSpawnZ] = sz;
	gRentalStations[slot][renSpawnA] = sa;
	format(gRentalStations[slot][renName], 48, "%s", name);

	gRentalStations[slot][renPickup] = CreatePickup(1274, 1, x, y, z, 0);

	new lbl[180];
	format(lbl, sizeof(lbl),
		"{00FF00}[ RENTAL SEPEDA & KENDARAAN ]\n{FFFFFF}%s\n{FFFF00}Gunakan /rentveh atau /sewa",
		name);

	gRentalStations[slot][renLabel] = Create3DTextLabel(lbl, COLOR_GREEN, x, y, z + 0.8, 15.0, 0, true);
	return 1;
}

stock ShowRentalMenuDialog(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;

	new bool:isNear = false;
	for (new i = 0; i < MAX_RENTAL_STATIONS; i++) {
		if (gRentalStations[i][renX] != 0.0 && IsPlayerNearPoint(playerid, gRentalStations[i][renX], gRentalStations[i][renY], gRentalStations[i][renZ], RANGE_RENTAL_STATION)) {
			isNear = true;
			break;
		}
	}

	if (!isNear) {
		if (gPlayerData[playerid][pLanguage] == LANG_EN) SendClientMessage(playerid, COLOR_ERROR, "ERROR: You must be at a vehicle rental station.");
		else SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus berada di stasiun rental kendaraan.");
		return 0;
	}

	new dialogContent[400];
	if (gPlayerData[playerid][pLanguage] == LANG_EN) {
		format(dialogContent, sizeof(dialogContent),
			"Vehicle Model\tRental Cost\n\
{FFFFFF}1. BMX Bicycle\t{4CD964}$15\n\
{FFFFFF}2. Mountain Bike (MTB)\t{4CD964}$20\n\
{FFFFFF}3. Faggio Scooter\t{4CD964}$50\n\
{FFFFFF}4. Previon City Car\t{4CD964}$150\n\
{FF3B30}X. Return Rented Vehicle\t{FF3B30}-");

		ShowPlayerDialog(playerid, DIALOG_RENTAL_MENU, DIALOG_STYLE_TABLIST_HEADERS,
			"PUBLIC VEHICLE RENTAL", dialogContent, "Rent", "Cancel");
	} else {
		format(dialogContent, sizeof(dialogContent),
			"Model Kendaraan\tBiaya Sewa\n\
{FFFFFF}1. Sepeda BMX\t{4CD964}$15\n\
{FFFFFF}2. Sepeda Gunung (Mountain Bike)\t{4CD964}$20\n\
{FFFFFF}3. Motor Scooter Faggio\t{4CD964}$50\n\
{FFFFFF}4. Mobil Kota Previon\t{4CD964}$150\n\
{FF3B30}X. Kembalikan Kendaraan Sewa\t{FF3B30}-");

		ShowPlayerDialog(playerid, DIALOG_RENTAL_MENU, DIALOG_STYLE_TABLIST_HEADERS,
			"RENTAL KENDARAAN UMUM", dialogContent, "Sewa", "Batal");
	}
	return 1;
}

stock OnRentalDialogResponse(playerid, listitem)
{
	new stationSlot = -1;
	for (new i = 0; i < MAX_RENTAL_STATIONS; i++) {
		if (gRentalStations[i][renX] != 0.0 && IsPlayerNearPoint(playerid, gRentalStations[i][renX], gRentalStations[i][renY], gRentalStations[i][renZ], RANGE_RENTAL_STATION)) {
			stationSlot = i;
			break;
		}
	}
	if (stationSlot == -1) return 0;

	// Opsi 4: Kembalikan kendaraan sewa
	if (listitem == 4) {
		if (gPlayerRentalVeh[playerid] != INVALID_VEHICLE_ID) {
			DestroyVehicle(gPlayerRentalVeh[playerid]);
			gPlayerRentalVeh[playerid] = INVALID_VEHICLE_ID;
			if (gPlayerData[playerid][pLanguage] == LANG_EN) SendClientMessage(playerid, COLOR_SUCCESS, "SUKSES: Rented vehicle returned successfully.");
			else SendClientMessage(playerid, COLOR_SUCCESS, "SUKSES: Kendaraan sewa berhasil dikembalikan.");
		} else {
			if (gPlayerData[playerid][pLanguage] == LANG_EN) SendClientMessage(playerid, COLOR_ERROR, "ERROR: You do not have any active rented vehicle.");
			else SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda tidak sedang menyewa kendaraan.");
		}
		return 1;
	}

	new model = 0;
	new cost = 0;
	new vehName[24];

	switch (listitem) {
		case 0: { model = 481; cost = 15; format(vehName, sizeof(vehName), "Sepeda BMX"); }
		case 1: { model = 510; cost = 20; format(vehName, sizeof(vehName), "Sepeda Gunung"); }
		case 2: { model = 462; cost = 50; format(vehName, sizeof(vehName), "Faggio"); }
		case 3: { model = 436; cost = 150; format(vehName, sizeof(vehName), "Previon"); }
		default: return 1;
	}

	if (gPlayerData[playerid][pMoney] < cost) {
		new errStr[128];
		if (gPlayerData[playerid][pLanguage] == LANG_EN) format(errStr, sizeof(errStr), "ERROR: Insufficient funds to rent this vehicle ($%d required).", cost);
		else format(errStr, sizeof(errStr), "ERROR: Uang tunai tidak mencukupi untuk menyewa kendaraan ini (Biaya: $%d).", cost);
		SendClientMessage(playerid, COLOR_ERROR, errStr);
		return 1;
	}

	// Hapus kendaraan sewa lama jika ada
	if (gPlayerRentalVeh[playerid] != INVALID_VEHICLE_ID) {
		DestroyVehicle(gPlayerRentalVeh[playerid]);
		gPlayerRentalVeh[playerid] = INVALID_VEHICLE_ID;
	}

	TakePlayerMoneyEx(playerid, cost, "sewa kendaraan umum");

	new v = CreateVehicle(model,
		gRentalStations[stationSlot][renSpawnX],
		gRentalStations[stationSlot][renSpawnY],
		gRentalStations[stationSlot][renSpawnZ],
		gRentalStations[stationSlot][renSpawnA],
		1, 1, 300);

	SetVehicleNumberPlate(v, "SEWA");
	SetVehicleToRespawn(v);

	// Nyalakan mesin jika sepeda
	if (model == 481 || model == 510) {
		Vehicle_SetEngineOn(v, true);
	}

	gPlayerRentalVeh[playerid] = v;
	AC_PutPlayerInVehicle(playerid, v, 0);

	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
	new succRent[144];
	if (gPlayerData[playerid][pLanguage] == LANG_EN) format(succRent, sizeof(succRent), "SUKSES: You rented a %s for $%d. Type /unrentveh when finished.", vehName, cost);
	else format(succRent, sizeof(succRent), "SUKSES: Anda telah menyewa %s seharga $%d. Gunakan /unrentveh untuk mengembalikan.", vehName, cost);
	SendClientMessage(playerid, COLOR_SUCCESS, succRent);
	return 1;
}

stock ProcessRentalDisconnect(playerid)
{
	if (gPlayerRentalVeh[playerid] != INVALID_VEHICLE_ID) {
		DestroyVehicle(gPlayerRentalVeh[playerid]);
		gPlayerRentalVeh[playerid] = INVALID_VEHICLE_ID;
	}
	return 1;
}
