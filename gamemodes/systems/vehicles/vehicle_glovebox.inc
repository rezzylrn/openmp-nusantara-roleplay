#if defined _INC_rp_vehicle_glovebox
	#endinput
#endif
#define _INC_rp_vehicle_glovebox

/*
==============================================================================
 vehicle_glovebox.inc
 NUSANTARA ROLEPLAY - Vehicle Dashboard Glovebox Storage & Vehicle STNK Registration
==============================================================================
*/

static gPlayerGloveboxVeh[MAX_PLAYERS];

stock ShowVehicleGloveboxDialog(playerid, slot)
{
	if (slot < 0 || slot >= MAX_OWNED_VEHICLES || !gVehicleData[slot][vUsed]) return 0;
	if (gVehicleData[slot][vOwnerID] != gPlayerData[playerid][pID]) {
		SendErr(playerid, "Anda tidak memiliki akses kunci untuk laci dashboard kendaraan ini.");
		return 0;
	}
	gPlayerGloveboxVeh[playerid] = slot;

	new vname[32];
	GetVehicleModelName(gVehicleData[slot][vModel], vname, sizeof(vname));

	new body[512];
	format(body, sizeof(body),
		"{FFFFFF}1. Periksa STNK Resmi Kendaraan ({FFFF00}%s{FFFFFF})\n\
{FFFFFF}2. Simpan Uang Tunai ke Laci Dashboard (Maks. $500)\n\
{FFFFFF}3. Ambil Uang Tunai dari Laci Dashboard", vname);

	Dialog_Show(playerid, DialogVehicleGlovebox, DIALOG_STYLE_LIST,
		"{FFFFFF}LACI DASHBOARD KENDARAAN (GLOVEBOX)", body, "Pilih", "Tutup");
	return 1;
}

Dialog:DialogVehicleGlovebox(playerid, response, listitem, inputtext[])
{
	if (!response) return 1;
	new slot = gPlayerGloveboxVeh[playerid];
	if (slot < 0 || slot >= MAX_OWNED_VEHICLES || !gVehicleData[slot][vUsed]) return 1;

	switch (listitem) {
		case 0: { // 1. Periksa STNK
			new vname[32];
			GetVehicleModelName(gVehicleData[slot][vModel], vname, sizeof(vname));

			new stnkInfo[512];
			format(stnkInfo, sizeof(stnkInfo),
				"{FFFFFF}PEMERINTAH KOTA LOS SANTOS - DOKUMEN STNK RESMI\n\
--------------------------------------------------\n\
{FFFFFF}Nomor Register DBID: {FFFF00}#NR-REG-%04d\n\
{FFFFFF}Model Kendaraan: {FFFFFF}%s\n\
{FFFFFF}Nomor Plat Resmi: {2ECC71}%s\n\
{FFFFFF}ID Pemilik Terdaftar: {FFFFFF}%d\n\
{FFFFFF}Status Pajak Kendaraan: {2ECC71}AKTIF / TERDAFTAR SAGOV\n\
--------------------------------------------------",
				gVehicleData[slot][vDBID], vname, gVehicleData[slot][vPlate], gVehicleData[slot][vOwnerID]);

			Dialog_Show(playerid, DialogVehicleSTNKView, DIALOG_STYLE_MSGBOX,
				"{2ECC71}SURAT TANDA NOMOR KENDARAAN (STNK)", stnkInfo, "Tutup", "");
		}
		case 1: { // 2. Simpan Uang
			Dialog_Show(playerid, DialogGloveboxDeposit, DIALOG_STYLE_INPUT,
				"{FFFFFF}SIMPAN UANG KE GLOVEBOX",
				"{FFFFFF}Masukkan jumlah uang tunai yang ingin disimpan (Maks. $500 total):", "Simpan", "Batal");
		}
		case 2: { // 3. Ambil Uang
			new str[128];
			format(str, sizeof(str), "{FFFFFF}Saldo uang di laci dashboard: {2ECC71}$%d\n{FFFFFF}Masukkan jumlah yang ingin diambil:", gVehicleData[slot][vTrunk]);
			Dialog_Show(playerid, DialogGloveboxWithdraw, DIALOG_STYLE_INPUT,
				"{FFFFFF}AMBIL UANG DARI GLOVEBOX", str, "Ambil", "Batal");
		}
	}
	return 1;
}

Dialog:DialogVehicleSTNKView(playerid, response, listitem, inputtext[])
{
	return 1;
}

Dialog:DialogGloveboxDeposit(playerid, response, listitem, inputtext[])
{
	if (!response) return 1;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return 1;

	new slot = gPlayerGloveboxVeh[playerid];
	if (slot < 0 || slot >= MAX_OWNED_VEHICLES || !gVehicleData[slot][vUsed]) return 1;

	new vehHandle = gVehicleData[slot][vVehicleHandle];
	if (vehHandle <= 0 || !IsValidVehicle(vehHandle)) return 1;
	new Float:vx, Float:vy, Float:vz;
	GetVehiclePos(vehHandle, vx, vy, vz);
	if (GetPlayerDistanceFromPoint(playerid, vx, vy, vz) > 6.0) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda terlalu jauh dari kendaraan.");
		return 1;
	}

	new amount = strval(inputtext);
	if (amount <= 0 || amount > 500) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Jumlah harus antara $1 sampai $500.");
		return 1;
	}

	if (gPlayerData[playerid][pMoney] < amount) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Uang tunai Anda tidak mencukupi.");
		return 1;
	}

	if (gVehicleData[slot][vTrunk] + amount > 500) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Laci dashboard hanya dapat menampung maksimal $500 total.");
		return 1;
	}

	if (!TakePlayerMoneyEx(playerid, amount, "simpan di glovebox")) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Gagal memproses penyimpanan uang.");
		return 1;
	}
	gVehicleData[slot][vTrunk] += amount;
	DB_SaveVehicle(slot);

	new msg[144];
	format(msg, sizeof(msg), "[GLOVEBOX] Anda menyimpan $%d ke dalam laci dashboard mobil.", amount);
	SendClientMessage(playerid, COLOR_SUCCESS, msg);
	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
	return 1;
}

Dialog:DialogGloveboxWithdraw(playerid, response, listitem, inputtext[])
{
	if (!response) return 1;
	if (gPlayerData[playerid][pInjured] || gPlayerData[playerid][pCuffed]) return 1;

	new slot = gPlayerGloveboxVeh[playerid];
	if (slot < 0 || slot >= MAX_OWNED_VEHICLES || !gVehicleData[slot][vUsed]) return 1;

	new vehHandle = gVehicleData[slot][vVehicleHandle];
	if (vehHandle <= 0 || !IsValidVehicle(vehHandle)) return 1;
	new Float:vx, Float:vy, Float:vz;
	GetVehiclePos(vehHandle, vx, vy, vz);
	if (GetPlayerDistanceFromPoint(playerid, vx, vy, vz) > 6.0) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Anda terlalu jauh dari kendaraan.");
		return 1;
	}

	new amount = strval(inputtext);
	if (amount <= 0) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Jumlah tidak valid.");
		return 1;
	}

	if (gVehicleData[slot][vTrunk] < amount) {
		SendClientMessage(playerid, COLOR_ERROR, "[ERROR] Saldo di laci dashboard tidak mencukupi.");
		return 1;
	}

	gVehicleData[slot][vTrunk] -= amount;
	DB_SaveVehicle(slot);
	GivePlayerMoneyEx(playerid, amount, "ambil dari glovebox");

	new msg[144];
	format(msg, sizeof(msg), "[GLOVEBOX] Anda mengambil $%d dari laci dashboard mobil.", amount);
	SendClientMessage(playerid, COLOR_SUCCESS, msg);
	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
	return 1;
}

stock ProcessInspectSTNK(playerid, targetid = INVALID_PLAYER_ID)
{
	new vehid = 0;
	if (targetid != INVALID_PLAYER_ID && IsPlayerConnected(targetid) && IsPlayerInAnyVehicle(targetid)) {
		vehid = GetPlayerVehicleID(targetid);
	} else if (IsPlayerInAnyVehicle(playerid)) {
		vehid = GetPlayerVehicleID(playerid);
	} else {
		new slotNear = Vehicle_FindNearest(playerid, 4.0);
		if (slotNear != -1) vehid = gVehicleData[slotNear][vVehicleHandle];
	}

	if (vehid <= 0 || !IsValidVehicle(vehid)) {
		SendErr(playerid, "Tidak ada kendaraan di dekat Anda atau Anda tidak berada di dalam mobil.");
		return 0;
	}

	new slot = Vehicle_GetSlot(vehid);
	if (slot == -1) {
		SendErr(playerid, "Ini adalah kendaraan dinas pemerintahan / kota (Tidak memiliki STNK sipil).");
		return 0;
	}

	new vname[32];
	GetVehicleModelName(gVehicleData[slot][vModel], vname, sizeof(vname));

	new stnkInfo[512];
	format(stnkInfo, sizeof(stnkInfo),
		"{FFFFFF}KANTOR SAMSAT LOS SANTOS - DOKUMEN STNK KENDARAAN\n\
--------------------------------------------------\n\
{FFFFFF}Nomor Registrasi: {FFFF00}#NR-REG-%04d\n\
{FFFFFF}Model Unit: {FFFFFF}%s\n\
{FFFFFF}Tanda Nomor Kendaraan (Plat): {2ECC71}%s\n\
{FFFFFF}Nomor Induk Pemilik: {FFFFFF}%d\n\
{FFFFFF}Legalitas: {2ECC71}SAH DAN TERDAFTAR SECARA RESMI\n\
--------------------------------------------------",
		gVehicleData[slot][vDBID], vname, gVehicleData[slot][vPlate], gVehicleData[slot][vOwnerID]);

	Dialog_Show(playerid, DialogVehicleSTNKView, DIALOG_STYLE_MSGBOX,
		"{2ECC71}DOKUMEN RESMI STNK KENDARAAN", stnkInfo, "Selesai", "");
	return 1;
}
