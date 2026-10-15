#define SAMP_COMPAT
#define LEGACY_SCRIPTING_API

#include <open.mp>
#include <a_mysql>
#include <sscanf2>
#include <izcmd>
#include <samp_bcrypt>
#include <easyDialog>

// 1. Core modules
#include "core/header.inc"
#include "core/database.inc"
#include "core/player_auth.inc"
#include "core/localization.inc"

// 2. Anti-Cheat Engine (Multi-Vector Security)
#include "anticheat/anticheat.inc"

// 3. User Interface (HUD, Speedo, Progress Bar, Notifications)
#include "ui/progress_bar.inc"
#include "ui/textdraws.inc"

// 4. World Systems
#include "systems/economy.inc"
#include "systems/inventory.inc"
#include "systems/properties.inc"
#include "systems/vehicles.inc"
#include "systems/shops.inc"
#include "systems/workshop.inc"
#include "systems/doors.inc"
#include "systems/atms.inc"
#include "systems/trash.inc"
#include "systems/traffic_camera.inc"
#include "systems/rental.inc"
#include "systems/store_robbery.inc"
#include "systems/weed_farm.inc"
#include "systems/metal_detector.inc"
#include "systems/dmv_plates.inc"
#include "systems/vending_machines.inc"
#include "systems/impound.inc"
#include "systems/graffiti.inc"
#include "systems/cooking.inc"
#include "systems/dice_game.inc"
#include "systems/dmv_exam.inc"
#include "systems/ammunation.inc"
#include "systems/tire_service.inc"
#include "systems/advertisements.inc"
#include "systems/house_sharing.inc"
#include "systems/weather_system.inc"
#include "systems/bar_alcohol.inc"
#include "systems/gym_fitness.inc"
#include "systems/fast_food.inc"
#include "systems/accessories.inc"
#include "systems/taxi_meter.inc"
#include "systems/farming.inc"
#include "systems/atm_robbery.inc"
#include "systems/boombox.inc"
#include "systems/mining.inc"
#include "systems/scuba_diving.inc"

// 5. Gameplay & Factions
#include "gameplay/mapping.inc"
#include "gameplay/factions.inc"
#include "gameplay/armory.inc"
#include "gameplay/trash_job.inc"
#include "gameplay/job_minigames.inc"
#include "gameplay/jobs.inc"
#include "gameplay/police_mdc.inc"
#include "gameplay/forensics.inc"
#include "gameplay/illegal_activities.inc"
#include "gameplay/phone.inc"
#include "gameplay/player_stats.inc"

// 6. Oldschool Dialog UI & Menus
#include "ui/inventory_dialog.inc"
#include "ui/radial_menu.inc"

// 7. Commands & Admin
#include "commands/commands_rp.inc"
#include "commands/admin.inc"
#include "commands/admin_panel.inc"

// 7. Test Suite
// 8. Event Callbacks & Dialog Handling
#include "core/dialog_handler.inc"
#include "core/gamemode_events.inc"

// 9. Automated Testing Suite
#include "tests/debug_tests.inc"

main()
{
	print("--------------------------------------------------");
	print("  NUSANTARA ROLEPLAY - open.mp Gamemode Loaded    ");
	print("  Komunitas Roleplay Indonesia                    ");
	print("--------------------------------------------------");
}

// gamemode init
public OnGameModeInit()
{
	SetGameModeText("Nusantara RP v1.0");
	ShowNameTags(true);
	EnableStuntBonusForAll(false);
	UsePlayerPedAnims();
	DisableInteriorEnterExits();
	ManualVehicleEngineAndLights();

	// db init
	AC_Init();
	House_Init();   // inisialisasi default rumah DULU, sebelum DB_Init men-trigger
	                // query async OnHousesLoaded yang butuh hUsed=true.
	DB_Init();
	Doors_Init();
	ATMs_Init();
	Trash_Init();
	TrafficCamera_Init();
	Rental_Init();
	Armory_Init();
	IllegalActivities_Init();
	StoreRobbery_Init();
	WeedFarm_Init();
	MetalDetector_Init();
	VendingMachines_Init();
	Impound_Init();
	Graffiti_Init();
	Cooking_Init();
	Dice_Init();
	DMV_Init();
	AmmuNation_Init();
	TireService_Init();
	Advertisements_Init();
	HouseSharing_Init();
	WeatherSystem_Init();
	BarAlcohol_Init();
	GymFitness_Init();
	FastFood_Init();
	Accessories_Init();
	TaxiMeter_Init();

	// default class spawn
	AddPlayerClass(0, SPAWN_X, SPAWN_Y, SPAWN_Z, SPAWN_A, WEAPON_FIST, 0, WEAPON_FIST, 0, WEAPON_FIST, 0);

	// pickups
	gBankPickup = CreatePickup(1274, 1, BANK_X, BANK_Y, BANK_Z);
	gATMPickup = CreatePickup(1239, 1, ATM_X, ATM_Y, ATM_Z);
	gJobPickup = CreatePickup(1239, 1, JOB_CENTER_X, JOB_CENTER_Y, JOB_CENTER_Z);
	gDealerPickup = CreatePickup(1239, 1, DEALER_X, DEALER_Y, DEALER_Z);
	gFuelPickup = CreatePickup(1650, 1, FUEL_STATION_X, FUEL_STATION_Y, FUEL_STATION_Z);
	gStorePickup = CreatePickup(1239, 1, STORE_IDLEWOOD_X, STORE_IDLEWOOD_Y, STORE_IDLEWOOD_Z);
	gPolicePickup = CreatePickup(1247, 1, POLICE_X, POLICE_Y, POLICE_Z);
	gMedicPickup = CreatePickup(1240, 1, MEDIC_X, MEDIC_Y, MEDIC_Z);
	gGovPickup = CreatePickup(1239, 1, GOV_X, GOV_Y, GOV_Z);

	// 3d text labels
	Create3DTextLabel("{33CCFF}[ BANK NUSANTARA ]\n{FFFFFF}Gunakan /bank untuk transaksi", COLOR_LIGHTBLUE, BANK_X, BANK_Y, BANK_Z + 0.8, 15.0, 0, false);
	Create3DTextLabel("{33CCFF}[ ATM ]\n{FFFFFF}Gunakan /atm untuk transaksi", COLOR_LIGHTBLUE, ATM_X, ATM_Y, ATM_Z + 0.8, 10.0, 0, false);
	Create3DTextLabel("{FFFF44}[ PUSAT PEKERJAAN ]\n{FFFFFF}Gunakan /job untuk memilih pekerjaan", COLOR_YELLOW, JOB_CENTER_X, JOB_CENTER_Y, JOB_CENTER_Z + 0.8, 15.0, 0, false);
	Create3DTextLabel("{00FF00}[ DEALERSHIP ]\n{FFFFFF}Gunakan /buyveh untuk membeli kendaraan", COLOR_GREEN, DEALER_X, DEALER_Y, DEALER_Z + 0.8, 15.0, 0, false);
	Create3DTextLabel("{FF9900}[ SPBU ]\n{FFFFFF}Gunakan /refuel untuk isi bensin", COLOR_ORANGE, FUEL_STATION_X, FUEL_STATION_Y, FUEL_STATION_Z + 0.8, 15.0, 0, false);
	Create3DTextLabel("{33CCFF}[ TOKO 24/7 ]\n{FFFFFF}Gunakan /buy untuk membeli kebutuhan", COLOR_LIGHTBLUE, STORE_IDLEWOOD_X, STORE_IDLEWOOD_Y, STORE_IDLEWOOD_Z + 0.8, 15.0, 0, false);
	Create3DTextLabel("{33CCFF}[ KANTOR KEPOLISIAN (SAPD) ]\n{FFFFFF}Markas Komando Polisi Los Santos", COLOR_LIGHTBLUE, POLICE_X, POLICE_Y, POLICE_Z + 0.8, 15.0, 0, false);
	Create3DTextLabel("{FF6B6B}[ RUMAH SAKIT (SAMD) ]\n{FFFFFF}Pelayanan Medis Darurat All Saints", COLOR_RED, MEDIC_X, MEDIC_Y, MEDIC_Z + 0.8, 15.0, 0, false);
	Create3DTextLabel("{00FF00}[ PASAR IKAN DERMAGA ]\n{FFFFFF}Gunakan /sellfish untuk menjual hasil tangkapan", COLOR_GREEN, FISH_PIER_X, FISH_PIER_Y, FISH_PIER_Z + 0.8, 15.0, 0, false);
	Create3DTextLabel("{FFFF44}[ PENGEPUL RONGSOCKAN ]\n{FFFFFF}Gunakan /selltrash untuk menjual barang bekas", COLOR_YELLOW, TRASH_BUYER_X, TRASH_BUYER_Y, TRASH_BUYER_Z + 0.8, 15.0, 0, false);
	Create3DTextLabel("{E67E22}[ PABRIK KAYU SAWMILL ]\n{FFFFFF}Gunakan /sellwood untuk menjual balok kayu", COLOR_ORANGE, SAWMILL_X, SAWMILL_Y, SAWMILL_Z + 0.8, 15.0, 0, false);

	// Load modular world mapping
	LoadNusantaraMapping();

	// Timers
	gTimerTick = SetTimer("RP_Tick", 1000, true);
	gTimerSpeedo = SetTimer("RP_SpeedoTick", 150, true);
	gTimerSave = SetTimer("RP_SaveTick", 60000, true);

	// Inisialisasi Sistem Pertanian, Kebersihan, Boombox, dan Forensik
	Farming_Init();
	TrashJob_Init();
	Boombox_Init();
	Forensics_Init();
	Mining_Init();
	Scuba_Init();

	// run automated tests
	SetTimer("RunAutomatedRPTests", 1500, false);
	return 1;
}

// gamemode exit
public OnGameModeExit()
{
	for (new i = 0; i < MAX_PLAYERS; i++) {
		if (IsPlayerConnected(i) && gPlayerData[i][pLoggedIn]) {
			DB_SavePlayerData(i);
			TextdrawDestroyAll(i);
		}
	}
	if (gTimerTick) KillTimer(gTimerTick);
	if (gTimerSpeedo) KillTimer(gTimerSpeedo);
	if (gTimerSave) KillTimer(gTimerSave);

	DB_Shutdown();
	return 1;
}

// player connection & auth
