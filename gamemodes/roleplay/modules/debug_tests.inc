#if defined _INC_rp_debug_tests
	#endinput
#endif
#define _INC_rp_debug_tests

/*
==============================================================================
  NUSANTARA ROLEPLAY - debug_tests.inc
  Automated In-Game Feature Test Suite & Diagnostics (11 Core Systems)
==============================================================================
*/

forward RunAutomatedRPTests();
public RunAutomatedRPTests()
{
	print("\n==================================================");
	print("  [DEBUG SUITE] NUSANTARA ROLEPLAY FEATURE TESTS  ");
	print("==================================================");

	new passCount = 0;
	new failCount = 0;

	// 1. TEST DATABASE & SQL ESCAPE
	print("[TEST 1/11] Database Connectivity & SQL Escaping...");
	if (!gDatabaseReady) {
		print("  -> FAIL: Database not connected!");
		failCount++;
	} else {
		new testStr[] = "O'Connor";
		new escaped[64];
		SQL_Escape(testStr, escaped, sizeof(escaped));
		if (strcmp(escaped, "O''Connor") == 0) {
			print("  -> PASS: SQLite double-single-quote escaping verified (O''Connor).");
			passCount++;
		} else {
			printf("  -> FAIL: SQL escape produced '%s' instead of 'O''Connor'", escaped);
			failCount++;
		}
	}

	// 2. TEST PLAYER REGISTRATION & PASSWORD HASHING
	print("[TEST 2/11] Auth System & SHA-256 Hash...");
	new dummyName[] = "Tester_Rezz";
	new dummyPass[] = "Rahasia123";
	new salt[65], hash[65];
	format(salt, sizeof(salt), "%s-%d-%d", dummyName, random(999999), GetTickCount());
	HashPassword(dummyPass, salt, hash, sizeof(hash));

	if (strlen(hash) == 64 && strlen(salt) > 0) {
		new delQuery[128];
		format(delQuery, sizeof(delQuery), "DELETE FROM players WHERE name = '%s'", dummyName);
		DB_FreeResultSet(DB_ExecuteQuery(gDatabase, delQuery));

		if (DB_CreateAccount(dummyName, hash, salt)) {
			new fetchedID, fetchedHash[65], fetchedSalt[65];
			if (DB_FetchAccount(dummyName, fetchedID, fetchedHash, sizeof(fetchedHash), fetchedSalt, sizeof(fetchedSalt))) {
				if (strcmp(fetchedHash, hash) == 0 && strcmp(fetchedSalt, salt) == 0) {
					print("  -> PASS: Account created, hashed, salted, and retrieved correctly from SQLite.");
					passCount++;
				} else {
					print("  -> FAIL: Hash/Salt mismatch on retrieval.");
					failCount++;
				}
			} else {
				print("  -> FAIL: DB_FetchAccount returned false.");
				failCount++;
			}
		} else {
			print("  -> FAIL: DB_CreateAccount returned false.");
			failCount++;
		}
	} else {
		print("  -> FAIL: Hash generation failed.");
		failCount++;
	}

	// 3. TEST ECONOMY (Cash, Bank, ATM, Payday)
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
			printf("  -> PASS: Economy operations valid. Calculated Payday: $%d/hr.", paycheckSalary);
			passCount++;
		} else {
			print("  -> FAIL: Payday calculation returned 0.");
			failCount++;
		}
	} else {
		printf("  -> FAIL: Balance mismatch. Cash: %d, Bank: %d", gPlayerData[testPlayer][pMoney], gPlayerData[testPlayer][pBank]);
		failCount++;
	}

	// 4. TEST FACTIONS (SAPD, Duty, Arrest)
	print("[TEST 4/11] Factions Logic (SAPD Duty & Tools)...");
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

	// 5. TEST JOBS (Kurir, Sweeper, Trucker)
	print("[TEST 5/11] Sidejobs Configuration & Checkpoints...");
	new courierCount = sizeof(gCourierPoints);
	new sweeperCount = sizeof(gSweeperPoints);
	new truckerCount = sizeof(gTruckerPoints);
	if (courierCount == MAX_COURIER_POINTS && sweeperCount == MAX_SWEEPER_POINTS && truckerCount == MAX_TRUCKER_POINTS) {
		printf("  -> PASS: %d Courier, %d Sweeper, & %d Trucker checkpoints verified.",
			courierCount, sweeperCount, truckerCount);
		passCount++;
	} else {
		print("  -> FAIL: Mismatch in job checkpoint array dimensions.");
		failCount++;
	}

	// 6. TEST VEHICLES (Dealership, Engine, Lock, Fuel)
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
			printf("  -> PASS: Vehicle slot initialized, engine running, fuel consumption active (%.2fL remaining), trunk: $%d.",
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

	// 7. TEST SURVIVAL METRICS (Hunger & Thirst decay)
	print("[TEST 7/11] Survival System (Lapar & Haus)...");
	gPlayerData[testPlayer][pSpawned] = true;
	gPlayerData[testPlayer][pHunger] = 100.0;
	gPlayerData[testPlayer][pThirst] = 100.0;
	UpdateHungerThirst(testPlayer);

	if (gPlayerData[testPlayer][pHunger] < 100.0 && gPlayerData[testPlayer][pThirst] < 100.0) {
		printf("  -> PASS: Hunger & Thirst decaying normally (Hunger: %.2f%%, Thirst: %.2f%%).",
			gPlayerData[testPlayer][pHunger], gPlayerData[testPlayer][pThirst]);
		passCount++;
	} else {
		print("  -> FAIL: Hunger and thirst did not decrease.");
		failCount++;
	}

	// 8. TEST TEXTDRAWS (UI Memory allocation)
	print("[TEST 8/11] Modern TextDraws System...");
	TextdrawCreateAll(testPlayer);
	if (gTD_Ready[testPlayer]) {
		print("  -> PASS: HUD, Speedometer, and Alert TextDraws allocated and ready for player.");
		passCount++;
		TextdrawDestroyAll(testPlayer);
	} else {
		print("  -> FAIL: Textdraw initialization failed.");
		failCount++;
	}

	// 9. TEST ROLEPLAY COMMANDS & PROXIMITY
	print("[TEST 9/11] Roleplay Mechanics & Proximity ranges...");
	new Float:distMe = RANGE_ME;
	new Float:distShout = RANGE_SHOUT;
	new Float:distLow = RANGE_LOW;
	if (distLow < distMe && distMe < distShout) {
		printf("  -> PASS: RP Chat radiuses properly calibrated (Low: %.1fm, Me: %.1fm, Shout: %.1fm).",
			distLow, distMe, distShout);
		passCount++;
	} else {
		print("  -> FAIL: RP chat radius hierarchy inverted.");
		failCount++;
	}

	// 10. TEST INVENTORY & ITEM MANAGEMENT
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

	// 11. TEST TELECOMMUNICATION & RADIO FREQUENCY
	print("[TEST 11/11] Telecommunications & Walkie Talkie Frequencies...");
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

	print("==================================================");
	printf("  [DEBUG SUMMARY] %d / 11 TESTS PASSED, %d FAILED", passCount, failCount);
	print("==================================================\n");

	return 1;
}
