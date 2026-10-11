#if defined _INC_rp_weather
	#endinput
#endif
#define _INC_rp_weather

/*
==============================================================================
 weather_system.inc
 NUSANTARA ROLEPLAY - Dynamic World Weather Cycle & Forecast Broadcast System
==============================================================================
*/

static const gWeatherIDs[] = {1, 2, 8, 9, 16};
static const gWeatherNames[][] = {
	"Cerah Berawan Segar",
	"Panas Terik Tropis",
	"Hujan Gerimis Sejuk",
	"Berkabut Lembut",
	"Hujan Badai & Kilat"
};

new gCurrentWeatherIndex = 0;
new gCurrentWorldHour = 12;

forward WeatherCycle_Tick();

stock WeatherSystem_Init()
{
	gCurrentWeatherIndex = 0;
	gCurrentWorldHour = 12;

	SetWeather(gWeatherIDs[gCurrentWeatherIndex]);
	SetWorldTime(gCurrentWorldHour);

	SetTimer("WeatherCycle_Tick", 600000, true); // Berganti tiap 10 Menit

	print("[WEATHER] Sistem siklus cuaca kota & waktu dinamis Los Santos aktif.");
	return 1;
}

public WeatherCycle_Tick()
{
	gCurrentWeatherIndex = (gCurrentWeatherIndex + 1) % sizeof(gWeatherIDs);
	new newWeather = gWeatherIDs[gCurrentWeatherIndex];
	SetWeather(newWeather);

	// Putar jam dunia
	gCurrentWorldHour = (gCurrentWorldHour + 4) % 24;
	SetWorldTime(gCurrentWorldHour);

	new forecastMsg[160];
	format(forecastMsg, sizeof(forecastMsg),
		"[BMKG CUACA] Laporan Cuaca Los Santos: Kondisi langit saat ini '%s' (Pukul %02d:00).",
		gWeatherNames[gCurrentWeatherIndex], gCurrentWorldHour);

	for (new p = 0; p < MAX_PLAYERS; p++) {
		if (IsPlayerConnected(p) && gPlayerData[p][pLoggedIn]) {
			SendClientMessage(p, 0x33CCFFFF, forecastMsg);
		}
	}
	return 1;
}

stock GetCurrentWeatherName(dest[], len = sizeof(dest))
{
	format(dest, len, "%s", gWeatherNames[gCurrentWeatherIndex]);
	return 1;
}
