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
#define FACTION_NONE           0
#define FACTION_POLICE         1 // SAPD
#define FACTION_MEDIC          2 // SAMD
#define FACTION_GOV            3 // SAGOV
#define FACTION_GANG           4 // Underground

// jobs (8 jobs)
#define JOB_NONE               0
#define JOB_COURIER            1 // Kurir Paket
#define JOB_SWEEPER            2 // Pembersih Jalan
#define JOB_TRUCKER            3 // Pengangkut Muatan
#define JOB_MECHANIC           4 // Mekanik Bengkel
#define JOB_SCAVENGER          5 // Pemulung Sampah
#define JOB_LUMBERJACK         6 // Penebang Pohon
#define JOB_FISHERMAN          7 // Nelayan Dermaga
#define JOB_BUSDRIVER          8 // Supir Bus Trans-LS

#define COURIER_VEH_MODEL      422 // Bobcat
#define SWEEPER_VEH_MODEL      574 // Sweeper
#define TRUCKER_VEH_MODEL      403 // Linerunner
#define TRUCKER_TRAILER_MODEL  435 // Article Trailer
#define BUS_VEH_MODEL          431 // Bus

// inventory items
#define MAX_INVENTORY_SLOTS    12
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
	ITEM_RECYCLED       // Barang Rongsokan
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
	vVehicleHandle
};

// dialog IDs
#define DIALOG_UCP_LOGIN          1
#define DIALOG_UCP_REG_CODE       2
#define DIALOG_UCP_REG_PASS       3
#define DIALOG_UCP_OTP_VERIFY     40
#define DIALOG_CHAR_SELECT        4
#define DIALOG_CHAR_CREATE_NAME   5
#define DIALOG_CHAR_CREATE_DOB    6
#define DIALOG_CHAR_CREATE_GENDER 7
#define DIALOG_REGISTER           8
#define DIALOG_REGISTER_CONFIRM   9
#define DIALOG_LOGIN              10
#define DIALOG_STATS              11
#define DIALOG_BANK_MENU          12
#define DIALOG_BANK_DEPOSIT       13
#define DIALOG_BANK_WITHDRAW      14
#define DIALOG_BANK_TRANSFER      15
#define DIALOG_ATM_MENU           16
#define DIALOG_ATM_WITHDRAW       17
#define DIALOG_ATM_DEPOSIT        18
#define DIALOG_PAY                19
#define DIALOG_JOB_MENU           20
#define DIALOG_DEALERSHIP         21
#define DIALOG_MYVEH              22
#define DIALOG_MYVEH_ACTION       23
#define DIALOG_TRUNK              24
#define DIALOG_TRUNK_DEPOSIT      25
#define DIALOG_TRUNK_WITHDRAW     26
#define DIALOG_REFUEL             27
#define DIALOG_SAPD_ACTIONS       28
#define DIALOG_HELP               29
#define DIALOG_INVENTORY          30
#define DIALOG_ITEM_ACTION        31
#define DIALOG_STORE_BUY          32
#define DIALOG_PHONE_MENU         33
#define DIALOG_PHONE_CALL         34
#define DIALOG_PHONE_SMS          35
#define DIALOG_RADIO_FREQ         36
#define DIALOG_GPS_MENU           37
#define DIALOG_GIVE_PLAYER        38
#define DIALOG_GIVE_AMOUNT        39

// server limits & config
#define MAX_DEALER_STOCK         8
#define MAX_COURIER_POINTS       8
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

#define COURIER_X                1647.0
#define COURIER_Y                -1886.0
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

#define HUNGER_DECAY             0.15
#define THIRST_DECAY             0.20
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

// job checkpoint coordinates
new const gCourierPoints[MAX_COURIER_POINTS][3] = {
	{2226, -1673, 15}, {1836, -1842, 13}, {1596, -2326, 13}, {1199, -1277, 14},
	{934, -1408, 13}, {812, -1616, 13}, {652, -1864, 5}, {494, -1760, 12}
};

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
new gGiveTarget[MAX_PLAYERS];
new gGiveItemType[MAX_PLAYERS];
new bool:gPhoneOpen[MAX_PLAYERS];
new bool:gInventoryOpen[MAX_PLAYERS];
new bool:gVehDashOpen[MAX_PLAYERS];
new bool:gBankUIOpen[MAX_PLAYERS];
new bool:gActMenuOpen[MAX_PLAYERS];
new gLastValidClickTick[MAX_PLAYERS];
new gPendingCharName[MAX_PLAYERS][24];
new gPendingCharDOB[MAX_PLAYERS][16];
new gUserCharIDs[MAX_PLAYERS][3];

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
	if (!IsPlayerConnected(playerid)) return 0;
	return IsPlayerInRangeOfPoint(playerid, range, x, y, z);
}
