#if defined _INC_rp_gym_fitness
	#endinput
#endif
#define _INC_rp_gym_fitness

/*
==============================================================================
 gym_fitness.inc
 NUSANTARA ROLEPLAY - Fitness Gym Workouts & Custom Martial Arts Fight Styles
==============================================================================
*/

#define GYM_TRAINING_FEE        150
#define BEACH_GYM_X             768.0
#define BEACH_GYM_Y             -1865.0
#define BEACH_GYM_Z             5.5

stock GymFitness_Init()
{
	CreatePickup(1239, 1, BEACH_GYM_X, BEACH_GYM_Y, BEACH_GYM_Z, 0);
	Create3DTextLabel(
		"{FFCC00}[ PUSAT KEBUGARAN GYM & BELA DIRI ]\n{FFFFFF}Latihan fisik & pelatih gaya bertarung\n{FFFF00}Gunakan /gym atau /fightstyle ($150)",
		0xFFCC00FF, BEACH_GYM_X, BEACH_GYM_Y, BEACH_GYM_Z + 0.8, 15.0, 0, true);

	print("[GYM] Pusat kebugaran fisik & martial arts fight styles aktif.");
	return 1;
}

stock ShowGymStyleDialog(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;

	if (!IsPlayerNearPoint(playerid, BEACH_GYM_X, BEACH_GYM_Y, BEACH_GYM_Z, 12.0)) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus berada di Pusat Gym Pantai Santa Maria atau Gym Ganton (/gps).");
		return 0;
	}

	new dialogContent[380];
	format(dialogContent, sizeof(dialogContent),
		"Gaya Bela Diri\tBiaya\tKarakteristik Bertarung\n\
{FFFFFF}1. Gaya Standar Biasa\t{4CD964}Gratis\tPukulan jalanan dasar\n\
{FFFFFF}2. Tinju Klasik (Boxing)\t{4CD964}$150\tKombinasi jab & hook cepat\n\
{FFFFFF}3. Seni Bela Diri Kung Fu\t{4CD964}$150\tTendangan akrobatik & tangkisan\n\
{FFFFFF}4. Kickboxing (Muay Thai)\t{4CD964}$150\tSerangan lutut & siku keras\n\
{FFFFFF}5. Grab Kick (Elbow Striking)\t{4CD964}$150\tKuncian bantingan jarak dekat");

	RP_ShowPlayerDialog(playerid, DIALOG_GYM_STYLES, DIALOG_STYLE_TABLIST_HEADERS,
		"PELATIHAN GAYA BERTARUNG GYM", dialogContent, "Pilih", "Batal");
	return 1;
}

stock OnGymStyleDialogResponse(playerid, listitem)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;

	new newStyle = 4;
	new styleName[32];
	new cost = (listitem == 0) ? 0 : GYM_TRAINING_FEE;

	switch (listitem) {
		case 0: { newStyle = 4; format(styleName, sizeof(styleName), "Gaya Standar"); }
		case 1: { newStyle = 5; format(styleName, sizeof(styleName), "Tinju Boxing"); }
		case 2: { newStyle = 6; format(styleName, sizeof(styleName), "Kung Fu"); }
		case 3: { newStyle = 7; format(styleName, sizeof(styleName), "Kickboxing Muay Thai"); }
		case 4: { newStyle = 8; format(styleName, sizeof(styleName), "Grab Kick"); }
		default: return 1;
	}

	if (cost > 0 && gPlayerData[playerid][pMoney] < cost) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Uang tunai Anda tidak mencukupi untuk biaya pelatihan ($150).");
		return 1;
	}

	if (cost > 0) {
		TakePlayerMoneyEx(playerid, cost, "biaya kursus fight style gym");
	}

	gPlayerData[playerid][pFightStyle] = newStyle;
	SetPlayerFightingStyle(playerid, FIGHT_STYLE:newStyle);
	DB_SavePlayerData(playerid);

	ApplyAnimation(playerid, "FIGHT_B", "FightB_G", 4.1, false, false, false, false, 0);
	PlayerPlaySound(playerid, 1058, 0.0, 0.0, 0.0);

	new succMsg[160];
	format(succMsg, sizeof(succMsg),
		"SUKSES: Anda telah menguasai gaya bertarung bela diri: '%s'! Gunakan klik kanan + klik kiri untuk memicu jurus.",
		styleName);
	SendClientMessage(playerid, COLOR_SUCCESS, succMsg);
	return 1;
}

stock ProcessGymWorkout(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;

	if (!IsPlayerNearPoint(playerid, BEACH_GYM_X, BEACH_GYM_Y, BEACH_GYM_Z, 12.0)) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus berada di area gym untuk berolahraga.");
		return 0;
	}

	// Animasi benchpress
	ApplyAnimation(playerid, "BENCHPRESS", "gym_bp_celebrate", 4.1, false, false, false, false, 0);
	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);

	new Float:hp;
	GetPlayerHealth(playerid, hp);
	if (hp < 100.0) {
		hp += 5.0;
		if (hp > 100.0) hp = 100.0;
		SetPlayerHealth(playerid, hp);
	}

	SendClientMessage(playerid, COLOR_SUCCESS,
		"SUKSES: Melakukan repetisi angkat beban barbell! Kebugaran fisik dan stamina Anda meningkat.");
	return 1;
}
