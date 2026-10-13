#if defined _INC_rp_vehicle_trunk
	#endinput
#endif
#define _INC_rp_vehicle_trunk

/*
==============================================================================
 vehicle_trunk.inc
 NUSANTARA ROLEPLAY - Vehicle Trunk Storage (Cash, Items & Weapons)
==============================================================================
*/

// ---------------------------------------------------------------------------
// Sistem Bagasi Kendaraan (Trunk Storage & Interaksi)
// ---------------------------------------------------------------------------
stock Vehicle_FindNearest(playerid, Float:radi)
{
	new Float:px, Float:py, Float:pz;
	GetPlayerPos(playerid, px, py, pz);

	new closestSlot = -1;
	new Float:minDist = radi;

	for (new i = 0; i < MAX_OWNED_VEHICLES; i++) {
		if (gVehicleData[i][vUsed]) {
			new handle = gVehicleData[i][vVehicleHandle];
			if (handle > 0 && IsValidVehicle(handle)) {
				new Float:vx, Float:vy, Float:vz;
				GetVehiclePos(handle, vx, vy, vz);
				new Float:d = floatsqroot((px-vx)*(px-vx) + (py-vy)*(py-vy) + (pz-vz)*(pz-vz));
				if (d < minDist) {
					minDist = d;
					closestSlot = i;
				}
			}
		}
	}
	return closestSlot;
}

stock ShowVehicleTrunkDialog(playerid, slot)
{
	if (slot < 0 || slot >= MAX_OWNED_VEHICLES || !gVehicleData[slot][vUsed]) return 0;
	gSelectedTrunkVeh[playerid] = slot;

	new bool:isEn = (gPlayerData[playerid][pLanguage] == LANG_EN);
	new str[700];
	new itemStr[32];
	if (isEn) {
		format(str, sizeof(str), "Item / Option	Quantity\n{4CD964}[$] Cash in Trunk	$%d\n", gVehicleData[slot][vTrunk]);
	} else {
		format(str, sizeof(str), "Item / Opsi	Jumlah\n{4CD964}[$] Saldo Uang Tunai Bagasi	$%d\n", gVehicleData[slot][vTrunk]);
	}

	// Slot Senjata Api
	new gunName[32];
	if (gVehicleData[slot][vTrunkGun] != WEAPON:0 && gVehicleData[slot][vTrunkAmmo] > 0) {
		GetWeaponName(gVehicleData[slot][vTrunkGun], gunName, sizeof(gunName));
		format(str, sizeof(str), "%s{FF3B30}[SENJATA] %s	{FFCC00}%d Peluru\n", str, gunName, gVehicleData[slot][vTrunkAmmo]);
	} else {
		format(str, sizeof(str), isEn ? "%s{888888}[WEAPON SLOT] (Empty)	{888888}-\n" : "%s{888888}[SLOT SENJATA] (Kosong)	{888888}-\n", str);
	}

	for (new s = 0; s < MAX_TRUNK_SLOTS; s++) {
		new iType = gVehicleData[slot][vTrunkItemType][s];
		new iAmt  = gVehicleData[slot][vTrunkItemAmount][s];
		if (iType != ITEM_NONE && iAmt > 0) {
			GetItemName(iType, itemStr, sizeof(itemStr));
			format(str, sizeof(str), "%s{FFFF00}[Slot %d] %s	{FFFFFF}x%d\n", str, s + 1, itemStr, iAmt);
		} else {
			if (isEn) format(str, sizeof(str), "%s{888888}[Slot %d] (Empty)	{888888}-\n", str, s + 1);
			else format(str, sizeof(str), "%s{888888}[Slot %d] (Kosong)	{888888}-\n", str, s + 1);
		}
	}
	if (isEn) {
		format(str, sizeof(str), "%s{33CCFF}[+] STORE ITEM INTO TRUNK	{33CCFF}[INVENTORY]", str);
	} else {
		format(str, sizeof(str), "%s{33CCFF}[+] SIMPAN BARANG KE BAGASI	{33CCFF}[KANTUNG]", str);
	}

	new vName[32];
	GetVehicleModelName(gVehicleData[slot][vModel], vName, sizeof(vName));
	new title[64];
	if (isEn) format(title, sizeof(title), "TRUNK - %s (%s)", vName, gVehicleData[slot][vPlate]);
	else format(title, sizeof(title), "BAGASI - %s (%s)", vName, gVehicleData[slot][vPlate]);

	if (isEn) {
		ShowPlayerDialog(playerid, DIALOG_TRUNK, DIALOG_STYLE_TABLIST_HEADERS,
			title, str, "Select", "Close");
	} else {
		ShowPlayerDialog(playerid, DIALOG_TRUNK, DIALOG_STYLE_TABLIST_HEADERS,
			title, str, "Pilih", "Tutup");
	}
	return 1;
}

stock OnVehicleTrunkResponse(playerid, listitem)
{
	new slot = gSelectedTrunkVeh[playerid];
	if (slot < 0 || slot >= MAX_OWNED_VEHICLES || !gVehicleData[slot][vUsed]) return 0;

	// Opsi 0: Saldo Uang Tunai
	if (listitem == 0) {
		new prompt[144];
		format(prompt, sizeof(prompt), "1. Simpan Uang Tunai ke Bagasi (Dompet: $%d)\n2. Ambil Uang Tunai dari Bagasi (Tersedia: $%d)",
			gPlayerData[playerid][pMoney], gVehicleData[slot][vTrunk]);
		ShowPlayerDialog(playerid, DIALOG_TRUNK_DEPOSIT, DIALOG_STYLE_LIST, "KELOLA UANG BAGASI", prompt, "Pilih", "Kembali");
		return 1;
	}

	// Opsi 1: Slot Senjata Api
	if (listitem == 1) {
		new gunPrompt[256];
		new gunName[32];
		if (gVehicleData[slot][vTrunkGun] != WEAPON:0 && gVehicleData[slot][vTrunkAmmo] > 0) {
			GetWeaponName(gVehicleData[slot][vTrunkGun], gunName, sizeof(gunName));
			format(gunPrompt, sizeof(gunPrompt), "1. Ambil Senjata (%s - %d Peluru)\n2. Ganti / Simpan Senjata Yang Sedang Dipegang",
				gunName, gVehicleData[slot][vTrunkAmmo]);
		} else {
			format(gunPrompt, sizeof(gunPrompt), "1. Simpan Senjata Yang Sedang Dipegang ke Bagasi\n2. (Bagasi senjata sedang kosong)");
		}
		ShowPlayerDialog(playerid, DIALOG_TRUNK_WEAPON, DIALOG_STYLE_LIST, "KELOLA SENJATA BAGASI", gunPrompt, "Pilih", "Kembali");
		return 1;
	}

	// Opsi 2..7: Slot Item Bagasi (Ambil Barang)
	if (listitem >= 2 && listitem <= (MAX_TRUNK_SLOTS + 1)) {
		new s = listitem - 2;
		if (gVehicleData[slot][vTrunkItemType][s] == ITEM_NONE || gVehicleData[slot][vTrunkItemAmount][s] <= 0) {
			SendErr(playerid, "Slot bagasi ini kosong.");
			ShowVehicleTrunkDialog(playerid, slot);
			return 1;
		}
		gSelectedTrunkSlot[playerid] = s;
		new itemName[32];
		GetItemName(gVehicleData[slot][vTrunkItemType][s], itemName, sizeof(itemName));
		new prompt[128];
		format(prompt, sizeof(prompt), "Masukkan jumlah %s yang ingin diambil (Tersedia: %d):",
			itemName, gVehicleData[slot][vTrunkItemAmount][s]);
		ShowPlayerDialog(playerid, DIALOG_TRUNK_WITHDRAW_ITEM, DIALOG_STYLE_INPUT, "AMBIL BARANG DARI BAGASI", prompt, "Ambil", "Batal");
		return 1;
	}

	// Opsi Terakhir: [+] Simpan Barang ke Bagasi
	if (listitem == (MAX_TRUNK_SLOTS + 2)) {
		new dialogList[800];
		new itemName[32];
		dialogList[0] = '\0';
		for (new s = 0; s < MAX_INVENTORY_SLOTS; s++) {
			new iType = gPlayerItems[playerid][s][itType];
			new iAmt  = gPlayerItems[playerid][s][itAmount];
			if (iType != ITEM_NONE && iAmt > 0) {
				GetItemName(iType, itemName, sizeof(itemName));
				format(dialogList, sizeof(dialogList), "%s[Slot %d] %s (x%d)\n", dialogList, s + 1, itemName, iAmt);
			} else {
				format(dialogList, sizeof(dialogList), "%s[Slot %d] (Kosong)\n", dialogList, s + 1);
			}
		}
		ShowPlayerDialog(playerid, DIALOG_TRUNK_DEPOSIT_ITEM, DIALOG_STYLE_LIST, "PILIH BARANG DARI KANTUNG", dialogList, "Simpan", "Kembali");
		return 1;
	}
	return 1;
}

stock OnTrunkDepositCashResponse(playerid, listitem)
{
	new slot = gSelectedTrunkVeh[playerid];
	if (slot < 0 || slot >= MAX_OWNED_VEHICLES || !gVehicleData[slot][vUsed]) return 0;

	if (listitem == 0) {
		// Simpan Uang ke Bagasi
		ShowPlayerDialog(playerid, DIALOG_TRUNK_DEPOSIT, DIALOG_STYLE_INPUT,
			"SIMPAN UANG KE BAGASI", "Masukkan nominal uang tunai yang ingin disimpan ke bagasi ($1 - $5.000.000):", "Simpan", "Kembali");
	} else if (listitem == 1) {
		// Ambil Uang dari Bagasi
		ShowPlayerDialog(playerid, DIALOG_TRUNK_TAKE_CASH, DIALOG_STYLE_INPUT,
			"AMBIL UANG DARI BAGASI", "Masukkan nominal uang tunai yang ingin diambil dari bagasi ($1 - $5.000.000):", "Ambil", "Kembali");
	}
	return 1;
}

stock OnTrunkSaveCashInput(playerid, const inputtext[])
{
	new slot = gSelectedTrunkVeh[playerid];
	if (slot < 0 || slot >= MAX_OWNED_VEHICLES || !gVehicleData[slot][vUsed]) return 0;

	new amount = strval(inputtext);
	if (amount < 1 || amount > 5000000) {
		SendErr(playerid, "Nominal tidak valid ($1 - $5.000.000).");
		ShowVehicleTrunkDialog(playerid, slot);
		return 0;
	}
	if (gPlayerData[playerid][pMoney] < amount) {
		SendErr(playerid, "Uang tunai Anda tidak mencukupi.");
		ShowVehicleTrunkDialog(playerid, slot);
		return 0;
	}

	TakePlayerMoneyEx(playerid, amount, "simpan bagasi");
	gVehicleData[slot][vTrunk] += amount;
	DB_SaveVehicle(slot);

	SendSucc(playerid, "Berhasil menyimpan $%d ke dalam bagasi kendaraan.", amount);
	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
	ShowVehicleTrunkDialog(playerid, slot);
	return 1;
}

stock OnTrunkTakeCashInput(playerid, const inputtext[])
{
	new slot = gSelectedTrunkVeh[playerid];
	if (slot < 0 || slot >= MAX_OWNED_VEHICLES || !gVehicleData[slot][vUsed]) return 0;

	new amount = strval(inputtext);
	if (amount < 1 || amount > 5000000) {
		SendErr(playerid, "Nominal tidak valid ($1 - $5.000.000).");
		ShowVehicleTrunkDialog(playerid, slot);
		return 0;
	}
	if (gVehicleData[slot][vTrunk] < amount) {
		SendErr(playerid, "Saldo uang di bagasi tidak mencukupi.");
		ShowVehicleTrunkDialog(playerid, slot);
		return 0;
	}

	gVehicleData[slot][vTrunk] -= amount;
	GivePlayerMoneyEx(playerid, amount, "ambil bagasi");
	DB_SaveVehicle(slot);

	SendSucc(playerid, "Berhasil mengambil $%d dari bagasi kendaraan.", amount);
	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
	ShowVehicleTrunkDialog(playerid, slot);
	return 1;
}

stock OnTrunkDepositItemResponse(playerid, listitem)
{
	new slot = gSelectedTrunkVeh[playerid];
	if (slot < 0 || slot >= MAX_OWNED_VEHICLES || !gVehicleData[slot][vUsed]) return 0;

	if (listitem < 0 || listitem >= MAX_INVENTORY_SLOTS) return 0;
	new iType = gPlayerItems[playerid][listitem][itType];
	new iAmt  = gPlayerItems[playerid][listitem][itAmount];

	if (iType == ITEM_NONE || iAmt <= 0) {
		SendErr(playerid, "Slot kantung ini kosong.");
		ShowVehicleTrunkDialog(playerid, slot);
		return 1;
	}

	// Cari slot kosong atau tipe sama di bagasi
	new targetTrunkSlot = -1;
	for (new s = 0; s < MAX_TRUNK_SLOTS; s++) {
		if (gVehicleData[slot][vTrunkItemType][s] == iType) {
			targetTrunkSlot = s;
			break;
		}
	}
	if (targetTrunkSlot == -1) {
		for (new s = 0; s < MAX_TRUNK_SLOTS; s++) {
			if (gVehicleData[slot][vTrunkItemType][s] == ITEM_NONE) {
				targetTrunkSlot = s;
				break;
			}
		}
	}

	if (targetTrunkSlot == -1) {
		SendErr(playerid, "Bagasi kendaraan sudah penuh (Maksimal 6 slot barang).");
		ShowVehicleTrunkDialog(playerid, slot);
		return 1;
	}

	// Pindahkan seluruh stack atau 1..5 unit
	new transferAmt = (iAmt > 5) ? 5 : iAmt;
	TakePlayerItem(playerid, iType, transferAmt);

	gVehicleData[slot][vTrunkItemType][targetTrunkSlot] = iType;
	gVehicleData[slot][vTrunkItemAmount][targetTrunkSlot] += transferAmt;
	DB_SaveVehicle(slot);

	new itemName[32];
	GetItemName(iType, itemName, sizeof(itemName));
	SendSucc(playerid, "Menyimpan %dx %s ke dalam bagasi kendaraan.", transferAmt, itemName);
	PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
	ShowVehicleTrunkDialog(playerid, slot);
	return 1;
}

stock OnTrunkWithdrawItemResponse(playerid, const inputtext[])
{
	new slot = gSelectedTrunkVeh[playerid];
	if (slot < 0 || slot >= MAX_OWNED_VEHICLES || !gVehicleData[slot][vUsed]) return 0;

	new trunkSlot = gSelectedTrunkSlot[playerid];
	if (trunkSlot < 0 || trunkSlot >= MAX_TRUNK_SLOTS) return 0;

	new iType = gVehicleData[slot][vTrunkItemType][trunkSlot];
	new iAmt  = gVehicleData[slot][vTrunkItemAmount][trunkSlot];

	if (iType == ITEM_NONE || iAmt <= 0) {
		SendErr(playerid, "Barang tidak ditemukan di bagasi.");
		ShowVehicleTrunkDialog(playerid, slot);
		return 1;
	}

	new takeAmt = strval(inputtext);
	if (takeAmt <= 0) takeAmt = 1;
	if (takeAmt > iAmt) takeAmt = iAmt;

	gVehicleData[slot][vTrunkItemAmount][trunkSlot] -= takeAmt;
	if (gVehicleData[slot][vTrunkItemAmount][trunkSlot] <= 0) {
		gVehicleData[slot][vTrunkItemType][trunkSlot] = ITEM_NONE;
		gVehicleData[slot][vTrunkItemAmount][trunkSlot] = 0;
	}

	GivePlayerItem(playerid, iType, takeAmt);
	DB_SaveVehicle(slot);

	new itemName[32];
	GetItemName(iType, itemName, sizeof(itemName));
	SendSucc(playerid, "Mengambil %dx %s dari bagasi kendaraan ke dalam kantung.", takeAmt, itemName);
	PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
	ShowVehicleTrunkDialog(playerid, slot);
	return 1;
}

stock OnTrunkWeaponResponse(playerid, listitem)
{
	new slot = gSelectedTrunkVeh[playerid];
	if (slot < 0 || slot >= MAX_OWNED_VEHICLES || !gVehicleData[slot][vUsed]) return 0;

	if (listitem == 0) {
		// Ambil Senjata dari Bagasi
		if (gVehicleData[slot][vTrunkGun] != WEAPON:0 && gVehicleData[slot][vTrunkAmmo] > 0) {
			new WEAPON:gun = gVehicleData[slot][vTrunkGun];
			new ammo = gVehicleData[slot][vTrunkAmmo];
			gVehicleData[slot][vTrunkGun] = WEAPON:0;
			gVehicleData[slot][vTrunkAmmo] = 0;
			DB_SaveVehicle(slot);

			AC_GiveWeapon(playerid, gun, ammo);
			new gunName[32];
			GetWeaponName(gun, gunName, sizeof(gunName));
			SendSucc(playerid, "Mengambil senjata dari bagasi kendaraan.");
			PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
		} else {
			// Simpan Senjata yang sedang dipegang
			new WEAPON:curGun = GetPlayerWeapon(playerid);
			new curAmmo = GetPlayerAmmo(playerid);
			if (curGun == WEAPON_FIST || curAmmo <= 0) {
				SendErr(playerid, "Anda tidak sedang memegang senjata api berpeluru.");
				ShowVehicleTrunkDialog(playerid, slot);
				return 1;
			}
			gVehicleData[slot][vTrunkGun] = curGun;
			gVehicleData[slot][vTrunkAmmo] = curAmmo;
			DB_SaveVehicle(slot);

			AC_ResetWeapons(playerid);
			SendSucc(playerid, "Menyimpan senjata ke dalam bagasi mobil.");
			PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
		}
	} else if (listitem == 1) {
		// Simpan Senjata yang sedang dipegang
		new WEAPON:curGun = GetPlayerWeapon(playerid);
		new curAmmo = GetPlayerAmmo(playerid);
		if (curGun == WEAPON_FIST || curAmmo <= 0) {
			SendErr(playerid, "Anda tidak sedang memegang senjata api berpeluru.");
			ShowVehicleTrunkDialog(playerid, slot);
			return 1;
		}
		gVehicleData[slot][vTrunkGun] = curGun;
		gVehicleData[slot][vTrunkAmmo] = curAmmo;
		DB_SaveVehicle(slot);

		AC_ResetWeapons(playerid);
		SendSucc(playerid, "Menyimpan senjata ke dalam bagasi mobil.");
		PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
	}
	ShowVehicleTrunkDialog(playerid, slot);
	return 1;
}
