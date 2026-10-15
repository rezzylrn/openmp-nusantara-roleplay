#if defined _INC_rp_header
	#endinput
#endif
#define _INC_rp_header

// colors
#define COLOR_WHITE            0xFFFFFFFF
#define COLOR_GREY             0xAFAFAFFF
#define COLOR_DARKGREY         0x666666FF
#define COLOR_RED              0xE74C3CFF
#define COLOR_GREEN            0x2ECC71FF
#define COLOR_BLUE             0x3498DBFF
#define COLOR_YELLOW           0xF1C40FFF
#define COLOR_ORANGE           0xE67E22FF
#define COLOR_LIGHTBLUE        0x33CCFFFF
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
#define COLOR_RADIO            0x8D8DFFFF
#define COLOR_DEPARTMENT       0xFF8282FF
#define COLOR_MEGAPHONE        0xFFFF00FF
#define COLOR_ADMIN_CHAT       0xFF8800FF
#define COLOR_PAYDAY           0x2ECC71FF
#define COLOR_ME               0xC2A2DAAA
#define COLOR_DO               0xC2A2DAAA

#define SALARY_POLICE_BASE     450
#define SALARY_MEDIC_BASE      400
#define SALARY_GOV_BASE        500
#define SALARY_GANG_BASE       200
#define SALARY_RANK_BONUS      50

// message shortcuts
#define SendRP SendClientMessage
#define SendSyntax(%0,%1)      SendClientMessage(%0, COLOR_SYNTAX, "PENGGUNAAN: " %1)
#define SendErr(%0,%1)         SendClientMessage(%0, COLOR_ERROR, "ERROR: " %1)
#define SendSucc(%0,%1)        SendClientMessage(%0, COLOR_SUCCESS, "SUKSES: " %1)
#define SendInfo(%0,%1)        SendClientMessage(%0, COLOR_INFO, "INFO: " %1)

// factions
enum {
	FACTION_NONE = 0,
	FACTION_POLICE,        // SAPD
	FACTION_MEDIC,         // SAMD
	FACTION_GOV,           // SAGOV
	FACTION_GANG,          // Underground
	MAX_FACTIONS
};

// jobs
enum {
	JOB_NONE = 0,
	JOB_COURIER,           // Kurir Paket
	JOB_SWEEPER,           // Pembersih Jalan
	JOB_TRUCKER,           // Pengangkut Muatan
	JOB_MECHANIC,          // Mekanik Bengkel
	JOB_SCAVENGER,         // Pemulung Sampah
	JOB_LUMBERJACK,        // Penebang Pohon
	JOB_FISHERMAN,         // Nelayan Dermaga
	JOB_BUSDRIVER,         // Supir Bus Trans-LS
	JOB_TRASHMAN,          // Dinas Kebersihan Kota
	JOB_MINER,             // Penambang Batu Quarry
	MAX_JOBS
};

#define COURIER_VEH_MODEL      422 // Bobcat
#define SWEEPER_VEH_MODEL      574 // Sweeper
#define TRUCKER_VEH_MODEL      403 // Linerunner
#define TRUCKER_TRAILER_MODEL  435 // Article Trailer
#define BUS_VEH_MODEL          431 // Bus

// inventory items
#define MAX_INVENTORY_SLOTS    25
#define MAX_TRUNK_SLOTS        8

enum {
	ITEM_NONE = 0,
	ITEM_SNACK,         // Roti (+30 Lapar)
	ITEM_WATER,         // Air Mineral (+35 Haus)
	ITEM_BANDAGE,       // Perban (+20 HP)
	ITEM_MEDKIT,        // Kotak P3K (+75 HP)
	ITEM_REPAIRKIT,     // Toolkit Kendaraan
	ITEM_JERRYCAN,      // Jerigen Bensin (20L)
	ITEM_CIGARETTE,     // Rokok
	ITEM_LIGHTER,       // Korek Api
	ITEM_PHONE,         // Ponsel
	ITEM_RADIO,         // Handy Talkie
	ITEM_GPS,           // GPS
	ITEM_ROD,           // Joran Pancing
	ITEM_BAIT,          // Umpan Ikan
	ITEM_WOOD,          // Balok Kayu
	ITEM_RECYCLED,      // Barang Rongsokan
	ITEM_LOCKPICK,      // Obeng Pembobol Kunci (Lockpick)
	ITEM_CROWBAR,       // Linggis Pembongkar (Crowbar)
	ITEM_CONTRABAND,    // Paket Kontraband Gelap
	ITEM_CARPARTS,      // Suku Cadang Mobil Curian
	ITEM_DETECTOR,      // Alat Detektor Logam Pantai
	ITEM_SHOVEL,        // Sekop Penggali Tanah
	ITEM_TREASURE,      // Koin & Perhiasan Kuno Terpendam
	ITEM_WEEDSEED,      // Benih Tanaman Ganja
	ITEM_WEED,          // Paket Daun Ganja Kering
	ITEM_SPRAYCAN,      // Kaleng Cat Semprot Grafiti
	ITEM_COOKEDFISH,    // Ikan Bakar Siap Makan
	ITEM_DICE,          // Dadu Taruhan Jalanan
	ITEM_AMMOBOX,       // Kotak Amunisi Resmi (50 Peluru)
	ITEM_BEER,          // Kaleng Bir Dingin
	ITEM_VODKA,         // Botol Minuman Keras Vodka
	ITEM_BURGER,        // Paket Burger Shot
	ITEM_PIZZA,         // Kotak Pizza Stack
	ITEM_WHEATSEED,     // Bibit Gandum Pertanian
	ITEM_CORNSEED,      // Bibit Jagung Pertanian
	ITEM_WHEAT,         // Karung Gandum Hasil Panen
	ITEM_CORN,          // Karung Jagung Hasil Panen
	ITEM_WATERCAN,      // Gembor Penyiram Air
	ITEM_DRILL,         // Bor Listrik Pembobol ATM
	ITEM_BOOMBOX,       // Portable Audio Boombox
	ITEM_PICKAXE,       // Beliung Tambang Batu (Mining)
	ITEM_IRON_ORE,      // Bongkahan Bijih Besi
	ITEM_COAL,          // Bongkahan Batu Bara
	ITEM_IRON_BAR,      // Batangan Besi Hasil Peleburan
	ITEM_SCUBA,         // Tabung Oksigen Penyelaman Laut
	ITEM_PEARL          // Butir Mutiara Laut Dalam
};

enum E_ITEM_DATA {
	itType,
	itAmount
};

// player enum
enum E_PLAYER_DATA {
	pID,
	pName[MAX_PLAYER_NAME],
	pPassword[65],
	pSalt[65],
	bool:pLoggedIn,
	pMoney,
	pBank,
	pLevel,
	pExp,
	pAdmin,
	pFaction,
	pFactionRank,
	bool:pDuty,
	pJob,
	Float:pHunger,
	Float:pThirst,
	pPaycheckTimer,
	pPaycheckAmount,
	pSkin,
	Float:pLastX,
	Float:pLastY,
	Float:pLastZ,
	Float:pLastA,
	pInterior,
	pVirtualWorld,
	bool:pCuffed,
	bool:pFrozen,
	pWarnings,
	pKills,
	pDeaths,
	pPlayHours,
	bool:pMuted,
	bool:pSpawned,
	// comms
	pPhone,
	bool:pPhoneStatus,
	pCallTarget,
	pCallState,
	pRadioFreq,
	bool:pRadioStatus,
	// health condition
	bool:pInjured,
	pInjuredTime,
	// ucp & character selection
	pUCPID,
	pUCPName[24],
	pBirthDate[16],
	pGender,
	// language & localization
	pLanguage,
	// traffic citations
	pTickets,
	pTicketAmount,
	bool:pDriveLic,
	bool:pGunLic,
	pFightStyle,
	pDrunkLevel,
	// job variables
	pJobCooldown,
	pTrashCount,
	pFishWeight
};

// vehicle enum
#define MAX_OWNED_VEHICLES      500

enum E_VEHICLE_DATA {
	bool:vUsed,
	vDBID,
	vOwnerID,
	vModel,
	Float:vPosX,
	Float:vPosY,
	Float:vPosZ,
	Float:vPosA,
	vColor1,
	vColor2,
	Float:vFuel,
	Float:vHealth,
	vPlate[16],
	bool:vEngine,
	bool:vLocked,
	bool:vLights,
	bool:vHood,
	bool:vTrunkOpen,
	vTrunk,
	vTrunkItemType[MAX_TRUNK_SLOTS],
	vTrunkItemAmount[MAX_TRUNK_SLOTS],
	WEAPON:vTrunkGun,
	vTrunkAmmo,
	bool:vImpounded,
	vVehicleHandle
};

// dialog IDs
enum {
	DIALOG_NONE = 0,
	DIALOG_UCP_LOGIN,
	DIALOG_UCP_REG_CODE,
	DIALOG_UCP_REG_PASS,
	DIALOG_UCP_OTP_VERIFY,
	DIALOG_CHAR_SELECT,
	DIALOG_CHAR_CREATE_NAME,
	DIALOG_CHAR_CREATE_DOB,
	DIALOG_CHAR_CREATE_GENDER,
	DIALOG_REGISTER,
	DIALOG_REGISTER_CONFIRM,
	DIALOG_LOGIN,
	DIALOG_STATS,
	DIALOG_BANK_MENU,
	DIALOG_BANK_DEPOSIT,
	DIALOG_BANK_WITHDRAW,
	DIALOG_BANK_TRANSFER,
	DIALOG_ATM_MENU,
	DIALOG_ATM_WITHDRAW,
	DIALOG_ATM_DEPOSIT,
	DIALOG_PAY,
	DIALOG_JOB_MENU,
	DIALOG_DEALERSHIP,
	DIALOG_MYVEH,
	DIALOG_MYVEH_ACTION,
	DIALOG_TRUNK,
	DIALOG_TRUNK_DEPOSIT,
	DIALOG_TRUNK_DEPOSIT_CASH,
	DIALOG_TRUNK_WITHDRAW,
	DIALOG_REFUEL,
	DIALOG_SAPD_ACTIONS,
	DIALOG_HELP,
	DIALOG_WORKSHOP_MENU,
	DIALOG_INVENTORY,
	DIALOG_ITEM_ACTION,
	DIALOG_STORE_BUY,
	DIALOG_PHONE_MENU,
	DIALOG_PHONE_CALL,
	DIALOG_PHONE_SMS,
	DIALOG_RADIO_FREQ,
	DIALOG_GPS_MENU,
	DIALOG_GIVE_PLAYER,
	DIALOG_GIVE_AMOUNT,
	DIALOG_TRUNK_DEPOSIT_ITEM,
	DIALOG_TRUNK_WITHDRAW_ITEM,
	DIALOG_911_SERVICE,
	DIALOG_911_MESSAGE,
	DIALOG_TRUNK_TAKE_CASH,
	DIALOG_LANGUAGE_SELECT,
	DIALOG_ARMORY_MENU,
	DIALOG_RENTAL_MENU,
	DIALOG_TRUNK_WEAPON,
	DIALOG_PAY_TICKETS,
	DIALOG_BLACKMARKET_BUY,
	DIALOG_BLACKMARKET_SELL,
	DIALOG_CHOPSHOP_CONFIRM,
	DIALOG_SMUGGLE_MENU,
	DIALOG_ROB_CONFIRM,
	DIALOG_DMV_PLATE,
	DIALOG_VENDING_SNACK,
	DIALOG_DMV_THEORY,
	DIALOG_DICE_BET,
	DIALOG_CAMPFIRE_MENU,
	DIALOG_AMMUNATION_BUY,
	DIALOG_SHARE_KEY,
	DIALOG_BAR_MENU,
	DIALOG_GYM_STYLES,
	DIALOG_FASTFOOD_MENU,
	DIALOG_ACCESSORIES,
	DIALOG_ACTION_HUB,
	DIALOG_ACTION_NEAR_PLAYER,
	DIALOG_ACTION_JOBS_MENU,
	DIALOG_ACTION_AD_INPUT,
	DIALOG_ACTION_PLATE_INPUT,
	DIALOG_ACTION_TAG_INPUT,
	MAX_DIALOGS
};

enum {
	LANG_ID = 0,
	LANG_EN
};

// Dynamic Systems Forward Declarations
forward ShowPlayerActionHub(playerid);
forward SpawnPlayerEx(playerid);
forward RP_AutoSpawnPlayer(playerid);
forward AC_OnPlayerSpawn(playerid);
forward IsPlayerNearAnyATM(playerid, Float:range = 2.5);
forward IsPlayerNearAnyTrash(playerid, Float:range = 2.5);
forward ProcessDoorEnter(playerid);
forward ProcessDoorExit(playerid);
forward ProcessPlayerScavenge(playerid);

#define MAX_DISPATCHES            20

enum E_DISPATCH_DATA {
	bool:dActive,
	dCallerID,
	dCallerName[MAX_PLAYER_NAME],
	dCallerPhone,
	dService, // 1 = POLICE, 2 = MEDIC
	dMessage[128],
	Float:dX,
	Float:dY,
	Float:dZ,
	dTime,
	dResponderID
};

// server limits & config
#define MAX_DEALER_STOCK         8
#define MAX_SWEEPER_POINTS       8
#define MAX_TRUCKER_POINTS       6
#define MAX_BUS_POINTS           8
#define MAX_TRASH_POINTS         6

// coordinates
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

#define JOB_CENTER_X             1481.0
#define JOB_CENTER_Y             -1770.0
#define JOB_CENTER_Z             15.3

#define COURIER_X                1936.0
#define COURIER_Y                -2115.0
#define COURIER_Z                13.5

#define SWEEPER_X                1634.0
#define SWEEPER_Y                -1886.0
#define SWEEPER_Z                13.5

#define TRUCKER_X                2750.0
#define TRUCKER_Y                -2454.0
#define TRUCKER_Z                13.6

#define BUS_DEPOT_X              1782.0
#define BUS_DEPOT_Y              -1903.0
#define BUS_DEPOT_Z              13.5

#define LUMBER_X                 -542.0
#define LUMBER_Y                 -70.0
#define LUMBER_Z                 63.0

#define SAWMILL_X                -1977.0
#define SAWMILL_Y                -2436.0
#define SAWMILL_Z                30.6

#define FISH_PIER_X              383.0
#define FISH_PIER_Y              -2079.0
#define FISH_PIER_Z              7.8

#define TRASH_BUYER_X            2178.0
#define TRASH_BUYER_Y            -1983.0
#define TRASH_BUYER_Z            13.5

#define DEALER_X                 2131.5
#define DEALER_Y                 -1150.0
#define DEALER_Z                 24.0

#define STORE_IDLEWOOD_X         1315.0
#define STORE_IDLEWOOD_Y         -1748.0
#define STORE_IDLEWOOD_Z         13.5
#define WORKSHOP_X              1995.0
#define WORKSHOP_Y              -2060.0
#define WORKSHOP_Z              13.5

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

#define FUEL_STATION_X           1004.0
#define FUEL_STATION_Y           -937.0
#define FUEL_STATION_Z           30.4

// economy parameters
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

#define HUNGER_DECAY             0.015
#define THIRST_DECAY             0.020
#define FUEL_CONSUMPTION_RATE    0.10
#define FUEL_PRICE               3

// distance ranges
#define RANGE_PAY                5.0
#define RANGE_BANK               15.0
#define RANGE_ATM                8.0
#define RANGE_DEALER             10.0
#define RANGE_JOBCENTER          10.0
#define RANGE_STORE              8.0
#define RANGE_FUEL               12.0
#define RANGE_ME                 15.0
#define RANGE_SHOUT              30.0
#define RANGE_LOW                3.0
#define RANGE_WHISPER            3.0
#define RANGE_OOC                20.0

// names and rank tables
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
		case JOB_COURIER:    format(dest, len, "Kurir Paket");
		case JOB_SWEEPER:    format(dest, len, "Pembersih Jalan");
		case JOB_TRUCKER:    format(dest, len, "Pengangkut Muatan");
		case JOB_MECHANIC:   format(dest, len, "Mekanik Bengkel");
		case JOB_SCAVENGER:  format(dest, len, "Pemulung Sampah");
		case JOB_LUMBERJACK: format(dest, len, "Penebang Pohon");
		case JOB_FISHERMAN:  format(dest, len, "Nelayan Dermaga");
		case JOB_BUSDRIVER:  format(dest, len, "Supir Bus Kota");
		case JOB_TRASHMAN:   format(dest, len, "Dinas Kebersihan");
		case JOB_MINER:      format(dest, len, "Penambang Batu");
		default:             format(dest, len, "Pengangguran");
	}
	return 1;
}

stock GetItemName(itemtype, dest[], len = sizeof(dest))
{
	switch (itemtype) {
		case ITEM_SNACK:     format(dest, len, "Snack Roti");
		case ITEM_WATER:     format(dest, len, "Air Mineral");
		case ITEM_BANDAGE:   format(dest, len, "Perban Medis");
		case ITEM_MEDKIT:    format(dest, len, "Kotak P3K Medkit");
		case ITEM_REPAIRKIT: format(dest, len, "Alat Perbaikan Mobil");
		case ITEM_JERRYCAN:  format(dest, len, "Jerigen Bensin (20L)");
		case ITEM_CIGARETTE: format(dest, len, "Sebungkus Rokok");
		case ITEM_LIGHTER:   format(dest, len, "Korek Api");
		case ITEM_PHONE:     format(dest, len, "Ponsel Pintar");
		case ITEM_RADIO:     format(dest, len, "Handy Talkie (HT)");
		case ITEM_GPS:       format(dest, len, "GPS Navigator");
		case ITEM_ROD:       format(dest, len, "Joran Pancing");
		case ITEM_BAIT:      format(dest, len, "Umpan Ikan");
		case ITEM_WOOD:      format(dest, len, "Balok Kayu");
		case ITEM_RECYCLED:  format(dest, len, "Barang Rongsokan");
		case ITEM_LOCKPICK:  format(dest, len, "Obeng Lockpick");
		case ITEM_CROWBAR:   format(dest, len, "Linggis Baja (Crowbar)");
		case ITEM_CONTRABAND:format(dest, len, "Paket Kontraband");
		case ITEM_CARPARTS:  format(dest, len, "Suku Cadang Mobil Curian");
		case ITEM_DETECTOR:  format(dest, len, "Detektor Logam");
		case ITEM_SHOVEL:    format(dest, len, "Sekop Penggali");
		case ITEM_TREASURE:  format(dest, len, "Harta Karun Kuno");
		case ITEM_WEEDSEED:  format(dest, len, "Benih Ganja");
		case ITEM_WEED:      format(dest, len, "Paket Daun Ganja");
		case ITEM_SPRAYCAN:  format(dest, len, "Kaleng Cat Semprot");
		case ITEM_COOKEDFISH:format(dest, len, "Ikan Bakar Matang");
		case ITEM_DICE:      format(dest, len, "Dadu Kasino");
		case ITEM_AMMOBOX:   format(dest, len, "Kotak Amunisi Resmi");
		case ITEM_BEER:      format(dest, len, "Bir Dingin");
		case ITEM_VODKA:     format(dest, len, "Botol Vodka");
		case ITEM_BURGER:    format(dest, len, "Paket Burger Shot");
		case ITEM_PIZZA:     format(dest, len, "Kotak Pizza Stack");
		case ITEM_WHEATSEED: format(dest, len, "Bibit Gandum");
		case ITEM_CORNSEED:  format(dest, len, "Bibit Jagung");
		case ITEM_WHEAT:     format(dest, len, "Karung Gandum");
		case ITEM_CORN:      format(dest, len, "Karung Jagung");
		case ITEM_WATERCAN:  format(dest, len, "Gembor Penyiram Air");
		case ITEM_DRILL:     format(dest, len, "Bor Listrik (Drill)");
		case ITEM_BOOMBOX:   format(dest, len, "Speaker Boombox");
		case ITEM_PICKAXE:   format(dest, len, "Beliung Tambang (Pickaxe)");
		case ITEM_IRON_ORE:  format(dest, len, "Bongkahan Bijih Besi");
		case ITEM_COAL:      format(dest, len, "Bongkahan Batu Bara");
		case ITEM_IRON_BAR:  format(dest, len, "Batangan Besi Murni");
		case ITEM_SCUBA:     format(dest, len, "Tabung Selam Scuba");
		case ITEM_PEARL:     format(dest, len, "Mutiara Laut Alami");
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

// dealership catalog
new const gDealerStock[MAX_DEALER_STOCK][4] = {
	{400, 32000, 1, 1},   // Landstalker
	{401, 22000, 7, 7},   // Bravura
	{404, 14000, 12, 12}, // Perennial
	{410, 11000, 6, 6},   // Manana
	{422, 18500, 40, 40}, // Bobcat
	{426, 38000, 2, 2},   // Premier
	{436, 17500, 36, 36}, // Previon
	{542, 21000, 66, 66}  // Clover
};

enum E_COURIER_DEST {
	cdAddress[48],
	Float:cdX,
	Float:cdY,
	Float:cdZ
};

new const gCourierDestList[8][E_COURIER_DEST] = {
	{"101 Grove Street, Ganton", 2495.3, -1690.8, 13.5},
	{"205 Idlewood Motel, Idlewood", 1978.2, -1582.4, 13.4},
	{"502 Verona Beach Pierway", 1142.0, -2040.5, 13.8},
	{"Balai Kota SAGOV, Pershing Square", 1481.0, -1770.0, 15.3},
	{"Minimarket 24/7 Idlewood", 1315.0, -1748.0, 13.5},
	{"Bengkel Modifikasi Commerce", 1995.0, -2060.0, 13.5},
	{"Dermaga Santa Maria Pier", 383.0, -2079.0, 7.8},
	{"104 East Ganton Boulevard", 2262.1, -1647.5, 15.2}
};

new gCourierTargetDest[MAX_PLAYERS];

// job checkpoint coordinates
new const gSweeperPoints[MAX_SWEEPER_POINTS][3] = {
	{1489, -1744, 13}, {1354, -1769, 13}, {1209, -1769, 13}, {1063, -1779, 13},
	{961, -1676, 13}, {1069, -1557, 13}, {1192, -1480, 13}, {1347, -1479, 13}
};

new const gTruckerPoints[MAX_TRUCKER_POINTS][3] = {
	{-2135, -247, 36}, {-1569, -2732, 48}, {614, -600, 17},
	{2290, 83, 26}, {1425, 1459, 10}, {2750, -2454, 13}
};

new const gBusPoints[MAX_BUS_POINTS][3] = {
	{1481, -1740, 13}, {1785, -1890, 13}, {2250, -1670, 15}, {1920, -1190, 20},
	{1360, -1080, 24}, {1000, -1350, 13}, {820, -1610, 13}, {1481, -1740, 13}
};

new const gTrashPoints[MAX_TRASH_POINTS][3] = {
	{1630, -1700, 13}, {1380, -1730, 13}, {1120, -1450, 13},
	{1980, -1620, 14}, {2240, -1650, 15}, {1800, -1920, 13}
};

// global variables
new MySQL:gDB;
new bool:gDBReady;
new gPlayerData[MAX_PLAYERS][E_PLAYER_DATA];
new gPlayerItems[MAX_PLAYERS][MAX_INVENTORY_SLOTS][E_ITEM_DATA];
new gVehicleData[MAX_OWNED_VEHICLES][E_VEHICLE_DATA];
new gVehicleTrunkItems[MAX_OWNED_VEHICLES][MAX_TRUNK_SLOTS][E_ITEM_DATA];
new bool:gVehicleEngineState[MAX_VEHICLES];
new gPlayerVehListSlot[MAX_PLAYERS][MAX_OWNED_VEHICLES];
new gVehicleCount;
new gCurrentTax = PAJAK_DEFAULT;

// job states
new gPlayerJobVehicle[MAX_PLAYERS] = {-1, ...};
new gPlayerJobTrailer[MAX_PLAYERS] = {-1, ...};
new gJobCheckpoint[MAX_PLAYERS];
new gJobCash[MAX_PLAYERS];
new bool:gJobActive[MAX_PLAYERS];
new gJobType[MAX_PLAYERS];

// selection states
new gPlayerVehicleSlot[MAX_PLAYERS];
new gSelectedInvSlot[MAX_PLAYERS];
new gInvSlotMap[MAX_PLAYERS][MAX_INVENTORY_SLOTS]; // listitem dialog -> index slot asli
new gGiveTarget[MAX_PLAYERS];
new gGiveItemType[MAX_PLAYERS];
new bool:gPhoneOpen[MAX_PLAYERS];
new bool:gInventoryOpen[MAX_PLAYERS];
new gPlayerSharedHouse[MAX_PLAYERS] = {-1, ...};

// forward prototypes
forward Float:GetItemWeight(itemtype);
forward GetItemModel(itemtype);
new gPendingCharName[MAX_PLAYERS][24];
new gPendingCharDOB[MAX_PLAYERS][16];
new gUserCharIDs[MAX_PLAYERS][3];

// trunk & dispatch states
new gDispatches[MAX_DISPATCHES][E_DISPATCH_DATA];
new gSelectedTrunkVeh[MAX_PLAYERS] = {-1, ...};
new gSelectedTrunkSlot[MAX_PLAYERS] = {-1, ...};
new gPending911Service[MAX_PLAYERS];

stock IsValidRoleplayName(const name[])
{
	new len = strlen(name);
	if (len < 4 || len > 24) return 0;

	new underscoreCount = 0;
	new underscorePos = -1;

	for (new i = 0; i < len; i++) {
		if (name[i] == '_') {
			underscoreCount++;
			underscorePos = i;
		} else if (!((name[i] >= 'a' && name[i] <= 'z') || (name[i] >= 'A' && name[i] <= 'Z'))) {
			return 0;
		}
	}

	if (underscoreCount != 1) return 0;
	if (underscorePos <= 0 || underscorePos >= len - 1) return 0;

	if (name[0] < 'A' || name[0] > 'Z') return 0;
	if (name[underscorePos + 1] < 'A' || name[underscorePos + 1] > 'Z') return 0;

	return 1;
}

// textdraw visibility flags
new bool:gTD_HUD[MAX_PLAYERS];
new bool:gTD_Speedo[MAX_PLAYERS];
new bool:gTD_Alert[MAX_PLAYERS];
new bool:gTD_Subtitle[MAX_PLAYERS];
new gAlertTimer[MAX_PLAYERS];
new gSubtitleTimer[MAX_PLAYERS];

// pickups
new gBankPickup = -1;
new gATMPickup = -1;
new gJobPickup = -1;
new gDealerPickup = -1;
new gFuelPickup = -1;
new gStorePickup = -1;
new gPolicePickup = -1;
new gMedicPickup = -1;
new gGovPickup = -1;

#if defined __Pawn
	#pragma unused gVehicleTrunkItems, gBankPickup, gATMPickup, gJobPickup
	#pragma unused gDealerPickup, gFuelPickup, gStorePickup, gPolicePickup, gMedicPickup, gGovPickup
#endif

// timer handles
new gTimerTick = 0;
new gTimerSpeedo = 0;
new gTimerSave = 0;

// forward declarations
forward RP_Tick();
forward RP_SpeedoTick();
forward RP_SaveTick();
forward RP_DelayLogin(playerid);
forward RP_HideAlert(playerid);
forward RP_HideSubtitle(playerid);

// helper broadcast chat proximity
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

stock IsPlayerNearPoint(playerid, Float:x, Float:y, Float:z, Float:range)
{
	if (IsPlayerConnected(playerid)) {
		return IsPlayerInRangeOfPoint(playerid, range, x, y, z);
	}
	new Float:dist = VectorSize(gPlayerData[playerid][pLastX] - x, gPlayerData[playerid][pLastY] - y, gPlayerData[playerid][pLastZ] - z);
	return (dist <= range);
}

stock IsPlayerNear(playerid, targetid, Float:range)
{
	if (!IsPlayerConnected(playerid) || !IsPlayerConnected(targetid)) return 0;
	new Float:px, Float:py, Float:pz, Float:tx, Float:ty, Float:tz;
	GetPlayerPos(playerid, px, py, pz);
	GetPlayerPos(targetid, tx, ty, tz);
	return (VectorSize(px - tx, py - ty, pz - tz) <= range);
}
