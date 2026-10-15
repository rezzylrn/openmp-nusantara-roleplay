#if defined _INC_localization
	#endinput
#endif
#define _INC_localization

/*
==============================================================================
 localization.inc
 NUSANTARA ROLEPLAY - Bilingual Localization Engine (ID / EN)
==============================================================================
*/

stock GetPlayerLang(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS) return LANG_ID;
	return gPlayerData[playerid][pLanguage];
}

stock SetPlayerLang(playerid, lang)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS) return 0;
	gPlayerData[playerid][pLanguage] = (lang == LANG_EN) ? LANG_EN : LANG_ID;
	DB_SavePlayerData(playerid);
	return 1;
}

stock ShowLanguageSelectionDialog(playerid)
{
	RP_ShowPlayerDialog(playerid, DIALOG_LANGUAGE_SELECT, DIALOG_STYLE_LIST,
		"PILIH BAHASA / SELECT LANGUAGE",
		"1. Bahasa Indonesia [ID] (Rekomendasi Warga Nusantara)\n2. English [EN] (International Language)",
		"Pilih / Select", "Batal / Cancel");
	return 1;
}
