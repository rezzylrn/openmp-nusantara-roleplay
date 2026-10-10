#if defined _INC_rp_database
	#endinput
#endif
#define _INC_rp_database

/*
==============================================================================
  NUSANTARA ROLEPLAY - database.inc
  Koneksi SQLite, pembuatan tabel, migrasi skema, simpan/muat data pemain & kendaraan
==============================================================================
*/

stock SQL_Escape(const src[], dest[], len = sizeof(dest))
{
	new j = 0;
	for (new i = 0; src[i] != '\0' && j < len - 1; i++) {
		if (src[i] == '\'') {
			if (j >= len - 2) break;
			dest[j++] = '\'';
			dest[j++] = '\'';
		} else {
			dest[j++] = src[i];
		}
	}
	dest[j] = '\0';
	return j;
}

// ---------------------------------------------------------------------------
// Inisialisasi Database
// ---------------------------------------------------------------------------
stock DB_Init()
{
	gDatabase = DB_Open("roleplay.db");
	if (!gDatabase) {
		print("[DB] GAGAL membuka roleplay.db!");
		gDatabaseReady = false;
		return 0;
	}
	gDatabaseReady = true;
	DB_CreateTables();
	DB_MigrateSchema();
	DB_LoadVehicles();
	printf("[DB] roleplay.db terhubung dan siap. Kendaraan aktif: %d", gVehicleCount);
	return 1;
}

stock DB_Shutdown()
{
	if (gDatabaseReady) {
		for (new i = 0; i < MAX_OWNED_VEHICLES; i++) {
			if (gVehicleData[i][vUsed]) DB_SaveVehicle(i);
		}
		DB_Close(gDatabase);
		gDatabaseReady = false;
	}
	return 1;
}

stock DB_CreateTables()
{
	if (!gDatabaseReady) return 0;

	DB_ExecuteQuery(gDatabase, "CREATE TABLE IF NOT EXISTS players (\
		id INTEGER PRIMARY KEY AUTOINCREMENT, \
		name TEXT NOT NULL UNIQUE, \
		password TEXT NOT NULL, \
		salt TEXT NOT NULL, \
		money INTEGER DEFAULT 500, \
		bank INTEGER DEFAULT 1000, \
		level INTEGER DEFAULT 1, \
		exp INTEGER DEFAULT 0, \
		admin INTEGER DEFAULT 0, \
		faction INTEGER DEFAULT 0, \
		factionrank INTEGER DEFAULT 0, \
		duty INTEGER DEFAULT 0, \
		job INTEGER DEFAULT 0, \
		hunger REAL DEFAULT 100.0, \
		thirst REAL DEFAULT 100.0, \
		skin INTEGER DEFAULT 0, \
		lastx REAL DEFAULT 1481.0, \
		lasty REAL DEFAULT -1748.0, \
		lastz REAL DEFAULT 13.5, \
		lasta REAL DEFAULT 270.0, \
		interior INTEGER DEFAULT 0, \
		vworld INTEGER DEFAULT 0, \
		cuffed INTEGER DEFAULT 0, \
		warnings INTEGER DEFAULT 0, \
		kills INTEGER DEFAULT 0, \
		deaths INTEGER DEFAULT 0, \
		playhours INTEGER DEFAULT 0, \
		muted INTEGER DEFAULT 0, \
		phone INTEGER DEFAULT 0, \
		phonestatus INTEGER DEFAULT 1, \
		radiofreq INTEGER DEFAULT 0, \
		radiostatus INTEGER DEFAULT 0, \
		injured INTEGER DEFAULT 0, \
		inventory TEXT DEFAULT '')");

	DB_ExecuteQuery(gDatabase, "CREATE TABLE IF NOT EXISTS vehicles (\
		id INTEGER PRIMARY KEY AUTOINCREMENT, \
		ownerid INTEGER NOT NULL, \
		model INTEGER NOT NULL, \
		posx REAL DEFAULT 0.0, \
		posy REAL DEFAULT 0.0, \
		posz REAL DEFAULT 0.0, \
		posa REAL DEFAULT 0.0, \
		color1 INTEGER DEFAULT 1, \
		color2 INTEGER DEFAULT 1, \
		fuel REAL DEFAULT 100.0, \
		health REAL DEFAULT 1000.0, \
		plate TEXT DEFAULT '', \
		engine INTEGER DEFAULT 0, \
		locked INTEGER DEFAULT 0, \
		trunk INTEGER DEFAULT 0)");

	DB_ExecuteQuery(gDatabase, "CREATE TABLE IF NOT EXISTS bans (\
		id INTEGER PRIMARY KEY AUTOINCREMENT, \
		name TEXT NOT NULL, \
		ip TEXT NOT NULL, \
		reason TEXT, \
		admin TEXT, \
		created INTEGER)");

	DB_ExecuteQuery(gDatabase, "CREATE TABLE IF NOT EXISTS settings (\
		key TEXT PRIMARY KEY, \
		value TEXT)");
	return 1;
}

stock DB_MigrateSchema()
{
	if (!gDatabaseReady) return 0;
	// Menambahkan kolom baru secara non-destructive jika belum ada
	DB_FreeResultSet(DB_ExecuteQuery(gDatabase, "ALTER TABLE players ADD COLUMN phone INTEGER DEFAULT 0"));
	DB_FreeResultSet(DB_ExecuteQuery(gDatabase, "ALTER TABLE players ADD COLUMN phonestatus INTEGER DEFAULT 1"));
	DB_FreeResultSet(DB_ExecuteQuery(gDatabase, "ALTER TABLE players ADD COLUMN radiofreq INTEGER DEFAULT 0"));
	DB_FreeResultSet(DB_ExecuteQuery(gDatabase, "ALTER TABLE players ADD COLUMN radiostatus INTEGER DEFAULT 0"));
	DB_FreeResultSet(DB_ExecuteQuery(gDatabase, "ALTER TABLE players ADD COLUMN injured INTEGER DEFAULT 0"));
	DB_FreeResultSet(DB_ExecuteQuery(gDatabase, "ALTER TABLE players ADD COLUMN inventory TEXT DEFAULT ''"));

	DB_FreeResultSet(DB_ExecuteQuery(gDatabase, "ALTER TABLE vehicles ADD COLUMN health REAL DEFAULT 1000.0"));
	DB_FreeResultSet(DB_ExecuteQuery(gDatabase, "ALTER TABLE vehicles ADD COLUMN plate TEXT DEFAULT ''"));
	return 1;
}

// ---------------------------------------------------------------------------
// Operasi Akun Pemain
// ---------------------------------------------------------------------------
stock DB_PlayerExists(const name[])
{
	if (!gDatabaseReady) return 0;
	new query[160], esc[MAX_PLAYER_NAME + 4];
	SQL_Escape(name, esc, sizeof(esc));
	format(query, sizeof(query), "SELECT id FROM players WHERE name = '%s' LIMIT 1", esc);
	new DBResult:r = DB_ExecuteQuery(gDatabase, query);
	new exists = DB_GetRowCount(r) > 0;
	DB_FreeResultSet(r);
	return exists;
}

stock DB_CreateAccount(const name[], const password[], const salt[])
{
	if (!gDatabaseReady) return 0;
	new query[512], escN[MAX_PLAYER_NAME + 4], escP[80], escS[80];
	SQL_Escape(name, escN, sizeof(escN));
	SQL_Escape(password, escP, sizeof(escP));
	SQL_Escape(salt, escS, sizeof(escS));
	format(query, sizeof(query),
		"INSERT INTO players (name, password, salt, money, bank, level, exp, skin) \
		VALUES ('%s', '%s', '%s', %d, %d, %d, %d, %d)",
		escN, escP, escS, START_MONEY, START_BANK, START_LEVEL, START_EXP, START_SKIN);
	new DBResult:r = DB_ExecuteQuery(gDatabase, query);
	new rowId = -1;
	if (r) {
		rowId = DB_GetInsertId();
		DB_FreeResultSet(r);
	}
	return rowId;
}

stock DB_GetInsertId()
{
	if (!gDatabaseReady) return -1;
	new DBResult:r = DB_ExecuteQuery(gDatabase, "SELECT last_insert_rowid()");
	new id = -1;
	if (DB_GetRowCount(r) > 0) id = DB_GetFieldInt(r, 0);
	DB_FreeResultSet(r);
	return id;
}

stock DB_FetchAccount(const name[], &playerid_row, password[], pwdLen, salt[], saltLen)
{
	if (!gDatabaseReady) return 0;
	new query[200], esc[MAX_PLAYER_NAME + 4];
	SQL_Escape(name, esc, sizeof(esc));
	format(query, sizeof(query), "SELECT id, password, salt FROM players WHERE name = '%s' LIMIT 1", esc);
	new DBResult:r = DB_ExecuteQuery(gDatabase, query);
	if (DB_GetRowCount(r) <= 0) {
		DB_FreeResultSet(r);
		return 0;
	}
	playerid_row = DB_GetFieldInt(r, 0);
	DB_GetFieldString(r, 1, password, pwdLen);
	DB_GetFieldString(r, 2, salt, saltLen);
	DB_FreeResultSet(r);
	return 1;
}

stock DB_LoadPlayerData(playerid)
{
	if (!gDatabaseReady || !IsPlayerConnected(playerid)) return 0;
	new name[MAX_PLAYER_NAME];
	GetPlayerName(playerid, name, sizeof(name));

	new query[220], esc[MAX_PLAYER_NAME + 4];
	SQL_Escape(name, esc, sizeof(esc));
	format(query, sizeof(query), "SELECT * FROM players WHERE name = '%s' LIMIT 1", esc);
	new DBResult:r = DB_ExecuteQuery(gDatabase, query);
	if (DB_GetRowCount(r) <= 0) {
		DB_FreeResultSet(r);
		return 0;
	}

	gPlayerData[playerid][pID] = DB_GetFieldIntByName(r, "id");
	format(gPlayerData[playerid][pName], MAX_PLAYER_NAME, "%s", name);
	gPlayerData[playerid][pMoney] = DB_GetFieldIntByName(r, "money");
	gPlayerData[playerid][pBank] = DB_GetFieldIntByName(r, "bank");
	gPlayerData[playerid][pLevel] = DB_GetFieldIntByName(r, "level");
	gPlayerData[playerid][pExp] = DB_GetFieldIntByName(r, "exp");
	gPlayerData[playerid][pAdmin] = DB_GetFieldIntByName(r, "admin");
	gPlayerData[playerid][pFaction] = DB_GetFieldIntByName(r, "faction");
	gPlayerData[playerid][pFactionRank] = DB_GetFieldIntByName(r, "factionrank");
	gPlayerData[playerid][pDuty] = DB_GetFieldIntByName(r, "duty") != 0;
	gPlayerData[playerid][pJob] = DB_GetFieldIntByName(r, "job");
	gPlayerData[playerid][pHunger] = DB_GetFieldFloatByName(r, "hunger");
	gPlayerData[playerid][pThirst] = DB_GetFieldFloatByName(r, "thirst");
	gPlayerData[playerid][pSkin] = DB_GetFieldIntByName(r, "skin");
	gPlayerData[playerid][pLastX] = DB_GetFieldFloatByName(r, "lastx");
	gPlayerData[playerid][pLastY] = DB_GetFieldFloatByName(r, "lasty");
	gPlayerData[playerid][pLastZ] = DB_GetFieldFloatByName(r, "lastz");
	gPlayerData[playerid][pLastA] = DB_GetFieldFloatByName(r, "lasta");
	gPlayerData[playerid][pInterior] = DB_GetFieldIntByName(r, "interior");
	gPlayerData[playerid][pVirtualWorld] = DB_GetFieldIntByName(r, "vworld");
	gPlayerData[playerid][pCuffed] = DB_GetFieldIntByName(r, "cuffed") != 0;
	gPlayerData[playerid][pWarnings] = DB_GetFieldIntByName(r, "warnings");
	gPlayerData[playerid][pKills] = DB_GetFieldIntByName(r, "kills");
	gPlayerData[playerid][pDeaths] = DB_GetFieldIntByName(r, "deaths");
	gPlayerData[playerid][pPlayHours] = DB_GetFieldIntByName(r, "playhours");
	gPlayerData[playerid][pMuted] = DB_GetFieldIntByName(r, "muted") != 0;
	gPlayerData[playerid][pPhone] = DB_GetFieldIntByName(r, "phone");
	gPlayerData[playerid][pPhoneStatus] = DB_GetFieldIntByName(r, "phonestatus") != 0;
	gPlayerData[playerid][pRadioFreq] = DB_GetFieldIntByName(r, "radiofreq");
	gPlayerData[playerid][pRadioStatus] = DB_GetFieldIntByName(r, "radiostatus") != 0;
	gPlayerData[playerid][pInjured] = DB_GetFieldIntByName(r, "injured") != 0;
	gPlayerData[playerid][pPaycheckTimer] = PAYCHECK_INTERVAL;

	DB_FreeResultSet(r);

	if (gPlayerData[playerid][pLevel] < 1) gPlayerData[playerid][pLevel] = 1;
	if (gPlayerData[playerid][pHunger] < 0.0) gPlayerData[playerid][pHunger] = 100.0;
	if (gPlayerData[playerid][pThirst] < 0.0) gPlayerData[playerid][pThirst] = 100.0;
	return 1;
}

stock DB_SavePlayerData(playerid)
{
	if (!gDatabaseReady || !IsPlayerConnected(playerid)) return 0;
	if (!gPlayerData[playerid][pLoggedIn]) return 0;

	new name[MAX_PLAYER_NAME];
	GetPlayerName(playerid, name, sizeof(name));
	new query[750], esc[MAX_PLAYER_NAME + 4];
	SQL_Escape(name, esc, sizeof(esc));

	new Float:px, Float:py, Float:pz, Float:pa;
	GetPlayerPos(playerid, px, py, pz);
	GetPlayerFacingAngle(playerid, pa);

	format(query, sizeof(query),
		"UPDATE players SET money=%d, bank=%d, level=%d, exp=%d, admin=%d, \
		faction=%d, factionrank=%d, duty=%d, job=%d, hunger=%.2f, thirst=%.2f, \
		skin=%d, lastx=%.2f, lasty=%.2f, lastz=%.2f, lasta=%.2f, interior=%d, \
		vworld=%d, cuffed=%d, warnings=%d, kills=%d, deaths=%d, playhours=%d, muted=%d, \
		phone=%d, phonestatus=%d, radiofreq=%d, radiostatus=%d, injured=%d \
		WHERE name = '%s'",
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
		_:gPlayerData[playerid][pInjured], esc);

	new DBResult:r = DB_ExecuteQuery(gDatabase, query);
	if (r) DB_FreeResultSet(r);
	return 1;
}

// ---------------------------------------------------------------------------
// Kendaraan
// ---------------------------------------------------------------------------
stock DB_LoadVehicles()
{
	gVehicleCount = 0;
	if (!gDatabaseReady) return 0;
	new DBResult:r = DB_ExecuteQuery(gDatabase, "SELECT * FROM vehicles");
	if (!r) return 0;

	new rows = DB_GetRowCount(r);
	for (new i = 0; i < rows; i++) {
		if (!DB_SelectNextRow(r)) break;
		if (gVehicleCount >= MAX_OWNED_VEHICLES) break;

		new slot = gVehicleCount;
		gVehicleData[slot][vUsed] = true;
		gVehicleData[slot][vDBID] = DB_GetFieldIntByName(r, "id");
		gVehicleData[slot][vOwnerID] = DB_GetFieldIntByName(r, "ownerid");
		gVehicleData[slot][vModel] = DB_GetFieldIntByName(r, "model");
		gVehicleData[slot][vPosX] = DB_GetFieldFloatByName(r, "posx");
		gVehicleData[slot][vPosY] = DB_GetFieldFloatByName(r, "posy");
		gVehicleData[slot][vPosZ] = DB_GetFieldFloatByName(r, "posz");
		gVehicleData[slot][vPosA] = DB_GetFieldFloatByName(r, "posa");
		gVehicleData[slot][vColor1] = DB_GetFieldIntByName(r, "color1");
		gVehicleData[slot][vColor2] = DB_GetFieldIntByName(r, "color2");
		gVehicleData[slot][vFuel] = DB_GetFieldFloatByName(r, "fuel");
		gVehicleData[slot][vHealth] = DB_GetFieldFloatByName(r, "health");
		DB_GetFieldStringByName(r, "plate", gVehicleData[slot][vPlate], 16);
		gVehicleData[slot][vEngine] = DB_GetFieldIntByName(r, "engine") != 0;
		gVehicleData[slot][vLocked] = DB_GetFieldIntByName(r, "locked") != 0;
		gVehicleData[slot][vTrunk] = DB_GetFieldIntByName(r, "trunk");
		gVehicleData[slot][vVehicleHandle] = -1;

		if (gVehicleData[slot][vHealth] < 300.0) gVehicleData[slot][vHealth] = 1000.0;
		gVehicleCount++;
	}
	DB_FreeResultSet(r);
	return gVehicleCount;
}

stock DB_SaveVehicle(slot)
{
	if (!gDatabaseReady || slot < 0 || slot >= MAX_OWNED_VEHICLES || !gVehicleData[slot][vUsed]) return 0;

	new query[500];
	format(query, sizeof(query),
		"UPDATE vehicles SET posx=%.2f, posy=%.2f, posz=%.2f, posa=%.2f, \
		color1=%d, color2=%d, fuel=%.2f, health=%.2f, plate='%s', engine=%d, locked=%d, trunk=%d WHERE id=%d",
		gVehicleData[slot][vPosX], gVehicleData[slot][vPosY],
		gVehicleData[slot][vPosZ], gVehicleData[slot][vPosA],
		gVehicleData[slot][vColor1], gVehicleData[slot][vColor2],
		gVehicleData[slot][vFuel], gVehicleData[slot][vHealth],
		gVehicleData[slot][vPlate], _:gVehicleData[slot][vEngine],
		_:gVehicleData[slot][vLocked], gVehicleData[slot][vTrunk],
		gVehicleData[slot][vDBID]);

	new DBResult:r = DB_ExecuteQuery(gDatabase, query);
	if (r) DB_FreeResultSet(r);
	return 1;
}

stock DB_CreateVehicleRow(ownerid, model, Float:x, Float:y, Float:z, Float:a, color1, color2)
{
	if (!gDatabaseReady) return -1;
	new query[300];
	format(query, sizeof(query),
		"INSERT INTO vehicles (ownerid, model, posx, posy, posz, posa, color1, color2, fuel, health) \
		VALUES (%d, %d, %.2f, %.2f, %.2f, %.2f, %d, %d, 100.0, 1000.0)",
		ownerid, model, x, y, z, a, color1, color2);
	new DBResult:r = DB_ExecuteQuery(gDatabase, query);
	new rowId = -1;
	if (r) {
		rowId = DB_GetInsertId();
		DB_FreeResultSet(r);
	}
	return rowId;
}

// ---------------------------------------------------------------------------
// Ban & Keamanan
// ---------------------------------------------------------------------------
stock DB_IsBanned(const name[], const ip[])
{
	if (!gDatabaseReady) return 0;
	new query[256], escN[MAX_PLAYER_NAME + 4], escI[20];
	SQL_Escape(name, escN, sizeof(escN));
	SQL_Escape(ip, escI, sizeof(escI));
	format(query, sizeof(query),
		"SELECT id FROM bans WHERE name='%s' OR ip='%s' LIMIT 1", escN, escI);
	new DBResult:r = DB_ExecuteQuery(gDatabase, query);
	new banned = DB_GetRowCount(r) > 0;
	DB_FreeResultSet(r);
	return banned;
}

stock DB_AddBan(const name[], const ip[], const reason[], const admin[])
{
	if (!gDatabaseReady) return 0;
	new query[400], escN[MAX_PLAYER_NAME + 4], escI[20], escR[96], escA[MAX_PLAYER_NAME + 4];
	SQL_Escape(name, escN, sizeof(escN));
	SQL_Escape(ip, escI, sizeof(escI));
	SQL_Escape(reason, escR, sizeof(escR));
	SQL_Escape(admin, escA, sizeof(escA));
	format(query, sizeof(query),
		"INSERT INTO bans (name, ip, reason, admin, created) VALUES ('%s', '%s', '%s', '%s', %d)",
		escN, escI, escR, escA, gettime());
	new DBResult:r = DB_ExecuteQuery(gDatabase, query);
	if (r) DB_FreeResultSet(r);
	return 1;
}
