#if defined _INC_rp_inventory
	#endinput
#endif
#define _INC_rp_inventory

/*
==============================================================================
  NUSANTARA ROLEPLAY - inventory.inc
  Sistem Slot Inventaris, Penggunaan Item, dan Manajemen Barang
==============================================================================
*/

forward OnPlayerUseItem(playerid, itemtype);

stock ClearPlayerInventory(playerid)
{
	for (new s = 0; s < MAX_INVENTORY_SLOTS; s++) {
		gPlayerItems[playerid][s][itType] = ITEM_NONE;
		gPlayerItems[playerid][s][itAmount] = 0;
	}
	return 1;
}

stock GetPlayerItemCount(playerid, itemtype)
{
	new count = 0;
	for (new s = 0; s < MAX_INVENTORY_SLOTS; s++) {
		if (gPlayerItems[playerid][s][itType] == itemtype) {
			count += gPlayerItems[playerid][s][itAmount];
		}
	}
	return count;
}

stock GivePlayerItem(playerid, itemtype, amount)
{
	if (amount <= 0 || itemtype <= ITEM_NONE) return 0;

	// Cek apakah item sudah ada di salah satu slot (stacking)
	for (new s = 0; s < MAX_INVENTORY_SLOTS; s++) {
		if (gPlayerItems[playerid][s][itType] == itemtype) {
			gPlayerItems[playerid][s][itAmount] += amount;
			return 1;
		}
	}

	// Jika belum ada, cari slot kosong pertama
	for (new s = 0; s < MAX_INVENTORY_SLOTS; s++) {
		if (gPlayerItems[playerid][s][itType] == ITEM_NONE) {
			gPlayerItems[playerid][s][itType] = itemtype;
			gPlayerItems[playerid][s][itAmount] = amount;
			return 1;
		}
	}

	SendErr(playerid, "Inventaris Anda sudah penuh (Maksimal 12 slot barang).");
	return 0;
}

stock TakePlayerItem(playerid, itemtype, amount)
{
	if (amount <= 0 || itemtype <= ITEM_NONE) return 0;
	for (new s = 0; s < MAX_INVENTORY_SLOTS; s++) {
		if (gPlayerItems[playerid][s][itType] == itemtype) {
			if (gPlayerItems[playerid][s][itAmount] > amount) {
				gPlayerItems[playerid][s][itAmount] -= amount;
				return 1;
			} else if (gPlayerItems[playerid][s][itAmount] == amount) {
				gPlayerItems[playerid][s][itType] = ITEM_NONE;
				gPlayerItems[playerid][s][itAmount] = 0;
				return 1;
			} else {
				amount -= gPlayerItems[playerid][s][itAmount];
				gPlayerItems[playerid][s][itType] = ITEM_NONE;
				gPlayerItems[playerid][s][itAmount] = 0;
			}
		}
	}
	return 1;
}

stock ShowPlayerInventory(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;

	new str[800], itemName[32];
	format(str, sizeof(str), "Slot\tNama Barang\tJumlah\n");

	new count = 0;
	for (new s = 0; s < MAX_INVENTORY_SLOTS; s++) {
		if (gPlayerItems[playerid][s][itType] != ITEM_NONE) {
			GetItemName(gPlayerItems[playerid][s][itType], itemName, sizeof(itemName));
			format(str, sizeof(str), "%s#%d\t%s\t%d buah\n", str, s + 1, itemName, gPlayerItems[playerid][s][itAmount]);
			count++;
		}
	}

	if (count == 0) {
		SendInfo(playerid, "Inventaris Anda kosong. Beli barang di toko 24/7 terdekat (/gps).");
		return 0;
	}

	ShowPlayerDialog(playerid, DIALOG_INVENTORY, DIALOG_STYLE_TABLIST_HEADERS,
		"INVENTARIS PRIBADI", str, "Pilih", "Tutup");
	return 1;
}

stock UsePlayerItemBySlot(playerid, slot)
{
	if (slot < 0 || slot >= MAX_INVENTORY_SLOTS) return 0;
	new itemType = gPlayerItems[playerid][slot][itType];
	if (itemType == ITEM_NONE) return 0;

	new itemName[32];
	GetItemName(itemType, itemName, sizeof(itemName));

	switch (itemType) {
		case ITEM_SNACK: {
			gPlayerData[playerid][pHunger] += 30.0;
			if (gPlayerData[playerid][pHunger] > 100.0) gPlayerData[playerid][pHunger] = 100.0;
			ApplyAnimation(playerid, "FOOD", "EAT_Burger", 4.1, false, false, false, false, 2500);
			SendRP(playerid, COLOR_SUCCESS, "Anda memakan snack. Rasa lapar berkurang (+30%% Lapar).");
			TakePlayerItem(playerid, ITEM_SNACK, 1);
		}
		case ITEM_WATER: {
			gPlayerData[playerid][pThirst] += 35.0;
			if (gPlayerData[playerid][pThirst] > 100.0) gPlayerData[playerid][pThirst] = 100.0;
			ApplyAnimation(playerid, "VENDING", "VEND_Drink_P", 4.1, false, false, false, false, 2500);
			SendRP(playerid, COLOR_SUCCESS, "Anda meminum air mineral. Rasa haus berkurang (+35%% Haus).");
			TakePlayerItem(playerid, ITEM_WATER, 1);
		}
		case ITEM_BANDAGE: {
			new Float:hp;
			GetPlayerHealth(playerid, hp);
			if (hp >= 100.0) {
				SendErr(playerid, "Kondisi fisik Anda sudah sehat.");
				return 0;
			}
			hp += 20.0;
			if (hp > 100.0) hp = 100.0;
			SetPlayerHealth(playerid, hp);
			SendRP(playerid, COLOR_SUCCESS, "Anda membalut luka dengan perban (+20 HP).");
			TakePlayerItem(playerid, ITEM_BANDAGE, 1);
		}
		case ITEM_MEDKIT: {
			new Float:hp;
			GetPlayerHealth(playerid, hp);
			if (hp >= 100.0) {
				SendErr(playerid, "Kondisi fisik Anda sudah maksimal.");
				return 0;
			}
			hp += 75.0;
			if (hp > 100.0) hp = 100.0;
			SetPlayerHealth(playerid, hp);
			SendRP(playerid, COLOR_SUCCESS, "Anda menggunakan kotak P3K (+75 HP).");
			TakePlayerItem(playerid, ITEM_MEDKIT, 1);
		}
		case ITEM_REPAIRKIT: {
			new veh = GetPlayerVehicleID(playerid);
			if (veh == 0) {
				new Float:px, Float:py, Float:pz;
				GetPlayerPos(playerid, px, py, pz);
				for (new v = 1; v < MAX_VEHICLES; v++) {
					if (!IsValidVehicle(v)) continue;
					new Float:vx, Float:vy, Float:vz;
					GetVehiclePos(v, vx, vy, vz);
					if (floatsqroot((px-vx)*(px-vx) + (py-vy)*(py-vy) + (pz-vz)*(pz-vz)) <= 4.0) {
						veh = v;
						break;
					}
				}
			}
			if (veh == 0) {
				SendErr(playerid, "Anda harus berada di dekat atau di dalam kendaraan untuk memperbaiki.");
				return 0;
			}
			RepairVehicle(veh);
			SetVehicleHealth(veh, 1000.0);
			SendRP(playerid, COLOR_SUCCESS, "Mesin dan bodi kendaraan berhasil diperbaiki.");
			TakePlayerItem(playerid, ITEM_REPAIRKIT, 1);
		}
		case ITEM_JERRYCAN: {
			new veh = GetPlayerVehicleID(playerid);
			if (veh == 0) {
				SendErr(playerid, "Anda harus berada di dalam kendaraan untuk mengisi bensin dengan jerigen.");
				return 0;
			}
			new vSlot = Vehicle_GetSlot(veh);
			if (vSlot == -1) {
				SendErr(playerid, "Bensin hanya dapat diisikan ke kendaraan pribadi.");
				return 0;
			}
			gVehicleData[vSlot][vFuel] += 25.0;
			if (gVehicleData[vSlot][vFuel] > 100.0) gVehicleData[vSlot][vFuel] = 100.0;
			SendRP(playerid, COLOR_SUCCESS, "25 Liter bahan bakar berhasil diisikan ke tangki kendaraan.");
			TakePlayerItem(playerid, ITEM_JERRYCAN, 1);
		}
		case ITEM_CIGARETTE: {
			if (GetPlayerItemCount(playerid, ITEM_LIGHTER) <= 0) {
				SendErr(playerid, "Anda memerlukan korek api untuk menyalakan rokok.");
				return 0;
			}
			ApplyAnimation(playerid, "SMOKING", "M_smkstnd_loop", 4.1, true, false, false, false, 0);
			SendRP(playerid, COLOR_RP_ME, "* %s menyalakan sebatang rokok dengan korek api.", gPlayerData[playerid][pName]);
			TakePlayerItem(playerid, ITEM_CIGARETTE, 1);
		}
		case ITEM_LIGHTER: {
			SendInfo(playerid, "Korek api digunakan bersama dengan rokok.");
		}
		case ITEM_PHONE: {
			ShowPhoneMenu(playerid);
		}
		case ITEM_RADIO: {
			ShowRadioDialog(playerid);
		}
		case ITEM_GPS: {
			ShowGPSMenu(playerid);
		}
		default: {
			SendInfo(playerid, "Barang ini tidak memiliki fungsi khusus.");
		}
	}
	return 1;
}
