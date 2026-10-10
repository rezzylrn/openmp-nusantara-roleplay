#if defined _INC_rp_header
	#endinput
#endif
#define _INC_rp_header

/*
==============================================================================
  NUSANTARA ROLEPLAY - header.inc
  Core definitions, enums, dialog IDs, colours, and forward declarations.
  Standard: Clean, modular, professional open.mp Pawn.
==============================================================================
*/

// ---------------------------------------------------------------------------
// WARNA STANDAR ROLEPLAY (Hex RGBA)
// ---------------------------------------------------------------------------
#define COLOR_WHITE            0xFFFFFFFF
#define COLOR_GREY             0xAFAFAFFF
#define COLOR_DARKGREY         0x666666FF
#define COLOR_RED              0xE74C3CFF
#define COLOR_GREEN            0x2ECC71FF
#define COLOR_BLUE             0x3498DBFF
#define COLOR_YELLOW           0xF1C40FFF
#define COLOR_ORANGE           0xE67E22FF
#define COLOR_PURPLE           0x9B59B6FF
#define COLOR_LIGHTBLUE        0x33CCFFFF
#define COLOR_CYAN             0x1ABC9CFF
#define COLOR_GOLD             0xF39C12FF

#define COLOR_SYNTAX           0xAFAFAFAA
#define COLOR_USAGE            0xFFFF00AA
#define COLOR_ERROR            0xFF6347AA
#define COLOR_WARNING          0xFFAA00FF
#define COLOR_INFO             0x33CCFFAA
#define COLOR_SUCCESS          0x4CD964AA

#define COLOR_RP_ME            0xC2A2DAAA
#define COLOR_RP_DO            0xC2A2DAAA
#define COLOR_OOC              0xC0C0C0AA
#define COLOR_LOCAL_OOC        0xBBBBBBFF
#define COLOR_RADIO            0x8D8DFFFF
#define COLOR_DEPARTMENT       0xFF8282FF
#define COLOR_MEGAPHONE        0xFFFF00FF
#define COLOR_ADMIN_CHAT       0xFF8800FF
#define COLOR_PAYDAY           0x2ECC71FF

// ---------------------------------------------------------------------------
// PESAN FORMAT RESMI
// ---------------------------------------------------------------------------
#define SendRP SendClientMessage
#define SendSyntax(%0,%1)      SendClientMessage(%0, COLOR_SYNTAX, "PENGGUNAAN: " %1)
#define SendErr(%0,%1)         SendClientMessage(%0, COLOR_ERROR, "ERROR: " %1)
#define SendSucc(%0,%1)        SendClientMessage(%0, COLOR_SUCCESS, "SUKSES: " %1)
#define SendInfo(%0,%1)        SendClientMessage(%0, COLOR_INFO, "INFO: " %1)

// ---------------------------------------------------------------------------
// FAKSI
// ---------------------------------------------------------------------------
#define FACTION_NONE           0
#define FACTION_POLICE         1 // SAPD (San Andreas Police Department)
#define FACTION_MEDIC          2 // SAMD (San Andreas Medical Department)
#define FACTION_GOV            3 // SAGOV (San Andreas Government)
#define FACTION_GANG           4 // Underground Family / Gang

// ---------------------------------------------------------------------------
// PEKERJAAN (SIDEJOBS)
// ---------------------------------------------------------------------------
#define JOB_NONE               0
#define JOB_COURIER            1 // Kurir Paket
#define JOB_SWEEPER            2 // Pembersih Jalan
#define JOB_TRUCKER            3 // Pengangkut Muatan (Hauler)
#define JOB_MECHANIC           4 // Mekanik Bengkel

#define COURIER_VEH_MODEL      422 // Bobcat
#define SWEEPER_VEH_MODEL      574 // Sweeper
#define TRUCKER_VEH_MODEL      403 // Linerunner
#define TRUCKER_TRAILER_MODEL  435 // Article Trailer

// ---------------------------------------------------------------------------
// ITEM INVENTARIS
// ---------------------------------------------------------------------------
#define MAX_INVENTORY_SLOTS    12
#define MAX_TRUNK_SLOTS        8

enum {
	ITEM_NONE = 0,
	ITEM_SNACK,         // Roti / Makanan ringan (+30 Lapar)
	ITEM_WATER,         // Air Mineral (+35 Haus)
	ITEM_BANDAGE,       // Perban (+20 HP)
	ITEM_MEDKIT,        // Kotak P3K (+75 HP)
	ITEM_REPAIRKIT,     // Alat Perbaikan Mesin Kendaraan
	ITEM_JERRYCAN,      // Jerigen Bensin (20 Liter)
	ITEM_CIGARETTE,     // Rokok (Animasi merokok)
	ITEM_LIGHTER,       // Korek Api
	ITEM_PHONE,         // Ponsel Pintar (Akses /call & /sms)
	ITEM_RADIO,         // Handy Talkie (Akses /r frekuensi)
	ITEM_GPS            // GPS Navigasi
};

enum E_ITEM_DATA {
	itType,
	itAmount
};

// ---------------------------------------------------------------------------
// ENUM PEMAIN (E_PLAYER_DATA)
// ---------------------------------------------------------------------------
enum E_PLAYER_DATA {
	pID,                         // ID database
	pName[MAX_PLAYER_NAME],      // Nama karakter
	pPassword[65],               // Hash SHA-256
	pSalt[65],                   // Garam hash
	bool:pLoggedIn,              // Status terautentikasi
	pMoney,                      // Uang tunai
	pBank,                       // Rekening bank
	pLevel,                      // Level karakter
	pExp,                        // Experience Points
	pAdmin,                      // Level administrator
	pFaction,                    // Faksi ID
	pFactionRank,                // Pangkat faksi
	bool:pDuty,                  // Status bertugas
	pJob,                        // ID pekerjaan
	Float:pHunger,               // Tingkat lapar (0-100)
	Float:pThirst,               // Tingkat haus (0-100)
	pPaycheckTimer,              // Timer paycheck per 3600s
	pPaycheckAmount,             // Akumulasi gaji
	pSkin,                       // Model skin
	Float:pLastX,                // Koordinat terakhir
	Float:pLastY,
	Float:pLastZ,
	Float:pLastA,
	pInterior,
	pVirtualWorld,
	bool:pCuffed,                // Diborgol polisi
	bool:pFrozen,                // Dibekukan admin
	pWarnings,                   // Total teguran
	pKills,
	pDeaths,
	pPlayHours,                  // Waktu bermain (jam)
	bool:pMuted,                 // Dibisukan
	bool:pSpawned,               // Telah spawn in-game
	// Komunikasi & Ponsel
	pPhone,                      // Nomor telepon (6 digit unik)
	bool:pPhoneStatus,           // Ponsel menyala / mode hening
	pCallTarget,                 // Target panggilan aktif (-1 jika idle)
	pCallState,                  // 0: idle, 1: memanggil/berdering, 2: terhubung
	pRadioFreq,                  // Frekuensi HT (1 - 999)
	bool:pRadioStatus,           // HT aktif
	// Kondisi Medis
	bool:pInjured,               // Keadaan sekarat / pingsan
	pInjuredTime                 // Waktu menuju respawn darurat
};

// ---------------------------------------------------------------------------
// ENUM KENDARAAN PRIBADI
// ---------------------------------------------------------------------------
#define MAX_OWNED_VEHICLES      500

enum E_VEHICLE_DATA {
	bool:vUsed,                  // Slot terisi
	vDBID,                       // ID database
	vOwnerID,                    // ID akun pemilik
	vModel,                      // Model ID kendaraan
	Float:vPosX,                 // Posisi parkir
	Float:vPosY,
	Float:vPosZ,
	Float:vPosA,
	vColor1,
	vColor2,
	Float:vFuel,                 // Bensin (0-100L)
	Float:vHealth,               // Kondisi mesin (300.0 - 1000.0)
	vPlate[16],                  // Plat nomor registrasi
	bool:vEngine,                // Mesin hidup
	bool:vLocked,                // Pintu terkunci
	bool:vLights,                // Lampu depan
	bool:vHood,                  // Kap mesin terbuka
	bool:vTrunkOpen,             // Bagasi terbuka
	vTrunk,                      // Uang di bagasi
	vVehicleHandle               // Handle entity game (-1 jika disimpan)
};

// ---------------------------------------------------------------------------
// ID DIALOG
// ---------------------------------------------------------------------------
#define DIALOG_REGISTER           1
#define DIALOG_REGISTER_CONFIRM   2
#define DIALOG_LOGIN              3
#define DIALOG_STATS              4
#define DIALOG_BANK_MENU          5
#define DIALOG_BANK_DEPOSIT       6
#define DIALOG_BANK_WITHDRAW      7
#define DIALOG_BANK_TRANSFER      8
#define DIALOG_BANK_TRANSFER_ACC  9
#define DIALOG_ATM_MENU           10
#define DIALOG_ATM_WITHDRAW       11
#define DIALOG_ATM_DEPOSIT        12
#define DIALOG_PAY                13
#define DIALOG_JOB_MENU           14
#define DIALOG_JOB_INFO           15
#define DIALOG_JOB_QUIT           16
#define DIALOG_DEALERSHIP         17
#define DIALOG_DEALERSHIP_CONFIRM 18
#define DIALOG_MYVEH              19
#define DIALOG_MYVEH_ACTION       20
#define DIALOG_TRUNK              21
#define DIALOG_TRUNK_DEPOSIT      22
#define DIALOG_TRUNK_WITHDRAW     23
#define DIALOG_REFUEL             24
#define DIALOG_DUTY               25
#define DIALOG_TARGET_SELECT      26
#define DIALOG_SAPD_ACTIONS       27
#define DIALOG_SAMD_ACTIONS       28
#define DIALOG_GOV_TAX            29
#define DIALOG_GANG_LOCKER        30
#define DIALOG_GANG_TURF          31
#define DIALOG_TICKET_AMOUNT      32
#define DIALOG_ARREST_TIME        33
#define DIALOG_ADMIN_ACTIONS      34
#define DIALOG_REPORT             35
#define DIALOG_ASK                36
#define DIALOG_HELP               37
#define DIALOG_SETSKIN            38
#define DIALOG_SETFACTION         39
// Dialog Baru: Inventaris, Toko, Ponsel, GPS
#define DIALOG_INVENTORY          40
#define DIALOG_ITEM_ACTION        41
#define DIALOG_ITEM_GIVE          42
#define DIALOG_ITEM_DROP          43
#define DIALOG_STORE_BUY          44
#define DIALOG_PHONE_MENU         45
#define DIALOG_PHONE_CALL         46
#define DIALOG_PHONE_SMS          47
#define DIALOG_RADIO_FREQ         48
#define DIALOG_GPS_MENU           49
#define DIALOG_VEHICLE_CONTROL    50

// ---------------------------------------------------------------------------
// KONFIGURASI DAN BATAS
// ---------------------------------------------------------------------------
#define MAX_DEALER_STOCK         8
#define MAX_COURIER_POINTS       8
#define MAX_SWEEPER_POINTS       8
#define MAX_TRUCKER_POINTS       6
#define MAX_STORE_ITEMS          10

// Koordinat Kota Utama (Los Santos)
#define SPAWN_X                  1481.0
#define SPAWN_Y                  -1748.0
#define SPAWN_Z                  13.5
#define SPAWN_A                  270.0

#define HOSPITAL_X               1172.8
#define HOSPITAL_Y               -1323.2
#define HOSPITAL_Z               14.0
#define HOSPITAL_A               270.0

#define BANK_X                   1461.0
#define BANK_Y                   -1011.0
#define BANK_Z                   26.8

#define ATM_X                    1497.0
#define ATM_Y                    -1739.0
#define ATM_Z                    13.6

#define COURIER_X                1647.0
#define COURIER_Y                -1886.0
#define COURIER_Z                13.5

#define SWEEPER_X                1634.0
#define SWEEPER_Y                -1886.0
#define SWEEPER_Z                13.5

#define TRUCKER_X                2750.0
#define TRUCKER_Y                -2454.0
#define TRUCKER_Z                13.6

#define MECHANIC_X               1973.0
#define MECHANIC_Y               -2178.0
#define MECHANIC_Z               13.5

#define DEALER_X                 1650.0
#define DEALER_Y                 -1886.0
#define DEALER_Z                 13.5

#define STORE_IDLEWOOD_X         1315.0
#define STORE_IDLEWOOD_Y         -898.0
#define STORE_IDLEWOOD_Z         39.5

#define POLICE_X                 1554.0
#define POLICE_Y                 -1675.0
#define POLICE_Z                 16.2

#define POLICE_JAIL_X            264.1
#define POLICE_JAIL_Y            77.5
#define POLICE_JAIL_Z            1001.0
#define POLICE_JAIL_INT          6

#define MEDIC_X                  1175.0
#define MEDIC_Y                  -1324.0
#define MEDIC_Z                  14.0

#define GOV_X                    1481.0
#define GOV_Y                    -1748.0
#define GOV_Z                    13.5

#define GANG_X                   2495.0
#define GANG_Y                   -1687.0
#define GANG_Z                   13.4

#define FUEL_STATION_X           1004.0
#define FUEL_STATION_Y           -937.0
#define FUEL_STATION_Z           30.4

// ---------------------------------------------------------------------------
// NILAI EKONOMI & SIMULASI
// ---------------------------------------------------------------------------
#define START_MONEY              500
#define START_BANK               1000
#define START_LEVEL              1
#define START_EXP                0
#define START_HUNGER             100.0
#define START_THIRST             100.0
#define START_SKIN               0

#define PAYCHECK_INTERVAL        3600
#define EXP_PER_LEVEL            100
#define EXP_JOB_DONE             25
#define EXP_PAYDAY               15
#define PAJAK_DEFAULT            5
#define TRANSFER_FEE             10

#define JOB_COURIER_PAY          150
#define JOB_COURIER_BONUS        500
#define JOB_SWEEPER_PAY          100
#define JOB_SWEEPER_BONUS        350
#define JOB_TRUCKER_PAY          350
#define JOB_TRUCKER_BONUS        800

#define HUNGER_DECAY             0.15     // Penurunan wajar per detik
#define THIRST_DECAY             0.20
#define HUNGER_DAMAGE            2.0
#define THIRST_DAMAGE            2.0

#define FUEL_CONSUMPTION_RATE    0.10     // Penurunan per detik saat mesin aktif
#define FUEL_PRICE               3        // $ per liter

// Proximity Jarak Chat
#define RANGE_PAY                5.0
#define RANGE_BANK               15.0
#define RANGE_ATM                8.0
#define RANGE_DEALER             10.0
#define RANGE_JOBCENTER          10.0
#define RANGE_STORE              8.0
#define RANGE_FUEL               12.0
#define RANGE_VEH_CMD            5.0
#define RANGE_ME                 15.0
#define RANGE_SHOUT              30.0
#define RANGE_LOW                3.0
#define RANGE_WHISPER            3.0
#define RANGE_OOC                20.0

// Gaji Faksi Pokok
#define SALARY_POLICE_BASE       450
#define SALARY_MEDIC_BASE        400
#define SALARY_GOV_BASE          500
#define SALARY_GANG_BASE         200
#define SALARY_RANK_BONUS        50
#define SALARY_LEVEL_BONUS       30

// ---------------------------------------------------------------------------
// NAMA DAN LABEL STRUKTUR
// ---------------------------------------------------------------------------
stock GetFactionName(factionid, dest[], len = sizeof(dest))
{
	switch (factionid) {
		case FACTION_POLICE: format(dest, len, "SAPD (Kepolisian)");
		case FACTION_MEDIC:  format(dest, len, "SAMD (Medis)");
		case FACTION_GOV:    format(dest, len, "SAGOV (Pemerintah)");
		case FACTION_GANG:   format(dest, len, "Families / Gang");
		default:             format(dest, len, "Warga Sipil");
	}
	return 1;
}

stock GetFactionRankName(factionid, rank, dest[], len = sizeof(dest))
{
	if (rank < 1) rank = 1;
	if (rank > 10) rank = 10;
	static const policeRanks[10][24] = {
		"Cadet", "Officer I", "Officer II", "Senior Officer", "Sergeant I",
		"Sergeant II", "Lieutenant", "Captain", "Commander", "Chief of Police"
	};
	static const medicRanks[10][24] = {
		"Intern", "Paramedic I", "Paramedic II", "Lead Paramedic", "Doctor",
		"Surgeon", "Chief of ER", "Deputy Director", "Medical Director", "Dean"
	};
	static const govRanks[10][24] = {
		"Staff Magang", "Staf Administrasi", "Staf Ahli", "Kepala Seksi", "Kepala Dinas",
		"Sekretaris Kota", "Asisten Walikota", "Wakil Walikota", "Walikota", "Gubernur"
	};
	static const gangRanks[10][24] = {
		"Outsider", "Associate", "Soldier", "Enforcer", "Caporegime",
		"Underboss", "Consigliere", "Boss", "Street Legend", "Don"
	};
	new idx = rank - 1;
	switch (factionid) {
		case FACTION_POLICE: format(dest, len, "%s", policeRanks[idx]);
		case FACTION_MEDIC:  format(dest, len, "%s", medicRanks[idx]);
		case FACTION_GOV:    format(dest, len, "%s", govRanks[idx]);
		case FACTION_GANG:   format(dest, len, "%s", gangRanks[idx]);
		default:             format(dest, len, "Warga");
	}
	return 1;
}

stock GetJobName(jobid, dest[], len = sizeof(dest))
{
	switch (jobid) {
		case JOB_COURIER:  format(dest, len, "Kurir Paket");
		case JOB_SWEEPER:  format(dest, len, "Pembersih Jalan");
		case JOB_TRUCKER:  format(dest, len, "Pengangkut Muatan");
		case JOB_MECHANIC: format(dest, len, "Mekanik Kendaraan");
		default:           format(dest, len, "Pengangguran");
	}
	return 1;
}

stock GetItemName(itemtype, dest[], len = sizeof(dest))
{
	switch (itemtype) {
		case ITEM_SNACK:     format(dest, len, "Snack Ringan");
		case ITEM_WATER:     format(dest, len, "Air Mineral");
		case ITEM_BANDAGE:   format(dest, len, "Perban");
		case ITEM_MEDKIT:    format(dest, len, "Kotak P3K");
		case ITEM_REPAIRKIT: format(dest, len, "Alat Perbaikan");
		case ITEM_JERRYCAN:  format(dest, len, "Jerigen Bensin");
		case ITEM_CIGARETTE: format(dest, len, "Rokok");
		case ITEM_LIGHTER:   format(dest, len, "Korek Api");
		case ITEM_PHONE:     format(dest, len, "Ponsel");
		case ITEM_RADIO:     format(dest, len, "Handy Talkie (HT)");
		case ITEM_GPS:       format(dest, len, "GPS Navigator");
		default:             format(dest, len, "Kosong");
	}
	return 1;
}

stock GetVehicleModelName(modelid, dest[], len = sizeof(dest))
{
	static const names[][] = {
		"Landstalker", "Bravura", "Buffalo", "Linerunner", "Perennial", "Sentinel",
		"Dumper", "Firetruck", "Trashmaster", "Stretch", "Manana", "Infernus",
		"Voodoo", "Pony", "Mule", "Cheetah", "Ambulance", "Leviathan", "Moonbeam",
		"Esperanto", "Taxi", "Washington", "Bobcat", "Whoopee", "BF Injection",
		"Hunter", "Premier", "Enforcer", "Securicar", "Banshee", "Predator",
		"Bus", "Rhino", "Barracks", "Hotknife", "Article Trailer", "Previon", "Coach",
		"Cabbie", "Stallion", "Rumpo", "RC Bandito", "Romero", "Packer", "Monster",
		"Admiral", "Squalo", "Seasparrow", "Pizzaboy", "Tram", "Article Trailer 2", "Turismo",
		"Speeder", "Reefer", "Tropic", "Flatbed", "Yankee", "Caddy", "Solair",
		"Berkley RC Van", "Skimmer", "PCJ-600", "Faggio", "Freeway", "RC Baron", "RC Raider",
		"Glendale", "Oceanic", "Sanchez", "Sparrow", "Patriot", "Quad", "Coastguard",
		"Dinghy", "Hermes", "Sabre", "Rustler", "ZR-350", "Walton", "Regina",
		"Comet", "BMX", "Burrito", "Camper", "Marquis", "Baggage", "Dozer",
		"Maverick", "News Chopper", "Rancher", "FBI Rancher", "Virgo", "Greenwood", "Jetmax",
		"Hotring", "Sandking", "Blista Compact", "Police Maverick", "Boxville", "Benson", "Mesa",
		"RC Goblin", "Hotring Racer A", "Hotring Racer B", "Bloodring Banger", "Rancher", "Super GT", "Elegant",
		"Journey", "Bike", "Mountain Bike", "Beagle", "Cropduster", "Stunt", "Tanker",
		"Roadtrain", "Nebula", "Majestic", "Buccaneer", "FGI", "Fortune", "Cadrona",
		"FBI Washington", "Willard", "Forklift", "Tractor", "Combine", "Feltzer", "Remington",
		"Slamvan", "Blade", "Freight", "Streak", "Vortex", "Vincent", "Bullet",
		"Clover", "Sadler", "Firetruck LA", "Hustler", "Intruder", "Primo", "Cargobob",
		"Tampa", "Sunrise", "Merit", "Utility", "Nevada", "Yosemite", "Windsor",
		"Monster A", "Monster B", "Uranus", "Jester", "Sultan", "Stratum", "Elegy",
		"Raindance", "RC Tiger", "Flash", "Tahoma", "Savanna", "Bandito", "Freight Flat",
		"Streak Carriage", "Kart", "Mower", "Dune", "Sweeper", "Broadway", "Tornado",
		"AT-400", "DFT-30", "Huntley", "Stafford", "BF-400", "Newsvan", "Tug",
		"Trailer 3", "Emperor", "Wayfarer", "Euros", "Hotdog", "Club", "Freight Box",
		"Trailer 4", "Andromada", "Dodo", "RC Cam", "Launch", "Police Car LSPD", "Police Car SFPD",
		"Police Car LVPD", "Police Ranger", "Picador", "S.W.A.T. Van", "Alpha", "Phoenix", "Glendale Damaged",
		"Sadler Damaged", "Luggage Trailer A", "Luggage Trailer B", "Stair Trailer", "Boxville", "Farm Plow", "Utility Trailer"
	};
	new idx = modelid - 400;
	if (idx < 0 || idx >= sizeof(names)) {
		format(dest, len, "Kendaraan");
		return 0;
	}
	format(dest, len, "%s", names[idx]);
	return 1;
}

// ---------------------------------------------------------------------------
// STOK DEALERSHIP (Model, Harga, Warna1, Warna2)
// ---------------------------------------------------------------------------
new const gDealerStock[MAX_DEALER_STOCK][4] = {
	{400, 32000, 1, 1},   // Landstalker (SUV)
	{401, 22000, 7, 7},   // Bravura (Compact Coupe)
	{404, 14000, 12, 12}, // Perennial (Station Wagon)
	{410, 11000, 6, 6},   // Manana (Sedan)
	{422, 18500, 40, 40}, // Bobcat (Pickup Truck)
	{426, 38000, 2, 2},   // Premier (Executive Sedan)
	{436, 17500, 36, 36}, // Previon (Sport Compact)
	{542, 21000, 66, 66}  // Clover (Classic Muscle)
};

// ---------------------------------------------------------------------------
// TITIK TUGAS PEKERJAAN
// ---------------------------------------------------------------------------
new const gCourierPoints[MAX_COURIER_POINTS][3] = {
	{2226, -1673, 15},   // Willowfield
	{1836, -1842, 13},   // Willowfield
	{1596, -2326, 13},   // El Corona
	{1199, -1277, 14},   // Idlewood
	{934, -1408, 13},    // Ganton
	{812, -1616, 13},    // East Los Santos
	{652, -1864, 5},     // Marina
	{494, -1760, 12}     // Rodeo
};

new const gSweeperPoints[MAX_SWEEPER_POINTS][3] = {
	{1489, -1744, 13},   // Pershing Square
	{1354, -1769, 13},   // Commerce
	{1209, -1769, 13},   // Commerce Street
	{1063, -1779, 13},   // Downtown
	{961, -1676, 13},    // Idlewood North
	{1069, -1557, 13},   // Glen Park South
	{1192, -1480, 13},   // Glen Park
	{1347, -1479, 13}    // Jefferson
};

new const gTruckerPoints[MAX_TRUCKER_POINTS][3] = {
	{-2135, -247, 36},   // San Fierro Docks
	{-1569, -2732, 48},  // Mount Chiliad Outpost
	{614, -600, 17},     // Red County Montgomery
	{2290, 83, 26},      // Palomino Creek
	{1425, 1459, 10},    // Las Venturas Freight Depot
	{2750, -2454, 13}    // Los Santos Ocean Docks Return
};

// ---------------------------------------------------------------------------
// VARIABLE GLOBAL
// ---------------------------------------------------------------------------
new DB:gDatabase;
new bool:gDatabaseReady;
new gPlayerData[MAX_PLAYERS][E_PLAYER_DATA];
new gPlayerItems[MAX_PLAYERS][MAX_INVENTORY_SLOTS][E_ITEM_DATA];
new gVehicleData[MAX_OWNED_VEHICLES][E_VEHICLE_DATA];
new gVehicleTrunkItems[MAX_OWNED_VEHICLES][MAX_TRUNK_SLOTS][E_ITEM_DATA];
new gVehicleCount;
new gCurrentTax = PAJAK_DEFAULT;

// Job State
new gPlayerJobVehicle[MAX_PLAYERS] = {-1, ...};
new gPlayerJobTrailer[MAX_PLAYERS] = {-1, ...};
new gJobCheckpoint[MAX_PLAYERS];
new gJobCash[MAX_PLAYERS];
new bool:gJobActive[MAX_PLAYERS];
new gJobType[MAX_PLAYERS];

// State Kendaraan Pribadi
new gPlayerVehicleSlot[MAX_PLAYERS];
new gSelectedInvSlot[MAX_PLAYERS];

#if defined __Pawn
	#pragma unused gVehicleTrunkItems
#endif

// Textdraw Status Flags
new bool:gTD_HUD[MAX_PLAYERS];
new bool:gTD_Speedo[MAX_PLAYERS];
new bool:gTD_Alert[MAX_PLAYERS];
new bool:gTD_Subtitle[MAX_PLAYERS];
new gAlertTimer[MAX_PLAYERS];
new gSubtitleTimer[MAX_PLAYERS];

// Pickups & 3D Labels
new gBankPickup = -1;
new gATMPickup = -1;
new gJobPickup = -1;
new gDealerPickup = -1;
new gFuelPickup = -1;
new gStorePickup = -1;
new gPolicePickup = -1;
new gMedicPickup = -1;
new gGovPickup = -1;
new gGangPickup = -1;

#if defined __Pawn
	#pragma unused gBankPickup, gATMPickup, gJobPickup, gDealerPickup, gFuelPickup
	#pragma unused gStorePickup, gPolicePickup, gMedicPickup, gGovPickup, gGangPickup
#endif

// Timer Handlers
new gTimerTick = 0;
new gTimerSpeedo = 0;
new gTimerSave = 0;

// Callbacks
forward RP_Tick();
forward RP_SpeedoTick();
forward RP_SaveTick();
forward RP_DelayLogin(playerid);
forward RP_HideAlert(playerid);
forward RP_HideSubtitle(playerid);

// ---------------------------------------------------------------------------
// BROADCAST CHAT PROXIMITY
// ---------------------------------------------------------------------------
stock SendNearbyPlayer(playerid, color, const msg[], Float:range)
{
	new Float:px, Float:py, Float:pz;
	GetPlayerPos(playerid, px, py, pz);
	new vw = GetPlayerVirtualWorld(playerid);
	new interior = GetPlayerInterior(playerid);

	for (new i = 0; i < MAX_PLAYERS; i++) {
		if (!IsPlayerConnected(i) || !gPlayerData[i][pLoggedIn]) continue;
		if (GetPlayerVirtualWorld(i) != vw || GetPlayerInterior(i) != interior) continue;
		if (IsPlayerInRangeOfPoint(i, range, px, py, pz)) {
			SendClientMessage(i, color, msg);
		}
	}
	return 1;
}

stock FindPlayerByName(const name[])
{
	new targetname[MAX_PLAYER_NAME];
	for (new i = 0; i < MAX_PLAYERS; i++) {
		if (!IsPlayerConnected(i) || !gPlayerData[i][pLoggedIn]) continue;
		GetPlayerName(i, targetname, sizeof(targetname));
		if (strcmp(targetname, name, true) == 0) return i;
	}
	return -1;
}
