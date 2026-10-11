/*
	easyDialog.inc - Dialogs made easier!
	Created by Emmet on Friday, January 24, 2014.
	Updated by Southclaws, MichelDev, adib-yg
*/

#if defined _easyDialog_included
	#endinput
#endif
#define _easyDialog_included

#if !defined isnull
	#define isnull(%1) \
		((!(%1[0])) || (((%1[0]) == '\1') && (!(%1[1]))))
#endif

#if !defined HidePlayerDialog
	#define HidePlayerDialog(%1) \
		ShowPlayerDialog(%1, -1, DIALOG_STYLE:0, " ", " ", " ", " ")
#endif

#define Dialog:%0(%1) \
	forward dialog_%0(%1); public dialog_%0(%1)

#define Dialog_Show(%0,%1, \
	Dialog_Open(%0, #%1,

#define Dialog_Opened(%0) \
	(CallRemoteFunction("Dialog_IsOpened", "i", (%0)))

#if !defined _INC_open_mp
	#if !defined DIALOG_STYLE
		#define DIALOG_STYLE: _:
	#endif
#endif

static
	s_DialogName[MAX_PLAYERS][32],
	bool: s_DialogOpened[MAX_PLAYERS]
;

forward OnDialogPerformed(playerid, const dialog[], response, success);

forward Dialog_IsOpened(playerid);
public Dialog_IsOpened(playerid) {
	return (s_DialogOpened[playerid]);
}

stock Dialog_Close(playerid) {
	s_DialogName[playerid][0] = '\0';
	s_DialogOpened[playerid] = false;

	return HidePlayerDialog(playerid);
}

stock Dialog_Open(playerid, const function[], DIALOG_STYLE:style, const caption[], const info[], const button1[], const button2[]) {
	if (!strlen(info)) {
		return 0;
	}
	ShowPlayerDialog(playerid, 32700, style, caption, info, button1, button2);
	s_DialogOpened[playerid] = true;
	format(s_DialogName[playerid], sizeof(s_DialogName[]), "%s", function);
	return 1;
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[]) {
	static s_Public = cellmax;
	if (s_Public == cellmax) {
		s_Public = funcidx("OnDialogPerformed");
	}

	for (new i = 0, l = strlen(inputtext); i < l; i ++) {
		if (inputtext[i] == '%') {
			inputtext[i] = '#';
		}
	}
	
	if (dialogid == 32700 && s_DialogName[playerid][0] != '\0') {
		new string[48];
		format(string, sizeof(string), "dialog_%s", s_DialogName[playerid]);

		Dialog_Close(playerid);

		if ((s_Public == -1) || (CallLocalFunction("OnDialogPerformed", "dsdd", playerid, string[7], response, funcidx(string) != -1))) {
	 		CallLocalFunction(string, "ddds", playerid, response, listitem, (!inputtext[0]) ? ("\1") : (inputtext));
		}
		return 1;
	}
	#if defined DR_OnDialogResponse
		return DR_OnDialogResponse(playerid, dialogid, response, listitem, inputtext);
	#else
		return 0;
	#endif
}

#if defined _ALS_OnDialogResponse
	#undef OnDialogResponse
#else
	#define _ALS_OnDialogResponse
#endif

#define OnDialogResponse DR_OnDialogResponse

#if defined DR_OnDialogResponse
	forward DR_OnDialogResponse(playerid, dialogid, response, listitem, inputtext[]);
#endif
