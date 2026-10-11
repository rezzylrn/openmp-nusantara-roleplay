#if defined _INC_rp_shops
	#endinput
#endif
#define _INC_rp_shops

/*
==============================================================================
  NUSANTARA ROLEPLAY - shops.inc
  Toko Serba Ada 24/7, Katalog Pembelian Barang & SPBU
==============================================================================
*/

stock ShowStoreBuyMenu(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;

	new Float:px, Float:py, Float:pz;
	GetPlayerPos(playerid, px, py, pz);

	// Verifikasi jarak ke toko 24/7 Idlewood
	if (!IsPlayerInRangeOfPoint(playerid, RANGE_STORE, STORE_IDLEWOOD_X, STORE_IDLEWOOD_Y, STORE_IDLEWOOD_Z) &&
	    !IsPlayerInRangeOfPoint(playerid, RANGE_STORE, 1352.0, -1758.0, 13.5)) {
		SendErr(playerid, "Anda tidak berada di kasir toko 24/7. Gunakan /gps untuk mencari toko.");
		return 0;
	}

	new list[700];
	format(list, sizeof(list),
		"Barang\tHarga\tKeterangan\n\
Snack Roti\t$5\tMengurangi rasa lapar (+30%%)\n\
Air Mineral\t$3\tMengurangi rasa haus (+35%%)\n\
Perban Medis\t$15\tMenyembuhkan luka ringan (+20 HP)\n\
Kotak P3K Medkit\t$50\tPerawatan medis darurat (+75 HP)\n\
Alat Perbaikan Mobil\t$75\tMemperbaiki mesin & bodi kendaraan\n\
Jerigen Bensin (20L)\t$40\tBahan bakar cadangan kendaraan\n\
Sebungkus Rokok\t$8\tMenikmati sebatang rokok\n\
Korek Api\t$2\tAlat pemantik api\n\
Ponsel Pintar\t$150\tAlat komunikasi (/call & /sms)\n\
Handy Talkie (HT)\t$100\tRadio komunikasi dua arah (/r)\n\
GPS Navigator\t$50\tPeta panduan fasilitas kota (/gps)\n\
Detektor Logam Pantai\t$120\tMencari koin & perhiasan terpendam (/detect)\n\
Sekop Penggali Tanah\t$45\tMenggali temuan detektor logam (/dig)\n\
Kaleng Cat Semprot\t$25\tMenggambar grafiti di dinding (/tag)\n\
Dadu Kasino Jalanan\t$15\tBermain dadu taruhan peran (/dice)");

	ShowPlayerDialog(playerid, DIALOG_STORE_BUY, DIALOG_STYLE_TABLIST_HEADERS,
		"TOKO 24/7 - KATALOG PEMBELIAN", list, "Beli", "Batal");
	return 1;
}

stock ProcessStorePurchase(playerid, listitem)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;

	new itemType = ITEM_NONE;
	new price = 0;
	new itemName[32];

	switch (listitem) {
		case 0: { itemType = ITEM_SNACK;     price = 5;   }
		case 1: { itemType = ITEM_WATER;     price = 3;   }
		case 2: { itemType = ITEM_BANDAGE;   price = 15;  }
		case 3: { itemType = ITEM_MEDKIT;    price = 50;  }
		case 4: { itemType = ITEM_REPAIRKIT; price = 75;  }
		case 5: { itemType = ITEM_JERRYCAN;  price = 40;  }
		case 6: { itemType = ITEM_CIGARETTE; price = 8;   }
		case 7: { itemType = ITEM_LIGHTER;   price = 2;   }
		case 8: { itemType = ITEM_PHONE;     price = 150; }
		case 9: { itemType = ITEM_RADIO;     price = 100; }
		case 10: { itemType = ITEM_GPS;      price = 50;  }
		case 11: { itemType = ITEM_DETECTOR; price = 120; }
		case 12: { itemType = ITEM_SHOVEL;   price = 45;  }
		case 13: { itemType = ITEM_SPRAYCAN; price = 25;  }
		case 14: { itemType = ITEM_DICE;     price = 15;  }
		default: return 0;
	}

	GetItemName(itemType, itemName, sizeof(itemName));

	if (gPlayerData[playerid][pMoney] < price) {
		SendErr(playerid, "Uang tunai Anda tidak mencukupi untuk membeli %s ($%d).", itemName, price);
		return 0;
	}

	if (!GivePlayerItem(playerid, itemType, 1)) {
		// Inventaris penuh
		return 0;
	}

	TakePlayerMoneyEx(playerid, price);
	SendSucc(playerid, "Anda membeli 1x %s seharga $%d. Barang masuk ke inventaris (/inv).", itemName, price);

	// Jika membeli ponsel untuk pertama kali dan belum ada nomor
	if (itemType == ITEM_PHONE && gPlayerData[playerid][pPhone] <= 0) {
		gPlayerData[playerid][pPhone] = 500000 + random(499999);
		gPlayerData[playerid][pPhoneStatus] = true;
		SendInfo(playerid, "Nomor telepon baru Anda telah aktif: %d. Gunakan /call atau /sms.", gPlayerData[playerid][pPhone]);
	}
	return 1;
}
