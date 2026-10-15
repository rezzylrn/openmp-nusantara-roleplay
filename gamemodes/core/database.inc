#if defined _INC_rp_database
	#endinput
#endif
#define _INC_rp_database

// db connection config
#define MYSQL_HOST     "127.0.0.1"
#define MYSQL_USER     "samp"
#define MYSQL_PASS     "samp123"
#define MYSQL_DB       "roleplay"

forward OnUCPCheck(playerid);
forward OnUCPCheckByName(playerid);
forward OnUCPPasswordUpdated(playerid);
forward OnUCPRegistered(playerid);
forward OnUCPCharactersLoaded(playerid);
forward OnCharacterCreated(playerid);
forward OnCharacterLoaded(playerid);
forward OnVehiclesLoaded();

// db init & setup
stock DB_Init()
{
	gDB = mysql_connect(MYSQL_HOST, MYSQL_USER, MYSQL_PASS, MYSQL_DB);
	if (mysql_errno(gDB) != 0) {
		printf("[MYSQL] ERROR: Gagal terhubung ke MySQL (Kode error: %d)!", mysql_errno(gDB));
		gDBReady = false;
		return 0;
	}
	gDBReady = true;
	print("[MYSQL] Terhubung ke database MySQL (roleplay) via plugin R41-4!");

	// 1. master ucp accounts table
	mysql_tquery(gDB, "CREATE TABLE IF NOT EXISTS ucp_accounts (\
		id INT AUTO_INCREMENT PRIMARY KEY, \
		ucp_name VARCHAR(24) NOT NULL UNIQUE, \
		player_name VARCHAR(24) NOT NULL DEFAULT '', \
		password VARCHAR(64) NOT NULL DEFAULT '', \
		salt VARCHAR(64) NOT NULL DEFAULT '', \
		security_code VARCHAR(16) NOT NULL DEFAULT '', \
		pin_used TINYINT DEFAULT 0, \
		created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)");

	// 2. multi-characters table
	mysql_tquery(gDB, "CREATE TABLE IF NOT EXISTS characters (\
		id INT AUTO_INCREMENT PRIMARY KEY, \
		ucp_id INT NOT NULL, \
		name VARCHAR(24) NOT NULL UNIQUE, \
		birthdate VARCHAR(16) DEFAULT '01/01/2000', \
		gender INT DEFAULT 1, \
		money INT DEFAULT 500, \
		bank INT DEFAULT 1000, \
		level INT DEFAULT 1, \
		exp INT DEFAULT 0, \
		admin INT DEFAULT 0, \
		faction INT DEFAULT 0, \
		factionrank INT DEFAULT 0, \
		duty INT DEFAULT 0, \
		job INT DEFAULT 0, \
		hunger FLOAT DEFAULT 100.0, \
		thirst FLOAT DEFAULT 100.0, \
		skin INT DEFAULT 0, \
		lastx FLOAT DEFAULT 1481.0, \
		lasty FLOAT DEFAULT -1748.0, \
		lastz FLOAT DEFAULT 13.5, \
		lasta FLOAT DEFAULT 270.0, \
		interior INT DEFAULT 0, \
		vworld INT DEFAULT 0, \
		cuffed INT DEFAULT 0, \
		warnings INT DEFAULT 0, \
		kills INT DEFAULT 0, \
		deaths INT DEFAULT 0, \
		playhours INT DEFAULT 0, \
		muted INT DEFAULT 0, \
		phone INT DEFAULT 0, \
		phonestatus INT DEFAULT 1, \
		radiofreq INT DEFAULT 0, \
		radiostatus INT DEFAULT 0, \
		injured INT DEFAULT 0, \
		language INT DEFAULT 1, \
		tickets INT DEFAULT 0, \
		ticket_amount INT DEFAULT 0, \
		drive_lic INT DEFAULT 0, \
		gun_lic INT DEFAULT 0, \
		fight_style INT DEFAULT 4, \
		created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, \
		INDEX (ucp_id))");

	// 3. vehicles table
	mysql_tquery(gDB, "CREATE TABLE IF NOT EXISTS vehicles (\
		id INT AUTO_INCREMENT PRIMARY KEY, \
		ownerid INT NOT NULL, \
		model INT NOT NULL, \
		posx FLOAT DEFAULT 0.0, \
		posy FLOAT DEFAULT 0.0, \
		posz FLOAT DEFAULT 0.0, \
		posa FLOAT DEFAULT 0.0, \
		color1 INT DEFAULT 1, \
		color2 INT DEFAULT 1, \
		fuel FLOAT DEFAULT 100.0, \
		health FLOAT DEFAULT 1000.0, \
		plate VARCHAR(16) DEFAULT '', \
		engine INT DEFAULT 0, \
		locked INT DEFAULT 0, \
		trunk INT DEFAULT 0, \
		trunk_items VARCHAR(128) DEFAULT '', \
		trunk_gun INT DEFAULT 0, \
		trunk_ammo INT DEFAULT 0, \
		impounded INT DEFAULT 0)");

	// 3b. Migrasi kolom untuk database lama (ALTER gagal jika kolom sudah ada — abaikan).
	mysql_tquery(gDB, "ALTER TABLE characters ADD COLUMN language INT DEFAULT 1", "", "");
	mysql_tquery(gDB, "ALTER TABLE characters ADD COLUMN tickets INT DEFAULT 0", "", "");
	mysql_tquery(gDB, "ALTER TABLE characters ADD COLUMN ticket_amount INT DEFAULT 0", "", "");
	mysql_tquery(gDB, "ALTER TABLE characters ADD COLUMN drive_lic INT DEFAULT 0", "", "");
	mysql_tquery(gDB, "ALTER TABLE characters ADD COLUMN gun_lic INT DEFAULT 0", "", "");
	mysql_tquery(gDB, "ALTER TABLE characters ADD COLUMN fight_style INT DEFAULT 4", "", "");
	mysql_tquery(gDB, "ALTER TABLE vehicles ADD COLUMN trunk_items VARCHAR(128) DEFAULT ''", "", "");
	mysql_tquery(gDB, "ALTER TABLE vehicles ADD COLUMN trunk_gun INT DEFAULT 0", "", "");
	mysql_tquery(gDB, "ALTER TABLE vehicles ADD COLUMN trunk_ammo INT DEFAULT 0", "", "");
	mysql_tquery(gDB, "ALTER TABLE vehicles ADD COLUMN impounded INT DEFAULT 0", "", "");

	// 4. bans table
	mysql_tquery(gDB, "CREATE TABLE IF NOT EXISTS bans (\
		id INT AUTO_INCREMENT PRIMARY KEY, \
		name VARCHAR(24) NOT NULL, \
		ip VARCHAR(20) NOT NULL, \
		reason VARCHAR(96), \
		admin VARCHAR(24), \
		created INT)");

	// 4b. houses table — properti harus persisten, bukan in-memory saja.
	// Tanpa ini rumah yang dibeli hilang setiap restart (uang pemain hangus).
	mysql_tquery(gDB, "CREATE TABLE IF NOT EXISTS houses (\
		id INT AUTO_INCREMENT PRIMARY KEY, \
		houseindex INT NOT NULL UNIQUE, \
		ownerid INT DEFAULT 0, \
		ownername VARCHAR(24) DEFAULT 'Pemerintah', \
		locked INT DEFAULT 1, \
		storage_cash INT DEFAULT 0)");

	// load vehicles
	mysql_tquery(gDB, "SELECT * FROM vehicles", "OnVehiclesLoaded", "");

	// load bans ke cache memori untuk pengecekan sinkron di OnPlayerConnect
	mysql_tquery(gDB, "SELECT name, ip FROM bans", "OnBansLoaded", "");

	// load properti rumah (owner, kunci, storage) — persisten
	mysql_tquery(gDB, "SELECT houseindex, ownerid, ownername, locked, storage_cash FROM houses", "OnHousesLoaded", "");
	return 1;
}

stock DB_Shutdown()
{
	if (gDBReady) {
		for (new i = 0; i < MAX_OWNED_VEHICLES; i++) {
			if (gVehicleData[i][vUsed]) DB_SaveVehicle(i);
		}
		mysql_close(gDB);
		gDBReady = false;
	}
	return 1;
}

// ---------------------------------------------------------------------------
// UCP Check & Login Flow (Auto-detect dari nama samp player)
// ---------------------------------------------------------------------------
stock DB_CheckUCPByPlayerName(playerid)
{
	if (!gDBReady) return 0;
	new playerName[MAX_PLAYER_NAME];
	GetPlayerName(playerid, playerName, sizeof(playerName));

	format(gPlayerData[playerid][pUCPName], 24, "%s", playerName);

	new query[192];
	mysql_format(gDB, query, sizeof(query),
		"SELECT id, password, salt, security_code, pin_used FROM ucp_accounts WHERE ucp_name = '%e' OR player_name = '%e' LIMIT 1",
		playerName, playerName);
	mysql_tquery(gDB, query, "OnUCPCheckByName", "i", playerid);
	return 1;
}

public OnUCPCheckByName(playerid)
{
	if (!IsPlayerConnected(playerid)) return 0;
	new rows = cache_num_rows();

	if (rows == 0) {
		// Akun belum ada -> kick dengan info registrasi via bot
		ShowUCPNotRegisteredKick(playerid);
		return 1;
	}

	cache_get_value_name_int(0, "id", gPlayerData[playerid][pUCPID]);
	cache_get_value_name(0, "password", gPlayerData[playerid][pPassword], 65);
	cache_get_value_name(0, "salt", gPlayerData[playerid][pSalt], 65);

	if (strlen(gPlayerData[playerid][pPassword]) == 0) {
		// pass masih kosong -> player daftar via bot, harus verifikasi OTP/PIN dulu
		ShowUCPOtpVerifyDialog(playerid);
	} else {
		// akun aktif -> langsung minta password
		ShowUCPLoginDialog(playerid);
	}
	return 1;
}

// OTP/PIN verification: cocokkan otp dari input player dengan security_code di DB
stock DB_VerifyUCPOtp(playerid, const otp[])
{
	if (!gDBReady || gPlayerData[playerid][pUCPID] <= 0) return 0;
	new query[224];
	mysql_format(gDB, query, sizeof(query),
		"SELECT security_code, pin_used FROM ucp_accounts WHERE id = %d LIMIT 1", gPlayerData[playerid][pUCPID]);
	mysql_tquery(gDB, query, "OnUCPOtpChecked", "is", playerid, otp);
	return 1;
}

forward OnUCPOtpChecked(playerid, const otp[]);
public OnUCPOtpChecked(playerid, const otp[])
{
	if (!IsPlayerConnected(playerid)) return 0;
	if (cache_num_rows() == 0) {
		SendErr(playerid, "Data registrasi tidak ditemukan.");
		Kick(playerid);
		return 0;
	}

	new dbOtp[16], pinUsed;
	cache_get_value_name(0, "security_code", dbOtp, 16);
	cache_get_value_name_int(0, "pin_used", pinUsed);

	if (strcmp(dbOtp, otp, false) != 0) {
		SendErr(playerid, "Kode verifikasi salah. Ambil ulang via bot resmi.");
		ShowUCPOtpVerifyDialog(playerid);
		return 0;
	}
	if (pinUsed != 0) {
		SendErr(playerid, "Kode verifikasi sudah dipakai. Hubungi admin.");
		ShowUCPOtpVerifyDialog(playerid);
		return 0;
	}

	// OTP valid -> lanjut buat password
	ShowUCPRegisterPassDialog(playerid);
	return 1;
}

// Update password setelah OTP valid (mark pin sebagai dipakai)
stock DB_UpdateUCPPassword(playerid, const hash[])
{
	if (!gDBReady || gPlayerData[playerid][pUCPID] <= 0) return 0;
	new query[384];
	mysql_format(gDB, query, sizeof(query),
		"UPDATE ucp_accounts SET password = '%e', salt = 'bcrypt', pin_used = 1 WHERE id = %d",
		hash, gPlayerData[playerid][pUCPID]);
	mysql_tquery(gDB, query, "OnUCPPasswordUpdated", "i", playerid);
	return 1;
}

public OnUCPPasswordUpdated(playerid)
{
	if (!IsPlayerConnected(playerid)) return 0;
	format(gPlayerData[playerid][pPassword], 65, "%s", "updated");
	DB_LoadUCPCharacters(playerid);
	return 1;
}

stock DB_RegisterUCP(playerid, const ucpName[], const pass[], const salt[], const secCode[])
{
	if (!gDBReady) return 0;
	new query[384];
	mysql_format(gDB, query, sizeof(query),
		"INSERT INTO ucp_accounts (ucp_name, player_name, password, salt, security_code) VALUES ('%e', '%e', '%e', '%e', '%e')",
		ucpName, ucpName, pass, salt, secCode);
	mysql_tquery(gDB, query, "OnUCPRegistered", "i", playerid);
	return 1;
}

public OnUCPRegistered(playerid)
{
	if (!IsPlayerConnected(playerid)) return 0;
	gPlayerData[playerid][pUCPID] = cache_insert_id();
	DB_LoadUCPCharacters(playerid);
	return 1;
}

// ---------------------------------------------------------------------------
// Multi-Character System
// ---------------------------------------------------------------------------
stock DB_LoadUCPCharacters(playerid)
{
	if (!gDBReady || gPlayerData[playerid][pUCPID] <= 0) return 0;
	new query[160];
	mysql_format(gDB, query, sizeof(query),
		"SELECT id, name, level, skin, faction FROM characters WHERE ucp_id = %d ORDER BY id ASC LIMIT 3",
		gPlayerData[playerid][pUCPID]);
	mysql_tquery(gDB, query, "OnUCPCharactersLoaded", "i", playerid);
	return 1;
}

public OnUCPCharactersLoaded(playerid)
{
	if (!IsPlayerConnected(playerid)) return 0;
	new rows = cache_num_rows();

	for (new i = 0; i < 3; i++) {
		gUserCharIDs[playerid][i] = 0;
	}

	new dialogList[512];
	format(dialogList, sizeof(dialogList), "Slot\tNama Karakter\tLevel & Faksi\n");

	for (new i = 0; i < rows; i++) {
		new charid, name[24], level, faction;
		cache_get_value_name_int(i, "id", charid);
		cache_get_value_name(i, "name", name, 24);
		cache_get_value_name_int(i, "level", level);
		cache_get_value_name_int(i, "faction", faction);

		gUserCharIDs[playerid][i] = charid;

		new factionLabel[32];
		switch (faction) {
			case FACTION_POLICE: format(factionLabel, sizeof(factionLabel), "SAPD");
			case FACTION_MEDIC:  format(factionLabel, sizeof(factionLabel), "SAMD");
			case FACTION_GOV:    format(factionLabel, sizeof(factionLabel), "SAGOV");
			case FACTION_GANG:   format(factionLabel, sizeof(factionLabel), "Gang");
			default:             format(factionLabel, sizeof(factionLabel), "Warga Sipil");
		}

		format(dialogList, sizeof(dialogList), "%sSlot %d\t%s\tLevel %d (%s)\n",
			dialogList, i + 1, name, level, factionLabel);
	}

	if (rows < 3) {
		format(dialogList, sizeof(dialogList), "%sSlot %d\t{2ECC71}[+] Buat Karakter Baru{FFFFFF}\t-\n",
			dialogList, rows + 1);
	}

	ShowPlayerDialog(playerid, DIALOG_CHAR_SELECT, DIALOG_STYLE_TABLIST_HEADERS,
		"PILIH KARAKTER - NUSANTARA ROLEPLAY", dialogList, "Pilih", "Keluar");
	return 1;
}

stock DB_CreateCharacter(playerid, const charName[], const birthdate[], gender, skin)
{
	if (!gDBReady || gPlayerData[playerid][pUCPID] <= 0) return 0;
	new query[400];
	mysql_format(gDB, query, sizeof(query),
		"INSERT INTO characters (ucp_id, name, birthdate, gender, skin, money, bank, level) \
		VALUES (%d, '%e', '%e', %d, %d, %d, %d, %d)",
		gPlayerData[playerid][pUCPID], charName, birthdate, gender, skin,
		START_MONEY, START_BANK, START_LEVEL);
	mysql_tquery(gDB, query, "OnCharacterCreated", "i", playerid);
	return 1;
}

public OnCharacterCreated(playerid)
{
	if (!IsPlayerConnected(playerid)) return 0;
	new charid = cache_insert_id();
	SendSucc(playerid, "Karakter baru berhasil dibuat!");
	DB_LoadCharacter(playerid, charid);
	return 1;
}

stock DB_LoadCharacter(playerid, charid)
{
	if (!gDBReady) return 0;
	new query[140];
	mysql_format(gDB, query, sizeof(query),
		"SELECT * FROM characters WHERE id = %d LIMIT 1", charid);
	mysql_tquery(gDB, query, "OnCharacterLoaded", "i", playerid);
	return 1;
}

public OnCharacterLoaded(playerid)
{
	if (!IsPlayerConnected(playerid)) return 0;
	if (cache_num_rows() <= 0) return 0;

	cache_get_value_name_int(0, "id", gPlayerData[playerid][pID]);
	cache_get_value_name(0, "name", gPlayerData[playerid][pName], MAX_PLAYER_NAME);
	cache_get_value_name(0, "birthdate", gPlayerData[playerid][pBirthDate], 16);
	cache_get_value_name_int(0, "gender", gPlayerData[playerid][pGender]);

	cache_get_value_name_int(0, "money", gPlayerData[playerid][pMoney]);
	cache_get_value_name_int(0, "bank", gPlayerData[playerid][pBank]);
	cache_get_value_name_int(0, "level", gPlayerData[playerid][pLevel]);
	cache_get_value_name_int(0, "exp", gPlayerData[playerid][pExp]);
	cache_get_value_name_int(0, "admin", gPlayerData[playerid][pAdmin]);
	cache_get_value_name_int(0, "faction", gPlayerData[playerid][pFaction]);
	cache_get_value_name_int(0, "factionrank", gPlayerData[playerid][pFactionRank]);

	new dutyVal;
	cache_get_value_name_int(0, "duty", dutyVal);
	gPlayerData[playerid][pDuty] = (dutyVal != 0);

	cache_get_value_name_int(0, "job", gPlayerData[playerid][pJob]);
	cache_get_value_name_float(0, "hunger", gPlayerData[playerid][pHunger]);
	cache_get_value_name_float(0, "thirst", gPlayerData[playerid][pThirst]);
	cache_get_value_name_int(0, "skin", gPlayerData[playerid][pSkin]);
	cache_get_value_name_float(0, "lastx", gPlayerData[playerid][pLastX]);
	cache_get_value_name_float(0, "lasty", gPlayerData[playerid][pLastY]);
	cache_get_value_name_float(0, "lastz", gPlayerData[playerid][pLastZ]);
	cache_get_value_name_float(0, "lasta", gPlayerData[playerid][pLastA]);
	cache_get_value_name_int(0, "interior", gPlayerData[playerid][pInterior]);
	cache_get_value_name_int(0, "vworld", gPlayerData[playerid][pVirtualWorld]);

	new cuffedVal, mutedVal;
	cache_get_value_name_int(0, "cuffed", cuffedVal);
	gPlayerData[playerid][pCuffed] = (cuffedVal != 0);

	cache_get_value_name_int(0, "warnings", gPlayerData[playerid][pWarnings]);
	cache_get_value_name_int(0, "kills", gPlayerData[playerid][pKills]);
	cache_get_value_name_int(0, "deaths", gPlayerData[playerid][pDeaths]);
	cache_get_value_name_int(0, "playhours", gPlayerData[playerid][pPlayHours]);

	cache_get_value_name_int(0, "muted", mutedVal);
	gPlayerData[playerid][pMuted] = (mutedVal != 0);

	cache_get_value_name_int(0, "phone", gPlayerData[playerid][pPhone]);

	new pStatusVal;
	cache_get_value_name_int(0, "phonestatus", pStatusVal);
	gPlayerData[playerid][pPhoneStatus] = (pStatusVal != 0);

	cache_get_value_name_int(0, "radiofreq", gPlayerData[playerid][pRadioFreq]);

	new rStatusVal;
	cache_get_value_name_int(0, "radiostatus", rStatusVal);
	gPlayerData[playerid][pRadioStatus] = (rStatusVal != 0);

	new injVal;
	cache_get_value_name_int(0, "injured", injVal);
	gPlayerData[playerid][pInjured] = (injVal != 0);

	cache_get_value_name_int(0, "language", gPlayerData[playerid][pLanguage]);
	if (gPlayerData[playerid][pLanguage] != LANG_EN) gPlayerData[playerid][pLanguage] = LANG_ID;

	cache_get_value_name_int(0, "tickets", gPlayerData[playerid][pTickets]);
	cache_get_value_name_int(0, "ticket_amount", gPlayerData[playerid][pTicketAmount]);
	new licVal = 0;
	cache_get_value_name_int(0, "drive_lic", licVal);
	gPlayerData[playerid][pDriveLic] = (licVal != 0);
	new gLicVal = 0;
	cache_get_value_name_int(0, "gun_lic", gLicVal);
	gPlayerData[playerid][pGunLic] = (gLicVal != 0);

	cache_get_value_name_int(0, "fight_style", gPlayerData[playerid][pFightStyle]);
	if (gPlayerData[playerid][pFightStyle] < 4 || gPlayerData[playerid][pFightStyle] > 16) {
		gPlayerData[playerid][pFightStyle] = 4; // FIGHT_STYLE_NORMAL
	}
	SetPlayerFightingStyle(playerid, FIGHT_STYLE:gPlayerData[playerid][pFightStyle]);

	if (gPlayerData[playerid][pLevel] < 1) gPlayerData[playerid][pLevel] = 1;
	if (gPlayerData[playerid][pHunger] < 0.0) gPlayerData[playerid][pHunger] = 100.0;
	if (gPlayerData[playerid][pThirst] < 0.0) gPlayerData[playerid][pThirst] = 100.0;

	// set player nickname in samp to the selected character name
	SetPlayerName(playerid, gPlayerData[playerid][pName]);

	// give starter items if new
	if (gPlayerData[playerid][pPhone] == 0) {
		gPlayerData[playerid][pPhone] = 500000 + random(499999);
		gPlayerData[playerid][pPhoneStatus] = true;
		GivePlayerItem(playerid, ITEM_SNACK, 2);
		GivePlayerItem(playerid, ITEM_WATER, 2);
		GivePlayerItem(playerid, ITEM_PHONE, 1);
		GivePlayerItem(playerid, ITEM_GPS, 1);
	}

	gPlayerData[playerid][pLoggedIn] = true;
	gPlayerData[playerid][pSpawned] = false;

	// PENTING: pInjured/pInjuredTime tidak disimpan permanen ke DB.
	// Status pingsan adalah state sesi — timer penyembuh (OnEmergencyHospitalRespawn)
	// dan TogglePlayerControllable(false) hilang saat disconnect, sehingga menyimpan
	// pInjured=1 akan membuat pemain terkunci permanen tanpa jalan sembuh setelah relog.
	gPlayerData[playerid][pInjured] = false;
	gPlayerData[playerid][pInjuredTime] = 0;

	new Float:sx = gPlayerData[playerid][pLastX];
	new Float:sy = gPlayerData[playerid][pLastY];
	new Float:sz = gPlayerData[playerid][pLastZ];
	new Float:sa = gPlayerData[playerid][pLastA];
	if (sx == 0.0 && sy == 0.0 && sz == 0.0) {
		sx = SPAWN_X; sy = SPAWN_Y; sz = SPAWN_Z; sa = SPAWN_A;
	}

	// Siapkan info spawn sebelum mematikan mode spectate agar client langsung spawn
	SetSpawnInfo(playerid, NO_TEAM, gPlayerData[playerid][pSkin], sx, sy, sz, sa);
	TogglePlayerSpectating(playerid, false);
	SpawnPlayer(playerid);
	SetTimerEx("RP_AutoSpawnPlayer", 200, false, "i", playerid);
	return 1;
}

// save character data
stock DB_SavePlayerData(playerid)
{
	if (!gDBReady || !IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;

	new Float:px, Float:py, Float:pz, Float:pa;
	GetPlayerPos(playerid, px, py, pz);
	GetPlayerFacingAngle(playerid, pa);

	new query[1200];
	mysql_format(gDB, query, sizeof(query),
		"UPDATE characters SET money=%d, bank=%d, level=%d, exp=%d, admin=%d, \
		faction=%d, factionrank=%d, duty=%d, job=%d, hunger=%.2f, thirst=%.2f, \
		skin=%d, lastx=%.2f, lasty=%.2f, lastz=%.2f, lasta=%.2f, interior=%d, \
		vworld=%d, cuffed=%d, warnings=%d, kills=%d, deaths=%d, playhours=%d, muted=%d, \
		phone=%d, phonestatus=%d, radiofreq=%d, radiostatus=%d, injured=%d, language=%d, \
		tickets=%d, ticket_amount=%d, drive_lic=%d, gun_lic=%d, fight_style=%d \
		WHERE id=%d",
		gPlayerData[playerid][pMoney], gPlayerData[playerid][pBank],
		gPlayerData[playerid][pLevel], gPlayerData[playerid][pExp],
		gPlayerData[playerid][pAdmin], gPlayerData[playerid][pFaction],
		gPlayerData[playerid][pFactionRank], _:gPlayerData[playerid][pDuty],
		gPlayerData[playerid][pJob], gPlayerData[playerid][pHunger], gPlayerData[playerid][pThirst],
		gPlayerData[playerid][pSkin], px, py, pz, pa,
		GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid),
		_:gPlayerData[playerid][pCuffed], gPlayerData[playerid][pWarnings],
		gPlayerData[playerid][pKills], gPlayerData[playerid][pDeaths],
		gPlayerData[playerid][pPlayHours], _:gPlayerData[playerid][pMuted],
		gPlayerData[playerid][pPhone], _:gPlayerData[playerid][pPhoneStatus],
		gPlayerData[playerid][pRadioFreq], _:gPlayerData[playerid][pRadioStatus],
		_:gPlayerData[playerid][pInjured], gPlayerData[playerid][pLanguage],
		gPlayerData[playerid][pTickets], gPlayerData[playerid][pTicketAmount],
		_:gPlayerData[playerid][pDriveLic], _:gPlayerData[playerid][pGunLic],
		gPlayerData[playerid][pFightStyle], gPlayerData[playerid][pID]);

	mysql_tquery(gDB, query, "", "");
	return 1;
}

forward OnVehicleRowCreated(ownerid, slot, const plate[]);

// vehicles loader & saver
public OnVehiclesLoaded()
{
	new rows = cache_num_rows();
	gVehicleCount = 0;

	for (new i = 0; i < rows; i++) {
		if (gVehicleCount >= MAX_OWNED_VEHICLES) break;
		new slot = gVehicleCount;

		gVehicleData[slot][vUsed] = true;
		cache_get_value_name_int(i, "id", gVehicleData[slot][vDBID]);
		cache_get_value_name_int(i, "ownerid", gVehicleData[slot][vOwnerID]);
		cache_get_value_name_int(i, "model", gVehicleData[slot][vModel]);
		cache_get_value_name_float(i, "posx", gVehicleData[slot][vPosX]);
		cache_get_value_name_float(i, "posy", gVehicleData[slot][vPosY]);
		cache_get_value_name_float(i, "posz", gVehicleData[slot][vPosZ]);
		cache_get_value_name_float(i, "posa", gVehicleData[slot][vPosA]);
		cache_get_value_name_int(i, "color1", gVehicleData[slot][vColor1]);
		cache_get_value_name_int(i, "color2", gVehicleData[slot][vColor2]);
		cache_get_value_name_float(i, "fuel", gVehicleData[slot][vFuel]);
		cache_get_value_name_float(i, "health", gVehicleData[slot][vHealth]);
		cache_get_value_name(i, "plate", gVehicleData[slot][vPlate], 16);

		new engVal, lockVal;
		cache_get_value_name_int(i, "engine", engVal);
		cache_get_value_name_int(i, "locked", lockVal);
		gVehicleData[slot][vEngine] = (engVal != 0);
		gVehicleData[slot][vLocked] = (lockVal != 0);

		cache_get_value_name_int(i, "trunk", gVehicleData[slot][vTrunk]);
		cache_get_value_name_int(i, "trunk_gun", _:gVehicleData[slot][vTrunkGun]);
		cache_get_value_name_int(i, "trunk_ammo", gVehicleData[slot][vTrunkAmmo]);
		new impVal = 0;
		cache_get_value_name_int(i, "impounded", impVal);
		gVehicleData[slot][vImpounded] = (impVal != 0);
		new trunkItemStr[128];
		cache_get_value_name(i, "trunk_items", trunkItemStr, sizeof(trunkItemStr));
		DeserializeTrunkItems(slot, trunkItemStr);
		gVehicleData[slot][vVehicleHandle] = -1;

		if (gVehicleData[slot][vHealth] < 300.0) gVehicleData[slot][vHealth] = 1000.0;
		gVehicleCount++;
	}

	printf("[MYSQL] %d kendaraan dimuat dari database MySQL.", gVehicleCount);
	Vehicle_SpawnAll();
	return 1;
}

stock SerializeTrunkItems(slot, output[], maxlen = sizeof(output))
{
	output[0] = '\0';
	new temp[32];
	for (new s = 0; s < MAX_TRUNK_SLOTS; s++) {
		if (gVehicleData[slot][vTrunkItemType][s] != ITEM_NONE && gVehicleData[slot][vTrunkItemAmount][s] > 0) {
			format(temp, sizeof(temp), "%d:%d,", gVehicleData[slot][vTrunkItemType][s], gVehicleData[slot][vTrunkItemAmount][s]);
			strcat(output, temp, maxlen);
		}
	}
	return 1;
}

stock DeserializeTrunkItems(slot, const input[])
{
	for (new s = 0; s < MAX_TRUNK_SLOTS; s++) {
		gVehicleData[slot][vTrunkItemType][s] = ITEM_NONE;
		gVehicleData[slot][vTrunkItemAmount][s] = 0;
	}
	if (input[0] == '\0') return 1;

	new curSlot = 0;
	new i = 0;
	while (input[i] != '\0' && curSlot < MAX_TRUNK_SLOTS) {
		new oldI = i;
		new type = 0, amt = 0;
		while (input[i] >= '0' && input[i] <= '9') {
			type = (type * 10) + (input[i] - '0');
			i++;
		}
		if (input[i] == ':') i++;
		while (input[i] >= '0' && input[i] <= '9') {
			amt = (amt * 10) + (input[i] - '0');
			i++;
		}
		if (input[i] == ',') i++;

		if (type > 0 && amt > 0) {
			gVehicleData[slot][vTrunkItemType][curSlot] = type;
			gVehicleData[slot][vTrunkItemAmount][curSlot] = amt;
			curSlot++;
		}
		if (i == oldI) i++;
	}
	return 1;
}

stock DB_SaveVehicle(slot)
{
	if (!gDBReady || slot < 0 || slot >= MAX_OWNED_VEHICLES || !gVehicleData[slot][vUsed]) return 0;
	// vDBID <= 0 berarti baris DB belum terbentuk (insert belum selesai).
	// Jangan menyimpan pakai WHERE id=0 / id=1 — itu menimpa baris kendaraan lain.
	if (gVehicleData[slot][vDBID] <= 0) return 0;

	new trunkItemStr[128];
	SerializeTrunkItems(slot, trunkItemStr, sizeof(trunkItemStr));

	new query[1024];
	mysql_format(gDB, query, sizeof(query),
		"UPDATE vehicles SET posx=%.2f, posy=%.2f, posz=%.2f, posa=%.2f, \
		color1=%d, color2=%d, fuel=%.2f, health=%.2f, plate='%e', engine=%d, locked=%d, trunk=%d, trunk_items='%e', \
		trunk_gun=%d, trunk_ammo=%d, impounded=%d WHERE id=%d",
		gVehicleData[slot][vPosX], gVehicleData[slot][vPosY],
		gVehicleData[slot][vPosZ], gVehicleData[slot][vPosA],
		gVehicleData[slot][vColor1], gVehicleData[slot][vColor2],
		gVehicleData[slot][vFuel], gVehicleData[slot][vHealth],
		gVehicleData[slot][vPlate], _:gVehicleData[slot][vEngine],
		_:gVehicleData[slot][vLocked], gVehicleData[slot][vTrunk],
		trunkItemStr, gVehicleData[slot][vTrunkGun], gVehicleData[slot][vTrunkAmmo],
		_:gVehicleData[slot][vImpounded], gVehicleData[slot][vDBID]);

	mysql_tquery(gDB, query, "", "");
	return 1;
}

stock DB_CreateVehicleRow(ownerid, model, Float:x, Float:y, Float:z, Float:a, color1, color2, const plate[], slot = -1)
{
	if (!gDBReady) return 0;
	new query[320];
	mysql_format(gDB, query, sizeof(query),
		"INSERT INTO vehicles (ownerid, model, posx, posy, posz, posa, color1, color2, fuel, health, plate) \
		VALUES (%d, %d, %.2f, %.2f, %.2f, %.2f, %d, %d, 100.0, 1000.0, '%e')",
		ownerid, model, x, y, z, a, color1, color2, plate);
	// Ambil AUTO_INCREMENT id yang sebenarnya; jika slot disertakan, set vDBID
	// setelah insert supaya semua mobil TIDAK menunjuk ke id yang sama.
	mysql_tquery(gDB, query, "OnVehicleRowCreated", "iis", ownerid, slot, plate);
	return 1;
}

public OnVehicleRowCreated(ownerid, slot, const plate[])
{
	if (slot < 0 || slot >= MAX_OWNED_VEHICLES) return 0;

	// Pastikan slot masih dipakai dan pemiliknya sama (mencegah race saat slot dipakai ulang)
	if (!gVehicleData[slot][vUsed] || gVehicleData[slot][vOwnerID] != ownerid) return 0;

	gVehicleData[slot][vDBID] = cache_insert_id();

	// Verifikasi plat nomor tetap sama (kalau slot sudah dipakai pemain lain, jangan timpa)
	if (strcmp(gVehicleData[slot][vPlate], plate) != 0) return 0;

	DB_SaveVehicle(slot);
	return 1;
}

forward OnBansLoaded();

// bans
// Cache ban dimuat sekali saat init; pengecekan di OnPlayerConnect harus sinkron,
// jadi kita tidak bisa query DB di dalamnya. Data di-refresh saat server start.
#define MAX_BAN_CACHE 500
new gBanName[MAX_BAN_CACHE][MAX_PLAYER_NAME];
new gBanIP[MAX_BAN_CACHE][20];
new gBanCount = 0;

public OnBansLoaded()
{
	new rows = cache_num_rows();
	gBanCount = 0;
	new nameBuf[MAX_PLAYER_NAME], ipBuf[20];
	for (new i = 0; i < rows && i < MAX_BAN_CACHE; i++) {
		cache_get_value_name(i, "name", nameBuf, sizeof(nameBuf));
		cache_get_value_name(i, "ip", ipBuf, sizeof(ipBuf));
		format(gBanName[gBanCount], MAX_PLAYER_NAME, "%s", nameBuf);
		format(gBanIP[gBanCount], 20, "%s", ipBuf);
		gBanCount++;
	}
	printf("[BANS] Memuat %d entri ban ke cache memori.", gBanCount);
	return 1;
}

// Pengecekan sinkron: nama (case-insensitive) ATAU IP harus cocok.
stock DB_IsBanned(const name[], const ip[])
{
	if (name[0] == '\0' && ip[0] == '\0') return 0;
	for (new i = 0; i < gBanCount; i++) {
		if (name[0] != '\0' && gBanName[i][0] != '\0' && strcmp(gBanName[i], name, true) == 0) return 1;
		if (ip[0] != '\0' && gBanIP[i][0] != '\0' && strcmp(gBanIP[i], ip, true) == 0) return 1;
	}
	return 0;
}

stock DB_RemoveBan(const name[])
{
	if (!gDBReady || name[0] == '\0') return 0;
	new query[160];
	mysql_format(gDB, query, sizeof(query), "DELETE FROM bans WHERE name = '%e'", name);
	mysql_tquery(gDB, query, "", "");

	// Sinkronkan cache memori
	for (new i = 0; i < gBanCount; i++) {
		if (strcmp(gBanName[i], name, true) == 0) {
			for (new j = i; j < gBanCount - 1; j++) {
				format(gBanName[j], MAX_PLAYER_NAME, "%s", gBanName[j + 1]);
				format(gBanIP[j], 20, "%s", gBanIP[j + 1]);
			}
			gBanCount--;
			i--;
		}
	}
	return 1;
}

stock DB_AddBan(const name[], const ip[], const reason[], const admin[])
{
	if (!gDBReady) return 0;
	new query[320];
	mysql_format(gDB, query, sizeof(query),
		"INSERT INTO bans (name, ip, reason, admin, created) VALUES ('%e', '%e', '%e', '%e', %d)",
		name, ip, reason, admin, gettime());
	mysql_tquery(gDB, query, "", "");

	// Tambahkan ke cache memori agar langsung berlaku tanpa restart
	if (gBanCount < MAX_BAN_CACHE) {
		format(gBanName[gBanCount], MAX_PLAYER_NAME, "%s", name);
		format(gBanIP[gBanCount], 20, "%s", ip);
		gBanCount++;
	}
	return 1;
}
