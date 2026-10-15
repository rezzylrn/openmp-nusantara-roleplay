#if defined _INC_illegal_activities
	#endinput
#endif
#define _INC_illegal_activities

/*
==============================================================================
 illegal_activities.inc
 NUSANTARA ROLEPLAY - Underworld Economy, Contraband Smuggling, Chop Shop,
                      Vehicle Lockpicking & Black Market Dealer
==============================================================================
*/

#define SMUGGLER_DEPOT_X        2780.0
#define SMUGGLER_DEPOT_Y        -2450.0
#define SMUGGLER_DEPOT_Z        13.6

#define CHOPSHOP_X              2490.5
#define CHOPSHOP_Y              -2125.0
#define CHOPSHOP_Z              13.5

#define BLACKMARKET_X           2225.5
#define BLACKMARKET_Y           -2280.0
#define BLACKMARKET_Z           14.5

// Titik Drop Penyelundupan Kontraband
static const Float:gSmugglerDrops[][4] = {
	{1780.5, -2050.2, 13.5, 0.0}, // El Corona Alley
	{1828.0, -1682.0, 13.5, 0.0}, // Alhambra Backstage
	{1550.0, -1350.0, 13.5, 0.0}, // Commerce Underground
	{2450.0, -2380.0, 13.5, 0.0}  // Ocean Docks Pier
};

static const gSmugglerDropNames[][] = {
	"Gudang Belakang El Corona",
	"Lorong Gelap Alhambra",
	"Garasi Bawah Tanah Commerce",
	"Dermaga Kargo Ocean Docks"
};

// Player states untuk penyelundupan
new bool:gPlayerSmuggleActive[MAX_PLAYERS];
new gPlayerSmuggleDrop[MAX_PLAYERS];
new gPlayerSmuggleCooldown[MAX_PLAYERS];
new gPlayerLockpickCooldown[MAX_PLAYERS];

// Pickup & 3D Text Labels
new gSmugglerPickup;
new Text3D:gSmugglerLabel;
new gChopShopPickup;
new Text3D:gChopShopLabel;
new gBlackMarketPickup;
new Text3D:gBlackMarketLabel;

#pragma unused gSmugglerPickup, gSmugglerLabel, gChopShopPickup, gChopShopLabel, gBlackMarketPickup, gBlackMarketLabel

stock IllegalActivities_Init()
{
	for (new i = 0; i < MAX_PLAYERS; i++) {
		gPlayerSmuggleActive[i] = false;
		gPlayerSmuggleDrop[i] = -1;
		gPlayerSmuggleCooldown[i] = 0;
		gPlayerLockpickCooldown[i] = 0;
	}

	// 1. Sindikat Penyelundupan Ocean Docks
	gSmugglerPickup = CreatePickup(1210, 1, SMUGGLER_DEPOT_X, SMUGGLER_DEPOT_Y, SMUGGLER_DEPOT_Z, 0);
	gSmugglerLabel = Create3DTextLabel(
		"{FF3B30}[ SINDIKAT PENYELUNDUP KONTRA BAND ]\n{FFFFFF}Ambil paket gelap rahasia untuk diantar\n{FFFF00}Gunakan /smuggle atau /selundup",
		0xFF3B30FF, SMUGGLER_DEPOT_X, SMUGGLER_DEPOT_Y, SMUGGLER_DEPOT_Z + 0.8, 15.0, 0, true);

	// 2. Illegal Chop Shop (Peleburan Suku Cadang)
	gChopShopPickup = CreatePickup(1239, 1, CHOPSHOP_X, CHOPSHOP_Y, CHOPSHOP_Z, 0);
	gChopShopLabel = Create3DTextLabel(
		"{FF3B30}[ BENGKEL GELAP CHOP SHOP ]\n{FFFFFF}Bongkar & mutilasi suku cadang kendaraan curian\n{FFFF00}Bawa kendaraan & gunakan /chopveh",
		0xFF3B30FF, CHOPSHOP_X, CHOPSHOP_Y, CHOPSHOP_Z + 0.8, 15.0, 0, true);

	// 3. Black Market Dealer
	gBlackMarketPickup = CreatePickup(1274, 1, BLACKMARKET_X, BLACKMARKET_Y, BLACKMARKET_Z, 0);
	gBlackMarketLabel = Create3DTextLabel(
		"{FF3B30}[ BLACK MARKET DEALER ]\n{FFFFFF}Penyedia perlengkapan & penadah hasil kejahatan\n{FFFF00}Gunakan /blackmarket atau /bm",
		COLOR_RED, BLACKMARKET_X, BLACKMARKET_Y, BLACKMARKET_Z + 0.8, 15.0, 0, true);

	print("[UNDERWORLD] Modul aktivitas ilegal (Penyelundup, Chop Shop, Lockpick & Black Market) aktif.");
	return 1;
}

// ---------------------------------------------------------------------------
// 1. Penyelundupan Paket Kontraband Gelap
// ---------------------------------------------------------------------------
stock ProcessStartSmuggle(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;

	if (gPlayerData[playerid][pFaction] == FACTION_POLICE && gPlayerData[playerid][pDuty]) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Petugas kepolisian yang sedang bertugas dilarang terlibat penyelundupan!");
		return 0;
	}

	if (!IsPlayerNearPoint(playerid, SMUGGLER_DEPOT_X, SMUGGLER_DEPOT_Y, SMUGGLER_DEPOT_Z, 4.0)) {
		new errStr[128];
		if (gPlayerData[playerid][pLanguage] == LANG_EN) format(errStr, sizeof(errStr), "ERROR: You must be at the Smuggler Depot in Ocean Docks.");
		else format(errStr, sizeof(errStr), "ERROR: Anda harus berada di Markas Penyelundup Ocean Docks.");
		SendClientMessage(playerid, COLOR_ERROR, errStr);
		return 0;
	}

	if (gPlayerSmuggleActive[playerid]) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda masih memiliki paket selundupan yang belum diantar!");
		return 0;
	}

	new now = gettime();
	if (now < gPlayerSmuggleCooldown[playerid]) {
		new sLeft = gPlayerSmuggleCooldown[playerid] - now;
		new waitStr[128];
		if (gPlayerData[playerid][pLanguage] == LANG_EN) format(waitStr, sizeof(waitStr), "ERROR: Police are actively watching the port! Wait %d seconds.", sLeft);
		else format(waitStr, sizeof(waitStr), "ERROR: Area dermaga sedang dipantau ketat polisi! Tunggu %d detik lagi.", sLeft);
		SendClientMessage(playerid, COLOR_ERROR, waitStr);
		return 0;
	}

	// Berikan paket kontraband ke inventaris
	if (!GivePlayerItem(playerid, ITEM_CONTRABAND, 1)) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Kantung inventaris Anda penuh untuk membawa paket kontraband.");
		return 0;
	}

	// Pilih titik drop acak
	new dropIdx = random(sizeof(gSmugglerDrops));
	gPlayerSmuggleActive[playerid] = true;
	gPlayerSmuggleDrop[playerid] = dropIdx;

	SetPlayerCheckpoint(playerid,
		gSmugglerDrops[dropIdx][0],
		gSmugglerDrops[dropIdx][1],
		gSmugglerDrops[dropIdx][2], 4.0);

	PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);

	new msg[160];
	if (gPlayerData[playerid][pLanguage] == LANG_EN) {
		format(msg, sizeof(msg),
			"[SMUGGLER] Deliver the contraband package to: %s! Check your GPS radar.",
			gSmugglerDropNames[dropIdx]);
	} else {
		format(msg, sizeof(msg),
			"[PENYELUNDUP] Antarkan paket kontraband ini ke: %s! Ikuti titik merah di radar GPS Anda.",
			gSmugglerDropNames[dropIdx]);
	}
	SendClientMessage(playerid, COLOR_YELLOW, msg);
	return 1;
}

stock ProcessSmuggleDelivery(playerid)
{
	if (!gPlayerSmuggleActive[playerid]) return 0;

	new dropIdx = gPlayerSmuggleDrop[playerid];
	if (dropIdx < 0 || dropIdx >= sizeof(gSmugglerDrops)) return 0;

	if (GetPlayerItemCount(playerid, ITEM_CONTRABAND) <= 0) {
		DisablePlayerCheckpoint(playerid);
		gPlayerSmuggleActive[playerid] = false;
		gPlayerSmuggleDrop[playerid] = -1;
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Paket kontraband tidak ditemukan di kantung Anda (mungkin disita/hilang).");
		return 0;
	}

	// Ambil paket kontraband dari inventaris
	TakePlayerItem(playerid, ITEM_CONTRABAND, 1);

	// Hadiah uang tunai kotor ($800 - $1400)
	new reward = 800 + random(601);
	GivePlayerMoneyEx(playerid, reward, "pengantaran kontraband ilegal");

	DisablePlayerCheckpoint(playerid);
	gPlayerSmuggleActive[playerid] = false;
	gPlayerSmuggleDrop[playerid] = -1;
	gPlayerSmuggleCooldown[playerid] = gettime() + 180; // 3 menit cooldown

	PlayerPlaySound(playerid, 1058, 0.0, 0.0, 0.0);

	new succMsg[160];
	if (gPlayerData[playerid][pLanguage] == LANG_EN) {
		format(succMsg, sizeof(succMsg),
			"[SMUGGLER] Package delivered safely to the contact! You earned $%d in illicit cash.", reward);
	} else {
		format(succMsg, sizeof(succMsg),
			"[PENYELUNDUP] Paket kontraband berhasil diserahkan ke penadah! Anda menerima $%d uang tunai.", reward);
	}
	SendClientMessage(playerid, COLOR_SUCCESS, succMsg);
	return 1;
}

// ---------------------------------------------------------------------------
// 2. Mutilasi Kendaraan Curian / Illegal Chop Shop
// ---------------------------------------------------------------------------
stock ProcessChopVehicle(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;

	if (!IsPlayerInAnyVehicle(playerid) || GetPlayerState(playerid) != PLAYER_STATE_DRIVER) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus mengendarai kendaraan di depan bengkel Chop Shop.");
		return 0;
	}

	if (!IsPlayerNearPoint(playerid, CHOPSHOP_X, CHOPSHOP_Y, CHOPSHOP_Z, 5.0)) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus berada di area bengkel Chop Shop Ocean Docks.");
		return 0;
	}

	// Butuh alat linggis atau repair kit
	if (GetPlayerItemCount(playerid, ITEM_CROWBAR) <= 0 && GetPlayerItemCount(playerid, ITEM_REPAIRKIT) <= 0) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda memerlukan Linggis Baja (Crowbar) atau Toolkit untuk membongkar mobil.");
		return 0;
	}

	new vehicleid = GetPlayerVehicleID(playerid);
	new model = GetVehicleModel(vehicleid);

	// Dilarang mencopot kendaraan dinas darurat
	if (model == 596 || model == 597 || model == 598 || model == 416 || model == 427) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Penadah menolak membongkar armada dinas kepolisian/ambulans!");
		return 0;
	}

	// [SECURITY] Tolak kendaraan milik pemain: mencegah money-printer (chop
	// mobil sendiri, restart server -> mobil di-spawn ulang oleh Vehicle_SpawnAll
	// lalu di-chop lagi tanpa batas). Kendaraan milik pemain harus dijual lewat
	// dealer, bukan dimutilasi di chop shop.
	new ownedSlot = Vehicle_GetSlot(vehicleid);
	if (ownedSlot != -1) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Penadah menolak mobil ber-STNK resmi! Jual kendaraan Anda di dealer, bukan di sini.");
		return 0;
	}

	// Keluarkan pemain dan hapus mobil
	RemovePlayerFromVehicle(playerid);
	Vehicle_SafeDestroy(vehicleid);

	// Hasil bongkaran: Uang tunai ($750 - $1600) + 2x - 4x Suku Cadang Curian
	new cashEarned = 750 + random(851);
	new partsEarned = 2 + random(3);

	GivePlayerMoneyEx(playerid, cashEarned, "mutilasi mobil chop shop");
	GivePlayerItem(playerid, ITEM_CARPARTS, partsEarned);

	PlayerPlaySound(playerid, 1133, 0.0, 0.0, 0.0);

	new chopMsg[180];
	if (gPlayerData[playerid][pLanguage] == LANG_EN) {
		format(chopMsg, sizeof(chopMsg),
			"[CHOP SHOP] Vehicle dismantled into parts! Earned: $%d and %dx Stolen Car Parts.", cashEarned, partsEarned);
	} else {
		format(chopMsg, sizeof(chopMsg),
			"[CHOP SHOP] Rangka mobil berhasil dimutilasi! Menerima: $%d tunai dan %dx Suku Cadang Mobil Curian.", cashEarned, partsEarned);
	}
	SendClientMessage(playerid, COLOR_SUCCESS, chopMsg);

	// 35% Chance memicu alarm ke kepolisian SAPD
	if (random(100) < 35) {
		new zoneName[48];
		GetPlayerZoneName(playerid, zoneName, sizeof(zoneName));
		new policeRadio[160];
		format(policeRadio, sizeof(policeRadio),
			"[DISPATCH 911] Laporan warga: Aktivitas pemotongan mobil curian (Chop Shop) terdeteksi di %s!", zoneName);

		for (new p = 0; p < MAX_PLAYERS; p++) {
			if (IsPlayerConnected(p) && gPlayerData[p][pLoggedIn] && gPlayerData[p][pDuty] && gPlayerData[p][pFaction] == FACTION_POLICE) {
				SendClientMessage(p, COLOR_RADIO, policeRadio);
			}
		}
	}
	return 1;
}

// ---------------------------------------------------------------------------
// 3. Pembobolan Kunci Kendaraan (Lockpicking)
// ---------------------------------------------------------------------------
stock ProcessLockpickVehicle(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;

	if (IsPlayerInAnyVehicle(playerid)) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus berada di luar kendaraan untuk membobol kunci.");
		return 0;
	}

	if (GetPlayerItemCount(playerid, ITEM_LOCKPICK) <= 0) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda memerlukan Obeng Lockpick di dalam inventaris (/inv). Beli di Black Market.");
		return 0;
	}

	new now = gettime();
	if (now < gPlayerLockpickCooldown[playerid]) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Tenangkan diri Anda sebelum mencoba membobol kunci kembali.");
		return 0;
	}

	// Cari kendaraan terdekat dalam radius 3.5m
	new targetVehSlot = -1;
	for (new i = 0; i < MAX_OWNED_VEHICLES; i++) {
		if (gVehicleData[i][vUsed] && gVehicleData[i][vVehicleHandle] != 0) {
			if (IsPlayerNearPoint(playerid, gVehicleData[i][vPosX], gVehicleData[i][vPosY], gVehicleData[i][vPosZ], 3.8)) {
				targetVehSlot = i;
				break;
			}
		}
	}

	if (targetVehSlot == -1) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Tidak ada kendaraan terdekat dalam radius 3.5 meter.");
		return 0;
	}

	if (!gVehicleData[targetVehSlot][vLocked]) {
		SendClientMessage(playerid, COLOR_INFO, "INFO: Pintu kendaraan ini sudah dalam keadaan tidak terkunci.");
		return 0;
	}

	// Gunakan 1x Lockpick
	TakePlayerItem(playerid, ITEM_LOCKPICK, 1);
	gPlayerLockpickCooldown[playerid] = now + 12;

	// Animasi membungkuk utak-atik pintu mobil
	ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.1, false, false, false, false, 0);

	// Peluang keberhasilan: 65%
	new bool:success = (random(100) < 65);

	if (success) {
		gVehicleData[targetVehSlot][vLocked] = false;
		SetVehicleParamsForPlayer(gVehicleData[targetVehSlot][vVehicleHandle], playerid, 0, 0);

		PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
		if (gPlayerData[playerid][pLanguage] == LANG_EN) {
			SendClientMessage(playerid, COLOR_SUCCESS, "SUKSES: Door lock picked open! Car alarm is blaring!");
		} else {
			SendClientMessage(playerid, COLOR_SUCCESS, "SUKSES: Kunci pintu mobil berhasil dibobol! Alarm mobil berbunyi nyaring!");
		}

		// Alarm berbunyi dan kirim notifikasi 911 ke SAPD
		new zoneName[48];
		GetPlayerZoneName(playerid, zoneName, sizeof(zoneName));
		new alarmAlert[160];
		format(alarmAlert, sizeof(alarmAlert),
			"[ALARM 911] Alarm Kendaraan Berbunyi: Percobaan pembobolan mobil dilaporkan di %s!", zoneName);

		for (new p = 0; p < MAX_PLAYERS; p++) {
			if (IsPlayerConnected(p) && gPlayerData[p][pLoggedIn] && gPlayerData[p][pDuty] && gPlayerData[p][pFaction] == FACTION_POLICE) {
				SendClientMessage(p, COLOR_RADIO, alarmAlert);
			}
		}
	} else {
		PlayerPlaySound(playerid, 1053, 0.0, 0.0, 0.0);
		if (gPlayerData[playerid][pLanguage] == LANG_EN) {
			SendClientMessage(playerid, COLOR_ERROR, "ERROR: The lockpick broke inside the keyhole!");
		} else {
			SendClientMessage(playerid, COLOR_ERROR, "ERROR: Obeng lockpick patah di dalam lubang kunci!");
		}
	}
	return 1;
}

// ---------------------------------------------------------------------------
// 4. Black Market Dealer Menu & Transaksi
// ---------------------------------------------------------------------------
stock ShowBlackMarketDialog(playerid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;

	if (!IsPlayerNearPoint(playerid, BLACKMARKET_X, BLACKMARKET_Y, BLACKMARKET_Z, 4.0)) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus berada di lokasi Black Market Dealer Ocean Docks.");
		return 0;
	}

	new dialogContent[550];
	if (gPlayerData[playerid][pLanguage] == LANG_EN) {
		format(dialogContent, sizeof(dialogContent),
			"Item / Tool	Price\n\
{FFFFFF}1. Lockpick Tool	{FF3B30}$150\n\
{FFFFFF}2. Steel Crowbar	{FF3B30}$100\n\
{FFFFFF}3. Illegal Colt .45 Pistol (50 Ammo)	{FF3B30}$1,200\n\
{FFFFFF}4. Raw Contraband Crate	{FF3B30}$250\n\
{FFFFFF}5. Pack of Weed Seeds	{FF3B30}$120\n\
{4CD964}$ Sell Stolen / Illicit Goods	{4CD964}->");

		RP_ShowPlayerDialog(playerid, DIALOG_BLACKMARKET_BUY, DIALOG_STYLE_TABLIST_HEADERS,
			"UNDERWORLD BLACK MARKET", dialogContent, "Select", "Close");
	} else {
		format(dialogContent, sizeof(dialogContent),
			"Barang Gelap / Peralatan	Harga\n\
{FFFFFF}1. Obeng Lockpick Pembobol Kunci	{FF3B30}$150\n\
{FFFFFF}2. Linggis Baja (Crowbar)	{FF3B30}$100\n\
{FFFFFF}3. Pistol Rakitan Colt .45 (50 Peluru)	{FF3B30}$1.200\n\
{FFFFFF}4. Paket Kontraband Mentah	{FF3B30}$250\n\
{FFFFFF}5. Sebungkus Benih Ganja	{FF3B30}$120\n\
{4CD964}$ Jual Suku Cadang & Barang Sitaan	{4CD964}->");

		RP_ShowPlayerDialog(playerid, DIALOG_BLACKMARKET_BUY, DIALOG_STYLE_TABLIST_HEADERS,
			"PASAR GELAP / BLACK MARKET", dialogContent, "Pilih", "Tutup");
	}
	return 1;
}

stock OnBlackMarketBuyResponse(playerid, listitem)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;

	// Opsi 5: Beralih ke menu Jual
	if (listitem == 5) {
		new sellContent[550];
		new parts = GetPlayerItemCount(playerid, ITEM_CARPARTS);
		new contra = GetPlayerItemCount(playerid, ITEM_CONTRABAND);
		new trash = GetPlayerItemCount(playerid, ITEM_RECYCLED);
		new weed = GetPlayerItemCount(playerid, ITEM_WEED);
		new treasure = GetPlayerItemCount(playerid, ITEM_TREASURE);

		format(sellContent, sizeof(sellContent),
			"Barang Jarahan	Jumlah Dimiliki	Harga Beli Penadah\n\
{FFFFFF}1. Suku Cadang Mobil Curian	{FFFF00}%dx	{4CD964}$280 / pcs\n\
{FFFFFF}2. Paket Kontraband Gelap	{FFFF00}%dx	{4CD964}$650 / pcs\n\
{FFFFFF}3. Daun Ganja Siap Edar	{FFFF00}%dx	{4CD964}$110 / pcs\n\
{FFFFFF}4. Harta Karun Antik Terpendam	{FFFF00}%dx	{4CD964}$250 / pcs\n\
{FFFFFF}5. Logam Daur Ulang Pemulung	{FFFF00}%dx	{4CD964}$35 / pcs",
			parts, contra, weed, treasure, trash);

		RP_ShowPlayerDialog(playerid, DIALOG_BLACKMARKET_SELL, DIALOG_STYLE_TABLIST_HEADERS,
			"PENADAH HASIL KEJAHATAN", sellContent, "Jual", "Kembali");
		return 1;
	}

	new cost = 0;
	switch (listitem) {
		case 0: cost = 150;  // Lockpick
		case 1: cost = 100;  // Crowbar
		case 2: cost = 1200; // Colt .45
		case 3: cost = 250;  // Contraband
		case 4: cost = 120;  // Weed Seeds
		default: return 1;
	}

	if (gPlayerData[playerid][pMoney] < cost) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Uang tunai Anda tidak mencukupi untuk membeli barang ini.");
		return 1;
	}

	switch (listitem) {
		case 0: {
			if (!GivePlayerItem(playerid, ITEM_LOCKPICK, 1)) {
				SendClientMessage(playerid, COLOR_ERROR, "ERROR: Kantung inventaris Anda penuh.");
				return 1;
			}
			TakePlayerMoneyEx(playerid, cost, "beli lockpick pasar gelap");
			SendClientMessage(playerid, COLOR_SUCCESS, "SUKSES: Membeli 1x Obeng Lockpick seharga $150.");
		}
		case 1: {
			if (!GivePlayerItem(playerid, ITEM_CROWBAR, 1)) {
				SendClientMessage(playerid, COLOR_ERROR, "ERROR: Kantung inventaris Anda penuh.");
				return 1;
			}
			TakePlayerMoneyEx(playerid, cost, "beli linggis pasar gelap");
			SendClientMessage(playerid, COLOR_SUCCESS, "SUKSES: Membeli 1x Linggis Baja seharga $100.");
		}
		case 2: {
			TakePlayerMoneyEx(playerid, cost, "beli senjata pasar gelap");
			AC_GiveWeapon(playerid, WEAPON_COLT45, 50);
			SendClientMessage(playerid, COLOR_SUCCESS, "SUKSES: Membeli 1x Pistol Rakitan Colt .45 (50 Peluru) seharga $1,200.");
		}
		case 3: {
			if (!GivePlayerItem(playerid, ITEM_CONTRABAND, 1)) {
				SendClientMessage(playerid, COLOR_ERROR, "ERROR: Kantung inventaris Anda penuh.");
				return 1;
			}
			TakePlayerMoneyEx(playerid, cost, "beli kontraband pasar gelap");
			SendClientMessage(playerid, COLOR_SUCCESS, "SUKSES: Membeli 1x Paket Kontraband Mentah seharga $250.");
		}
		case 4: {
			if (!GivePlayerItem(playerid, ITEM_WEEDSEED, 3)) {
				SendClientMessage(playerid, COLOR_ERROR, "ERROR: Kantung inventaris Anda penuh.");
				return 1;
			}
			TakePlayerMoneyEx(playerid, cost, "beli benih ganja");
			SendClientMessage(playerid, COLOR_SUCCESS, "SUKSES: Membeli 3x Benih Tanaman Ganja seharga $120.");
		}
	}
	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
	return 1;
}

stock OnBlackMarketSellResponse(playerid, listitem)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;

	switch (listitem) {
		case 0: {
			// Suku Cadang Curian ($280 / pcs)
			new count = GetPlayerItemCount(playerid, ITEM_CARPARTS);
			if (count <= 0) {
				SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda tidak membawa suku cadang mobil curian.");
				return 1;
			}
			new earnings = count * 280;
			TakePlayerItem(playerid, ITEM_CARPARTS, count);
			GivePlayerMoneyEx(playerid, earnings, "jual suku cadang curian");
			new msg[128];
			format(msg, sizeof(msg), "SUKSES: Menjual %dx Suku Cadang Curian ke penadah seharga $%d.", count, earnings);
			SendClientMessage(playerid, COLOR_SUCCESS, msg);
		}
		case 1: {
			// Paket Kontraband ($650 / pcs)
			new count = GetPlayerItemCount(playerid, ITEM_CONTRABAND);
			if (count <= 0) {
				SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda tidak membawa paket kontraband.");
				return 1;
			}
			new earnings = count * 650;
			TakePlayerItem(playerid, ITEM_CONTRABAND, count);
			GivePlayerMoneyEx(playerid, earnings, "jual kontraband gelap");
			new msg[128];
			format(msg, sizeof(msg), "SUKSES: Menjual %dx Paket Kontraband ke penadah seharga $%d.", count, earnings);
			SendClientMessage(playerid, COLOR_SUCCESS, msg);
		}
		case 2: {
			// Daun Ganja ($110 / pcs)
			new count = GetPlayerItemCount(playerid, ITEM_WEED);
			if (count <= 0) {
				SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda tidak membawa daun ganja siap edar.");
				return 1;
			}
			new earnings = count * 110;
			TakePlayerItem(playerid, ITEM_WEED, count);
			GivePlayerMoneyEx(playerid, earnings, "jual daun ganja pasar gelap");
			new msg[128];
			format(msg, sizeof(msg), "SUKSES: Menjual %dx Daun Ganja ke penadah seharga $%d.", count, earnings);
			SendClientMessage(playerid, COLOR_SUCCESS, msg);
		}
		case 3: {
			// Harta Karun Antik ($250 / pcs)
			new count = GetPlayerItemCount(playerid, ITEM_TREASURE);
			if (count <= 0) {
				SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda tidak membawa harta karun antik.");
				return 1;
			}
			new earnings = count * 250;
			TakePlayerItem(playerid, ITEM_TREASURE, count);
			GivePlayerMoneyEx(playerid, earnings, "jual harta karun antik");
			new msg[128];
			format(msg, sizeof(msg), "SUKSES: Menjual %dx Harta Karun Antik seharga $%d.", count, earnings);
			SendClientMessage(playerid, COLOR_SUCCESS, msg);
		}
		case 4: {
			// Logam Rongsokan ($35 / pcs)
			new count = GetPlayerItemCount(playerid, ITEM_RECYCLED);
			if (count <= 0) {
				SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda tidak membawa barang rongsokan.");
				return 1;
			}
			new earnings = count * 35;
			TakePlayerItem(playerid, ITEM_RECYCLED, count);
			GivePlayerMoneyEx(playerid, earnings, "jual rongsokan pasar gelap");
			new msg[128];
			format(msg, sizeof(msg), "SUKSES: Menjual %dx Logam Rongsokan seharga $%d.", count, earnings);
			SendClientMessage(playerid, COLOR_SUCCESS, msg);
		}
	}
	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
	return 1;
}

// ---------------------------------------------------------------------------
// 5. Polisi Menyita Barang Bukti Kejahatan (/confiscate)
// ---------------------------------------------------------------------------
stock ProcessConfiscateIllicitItems(playerid, targetid)
{
	if (!IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;
	if (gPlayerData[playerid][pFaction] != FACTION_POLICE || !gPlayerData[playerid][pDuty]) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Perintah ini khusus untuk petugas Kepolisian SAPD yang bertugas!");
		return 0;
	}

	if (!IsPlayerConnected(targetid) || !gPlayerData[targetid][pLoggedIn]) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Pemain sasaran tidak terhubung.");
		return 0;
	}

	new Float:tx, Float:ty, Float:tz;
	GetPlayerPos(targetid, tx, ty, tz);
	if (!IsPlayerNearPoint(playerid, tx, ty, tz, 3.5)) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus berada di dekat pemain (maks. 3.5 meter).");
		return 0;
	}

	new confiscatedCount = 0;

	// Sita kontraband, suku cadang curian, dan lockpick
	new c1 = GetPlayerItemCount(targetid, ITEM_CONTRABAND);
	if (c1 > 0) { TakePlayerItem(targetid, ITEM_CONTRABAND, c1); confiscatedCount += c1; }

	new c2 = GetPlayerItemCount(targetid, ITEM_CARPARTS);
	if (c2 > 0) { TakePlayerItem(targetid, ITEM_CARPARTS, c2); confiscatedCount += c2; }

	new c3 = GetPlayerItemCount(targetid, ITEM_LOCKPICK);
	if (c3 > 0) { TakePlayerItem(targetid, ITEM_LOCKPICK, c3); confiscatedCount += c3; }

	if (confiscatedCount == 0) {
		SendClientMessage(playerid, COLOR_INFO, "INFO: Target tidak membawa barang ilegal atau kontraband sitaan.");
		return 1;
	}

	PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
	PlayerPlaySound(targetid, 1057, 0.0, 0.0, 0.0);

	new copMsg[144], vicMsg[144];
	format(copMsg, sizeof(copMsg), "[SAPD] Berhasil menyita %d barang bukti kejahatan dari tersangka %s.",
		confiscatedCount, gPlayerData[targetid][pName]);
	format(vicMsg, sizeof(vicMsg), "[DISITA] Petugas %s telah menyita seluruh barang ilegal dari kantung Anda!",
		gPlayerData[playerid][pName]);

	SendClientMessage(playerid, COLOR_LIGHTBLUE, copMsg);
	SendClientMessage(targetid, COLOR_ERROR, vicMsg);
	return 1;
}
