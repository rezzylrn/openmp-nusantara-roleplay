#if defined _INC_rp_database
	#endinput
#endif
#define _INC_rp_database

// db connection config
#define MYSQL_HOST     "127.0.0.1"
#define MYSQL_USER     "samp"
#define MYSQL_PASS     "samp123"
#define MYSQL_DB       "roleplay"

forward OnAccountCheck(playerid);
forward OnAccountCreated(playerid);
forward OnAccountLoaded(playerid);
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

	// create tables
	mysql_tquery(gDB, "CREATE TABLE IF NOT EXISTS players (\
		id INT AUTO_INCREMENT PRIMARY KEY, \
		name VARCHAR(24) NOT NULL UNIQUE, \
		password VARCHAR(64) NOT NULL, \
		salt VARCHAR(64) NOT NULL, \
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
		injured INT DEFAULT 0)");

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
		trunk INT DEFAULT 0)");

	mysql_tquery(gDB, "CREATE TABLE IF NOT EXISTS bans (\
		id INT AUTO_INCREMENT PRIMARY KEY, \
		name VARCHAR(24) NOT NULL, \
		ip VARCHAR(20) NOT NULL, \
		reason VARCHAR(96), \
		admin VARCHAR(24), \
		created INT)");

	// load vehicles
	mysql_tquery(gDB, "SELECT * FROM vehicles", "OnVehiclesLoaded", "");
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

// account check on connect
stock DB_CheckAccount(playerid)
{
	if (!gDBReady) return 0;
	new query[128];
	mysql_format(gDB, query, sizeof(query), "SELECT id, password, salt FROM players WHERE name = '%e' LIMIT 1", gPlayerData[playerid][pName]);
	mysql_tquery(gDB, query, "OnAccountCheck", "i", playerid);
	return 1;
}

public OnAccountCheck(playerid)
{
	if (!IsPlayerConnected(playerid)) return 0;
	new rows = cache_num_rows();

	if (rows > 0) {
		cache_get_value_name_int(0, "id", gPlayerData[playerid][pID]);
		cache_get_value_name(0, "password", gPlayerData[playerid][pPassword], 65);
		cache_get_value_name(0, "salt", gPlayerData[playerid][pSalt], 65);
		ShowLoginDialog(playerid);
	} else {
		ShowRegisterDialog(playerid);
	}
	return 1;
}

// register
stock DB_RegisterAccount(playerid, const hash[], const salt[])
{
	if (!gDBReady) return 0;
	new query[384];
	mysql_format(gDB, query, sizeof(query),
		"INSERT INTO players (name, password, salt, money, bank, level, skin) \
		VALUES ('%e', '%e', '%e', %d, %d, %d, %d)",
		gPlayerData[playerid][pName], hash, salt, START_MONEY, START_BANK, START_LEVEL, START_SKIN);
	mysql_tquery(gDB, query, "OnAccountCreated", "i", playerid);
	return 1;
}

public OnAccountCreated(playerid)
{
	if (!IsPlayerConnected(playerid)) return 0;
	gPlayerData[playerid][pID] = cache_insert_id();
	SpawnPlayerEx(playerid);
	SendSucc(playerid, "Akun berhasil didaftarkan di MySQL. Selamat bermain!");
	return 1;
}

// load player data
stock DB_LoadPlayerData(playerid)
{
	if (!gDBReady) return 0;
	new query[128];
	mysql_format(gDB, query, sizeof(query), "SELECT * FROM players WHERE id = %d LIMIT 1", gPlayerData[playerid][pID]);
	mysql_tquery(gDB, query, "OnAccountLoaded", "i", playerid);
	return 1;
}

public OnAccountLoaded(playerid)
{
	if (!IsPlayerConnected(playerid)) return 0;
	if (cache_num_rows() <= 0) return 0;

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

	if (gPlayerData[playerid][pLevel] < 1) gPlayerData[playerid][pLevel] = 1;
	if (gPlayerData[playerid][pHunger] < 0.0) gPlayerData[playerid][pHunger] = 100.0;
	if (gPlayerData[playerid][pThirst] < 0.0) gPlayerData[playerid][pThirst] = 100.0;

	SpawnPlayerEx(playerid);
	return 1;
}

// save player data
stock DB_SavePlayerData(playerid)
{
	if (!gDBReady || !IsPlayerConnected(playerid) || !gPlayerData[playerid][pLoggedIn]) return 0;

	new Float:px, Float:py, Float:pz, Float:pa;
	GetPlayerPos(playerid, px, py, pz);
	GetPlayerFacingAngle(playerid, pa);

	new query[650];
	mysql_format(gDB, query, sizeof(query),
		"UPDATE players SET money=%d, bank=%d, level=%d, exp=%d, admin=%d, \
		faction=%d, factionrank=%d, duty=%d, job=%d, hunger=%.2f, thirst=%.2f, \
		skin=%d, lastx=%.2f, lasty=%.2f, lastz=%.2f, lasta=%.2f, interior=%d, \
		vworld=%d, cuffed=%d, warnings=%d, kills=%d, deaths=%d, playhours=%d, muted=%d, \
		phone=%d, phonestatus=%d, radiofreq=%d, radiostatus=%d, injured=%d \
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
		_:gPlayerData[playerid][pInjured], gPlayerData[playerid][pID]);

	mysql_tquery(gDB, query, "", "");
	return 1;
}

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
		gVehicleData[slot][vVehicleHandle] = -1;

		if (gVehicleData[slot][vHealth] < 300.0) gVehicleData[slot][vHealth] = 1000.0;
		gVehicleCount++;
	}

	printf("[MYSQL] %d kendaraan dimuat dari database MySQL.", gVehicleCount);
	Vehicle_SpawnAll();
	return 1;
}

stock DB_SaveVehicle(slot)
{
	if (!gDBReady || slot < 0 || slot >= MAX_OWNED_VEHICLES || !gVehicleData[slot][vUsed]) return 0;

	new query[450];
	mysql_format(gDB, query, sizeof(query),
		"UPDATE vehicles SET posx=%.2f, posy=%.2f, posz=%.2f, posa=%.2f, \
		color1=%d, color2=%d, fuel=%.2f, health=%.2f, plate='%e', engine=%d, locked=%d, trunk=%d WHERE id=%d",
		gVehicleData[slot][vPosX], gVehicleData[slot][vPosY],
		gVehicleData[slot][vPosZ], gVehicleData[slot][vPosA],
		gVehicleData[slot][vColor1], gVehicleData[slot][vColor2],
		gVehicleData[slot][vFuel], gVehicleData[slot][vHealth],
		gVehicleData[slot][vPlate], _:gVehicleData[slot][vEngine],
		_:gVehicleData[slot][vLocked], gVehicleData[slot][vTrunk],
		gVehicleData[slot][vDBID]);

	mysql_tquery(gDB, query, "", "");
	return 1;
}

stock DB_CreateVehicleRow(ownerid, model, Float:x, Float:y, Float:z, Float:a, color1, color2, const plate[])
{
	if (!gDBReady) return -1;
	new query[320];
	mysql_format(gDB, query, sizeof(query),
		"INSERT INTO vehicles (ownerid, model, posx, posy, posz, posa, color1, color2, fuel, health, plate) \
		VALUES (%d, %d, %.2f, %.2f, %.2f, %.2f, %d, %d, 100.0, 1000.0, '%e')",
		ownerid, model, x, y, z, a, color1, color2, plate);
	mysql_tquery(gDB, query, "", "");
	return 1;
}

// bans
stock DB_IsBanned(const name[], const ip[])
{
	#pragma unused name, ip
	return 0;
}

stock DB_AddBan(const name[], const ip[], const reason[], const admin[])
{
	if (!gDBReady) return 0;
	new query[320];
	mysql_format(gDB, query, sizeof(query),
		"INSERT INTO bans (name, ip, reason, admin, created) VALUES ('%e', '%e', '%e', '%e', %d)",
		name, ip, reason, admin, gettime());
	mysql_tquery(gDB, query, "", "");
	return 1;
}
