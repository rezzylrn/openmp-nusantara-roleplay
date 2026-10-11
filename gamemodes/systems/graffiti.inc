#if defined _INC_rp_graffiti
	#endinput
#endif
#define _INC_rp_graffiti

/*
==============================================================================
 graffiti.inc
 NUSANTARA ROLEPLAY - Street Graffiti Tagging & Municipal Cleaning System
==============================================================================
*/

#define MAX_GRAFFITI_TAGS       6

enum E_GRAFFITI_SPOT {
	Float:gfX,
	Float:gfY,
	Float:gfZ,
	gfName[32],
	gfText[48],
	Text3D:gfLabel
};

new gGraffitiSpots[MAX_GRAFFITI_TAGS][E_GRAFFITI_SPOT] = {
	{2485.0, -1665.0, 13.5, "Ganton Alleyway",       "GROVE STREET FAMILIES", Text3D:INVALID_3DTEXT_ID},
	{2040.0, -1700.0, 13.5, "Idlewood Motel Corner",  "ROLLIN HEIGHTS BALLAS", Text3D:INVALID_3DTEXT_ID},
	{2420.0, -1220.0, 24.5, "East LS Barrio",         "VAGOS NORTENOS",        Text3D:INVALID_3DTEXT_ID},
	{1980.0, -1190.0, 20.0, "Glen Park Overpass",     "KLAUS UNDERGROUND",     Text3D:INVALID_3DTEXT_ID},
	{830.0,  -1960.0, 12.8, "Verona Boardwalk Wall",  "NUSANTARA STREET RP",   Text3D:INVALID_3DTEXT_ID},
	{1760.0, -2040.0, 13.5, "El Corona Rail Siding",  "AZTECAS CALLE SUR",     Text3D:INVALID_3DTEXT_ID}
};

stock Graffiti_Init()
{
	for (new i = 0; i < MAX_GRAFFITI_TAGS; i++) {
		new lbl[160];
		format(lbl, sizeof(lbl),
			"{FFCC00}[ GRAFITI JALANAN ]\n{FFFFFF}\"%s\"\n{888888}Lokasi: %s\n{FFFF00}Gunakan /tag [teks] atau /cleantag",
			gGraffitiSpots[i][gfText], gGraffitiSpots[i][gfName]);

		gGraffitiSpots[i][gfLabel] = Create3DTextLabel(lbl, 0xFFCC00FF,
			gGraffitiSpots[i][gfX], gGraffitiSpots[i][gfY], gGraffitiSpots[i][gfZ] + 0.8,
			15.0, 0, true);
	}

	print("[GRAFFITI] 6 Titik dinding grafiti & semprot cat Los Santos aktif.");
	return 1;
}

stock ProcessTagGraffiti(playerid, const tagText[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;

	if (GetPlayerItemCount(playerid, ITEM_SPRAYCAN) <= 0) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda memerlukan Kaleng Cat Semprot (Spraycan) di inventaris. Beli di toko 24/7.");
		return 0;
	}

	new spot = -1;
	for (new i = 0; i < MAX_GRAFFITI_TAGS; i++) {
		if (IsPlayerNearPoint(playerid, gGraffitiSpots[i][gfX], gGraffitiSpots[i][gfY], gGraffitiSpots[i][gfZ], 3.0)) {
			spot = i;
			break;
		}
	}

	if (spot == -1) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda tidak berada di dekat dinding grafiti kota (maks. 3 meter).");
		return 0;
	}

	if (strlen(tagText) < 2 || strlen(tagText) > 40) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Panjang teks grafiti harus antara 2 hingga 40 karakter.");
		return 0;
	}

	// Perbarui grafiti
	format(gGraffitiSpots[spot][gfText], 48, "%s", tagText);

	if (gGraffitiSpots[spot][gfLabel] != Text3D:INVALID_3DTEXT_ID) {
		Delete3DTextLabel(gGraffitiSpots[spot][gfLabel]);
	}

	new lbl[160];
	format(lbl, sizeof(lbl),
		"{FF3B30}[ GRAFITI WILAYAH ]\n{FFFFFF}\"%s\"\n{888888}Oleh: %s | Lokasi: %s\n{FFFF00}Gunakan /cleantag untuk membersihkan",
		tagText, gPlayerData[playerid][pName], gGraffitiSpots[spot][gfName]);

	gGraffitiSpots[spot][gfLabel] = Create3DTextLabel(lbl, 0xFF3B30FF,
		gGraffitiSpots[spot][gfX], gGraffitiSpots[spot][gfY], gGraffitiSpots[spot][gfZ] + 0.8,
		15.0, 0, true);

	// Animasi semprot cat
	ApplyAnimation(playerid, "GRAFFITI", "spraycan_fire", 4.1, false, false, false, false, 0);
	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);

	new succMsg[160];
	format(succMsg, sizeof(succMsg),
		"SUKSES: Dinding grafiti di %s berhasil disemprot dengan tulisan: \"%s\"!",
		gGraffitiSpots[spot][gfName], tagText);
	SendClientMessage(playerid, COLOR_SUCCESS, succMsg);
	return 1;
}

stock ProcessCleanGraffiti(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;

	new spot = -1;
	for (new i = 0; i < MAX_GRAFFITI_TAGS; i++) {
		if (IsPlayerNearPoint(playerid, gGraffitiSpots[i][gfX], gGraffitiSpots[i][gfY], gGraffitiSpots[i][gfZ], 3.0)) {
			spot = i;
			break;
		}
	}

	if (spot == -1) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda tidak berada di dekat dinding grafiti mana pun.");
		return 0;
	}

	format(gGraffitiSpots[spot][gfText], 48, "DINDING BERSIH");

	if (gGraffitiSpots[spot][gfLabel] != Text3D:INVALID_3DTEXT_ID) {
		Delete3DTextLabel(gGraffitiSpots[spot][gfLabel]);
	}

	new lbl[160];
	format(lbl, sizeof(lbl),
		"{888888}[ DINDING KOSONG ]\n{FFFFFF}Dinding telah dibersihkan rapi (%s)\n{FFFF00}Gunakan /tag [teks] untuk menggambar grafiti",
		gGraffitiSpots[spot][gfName]);

	gGraffitiSpots[spot][gfLabel] = Create3DTextLabel(lbl, 0x888888FF,
		gGraffitiSpots[spot][gfX], gGraffitiSpots[spot][gfY], gGraffitiSpots[spot][gfZ] + 0.8,
		15.0, 0, true);

	// Hadiah upah kebersihan dinas kota
	GivePlayerMoneyEx(playerid, 75, "upah membersihkan grafiti liar");
	PlayerPlaySound(playerid, 1058, 0.0, 0.0, 0.0);

	SendClientMessage(playerid, COLOR_SUCCESS,
		"SUKSES: Dinding grafiti berhasil dibersihkan rapi! Dinas Kota memberikan Anda upah $75.");
	return 1;
}
