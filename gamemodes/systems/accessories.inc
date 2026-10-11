#if defined _INC_rp_accessories
	#endinput
#endif
#define _INC_rp_accessories

/*
==============================================================================
 accessories.inc
 NUSANTARA ROLEPLAY - Character Fashion Accessories & Attachable Objects
==============================================================================
*/

#define ACC_SLOT_HEAD           0
#define ACC_SLOT_GLASSES        1
#define ACC_SLOT_BACKPACK       2

stock Accessories_Init()
{
	print("[ACCESSORIES] Sistem aksesoris fashion & attached objects karakter aktif.");
	return 1;
}

stock ShowAccessoriesMenu(playerid)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;

	new dialogContent[380];
	format(dialogContent, sizeof(dialogContent),
		"Katalog Aksesoris\tPosisi Tubuh\n\
{FFFFFF}1. Topi Fedora / Hat\t{33CCFF}Kepala (Head)\n\
{FFFFFF}2. Kacamata Hitam Polisi / Aviator\t{33CCFF}Mata (Glasses)\n\
{FFFFFF}3. Tas Ransel Petualang / Backpack\t{33CCFF}Punggung (Spine)\n\
{FF3B30}X. Lepas Seluruh Aksesoris\t{FF3B30}Reset Model");

	ShowPlayerDialog(playerid, DIALOG_ACCESSORIES, DIALOG_STYLE_TABLIST_HEADERS,
		"MENU AKSESORIS FASHION", dialogContent, "Pakai", "Tutup");
	return 1;
}

stock OnAccessoriesDialogResponse(playerid, listitem)
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;

	switch (listitem) {
		case 0: {
			// Topi Fedora (Model 18636) di Bone 2 (Head)
			SetPlayerAttachedObject(playerid, ACC_SLOT_HEAD, 18636, 2,
				0.07, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0);
			SendClientMessage(playerid, COLOR_SUCCESS, "SUKSES: Mengenakan Topi Fedora di kepala.");
		}
		case 1: {
			// Kacamata Hitam (Model 19011) di Bone 2 (Head)
			SetPlayerAttachedObject(playerid, ACC_SLOT_GLASSES, 19011, 2,
				0.08, 0.04, 0.0, 0.0, 90.0, 0.0, 1.0, 1.0, 1.0);
			SendClientMessage(playerid, COLOR_SUCCESS, "SUKSES: Mengenakan Kacamata Hitam Aviator.");
		}
		case 2: {
			// Tas Ransel Backpack (Model 19559) di Bone 1 (Spine)
			SetPlayerAttachedObject(playerid, ACC_SLOT_BACKPACK, 19559, 1,
				-0.12, -0.15, 0.0, 0.0, 90.0, 90.0, 1.0, 1.0, 1.0);
			SendClientMessage(playerid, COLOR_SUCCESS, "SUKSES: Mengenakan Tas Ransel di punggung.");
		}
		case 3: {
			// Lepas seluruh aksesoris
			if (IsPlayerAttachedObjectSlotUsed(playerid, ACC_SLOT_HEAD)) RemovePlayerAttachedObject(playerid, ACC_SLOT_HEAD);
			if (IsPlayerAttachedObjectSlotUsed(playerid, ACC_SLOT_GLASSES)) RemovePlayerAttachedObject(playerid, ACC_SLOT_GLASSES);
			if (IsPlayerAttachedObjectSlotUsed(playerid, ACC_SLOT_BACKPACK)) RemovePlayerAttachedObject(playerid, ACC_SLOT_BACKPACK);
			SendClientMessage(playerid, COLOR_INFO, "INFO: Seluruh aksesoris berhasil dilepaskan.");
		}
	}

	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
	return 1;
}
