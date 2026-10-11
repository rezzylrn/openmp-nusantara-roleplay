#if defined _INC_metal_detector
	#endinput
#endif
#define _INC_metal_detector

/*
==============================================================================
 metal_detector.inc
 NUSANTARA ROLEPLAY - Beach Metal Detecting & Buried Treasure Digging System
==============================================================================
*/

#define MAX_BURIED_HOTSPOTS    6

stock const Float:gBuriedSpots[MAX_BURIED_HOTSPOTS][3] = {
	{360.5,  -2060.0, 7.8},  // Santa Maria Beach Pier East
	{280.0,  -1880.0, 3.2},  // Santa Maria Beach West
	{840.5,  -2050.0, 12.5}, // Verona Beach Sands
	{140.0,  -1790.0, 4.0},  // Lighthouse Beach Cove
	{2450.0, -2600.0, 2.5},  // Ocean Docks Sandbank
	{-350.0, -1450.0, 8.5}   // Flint Waterways Shore
};

new gPlayerDetectorActive[MAX_PLAYERS];
new gPlayerDigCooldown[MAX_PLAYERS];

stock MetalDetector_Init()
{
	for (new i = 0; i < MAX_PLAYERS; i++) {
		gPlayerDetectorActive[i] = false;
		gPlayerDigCooldown[i] = 0;
	}
	print("[DETECTOR] Sistem detektor logam & pencarian harta karun pantai aktif.");
	return 1;
}

stock ProcessMetalDetect(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;

	if (IsPlayerConnected(playerid) && IsPlayerInAnyVehicle(playerid)) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus berjalan kaki untuk menggunakan detektor logam!");
		return 0;
	}

	if (GetPlayerItemCount(playerid, ITEM_DETECTOR) <= 0) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda memerlukan Detektor Logam di inventaris (/inv). Beli di toko 24/7.");
		return 0;
	}

	new Float:px, Float:py, Float:pz;
	if (IsPlayerConnected(playerid)) {
		GetPlayerPos(playerid, px, py, pz);
	} else {
		px = gPlayerData[playerid][pLastX];
		py = gPlayerData[playerid][pLastY];
		pz = gPlayerData[playerid][pLastZ];
	}

	// Cari titik harta karun terdekat
	new Float:minDist = 9999.0;

	for (new i = 0; i < MAX_BURIED_HOTSPOTS; i++) {
		new Float:dist = VectorSize(px - gBuriedSpots[i][0], py - gBuriedSpots[i][1], pz - gBuriedSpots[i][2]);
		if (dist < minDist) {
			minDist = dist;
		}
	}

	if (minDist > 150.0) {
		SendClientMessage(playerid, COLOR_INFO, "INFO: Detektor hening (Tidak ada sinyal logam terdeteksi di area ini. Coba telusuri garis pantai!).");
		return 1;
	}

	// Deteksi sinyal jarak
	if (minDist <= 3.0) {
		PlayerPlaySound(playerid, 1058, 0.0, 0.0, 0.0);
		SendClientMessage(playerid, COLOR_SUCCESS,
			"[DETEKTOR BIP-BIP-BIP!] Sinyal logam kuat tepat di bawah kaki Anda! Gunakan /dig atau /gali untuk membongkar!");
		GameTextForPlayer(playerid, "~g~SINYAL LOGAM KUAT!~n~~w~GUNAKAN /DIG", 3000, 3);
	} else if (minDist <= 25.0) {
		PlayerPlaySound(playerid, 1056, 0.0, 0.0, 0.0);
		new msg[128];
		format(msg, sizeof(msg), "[DETEKTOR] Sinyal logam terdeteksi cukup dekat (~%.0f meter). Teruslah mencari di sekitar sini!", minDist);
		SendClientMessage(playerid, COLOR_YELLOW, msg);
	} else {
		PlayerPlaySound(playerid, 1056, 0.0, 0.0, 0.0);
		new msg[128];
		format(msg, sizeof(msg), "[DETEKTOR] Sinyal logam sayup-sayup terdeteksi di kejauhan (~%.0f meter).", minDist);
		SendClientMessage(playerid, COLOR_GREY, msg);
	}
	return 1;
}

stock ProcessDigTreasure(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;

	if (IsPlayerConnected(playerid) && IsPlayerInAnyVehicle(playerid)) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus berjalan kaki untuk menggali tanah!");
		return 0;
	}

	if (GetPlayerItemCount(playerid, ITEM_SHOVEL) <= 0) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda memerlukan Sekop Penggali Tanah di inventaris (/inv). Beli di toko 24/7.");
		return 0;
	}

	new now = gettime();
	if (now < gPlayerDigCooldown[playerid]) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda masih lelah sehabis menggali. Tunggu beberapa detik.");
		return 0;
	}

	new Float:px, Float:py, Float:pz;
	if (IsPlayerConnected(playerid)) {
		GetPlayerPos(playerid, px, py, pz);
	} else {
		px = gPlayerData[playerid][pLastX];
		py = gPlayerData[playerid][pLastY];
		pz = gPlayerData[playerid][pLastZ];
	}

	new bool:onSpot = false;
	for (new i = 0; i < MAX_BURIED_HOTSPOTS; i++) {
		if (VectorSize(px - gBuriedSpots[i][0], py - gBuriedSpots[i][1], pz - gBuriedSpots[i][2]) <= 4.0) {
			onSpot = true;
			break;
		}
	}

	if (!onSpot) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Tidak ada temuan logam berharga di titik ini. Gunakan /detect terlebih dahulu!");
		return 0;
	}

	gPlayerDigCooldown[playerid] = now + 15;

	// Animasi menggali
	ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.1, false, false, false, false, 0);
	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);

	new roll = random(100);

	if (roll < 45) {
		// Harta Karun Antik
		GivePlayerItem(playerid, ITEM_TREASURE, 1);
		SendClientMessage(playerid, COLOR_SUCCESS,
			"[PENGGALIAN SUKSES] Anda menemukan 1x Harta Karun Kuno (Koin perak & cincin antik)! Jual ke Black Market seharga $250.");
	} else if (roll < 70) {
		// Uang tunai receh tercecer di pasir
		new foundCash = 40 + random(81);
		GivePlayerMoneyEx(playerid, foundCash, "uang tercecer hasil detektor logam");
		new msg[128];
		format(msg, sizeof(msg), "[PENGGALIAN SUKSES] Menemukan dompet lapuk berisi $%d uang tunai tercecer di pasir!", foundCash);
		SendClientMessage(playerid, COLOR_SUCCESS, msg);
	} else if (roll < 85) {
		// Peluru lama (Deagle 20 Ammo)
		GivePlayerWeapon(playerid, WEAPON_DEAGLE, 20);
		SendClientMessage(playerid, COLOR_SUCCESS,
			"[PENGGALIAN SUKSES] Menemukan kotak amunisi militer tua berisi 20 Peluru Desert Eagle!");
	} else {
		// Rongsokan kaleng
		GivePlayerItem(playerid, ITEM_RECYCLED, 3);
		SendClientMessage(playerid, COLOR_INFO,
			"[PENGGALIAN] Menggali tumpukan 3x Kaleng & Logam Rongsokan bekas wisatawan.");
	}
	return 1;
}
