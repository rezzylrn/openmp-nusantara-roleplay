#if defined _INC_progress_bar
	#endinput
#endif
#define _INC_progress_bar

/*
==============================================================================
 progress_bar.inc
 NUSANTARA ROLEPLAY - Activity Progress Bar TextDraw
 Clean classic SA-MP / JGRP style activity progress bar.
 Displays real-time progress while player is performing an action
 (refueling, tire repair, fishing, cooking, lockpicking, etc.).
==============================================================================
*/

static bool:gProgressActive[MAX_PLAYERS];
static gProgressValue[MAX_PLAYERS];
static gProgressStep[MAX_PLAYERS];
static gProgressTimer[MAX_PLAYERS];
static gProgressTitle[MAX_PLAYERS][48];
static gProgressCallback[MAX_PLAYERS][32];
static gProgressParam[MAX_PLAYERS];

// Player TextDraw handles
static PlayerText:gTD_ProgressBg[MAX_PLAYERS] = {INVALID_PLAYER_TEXT_DRAW, ...};
static PlayerText:gTD_ProgressTitle[MAX_PLAYERS] = {INVALID_PLAYER_TEXT_DRAW, ...};
static PlayerText:gTD_ProgressBar[MAX_PLAYERS] = {INVALID_PLAYER_TEXT_DRAW, ...};

forward RP_ProgressBarTick(playerid);

stock ProgressBar_Init(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;

	// Background box
	gTD_ProgressBg[playerid] = CreatePlayerTextDraw(playerid, 235.0, 380.0, "_");
	PlayerTextDrawLetterSize(playerid, gTD_ProgressBg[playerid], 0.0, 2.5);
	PlayerTextDrawTextSize(playerid, gTD_ProgressBg[playerid], 405.0, 0.0);
	PlayerTextDrawAlignment(playerid, gTD_ProgressBg[playerid], TEXT_DRAW_ALIGN_LEFT);
	PlayerTextDrawColour(playerid, gTD_ProgressBg[playerid], 0xFFFFFFFF);
	PlayerTextDrawUseBox(playerid, gTD_ProgressBg[playerid], true);
	PlayerTextDrawBoxColour(playerid, gTD_ProgressBg[playerid], 0x000000AA);

	// Title label
	gTD_ProgressTitle[playerid] = CreatePlayerTextDraw(playerid, 320.0, 382.0, "MEMPROSES...");
	PlayerTextDrawLetterSize(playerid, gTD_ProgressTitle[playerid], 0.17, 0.85);
	PlayerTextDrawAlignment(playerid, gTD_ProgressTitle[playerid], TEXT_DRAW_ALIGN_CENTER);
	PlayerTextDrawColour(playerid, gTD_ProgressTitle[playerid], 0xFFFFFFFF);
	PlayerTextDrawFont(playerid, gTD_ProgressTitle[playerid], TEXT_DRAW_FONT_1);
	PlayerTextDrawSetProportional(playerid, gTD_ProgressTitle[playerid], true);
	PlayerTextDrawSetShadow(playerid, gTD_ProgressTitle[playerid], 1);

	// Fill bar
	gTD_ProgressBar[playerid] = CreatePlayerTextDraw(playerid, 320.0, 393.0, "~w~[ ~g~|||||||||| ~w~] ~y~50%");
	PlayerTextDrawLetterSize(playerid, gTD_ProgressBar[playerid], 0.18, 0.85);
	PlayerTextDrawAlignment(playerid, gTD_ProgressBar[playerid], TEXT_DRAW_ALIGN_CENTER);
	PlayerTextDrawColour(playerid, gTD_ProgressBar[playerid], 0xFFFFFFFF);
	PlayerTextDrawFont(playerid, gTD_ProgressBar[playerid], TEXT_DRAW_FONT_2);
	PlayerTextDrawSetProportional(playerid, gTD_ProgressBar[playerid], true);
	PlayerTextDrawSetShadow(playerid, gTD_ProgressBar[playerid], 1);

	gProgressActive[playerid] = false;
	gProgressTimer[playerid] = 0;
	return 1;
}

stock ProgressBar_Destroy(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;
	if (gProgressTimer[playerid] != 0) {
		KillTimer(gProgressTimer[playerid]);
		gProgressTimer[playerid] = 0;
	}
	if (gTD_ProgressBg[playerid] != INVALID_PLAYER_TEXT_DRAW) {
		PlayerTextDrawDestroy(playerid, gTD_ProgressBg[playerid]);
		gTD_ProgressBg[playerid] = INVALID_PLAYER_TEXT_DRAW;
	}
	if (gTD_ProgressTitle[playerid] != INVALID_PLAYER_TEXT_DRAW) {
		PlayerTextDrawDestroy(playerid, gTD_ProgressTitle[playerid]);
		gTD_ProgressTitle[playerid] = INVALID_PLAYER_TEXT_DRAW;
	}
	if (gTD_ProgressBar[playerid] != INVALID_PLAYER_TEXT_DRAW) {
		PlayerTextDrawDestroy(playerid, gTD_ProgressBar[playerid]);
		gTD_ProgressBar[playerid] = INVALID_PLAYER_TEXT_DRAW;
	}
	gProgressActive[playerid] = false;
	return 1;
}

stock ApplyProgressBar(playerid, const title[], durationSeconds, const callbackFunc[], extraParam = 0)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;
	if (durationSeconds <= 0) durationSeconds = 1;

	if (gProgressActive[playerid]) {
		StopProgressBar(playerid, false);
	}

	gProgressActive[playerid] = true;
	gProgressValue[playerid] = 0;
	format(gProgressTitle[playerid], sizeof(gProgressTitle[]), "%s", title);
	format(gProgressCallback[playerid], sizeof(gProgressCallback[]), "%s", callbackFunc);
	gProgressParam[playerid] = extraParam;

	// Update tick interval: 250ms
	new totalTicks = durationSeconds * 4;
	if (totalTicks <= 0) totalTicks = 1;
	gProgressStep[playerid] = 100 / totalTicks;
	if (gProgressStep[playerid] <= 0) gProgressStep[playerid] = 1;

	PlayerTextDrawSetString(playerid, gTD_ProgressTitle[playerid], title);
	PlayerTextDrawSetString(playerid, gTD_ProgressBar[playerid], "~w~[ ~g~-------------------- ~w~] ~y~0%");

	PlayerTextDrawShow(playerid, gTD_ProgressBg[playerid]);
	PlayerTextDrawShow(playerid, gTD_ProgressTitle[playerid]);
	PlayerTextDrawShow(playerid, gTD_ProgressBar[playerid]);

	TogglePlayerControllable(playerid, false);

	gProgressTimer[playerid] = SetTimerEx("RP_ProgressBarTick", 250, true, "i", playerid);
	return 1;
}

public RP_ProgressBarTick(playerid)
{
	if (!IsPlayerConnected(playerid) || !gProgressActive[playerid]) {
		if (gProgressTimer[playerid] != 0) {
			KillTimer(gProgressTimer[playerid]);
			gProgressTimer[playerid] = 0;
		}
		return 0;
	}

	gProgressValue[playerid] += gProgressStep[playerid];
	if (gProgressValue[playerid] >= 100) {
		gProgressValue[playerid] = 100;

		// Selesai
		KillTimer(gProgressTimer[playerid]);
		gProgressTimer[playerid] = 0;
		gProgressActive[playerid] = false;

		PlayerTextDrawHide(playerid, gTD_ProgressBg[playerid]);
		PlayerTextDrawHide(playerid, gTD_ProgressTitle[playerid]);
		PlayerTextDrawHide(playerid, gTD_ProgressBar[playerid]);

		TogglePlayerControllable(playerid, true);
		ClearAnimations(playerid);

		if (strlen(gProgressCallback[playerid]) > 0) {
			CallLocalFunction(gProgressCallback[playerid], "dd", playerid, gProgressParam[playerid]);
		}
		return 1;
	}

	// Format visual bar
	new filled = gProgressValue[playerid] / 5; // max 20 bars
	new barStr[128];
	barStr[0] = '\0';
	strcat(barStr, "~w~[ ~g~");
	for (new i = 0; i < filled; i++) strcat(barStr, "|");
	strcat(barStr, "~r~");
	for (new i = filled; i < 20; i++) strcat(barStr, "-");

	new percentStr[24];
	format(percentStr, sizeof(percentStr), " ~w~] ~y~%d%%", gProgressValue[playerid]);
	strcat(barStr, percentStr);

	PlayerTextDrawSetString(playerid, gTD_ProgressBar[playerid], barStr);
	return 1;
}

stock StopProgressBar(playerid, bool:notify = true)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;
	if (!gProgressActive[playerid]) return 0;

	if (gProgressTimer[playerid] != 0) {
		KillTimer(gProgressTimer[playerid]);
		gProgressTimer[playerid] = 0;
	}
	gProgressActive[playerid] = false;

	if (IsPlayerConnected(playerid)) {
		PlayerTextDrawHide(playerid, gTD_ProgressBg[playerid]);
		PlayerTextDrawHide(playerid, gTD_ProgressTitle[playerid]);
		PlayerTextDrawHide(playerid, gTD_ProgressBar[playerid]);

		TogglePlayerControllable(playerid, true);
		ClearAnimations(playerid);

		if (notify) {
			SendClientMessage(playerid, COLOR_ERROR, "[INFO] Aktivitas Anda telah dibatalkan.");
		}
	}
	return 1;
}

stock IsPlayerInActivity(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;
	return gProgressActive[playerid];
}
