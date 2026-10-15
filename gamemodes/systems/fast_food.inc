#if defined _INC_rp_fast_food
	#endinput
#endif
#define _INC_rp_fast_food

/*
==============================================================================
 fast_food.inc
 NUSANTARA ROLEPLAY - Fast Food Restaurants (Burger Shot, Cluckin Bell, Pizza)
==============================================================================
*/

#define MAX_FASTFOOD_OUTLETS    3

stock const Float:gFastFoodSpots[MAX_FASTFOOD_OUTLETS][3] = {
	{1199.0, -918.0, 43.0},  // Burger Shot Marina
	{2393.0, -1898.0, 13.5}, // Cluckin Bell Willowfield
	{2105.0, -1806.0, 13.5}  // Well Stacked Pizza Idlewood
};

stock const gFastFoodNames[][] = {
	"Burger Shot Marina",
	"Cluckin Bell Willowfield",
	"Well Stacked Pizza Idlewood"
};

stock FastFood_Init()
{
	for (new i = 0; i < MAX_FASTFOOD_OUTLETS; i++) {
		CreatePickup(1239, 1, gFastFoodSpots[i][0], gFastFoodSpots[i][1], gFastFoodSpots[i][2], 0);

		new lbl[180];
		format(lbl, sizeof(lbl),
			"{FFCC00}[ RESTORAN %s ]\n{FFFFFF}Menu makanan cepat saji lezat\n{FFFF00}Gunakan /orderfood atau /pesanmakan",
			gFastFoodNames[i]);

		Create3DTextLabel(lbl, 0xFFCC00FF,
			gFastFoodSpots[i][0], gFastFoodSpots[i][1], gFastFoodSpots[i][2] + 0.8,
			12.0, 0, true);
	}
	print("[FAST FOOD] 3 Restoran cepat saji (Burger Shot, Cluckin Bell, Pizza) aktif.");
	return 1;
}

stock ShowFastFoodMenu(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;

	new slot = -1;
	for (new i = 0; i < MAX_FASTFOOD_OUTLETS; i++) {
		if (IsPlayerNearPoint(playerid, gFastFoodSpots[i][0], gFastFoodSpots[i][1], gFastFoodSpots[i][2], 5.0)) {
			slot = i;
			break;
		}
	}

	if (slot == -1) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus berada di kasir restoran cepat saji (/gps).");
		return 0;
	}

	new dialogContent[380];
	format(dialogContent, sizeof(dialogContent),
		"Menu Makanan Cepat Saji\tHarga\tEfek Pemulihan\n\
{FFFFFF}1. Paket Burger Shot Double Meat\t{4CD964}$12\t{FFCC00}+50%% Lapar | +20 HP\n\
{FFFFFF}2. Cluckin' Crispy Fried Chicken\t{4CD964}$15\t{FFCC00}+60%% Lapar | +25 HP\n\
{FFFFFF}3. Pizza Stack Meat Lovers Slice\t{4CD964}$14\t{FFCC00}+55%% Lapar | +25 HP");

	RP_ShowPlayerDialog(playerid, DIALOG_FASTFOOD_MENU, DIALOG_STYLE_TABLIST_HEADERS,
		gFastFoodNames[slot], dialogContent, "Pesan", "Batal");
	return 1;
}

stock OnFastFoodDialogResponse(playerid, listitem)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;

	new cost = 0;
	new Float:hungerAdd = 0.0, Float:hpAdd = 0.0;
	new mealName[32];

	switch (listitem) {
		case 0: { cost = 12; hungerAdd = 50.0; hpAdd = 20.0; format(mealName, sizeof(mealName), "Burger Shot Combo"); }
		case 1: { cost = 15; hungerAdd = 60.0; hpAdd = 25.0; format(mealName, sizeof(mealName), "Cluckin Chicken Basket"); }
		case 2: { cost = 14; hungerAdd = 55.0; hpAdd = 25.0; format(mealName, sizeof(mealName), "Pizza Stack Slice"); }
		default: return 1;
	}

	if (gPlayerData[playerid][pMoney] < cost) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Uang tunai Anda tidak mencukupi untuk memesan paket makanan ini.");
		return 1;
	}

	TakePlayerMoneyEx(playerid, cost, "pesan makanan cepat saji");

	gPlayerData[playerid][pHunger] += hungerAdd;
	if (gPlayerData[playerid][pHunger] > 100.0) gPlayerData[playerid][pHunger] = 100.0;

	new Float:hp;
	GetPlayerHealth(playerid, hp);
	hp += hpAdd;
	if (hp > 100.0) hp = 100.0;
	SetPlayerHealth(playerid, hp);

	UpdatePlayerHUD(playerid);
	ApplyAnimation(playerid, "FOOD", "EAT_Burger", 4.1, false, false, false, false, 0);
	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);

	new succMsg[160];
	format(succMsg, sizeof(succMsg),
		"SUKSES: Menyantap paket %s seharga $%d! Rasa lapar dan kondisi fisik darah Anda pulih kembali.",
		mealName, cost);
	SendClientMessage(playerid, COLOR_SUCCESS, succMsg);
	return 1;
}
