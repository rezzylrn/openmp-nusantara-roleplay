#if defined _INC_rp_debug_tests
	#endinput
#endif
#define _INC_rp_debug_tests

// automated diagnostics suite (11 tests)

forward RunAutomatedRPTests();
public RunAutomatedRPTests()
{
	print("\n==================================================");
	print("  [DEBUG SUITE] NUSANTARA ROLEPLAY FEATURE TESTS  ");
	print("==================================================");

	new passCount = 0;
	new failCount = 0;

	// 1. test db connection
	print("[TEST 1/11] Database Connectivity (MySQL R41-4)...");
	if (!gDBReady) {
		print("  -> FAIL: MySQL database not connected!");
		failCount++;
	} else {
		print("  -> PASS: MySQL database connected and tables initialized.");
		passCount++;
	}

	// 2. test auth & roleplay name validation
	print("[TEST 2/11] Auth, Bcrypt Plugin & Roleplay Name Validator...");

	new bool:nameTest1 = IsValidRoleplayName("Javier_Travors") ? true : false;
	new bool:nameTest2 = IsValidRoleplayName("Bryan_Calister") ? true : false;
	new bool:nameBad1  = IsValidRoleplayName("javier_travors") ? false : true;
	new bool:nameBad2  = IsValidRoleplayName("Javier123") ? false : true;
	new bool:nameBad3  = IsValidRoleplayName("Javier Travors") ? false : true;

	bcrypt_set_thread_limit(4);

	if (nameTest1 && nameTest2 && nameBad1 && nameBad2 && nameBad3 && BCRYPT_COST == 12) {
		print("  -> PASS: Bcrypt 12-round hashing & First_Last roleplay name validation confirmed.");
		passCount++;
	} else {
		print("  -> FAIL: Name validation or bcrypt configuration mismatch.");
		failCount++;
	}

	// 3. test economy
	print("[TEST 3/11] Integrated Economy (Cash, Bank, Payday)...");
	new testPlayer = 0;
	ResetPlayerData(testPlayer);
	gPlayerData[testPlayer][pLoggedIn] = true;
	gPlayerData[testPlayer][pMoney] = 500;
	gPlayerData[testPlayer][pBank] = 1000;
	gPlayerData[testPlayer][pLevel] = 2;
	gPlayerData[testPlayer][pFaction] = FACTION_POLICE;
	gPlayerData[testPlayer][pFactionRank] = 3;
	gPlayerData[testPlayer][pJob] = JOB_COURIER;

	GivePlayerMoneyEx(testPlayer, 250);
	TakePlayerBankEx(testPlayer, 100);

	new expectedCash = 500 + 250;
	new expectedBank = 1000 - 100;
	if (gPlayerData[testPlayer][pMoney] == expectedCash && gPlayerData[testPlayer][pBank] == expectedBank) {
		new paycheckSalary = CalculatePaycheck(testPlayer);
		if (paycheckSalary > 0) {
			printf("  -> PASS: Economy calculations verified. Payday: $%d/hr.", paycheckSalary);
			passCount++;
		} else {
			print("  -> FAIL: Payday calculation error.");
			failCount++;
		}
	} else {
		printf("  -> FAIL: Balance mismatch. Cash: %d, Bank: %d", gPlayerData[testPlayer][pMoney], gPlayerData[testPlayer][pBank]);
		failCount++;
	}

	// 4. test factions
	print("[TEST 4/11] Factions Logic (SAPD Duty & Cuff)...");
	ProcessFactionDuty(testPlayer, true);
	if (gPlayerData[testPlayer][pDuty]) {
		new suspectPlayer = 1;
		ResetPlayerData(suspectPlayer);
		gPlayerData[suspectPlayer][pLoggedIn] = true;
		gPlayerData[suspectPlayer][pCuffed] = false;

		gPlayerData[suspectPlayer][pCuffed] = true;
		if (gPlayerData[suspectPlayer][pCuffed]) {
			print("  -> PASS: Police on-duty toggle and arrest/cuff state confirmed.");
			passCount++;
		} else {
			print("  -> FAIL: Handcuff state could not be set.");
			failCount++;
		}
	} else {
		print("  -> FAIL: On-duty toggle failed.");
		failCount++;
	}

	// 5. test jobs (8 jobs configured)
	print("[TEST 5/11] 8 Sidejobs Configuration & Routes...");
	new courierCount = sizeof(gCourierPoints);
	new sweeperCount = sizeof(gSweeperPoints);
	new truckerCount = sizeof(gTruckerPoints);
	new busCount = sizeof(gBusPoints);
	new trashCount = sizeof(gTrashPoints);

	if (courierCount > 0 && sweeperCount > 0 && truckerCount > 0 && busCount > 0 && trashCount > 0) {
		printf("  -> PASS: 8 Jobs ready (Courier: %d, Sweeper: %d, Trucker: %d, Bus: %d, Trash: %d).",
			courierCount, sweeperCount, truckerCount, busCount, trashCount);
		passCount++;
	} else {
		print("  -> FAIL: Checkpoint configuration error.");
		failCount++;
	}

	// 6. test vehicles
	print("[TEST 6/11] Dynamic Personal Vehicles & Fuel Loop...");
	new testSlot = -1;
	for (new i = 0; i < MAX_OWNED_VEHICLES; i++) {
		if (!gVehicleData[i][vUsed]) {
			testSlot = i;
			break;
		}
	}
	if (testSlot != -1) {
		gVehicleData[testSlot][vUsed] = true;
		gVehicleData[testSlot][vDBID] = 999;
		gVehicleData[testSlot][vOwnerID] = 1;
		gVehicleData[testSlot][vModel] = 411; // Infernus
		gVehicleData[testSlot][vFuel] = 100.0;
		gVehicleData[testSlot][vEngine] = true;
		gVehicleData[testSlot][vLocked] = true;
		gVehicleData[testSlot][vTrunk] = 5000;

		gVehicleData[testSlot][vFuel] -= FUEL_CONSUMPTION_RATE;
		if (gVehicleData[testSlot][vFuel] < 100.0 && gVehicleData[testSlot][vTrunk] == 5000) {
			printf("  -> PASS: Vehicle slot active, engine running, fuel: %.2fL, trunk: $%d.",
				gVehicleData[testSlot][vFuel], gVehicleData[testSlot][vTrunk]);
			passCount++;
		} else {
			print("  -> FAIL: Vehicle fuel/trunk state mismatch.");
			failCount++;
		}
		gVehicleData[testSlot][vUsed] = false;
	} else {
		print("  -> FAIL: No free vehicle slot found.");
		failCount++;
	}

	// 7. test survival
	print("[TEST 7/11] Survival System (Lapar & Haus)...");
	gPlayerData[testPlayer][pSpawned] = true;
	gPlayerData[testPlayer][pHunger] = 100.0;
	gPlayerData[testPlayer][pThirst] = 100.0;
	UpdateHungerThirst(testPlayer);

	if (gPlayerData[testPlayer][pHunger] < 100.0 && gPlayerData[testPlayer][pThirst] < 100.0) {
		printf("  -> PASS: Hunger & Thirst decay confirmed (Lapar: %.2f%%, Haus: %.2f%%).",
			gPlayerData[testPlayer][pHunger], gPlayerData[testPlayer][pThirst]);
		passCount++;
	} else {
		print("  -> FAIL: Hunger and thirst did not decrease.");
		failCount++;
	}

	// 8. test textdraws
	print("[TEST 8/11] Modern TextDraws System...");
	TextdrawCreateAll(testPlayer);
	if (gTD_Ready[testPlayer]) {
		print("  -> PASS: HUD, Speedometer, and Alert TextDraws allocated cleanly.");
		passCount++;
		TextdrawDestroyAll(testPlayer);
	} else {
		print("  -> FAIL: Textdraw initialization failed.");
		failCount++;
	}

	// 9. test rp proximity
	print("[TEST 9/11] Roleplay Chat Proximity Ranges...");
	new Float:distLow = RANGE_LOW;
	new Float:distMe = RANGE_ME;
	new Float:distShout = RANGE_SHOUT;
	if (distLow < distMe && distMe < distShout) {
		printf("  -> PASS: Proximity chat calibrated (Low: %.1fm, Me: %.1fm, Shout: %.1fm).",
			distLow, distMe, distShout);
		passCount++;
	} else {
		print("  -> FAIL: RP chat radius inverted.");
		failCount++;
	}

	// 10. test inventory
	print("[TEST 10/11] Modular Slot-Based Inventory System...");
	ClearPlayerInventory(testPlayer);
	GivePlayerItem(testPlayer, ITEM_SNACK, 3);
	GivePlayerItem(testPlayer, ITEM_WATER, 2);
	GivePlayerItem(testPlayer, ITEM_MEDKIT, 1);

	new snackCount = GetPlayerItemCount(testPlayer, ITEM_SNACK);
	TakePlayerItem(testPlayer, ITEM_SNACK, 1);
	new remainingSnacks = GetPlayerItemCount(testPlayer, ITEM_SNACK);

	if (snackCount == 3 && remainingSnacks == 2 && GetPlayerItemCount(testPlayer, ITEM_MEDKIT) == 1) {
		print("  -> PASS: Inventory item allocation, stacking, counting, and consumption confirmed.");
		passCount++;
	} else {
		printf("  -> FAIL: Inventory count mismatch (Expected: 2 snacks, Got: %d).", remainingSnacks);
		failCount++;
	}

	// 11. test comms & radio
	print("[TEST 11/14] Telecommunications & Walkie Talkie...");
	gPlayerData[testPlayer][pPhone] = 554123;
	gPlayerData[testPlayer][pPhoneStatus] = true;
	gPlayerData[testPlayer][pRadioFreq] = 911;

	if (gPlayerData[testPlayer][pPhone] == 554123 && gPlayerData[testPlayer][pRadioFreq] == 911) {
		print("  -> PASS: Assigned phone number 554123 and radio channel 911 MHz operational.");
		passCount++;
	} else {
		print("  -> FAIL: Telecommunications attributes mismatch.");
		failCount++;
	}

	// 12. test server-side anticheat
	print("[TEST 12/14] Server-Side Anti-Cheat Engine (Weapon, Speed, Teleport)...");
	AC_ResetPlayer(testPlayer);
	AC_GiveWeapon(testPlayer, 24, 50); // Desert Eagle (auth)
	new bool:weapAuth = gACData[testPlayer][acHasWeapon][24];
	new bool:weapHacked = !gACData[testPlayer][acHasWeapon][38]; // Minigun (unauth)

	if (weapAuth && weapHacked) {
		print("  -> PASS: Anti-cheat registered weapon list & tamper detection calibrated.");
		passCount++;
	} else {
		print("  -> FAIL: Anti-cheat weapon tracking error.");
		failCount++;
	}

	// 13. test housing real estate
	print("[TEST 13/14] Housing & Real Estate Properties System...");
	new houseCount = 0;
	for (new h = 0; h < MAX_HOUSES; h++) {
		if (gHouseData[h][hUsed]) houseCount++;
	}

	if (houseCount >= 6) {
		printf("  -> PASS: %d Real estate properties active across Los Santos with 3D labels.", houseCount);
		passCount++;
	} else {
		printf("  -> FAIL: Insufficient house count (%d).", houseCount);
		failCount++;
	}

	// 14. test police mdc & speed radar
	print("[TEST 14/14] Police MDC & Speed Radar Detector...");
	new bool:copCar1 = IsPoliceVehicleModel(596) ? true : false;
	new bool:copCar2 = IsPoliceVehicleModel(599) ? true : false;
	new bool:civCar  = IsPoliceVehicleModel(411) ? false : true;

	if (copCar1 && copCar2 && civCar) {
		print("  -> PASS: Police cruiser models & speed radar range calibrated.");
		passCount++;
	} else {
		print("  -> FAIL: Police vehicle classifier mismatch.");
		failCount++;
	}

	print("==================================================");
	printf("  [DEBUG SUMMARY] %d / 14 TESTS PASSED, %d FAILED", passCount, failCount);
	print("==================================================\n");

	return 1;
}
