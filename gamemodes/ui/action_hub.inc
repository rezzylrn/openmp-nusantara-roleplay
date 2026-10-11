#if defined _INC_ui_action_hub
	#endinput
#endif
#define _INC_ui_action_hub

/*
==============================================================================
 action_hub.inc
 NUSANTARA ROLEPLAY - Unified Touch & Key Dynamic Context Interaction Hub
 Mengeliminasi ketergantungan perintah ketik (slash command) untuk pemain
 Mobile (Touchscreen) dan Desktop (Keyboard/Mouse).
==============================================================================
*/

#define MAX_HUB_ACTIONS         35

enum {
	HUB_ACT_NONE = 0,
	// Kendaraan (di dalam mobil)
	HUB_ACT_ENGINE,
	HUB_ACT_LOCK,
	HUB_ACT_LIGHTS,
	HUB_ACT_HOOD,
	HUB_ACT_TRUNK,
	HUB_ACT_REFUEL,
	HUB_ACT_PLATE,
	HUB_ACT_TAXI_METER,
	HUB_ACT_TOW,
	HUB_ACT_UNTOW,
	HUB_ACT_IMPOUND,
	HUB_ACT_UNRENT,
	// Kendaraan (di luar mobil)
	HUB_ACT_NEAR_TRUNK,
	HUB_ACT_LOCKPICK,
	HUB_ACT_CHOPSHOP,
	HUB_ACT_REPAIR_TIRE,
	HUB_ACT_POUR_FUEL,
	// Lingkungan & Fasilitas Sekitar
	HUB_ACT_STORE_BUY,
	HUB_ACT_STORE_ROB,
	HUB_ACT_BANK,
	HUB_ACT_ATM,
	HUB_ACT_PAY_TICKET,
	HUB_ACT_TRASH_SCAVENGE,
	HUB_ACT_WEED_HARVEST,
	HUB_ACT_WEED_BURN,
	HUB_ACT_DETECT,
	HUB_ACT_DIG,
	HUB_ACT_FISH,
	HUB_ACT_CAMPFIRE_COOK,
	HUB_ACT_CAMPFIRE_EAT,
	HUB_ACT_GRAFFITI_TAG,
	HUB_ACT_GRAFFITI_CLEAN,
	HUB_ACT_VENDING,
	HUB_ACT_BLACKMARKET,
	HUB_ACT_SMUGGLE,
	HUB_ACT_BAR,
	HUB_ACT_GYM_STYLES,
	HUB_ACT_GYM_WORKOUT,
	HUB_ACT_FASTFOOD,
	HUB_ACT_AMMUNATION,
	HUB_ACT_DMV,
	HUB_ACT_CLAIM_VEH,
	HUB_ACT_RENTAL,
	HUB_ACT_DOOR_ENTER,
	HUB_ACT_DOOR_EXIT,
	HUB_ACT_PLAYER_MENU,
	// Menu Karakter & Navigasi Utama
	HUB_ACT_INV,
	HUB_ACT_PHONE,
	HUB_ACT_MYVEH,
	HUB_ACT_JOBS,
	HUB_ACT_PLANT_WEED,
	HUB_ACT_CAMPFIRE_BUILD,
	HUB_ACT_CALL_TAXI,
	HUB_ACT_CALL_911,
	HUB_ACT_AD,
	HUB_ACT_ACCESSORIES,
	HUB_ACT_STATS,
	HUB_ACT_HELP,
	HUB_ACT_LANG
};

new gPlayerHubAction[MAX_PLAYERS][MAX_HUB_ACTIONS];
new gPlayerHubContextTarget[MAX_PLAYERS];

stock ShowPlayerActionHub(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;

	new dialogList[1600];
	dialogList[0] = '\0';

	new count = 0;
	gPlayerHubContextTarget[playerid] = -1;

	new bool:isEn = (gPlayerData[playerid][pLanguage] == LANG_EN);

	// =========================================================================
	// 1. CONTEXT DETECTOR: DI DALAM KENDARAAN (DRIVER)
	// =========================================================================
	if (IsPlayerInAnyVehicle(playerid) && GetPlayerState(playerid) == PLAYER_STATE_DRIVER) {
		new veh = GetPlayerVehicleID(playerid);
		new model = GetVehicleModel(veh);

		strcat(dialogList, isEn ?
			"{00FF00}=== CURRENT VEHICLE CONTROLS ===\n" :
			"{00FF00}=== KONTROL KENDARAAN SAAT INI ===\n");

		gPlayerHubAction[playerid][count++] = HUB_ACT_ENGINE;
		strcat(dialogList, isEn ?
			"{FFFF00}⚡ [ENGINE] Toggle Engine On / Off\n" :
			"{FFFF00}⚡ [MESIN] Hidupkan / Matikan Mesin Kendaraan\n");

		gPlayerHubAction[playerid][count++] = HUB_ACT_LOCK;
		strcat(dialogList, isEn ?
			"{FFFF00}⚡ [LOCK] Toggle Door Lock / Unlock\n" :
			"{FFFF00}⚡ [KUNCI] Kunci / Buka Pintu Kendaraan\n");

		gPlayerHubAction[playerid][count++] = HUB_ACT_LIGHTS;
		strcat(dialogList, isEn ?
			"{FFFF00}⚡ [LIGHTS] Toggle Headlights On / Off\n" :
			"{FFFF00}⚡ [LAMPU] Nyalakan / Matikan Lampu Utama\n");

		gPlayerHubAction[playerid][count++] = HUB_ACT_TRUNK;
		strcat(dialogList, isEn ?
			"{FFFF00}⚡ [TRUNK] Open / Close Rear Trunk\n" :
			"{FFFF00}⚡ [BAGASI] Buka / Tutup Bagasi Belakang\n");

		gPlayerHubAction[playerid][count++] = HUB_ACT_HOOD;
		strcat(dialogList, isEn ?
			"{FFFF00}⚡ [HOOD] Open / Close Front Engine Hood\n" :
			"{FFFF00}⚡ [KAP MESIN] Buka / Tutup Kap Mesin Depan\n");

		// SPBU
		if (IsPlayerNearPoint(playerid, FUEL_STATION_X, FUEL_STATION_Y, FUEL_STATION_Z, RANGE_FUEL)) {
			gPlayerHubAction[playerid][count++] = HUB_ACT_REFUEL;
			strcat(dialogList, "{4CD964}⚡ [SPBU] Isi Bahan Bakar Kendaraan\n");
		}

		// Taksi Argometer
		if (model == 420 || model == 438) {
			gPlayerHubAction[playerid][count++] = HUB_ACT_TAXI_METER;
			strcat(dialogList, "{33CCFF}⚡ [TAKSI] Aktifkan / Matikan Argometer Tarif\n");
		}

		// Towtruck Derek
		if (model == 525) {
			if (gPlayerTowedVeh[playerid] == INVALID_VEHICLE_ID) {
				gPlayerHubAction[playerid][count++] = HUB_ACT_TOW;
				strcat(dialogList, "{33CCFF}⚡ [DEREK] Pasang Kabel Sling Derek ke Mobil Belakang\n");
			} else {
				gPlayerHubAction[playerid][count++] = HUB_ACT_UNTOW;
				strcat(dialogList, "{FF9900}⚡ [DEREK] Lepaskan Kaitan Truk Derek\n");
			}

			if (IsPlayerNearPoint(playerid, IMPOUND_YARD_X, IMPOUND_YARD_Y, IMPOUND_YARD_Z, 15.0)) {
				gPlayerHubAction[playerid][count++] = HUB_ACT_IMPOUND;
				strcat(dialogList, "{FF3B30}⚡ [SITA LSPD] Sita Kendaraan yang Diderek ke Depo ($100 Upah)\n");
			}
		}

		// Mobil Sewa
		if (gPlayerRentalVeh[playerid] == veh) {
			gPlayerHubAction[playerid][count++] = HUB_ACT_UNRENT;
			strcat(dialogList, "{FF3B30}⚡ [RENTAL] Kembalikan Kendaraan Sewa Ini\n");
		}

		// Ganti Plat Kustom DMV
		gPlayerHubAction[playerid][count++] = HUB_ACT_PLATE;
		strcat(dialogList, "{FFFF00}⚡ [PLAT DMV] Daftarkan Nomor Plat Kustom ($500)\n");
	}
	else {
		// =====================================================================
		// 2. CONTEXT DETECTOR: JALAN KAKI (ON FOOT CONTEXT)
		// =====================================================================
		new bool:hasContext = false;

		// 2.1 Dekat Kendaraan Luar (<= 4.2m)
		new nearVehSlot = -1;
		for (new i = 0; i < MAX_OWNED_VEHICLES; i++) {
			if (gVehicleData[i][vUsed] && gVehicleData[i][vVehicleHandle] > 0) {
				if (IsPlayerNearPoint(playerid, gVehicleData[i][vPosX], gVehicleData[i][vPosY], gVehicleData[i][vPosZ], 4.2)) {
					nearVehSlot = i;
					break;
				}
			}
		}

		if (nearVehSlot != -1) {
			if (!hasContext) {
				strcat(dialogList, "{00FF00}=== INTERAKSI KENDARAAN TERDEKAT ===\n");
				hasContext = true;
			}
			gPlayerHubContextTarget[playerid] = nearVehSlot;

			gPlayerHubAction[playerid][count++] = HUB_ACT_NEAR_TRUNK;
			strcat(dialogList, "{FFFF00}⚡ [BAGASI] Buka & Akses Bagasi (Uang, Item, Senjata)\n");

			gPlayerHubAction[playerid][count++] = HUB_ACT_LOCKPICK;
			strcat(dialogList, "{FF3B30}⚡ [LOCKPICK] Bobol Kunci Pintu Mobil (Obeng Lockpick)\n");

			if (IsPlayerNearPoint(playerid, CHOPSHOP_X, CHOPSHOP_Y, CHOPSHOP_Z, 6.0)) {
				gPlayerHubAction[playerid][count++] = HUB_ACT_CHOPSHOP;
				strcat(dialogList, "{FF3B30}⚡ [CHOP SHOP] Mutilasi & Preteli Suku Cadang Mobil Curian\n");
			}

			if (GetPlayerItemCount(playerid, ITEM_REPAIRKIT) > 0) {
				gPlayerHubAction[playerid][count++] = HUB_ACT_REPAIR_TIRE;
				strcat(dialogList, "{33CCFF}⚡ [TAMBAL BAN] Perbaiki Ban Bocor / Kempes (Repairkit)\n");
			}

			if (GetPlayerItemCount(playerid, ITEM_JERRYCAN) > 0) {
				gPlayerHubAction[playerid][count++] = HUB_ACT_POUR_FUEL;
				strcat(dialogList, "{33CCFF}⚡ [TUANG BENSIN] Tuang 20L Bensin dari Jerigen\n");
			}
		}

		// 2.2 Dekat Pemain Lain (<= 3.8m)
		new nearPlayer = -1;
		for (new p = 0; p < MAX_PLAYERS; p++) {
			if (p != playerid && IsPlayerConnected(p) && gPlayerData[p][pLoggedIn] && IsPlayerNear(playerid, p, 3.8)) {
				nearPlayer = p;
				break;
			}
		}

		if (nearPlayer != -1) {
			if (!hasContext) {
				strcat(dialogList, "{00FF00}=== INTERAKSI PEMAIN SEKITAR ===\n");
				hasContext = true;
			}
			gPlayerHubContextTarget[playerid] = nearPlayer;

			gPlayerHubAction[playerid][count++] = HUB_ACT_PLAYER_MENU;
			new pItem[96];
			format(pItem, sizeof(pItem), "{FFFF00}⚡ [INTERAKSI WARGA] Buka Menu Interaksi dengan %s\n", gPlayerData[nearPlayer][pName]);
			strcat(dialogList, pItem);
		}

		// 2.3 Dekat Toko 24/7 (<= 4.5m)
		if (IsPlayerNearPoint(playerid, STORE_IDLEWOOD_X, STORE_IDLEWOOD_Y, STORE_IDLEWOOD_Z, 4.5) ||
		    IsPlayerNearPoint(playerid, 1352.0, -1758.0, 13.5, 4.5)) {
			if (!hasContext) { strcat(dialogList, "{00FF00}=== FASILITAS SEKITAR ===\n"); hasContext = true; }

			gPlayerHubAction[playerid][count++] = HUB_ACT_STORE_BUY;
			strcat(dialogList, "{4CD964}⚡ [KASIR 24/7] Belanja Makanan, Toolkit, Jerigen & Peralatan\n");

			gPlayerHubAction[playerid][count++] = HUB_ACT_STORE_ROB;
			strcat(dialogList, "{FF3B30}⚡ [RAMPOK TOKO] Todong Kasir & Bobol Brankas Toko Bersenjata\n");
		}

		// 2.4 Dekat Bank / ATM
		if (IsPlayerNearPoint(playerid, BANK_X, BANK_Y, BANK_Z, 4.0)) {
			if (!hasContext) { strcat(dialogList, "{00FF00}=== FASILITAS SEKITAR ===\n"); hasContext = true; }
			gPlayerHubAction[playerid][count++] = HUB_ACT_BANK;
			strcat(dialogList, "{4CD964}⚡ [BANK NUSANTARA] Buka Layanan Transaksi Teller Bank\n");

			gPlayerHubAction[playerid][count++] = HUB_ACT_PAY_TICKET;
			strcat(dialogList, "{FFFF00}⚡ [TILANG ETLE] Bayar & Lunasi Surat Tilang Kecepatan\n");
		} else if (IsPlayerNearAnyATM(playerid, 3.0)) {
			if (!hasContext) { strcat(dialogList, "{00FF00}=== FASILITAS SEKITAR ===\n"); hasContext = true; }
			gPlayerHubAction[playerid][count++] = HUB_ACT_ATM;
			strcat(dialogList, "{4CD964}⚡ [MESIN ATM] Buka Antarmuka Mesin ATM Tunai\n");

			gPlayerHubAction[playerid][count++] = HUB_ACT_PAY_TICKET;
			strcat(dialogList, "{FFFF00}⚡ [TILANG ETLE] Bayar & Lunasi Surat Tilang Kecepatan\n");
		}

		// 2.5 Dekat Tong Sampah
		if (IsPlayerNearAnyTrash(playerid, 3.0)) {
			if (!hasContext) { strcat(dialogList, "{00FF00}=== FASILITAS SEKITAR ===\n"); hasContext = true; }
			gPlayerHubAction[playerid][count++] = HUB_ACT_TRASH_SCAVENGE;
			strcat(dialogList, "{FFFF00}⚡ [TONG SAMPAH] Mengais Rongsokan & Barang Daur Ulang\n");
		}

		// 2.6 Dekat Tanaman Ganja
		new nearWeed = -1;
		for (new w = 0; w < MAX_WEED_PLANTS; w++) {
			if (gWeedPlants[w][wpUsed] && IsPlayerNearPoint(playerid, gWeedPlants[w][wpX], gWeedPlants[w][wpY], gWeedPlants[w][wpZ], 3.0)) {
				nearWeed = w;
				break;
			}
		}
		if (nearWeed != -1) {
			if (!hasContext) { strcat(dialogList, "{00FF00}=== FASILITAS SEKITAR ===\n"); hasContext = true; }
			gPlayerHubAction[playerid][count++] = HUB_ACT_WEED_HARVEST;
			strcat(dialogList, "{00FF00}⚡ [TANAMAN GANJA] Panen Daun Ganja Kering Siap Edar\n");

			if (gPlayerData[playerid][pFaction] == FACTION_POLICE && gPlayerData[playerid][pDuty]) {
				gPlayerHubAction[playerid][count++] = HUB_ACT_WEED_BURN;
				strcat(dialogList, "{FF3B30}⚡ [SITA SAPD] Bakar & Musnahkan Tanaman Narkotika Ini\n");
			}
		}

		// 2.7 Pantai & Pasir
		if (GetPlayerItemCount(playerid, ITEM_DETECTOR) > 0) {
			if (!hasContext) { strcat(dialogList, "{00FF00}=== FASILITAS SEKITAR ===\n"); hasContext = true; }
			gPlayerHubAction[playerid][count++] = HUB_ACT_DETECT;
			strcat(dialogList, "{33CCFF}⚡ [DETEKTOR LOGAM] Pindai Pasir Mencari Logam Berharga\n");

			if (GetPlayerItemCount(playerid, ITEM_SHOVEL) > 0) {
				gPlayerHubAction[playerid][count++] = HUB_ACT_DIG;
				strcat(dialogList, "{33CCFF}⚡ [SEKOP PENGGALI] Gali Pasir Titik Sinyal Harta Karun\n");
			}
		}

		// 2.8 Api Unggun
		new nearFire = -1;
		for (new f = 0; f < MAX_CAMPFIRES; f++) {
			if (gCampfires[f][cfUsed] && IsPlayerNearPoint(playerid, gCampfires[f][cfX], gCampfires[f][cfY], gCampfires[f][cfZ], 3.5)) {
				nearFire = f;
				break;
			}
		}
		if (nearFire != -1) {
			if (!hasContext) { strcat(dialogList, "{00FF00}=== FASILITAS SEKITAR ===\n"); hasContext = true; }
			gPlayerHubAction[playerid][count++] = HUB_ACT_CAMPFIRE_COOK;
			strcat(dialogList, "{FF9900}⚡ [API UNGGUN] Panggang & Masak Ikan Mentah Hasil Pancingan\n");

			if (GetPlayerItemCount(playerid, ITEM_COOKEDFISH) > 0) {
				gPlayerHubAction[playerid][count++] = HUB_ACT_CAMPFIRE_EAT;
				strcat(dialogList, "{4CD964}⚡ [API UNGGUN] Santap Ikan Bakar (+Lapar & HP)\n");
			}
		}

		// 2.9 Dinding Grafiti
		new nearTag = -1;
		for (new g = 0; g < MAX_GRAFFITI_TAGS; g++) {
			if (IsPlayerNearPoint(playerid, gGraffitiSpots[g][gfX], gGraffitiSpots[g][gfY], gGraffitiSpots[g][gfZ], 3.5)) {
				nearTag = g;
				break;
			}
		}
		if (nearTag != -1) {
			if (!hasContext) { strcat(dialogList, "{00FF00}=== FASILITAS SEKITAR ===\n"); hasContext = true; }
			if (GetPlayerItemCount(playerid, ITEM_SPRAYCAN) > 0) {
				gPlayerHubAction[playerid][count++] = HUB_ACT_GRAFFITI_TAG;
				strcat(dialogList, "{FF3B30}⚡ [DINDING GRAFITI] Semprotkan Cat Grafiti Tag Wilayah\n");
			}
			gPlayerHubAction[playerid][count++] = HUB_ACT_GRAFFITI_CLEAN;
			strcat(dialogList, "{888888}⚡ [DINDING GRAFITI] Bersihkan Coretan Liar ($75 Upah Kota)\n");
		}

		// 2.10 Mesin Vending
		if (IsPlayerNearVendingMachine(playerid, 3.0) != -1) {
			if (!hasContext) { strcat(dialogList, "{00FF00}=== FASILITAS SEKITAR ===\n"); hasContext = true; }
			gPlayerHubAction[playerid][count++] = HUB_ACT_VENDING;
			strcat(dialogList, "{33CCFF}⚡ [MESIN VENDING] Beli Minuman Sprunk ($3) / Cokelat ($4)\n");
		}

		// 2.11 Black Market
		if (IsPlayerNearPoint(playerid, BLACKMARKET_X, BLACKMARKET_Y, BLACKMARKET_Z, 4.5)) {
			if (!hasContext) { strcat(dialogList, "{00FF00}=== FASILITAS SEKITAR ===\n"); hasContext = true; }
			gPlayerHubAction[playerid][count++] = HUB_ACT_BLACKMARKET;
			strcat(dialogList, "{FF3B30}⚡ [BLACK MARKET] Beli Barang Gelap / Jual Hasil Kejahatan\n");
		}

		// 2.12 Smuggler Depot
		if (IsPlayerNearPoint(playerid, SMUGGLER_DEPOT_X, SMUGGLER_DEPOT_Y, SMUGGLER_DEPOT_Z, 4.5)) {
			if (!hasContext) { strcat(dialogList, "{00FF00}=== FASILITAS SEKITAR ===\n"); hasContext = true; }
			gPlayerHubAction[playerid][count++] = HUB_ACT_SMUGGLE;
			strcat(dialogList, "{FF3B30}⚡ [PENYELUNDUP] Ambil Paket Kontraband Antar-Kota ($800+)\n");
		}

		// 2.13 Bar Minuman
		new bool:nearBar = false;
		for (new b = 0; b < MAX_BAR_LOCATIONS; b++) {
			if (IsPlayerNearPoint(playerid, gBarSpots[b][0], gBarSpots[b][1], gBarSpots[b][2], 4.0)) {
				nearBar = true; break;
			}
		}
		if (nearBar) {
			if (!hasContext) { strcat(dialogList, "{00FF00}=== FASILITAS SEKITAR ===\n"); hasContext = true; }
			gPlayerHubAction[playerid][count++] = HUB_ACT_BAR;
			strcat(dialogList, "{FF66CC}⚡ [BAR KLUB] Pesan Minuman Bir, Whiskey & Vodka\n");
		}

		// 2.14 Gym Fitness
		if (IsPlayerNearPoint(playerid, BEACH_GYM_X, BEACH_GYM_Y, BEACH_GYM_Z, 12.0)) {
			if (!hasContext) { strcat(dialogList, "{00FF00}=== FASILITAS SEKITAR ===\n"); hasContext = true; }
			gPlayerHubAction[playerid][count++] = HUB_ACT_GYM_STYLES;
			strcat(dialogList, "{FFFF00}⚡ [GYM] Pelatihan Seni Bela Diri (Boxing/KungFu/MuayThai)\n");

			gPlayerHubAction[playerid][count++] = HUB_ACT_GYM_WORKOUT;
			strcat(dialogList, "{4CD964}⚡ [GYM] Latihan Barbell Benchpress (+HP Stamina)\n");
		}

		// 2.15 Fast Food
		new bool:nearFood = false;
		for (new ff = 0; ff < MAX_FASTFOOD_OUTLETS; ff++) {
			if (IsPlayerNearPoint(playerid, gFastFoodSpots[ff][0], gFastFoodSpots[ff][1], gFastFoodSpots[ff][2], 5.0)) {
				nearFood = true; break;
			}
		}
		if (nearFood) {
			if (!hasContext) { strcat(dialogList, "{00FF00}=== FASILITAS SEKITAR ===\n"); hasContext = true; }
			gPlayerHubAction[playerid][count++] = HUB_ACT_FASTFOOD;
			strcat(dialogList, "{FFCC00}⚡ [RESTORAN] Pesan Paket Burger, Cluckin Fried Chicken / Pizza\n");
		}

		// 2.16 Ammu-Nation
		if (IsPlayerNearPoint(playerid, AMMU_NATION_X, AMMU_NATION_Y, AMMU_NATION_Z, 4.5)) {
			if (!hasContext) { strcat(dialogList, "{00FF00}=== FASILITAS SEKITAR ===\n"); hasContext = true; }
			gPlayerHubAction[playerid][count++] = HUB_ACT_AMMUNATION;
			strcat(dialogList, "{FFCC00}⚡ [AMMU-NATION] Beli Senjata Legal, Amunisi & Rompi Kevlar\n");
		}

		// 2.17 DMV / Balai Kota
		if (IsPlayerNearPoint(playerid, GOV_X, GOV_Y, GOV_Z, 10.0)) {
			if (!hasContext) { strcat(dialogList, "{00FF00}=== FASILITAS SEKITAR ===\n"); hasContext = true; }
			gPlayerHubAction[playerid][count++] = HUB_ACT_DMV;
			strcat(dialogList, "{33CCFF}⚡ [BALAI KOTA] Ujian Teori SIM Surat Izin Mengemudi ($150)\n");
		}

		// 2.18 Depo Sitaan LSPD
		if (IsPlayerNearPoint(playerid, IMPOUND_YARD_X, IMPOUND_YARD_Y, IMPOUND_YARD_Z, 10.0)) {
			if (!hasContext) { strcat(dialogList, "{00FF00}=== FASILITAS SEKITAR ===\n"); hasContext = true; }
			gPlayerHubAction[playerid][count++] = HUB_ACT_CLAIM_VEH;
			strcat(dialogList, "{33CCFF}⚡ [DEPO SITAAN] Tebus Kendaraan yang Disita Polisi ($200)\n");
		}

		// 2.19 Rental Stasiun
		new bool:nearRental = false;
		for (new r = 0; r < MAX_RENTAL_STATIONS; r++) {
			if (gRentalStations[r][renX] != 0.0 && IsPlayerNearPoint(playerid, gRentalStations[r][renX], gRentalStations[r][renY], gRentalStations[r][renZ], 4.0)) {
				nearRental = true; break;
			}
		}
		if (nearRental) {
			if (!hasContext) { strcat(dialogList, "{00FF00}=== FASILITAS SEKITAR ===\n"); hasContext = true; }
			gPlayerHubAction[playerid][count++] = HUB_ACT_RENTAL;
			strcat(dialogList, "{00FF00}⚡ [RENTAL] Sewa Sepeda BMX, MTB, Faggio / Previon\n");
		}

		// 2.20 Pintu Gedung
		if (IsPlayerNearAnyEntranceDoor(playerid)) {
			if (!hasContext) { strcat(dialogList, "{00FF00}=== FASILITAS SEKITAR ===\n"); hasContext = true; }
			gPlayerHubAction[playerid][count++] = HUB_ACT_DOOR_ENTER;
			strcat(dialogList, "{FFFFFF}⚡ [PINTU] Masuk ke Dalam Gedung\n");
		} else if (IsPlayerNearAnyExitDoor(playerid)) {
			if (!hasContext) { strcat(dialogList, "{00FF00}=== FASILITAS SEKITAR ===\n"); hasContext = true; }
			gPlayerHubAction[playerid][count++] = HUB_ACT_DOOR_EXIT;
			strcat(dialogList, "{FFFFFF}⚡ [PINTU] Keluar dari Gedung\n");
		}
		#pragma unused hasContext
	}

	// =========================================================================
	// 3. MENU UTAMA KARAKTER & NAVIGASI LENGKAP (ALWAYS AVAILABLE)
	// =========================================================================
	strcat(dialogList, isEn ?
		"{33CCFF}=== MAIN CHARACTER FEATURES ===\n" :
		"{33CCFF}=== FITUR UTAMA KARAKTER ===\n");

	gPlayerHubAction[playerid][count++] = HUB_ACT_INV;
	strcat(dialogList, isEn ?
		"{FFFFFF}[1] 🎒 Open Inventory (25-Slot 3D Preview GUI)\n" :
		"{FFFFFF}[1] 🎒 Buka Inventaris (25-Slot 3D Preview GUI)\n");

	gPlayerHubAction[playerid][count++] = HUB_ACT_PHONE;
	strcat(dialogList, isEn ?
		"{FFFFFF}[2] 📱 Smartphone OLED Screen & Apps\n" :
		"{FFFFFF}[2] 📱 Smartphone Layar Sentuh OLED & Aplikasi\n");

	gPlayerHubAction[playerid][count++] = HUB_ACT_MYVEH;
	strcat(dialogList, isEn ?
		"{FFFFFF}[3] 🚗 My Vehicles & Garage Management\n" :
		"{FFFFFF}[3] 🚗 Kendaraan Saya & Manajemen Garasi (/v)\n");

	gPlayerHubAction[playerid][count++] = HUB_ACT_JOBS;
	strcat(dialogList, isEn ?
		"{FFFFFF}[4] 💼 Labor Job Center & Occupations\n" :
		"{FFFFFF}[4] 💼 Pusat Lowongan Pekerjaan Kota (/job)\n");

	gPlayerHubAction[playerid][count++] = HUB_ACT_PLANT_WEED;
	strcat(dialogList, isEn ?
		"{FFFFFF}[5] 🌿 Plant Weed Seeds Pot in Ground\n" :
		"{FFFFFF}[5] 🌿 Tanam Pot Benih Ganja ke Tanah\n");

	gPlayerHubAction[playerid][count++] = HUB_ACT_CAMPFIRE_BUILD;
	strcat(dialogList, isEn ?
		"{FFFFFF}[6] 🔥 Light Campfire Outdoors (Wood + Lighter)\n" :
		"{FFFFFF}[6] 🔥 Nyalakan Api Unggun (Kayu + Korek Api)\n");

	gPlayerHubAction[playerid][count++] = HUB_ACT_CALL_TAXI;
	strcat(dialogList, isEn ?
		"{FFFFFF}[7] 🚕 Order / Call City Taxi Cab to Location\n" :
		"{FFFFFF}[7] 🚕 Pesan & Panggil Taksi ke Lokasi Saya\n");

	gPlayerHubAction[playerid][count++] = HUB_ACT_CALL_911;
	strcat(dialogList, isEn ?
		"{FFFFFF}[8] 🚨 Call 911 Emergency (Police / Paramedic)\n" :
		"{FFFFFF}[8] 🚨 Panggilan Darurat 911 (Polisi / Medis)\n");

	gPlayerHubAction[playerid][count++] = HUB_ACT_AD;
	strcat(dialogList, isEn ?
		"{FFFFFF}[9] 📢 Publish SANews Classifieds Ad ($50)\n" :
		"{FFFFFF}[9] 📢 Pasang Iklan Baris SANews ($50)\n");

	gPlayerHubAction[playerid][count++] = HUB_ACT_ACCESSORIES;
	strcat(dialogList, isEn ?
		"{FFFFFF}[10] 👓 Fashion Accessories (Hat, Glasses, Bag)\n" :
		"{FFFFFF}[10] 👓 Aksesoris Fashion (Topi, Kacamata, Tas)\n");

	gPlayerHubAction[playerid][count++] = HUB_ACT_STATS;
	strcat(dialogList, isEn ?
		"{FFFFFF}[11] 🆔 Identity Card & Character Statistics\n" :
		"{FFFFFF}[11] 🆔 Kartu Tanda Penduduk & Status Karakter\n");

	gPlayerHubAction[playerid][count++] = HUB_ACT_LANG;
	strcat(dialogList, isEn ?
		"{FFFFFF}[12] 🌐 Toggle System Language (ID / EN)\n" :
		"{FFFFFF}[12] 🌐 Alihkan Bahasa Sistem (ID / EN)\n");

	gPlayerHubAction[playerid][count++] = HUB_ACT_HELP;
	strcat(dialogList, isEn ?
		"{888888}[?] 📖 Complete Roleplay Guide & Shortcuts\n" :
		"{888888}[?] 📖 Buku Panduan Bermain Lengkap Server\n");

	#pragma unused count

	ShowPlayerDialog(playerid, DIALOG_ACTION_HUB, DIALOG_STYLE_LIST,
		isEn ? "NUSANTARA RP - INTERACTION & ACTION HUB" : "NUSANTARA RP - PUSAT AKSI & INTERAKSI LENGKAP",
		dialogList, isEn ? "Select" : "Pilih", isEn ? "Close" : "Tutup");
	return 1;
}

stock OnActionHubDialogResponse(playerid, listitem)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (listitem < 0 || listitem >= MAX_HUB_ACTIONS) return 0;

	new act = gPlayerHubAction[playerid][listitem];
	new target = gPlayerHubContextTarget[playerid];

	switch (act) {
		case HUB_ACT_ENGINE: {
			new veh = GetPlayerVehicleID(playerid);
			if (veh > 0) Vehicle_SetEngineOn(veh, !Vehicle_IsEngineOn(veh));
		}
		case HUB_ACT_LOCK: {
			new veh = GetPlayerVehicleID(playerid);
			if (veh > 0) Vehicle_SetLocked(veh, !Vehicle_IsLocked(veh));
		}
		case HUB_ACT_LIGHTS: {
			new veh = GetPlayerVehicleID(playerid);
			new slot = Vehicle_GetSlot(veh);
			if (slot != -1) Vehicle_SetLights(veh, !gVehicleData[slot][vLights]);
		}
		case HUB_ACT_HOOD: {
			new veh = GetPlayerVehicleID(playerid);
			new slot = Vehicle_GetSlot(veh);
			if (slot != -1) Vehicle_SetHood(veh, !gVehicleData[slot][vHood]);
		}
		case HUB_ACT_TRUNK: {
			new veh = GetPlayerVehicleID(playerid);
			new slot = Vehicle_GetSlot(veh);
			if (slot != -1) Vehicle_SetTrunk(veh, !gVehicleData[slot][vTrunkOpen]);
		}
		case HUB_ACT_REFUEL:         ProcessRefuel(playerid);
		case HUB_ACT_TAXI_METER:      ProcessToggleTaxiMeter(playerid, DEFAULT_TAXI_FARE);
		case HUB_ACT_TOW:             ProcessTowVehicle(playerid);
		case HUB_ACT_UNTOW:           ProcessUntowVehicle(playerid);
		case HUB_ACT_IMPOUND:         ProcessImpoundVehicle(playerid);
		case HUB_ACT_UNRENT:          OnRentalDialogResponse(playerid, 4);
		case HUB_ACT_PLATE: {
			ShowPlayerDialog(playerid, DIALOG_ACTION_PLATE_INPUT, DIALOG_STYLE_INPUT,
				"PENDAFTARAN PLAT NOMOR KUSTOM DMV",
				"Masukkan kombinasi plat nomor baru (3-8 karakter, misal: REZZ-01):", "Daftarkan", "Batal");
		}
		case HUB_ACT_NEAR_TRUNK: {
			if (target >= 0 && target < MAX_OWNED_VEHICLES) ShowVehicleTrunkDialog(playerid, target);
		}
		case HUB_ACT_LOCKPICK:        ProcessLockpickVehicle(playerid);
		case HUB_ACT_CHOPSHOP:        ProcessChopVehicle(playerid);
		case HUB_ACT_REPAIR_TIRE:     ProcessRepairTire(playerid);
		case HUB_ACT_POUR_FUEL:       ProcessPourFuelCanister(playerid);
		case HUB_ACT_STORE_BUY:       ShowStoreBuyMenu(playerid);
		case HUB_ACT_STORE_ROB:       ProcessRobStore(playerid);
		case HUB_ACT_BANK:            ShowBankDialog(playerid);
		case HUB_ACT_ATM:             ShowATMDialog(playerid);
		case HUB_ACT_PAY_TICKET:      ProcessPayTrafficTickets(playerid);
		case HUB_ACT_TRASH_SCAVENGE:  ProcessPlayerScavenge(playerid);
		case HUB_ACT_WEED_HARVEST:    ProcessHarvestWeed(playerid);
		case HUB_ACT_WEED_BURN:       ProcessBurnWeed(playerid);
		case HUB_ACT_DETECT:          ProcessMetalDetect(playerid);
		case HUB_ACT_DIG:             ProcessDigTreasure(playerid);
		case HUB_ACT_FISH:            ProcessFish(playerid);
		case HUB_ACT_CAMPFIRE_COOK:   ProcessCookFish(playerid);
		case HUB_ACT_CAMPFIRE_EAT:    ProcessEatCookedFish(playerid);
		case HUB_ACT_GRAFFITI_TAG: {
			ShowPlayerDialog(playerid, DIALOG_ACTION_TAG_INPUT, DIALOG_STYLE_INPUT,
				"SEMPROT CAT GRAFITI WILAYAH",
				"Ketikkan teks grafiti yang ingin disemprotkan ke dinding (2-40 karakter):", "Semprot", "Batal");
		}
		case HUB_ACT_GRAFFITI_CLEAN:  ProcessCleanGraffiti(playerid);
		case HUB_ACT_VENDING:         ShowVendingDialog(playerid);
		case HUB_ACT_BLACKMARKET:     ShowBlackMarketDialog(playerid);
		case HUB_ACT_SMUGGLE:         ProcessStartSmuggle(playerid);
		case HUB_ACT_BAR:             ShowBarDrinksMenu(playerid);
		case HUB_ACT_GYM_STYLES:      ShowGymStyleDialog(playerid);
		case HUB_ACT_GYM_WORKOUT:     ProcessGymWorkout(playerid);
		case HUB_ACT_FASTFOOD:        ShowFastFoodMenu(playerid);
		case HUB_ACT_AMMUNATION:      ShowAmmuNationMenu(playerid);
		case HUB_ACT_DMV:             ShowDMVExamDialog(playerid);
		case HUB_ACT_CLAIM_VEH:       ProcessClaimImpoundedVehicle(playerid);
		case HUB_ACT_RENTAL:          ShowRentalMenuDialog(playerid);
		case HUB_ACT_DOOR_ENTER:      ProcessDoorEnter(playerid);
		case HUB_ACT_DOOR_EXIT:       ProcessDoorExit(playerid);
		case HUB_ACT_PLAYER_MENU: {
			if (target >= 0 && target < MAX_PLAYERS) ShowPlayerNearActionMenu(playerid, target);
		}
		case HUB_ACT_INV:             ShowPlayerInventoryUI(playerid);
		case HUB_ACT_PHONE:           ShowPlayerPhone(playerid);
		case HUB_ACT_MYVEH:           ShowVehicleActionDialog(playerid, 0);
		case HUB_ACT_JOBS:            ShowJobDialog(playerid);
		case HUB_ACT_PLANT_WEED:      ProcessPlantWeed(playerid);
		case HUB_ACT_CAMPFIRE_BUILD:  ProcessCreateCampfire(playerid);
		case HUB_ACT_CALL_TAXI:       ProcessCallTaxi(playerid);
		case HUB_ACT_CALL_911: {
			ShowPlayerDialog(playerid, DIALOG_911_SERVICE, DIALOG_STYLE_LIST,
				"LAYANAN DARURAT 911", "1. Kepolisian (SAPD)\n2. Ambulans & Medis (SAMD)", "Pilih", "Batal");
		}
		case HUB_ACT_AD: {
			ShowPlayerDialog(playerid, DIALOG_ACTION_AD_INPUT, DIALOG_STYLE_INPUT,
				"PASANG IKLAN BARIS SANEWS ($50)",
				"Masukkan teks iklan baris komersial Anda (5-100 karakter):", "Kirim", "Batal");
		}
		case HUB_ACT_ACCESSORIES:     ShowAccessoriesMenu(playerid);
		case HUB_ACT_STATS:           ShowStatsDialog(playerid);
		case HUB_ACT_HELP:            ShowHelpDialog(playerid);
		case HUB_ACT_LANG: {
			ShowPlayerDialog(playerid, DIALOG_LANGUAGE_SELECT, DIALOG_STYLE_LIST,
				"SYSTEM LANGUAGE / PILIHAN BAHASA", "1. Bahasa Indonesia [ID] (Default)\n2. English [EN]", "Pilih", "Batal");
		}
	}
	return 1;
}

stock ShowPlayerNearActionMenu(playerid, targetid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;
	if (!IsPlayerConnected(targetid) || !gPlayerData[targetid][pLoggedIn]) return 0;

	gPlayerHubContextTarget[playerid] = targetid;

	new list[700];
	format(list, sizeof(list),
		"1. Berikan Uang Tunai (/pay)\n\
2. Berikan Barang dari Inventaris (/give)\n\
3. Ajak Main Taruhan Dadu Kasino ($5 - $5.000)\n\
4. Bagikan Kunci Cadangan Rumah\n\
5. Tarik Kembali Kunci Rumah");

	if (gPlayerData[playerid][pFaction] == FACTION_POLICE && gPlayerData[playerid][pDuty]) {
		format(list, sizeof(list), "%s\n\
{33CCFF}6. [SAPD] Pasang Borgol / Lepas Borgol Tersangka\n\
{33CCFF}7. [SAPD] Geledah Barang Bawaan (Frisk)\n\
{33CCFF}8. [SAPD] Sita Barang Bukti Kejahatan (Confiscate)\n\
{33CCFF}9. [SAPD] Periksa Kelayakan SIM Pengemudi\n\
{33CCFF}10. [SAPD] Uji Kadar Alkohol Breathalyzer\n\
{33CCFF}11. [SAPD] Terbitkan Lisensi Senjata Api Legal", list);
	} else if (gPlayerData[playerid][pFaction] == FACTION_MEDIC && gPlayerData[playerid][pDuty]) {
		format(list, sizeof(list), "%s\n\
{4CD964}6. [SAMD] Berikan Perawatan P3K Perban (/treat)\n\
{4CD964}7. [SAMD] Resusitasi CPR Korban Sekarat (/revive)", list);
	}

	new title[64];
	format(title, sizeof(title), "INTERAKSI DENGAN: %s", gPlayerData[targetid][pName]);
	ShowPlayerDialog(playerid, DIALOG_ACTION_NEAR_PLAYER, DIALOG_STYLE_LIST, title, list, "Pilih", "Kembali");
	return 1;
}

stock OnActionNearPlayerResponse(playerid, listitem)
{
	new targetid = gPlayerHubContextTarget[playerid];
	if (targetid < 0 || targetid >= MAX_PLAYERS || !IsPlayerConnected(targetid)) return 0;

	switch (listitem) {
		case 0: {
			// Beri uang
			gGiveTarget[playerid] = targetid;
			ShowPlayerDialog(playerid, DIALOG_PAY, DIALOG_STYLE_INPUT,
				"BERIKAN UANG TUNAI", "Masukkan nominal uang tunai yang ingin diberikan:", "Beri", "Batal");
		}
		case 1: {
			// Beri item
			gGiveTarget[playerid] = targetid;
			ShowPlayerInventoryUI(playerid);
		}
		case 2: {
			// Main dadu $100
			ProcessPlayDice(playerid, targetid, 100);
		}
		case 3: ProcessShareHouseKey(playerid, targetid);
		case 4: ProcessRevokeHouseKey(playerid, targetid);
		case 5: {
			if (gPlayerData[playerid][pFaction] == FACTION_POLICE) {
				if (!gPlayerData[targetid][pCuffed]) {
					gPlayerData[targetid][pCuffed] = true;
					SetPlayerSpecialAction(targetid, SPECIAL_ACTION_CUFFED);
					SendClientMessage(targetid, COLOR_ERROR, "Anda telah diborgol oleh petugas polisi!");
					SendClientMessage(playerid, COLOR_SUCCESS, "Berhasil memborgol tersangka.");
				} else {
					gPlayerData[targetid][pCuffed] = false;
					SetPlayerSpecialAction(targetid, SPECIAL_ACTION_NONE);
					SendClientMessage(targetid, COLOR_SUCCESS, "Borgol Anda telah dilepas oleh petugas.");
					SendClientMessage(playerid, COLOR_INFO, "Borgol tersangka berhasil dilepas.");
				}
			} else if (gPlayerData[playerid][pFaction] == FACTION_MEDIC) {
				// Treat
				gPlayerData[targetid][pInjured] = false;
				gPlayerData[targetid][pInjuredTime] = 0;
				SetPlayerHealth(targetid, 35.0);
				SendClientMessage(targetid, COLOR_SUCCESS, "Anda telah diberikan pertolongan pertama oleh petugas medis!");
				SendClientMessage(playerid, COLOR_SUCCESS, "Berhasil memberikan pertolongan pertama.");
			}
		}
		case 6: {
			if (gPlayerData[playerid][pFaction] == FACTION_POLICE) {
				// Frisk
				new invStr[300];
				format(invStr, sizeof(invStr),
					"Uang Tunai: $%d | Paket Kontraband: %d | Suku Cadang: %d | Lockpick: %d",
					gPlayerData[targetid][pMoney],
					GetPlayerItemCount(targetid, ITEM_CONTRABAND),
					GetPlayerItemCount(targetid, ITEM_CARPARTS),
					GetPlayerItemCount(targetid, ITEM_LOCKPICK));
				ShowPlayerDialog(playerid, DIALOG_HELP, DIALOG_STYLE_MSGBOX, "HASIL PENGGELEDAHAN", invStr, "Tutup", "");
			} else if (gPlayerData[playerid][pFaction] == FACTION_MEDIC) {
				// Revive
				gPlayerData[targetid][pInjured] = false;
				gPlayerData[targetid][pInjuredTime] = 0;
				SetPlayerHealth(targetid, 100.0);
				SendClientMessage(targetid, COLOR_SUCCESS, "Anda telah diresusitasi penuh oleh petugas medis!");
				SendClientMessage(playerid, COLOR_SUCCESS, "Berhasil melakukan resusitasi CPR.");
			}
		}
		case 7: {
			if (gPlayerData[playerid][pFaction] == FACTION_POLICE) ProcessConfiscateIllicitItems(playerid, targetid);
		}
		case 8: {
			if (gPlayerData[playerid][pFaction] == FACTION_POLICE) ProcessCheckDriverLicense(playerid, targetid);
		}
		case 9: {
			if (gPlayerData[playerid][pFaction] == FACTION_POLICE) ProcessBreathalyzerTest(playerid, targetid);
		}
		case 10: {
			if (gPlayerData[playerid][pFaction] == FACTION_POLICE) ProcessIssueGunLicense(playerid, targetid);
		}
	}
	return 1;
}
