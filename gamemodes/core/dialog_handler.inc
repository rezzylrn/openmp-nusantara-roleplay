#if defined _INC_rp_dialog_handler
	#endinput
#endif
#define _INC_rp_dialog_handler

/*
==============================================================================
 dialog_handler.inc
 NUSANTARA ROLEPLAY - Master Legacy & System Dialog Response Router
==============================================================================
*/

/* ==========================================================================
   [SECURITY] Dialog-response spoofing guard
   --------------------------------------------------------------------------
   easyDialog memakai id tetap 32700 sehingga aman. Namun gamemode ini masih
   memakai dialog legacy bernomor (DIALOG_AMMUNATION_BUY, DIALOG_VENDING_SNACK,
   DIALOG_BAR_MENU, ...). Router me-dispatch nomor itu apa adanya tanpa ingat
   dialog mana yang terakhir ditampilkan -> client termodifikasi bisa mengirim
   response dialog yang tak pernah dibuka (beli senjata / tarik ATM dari jauh).

   Guard: tiap dialog legacy yang tampil dicatat (id + session token). Router
   hanya memproses response bila id == dialog terakhir & session masih sama
   (mencegah callback bocor antar slot playerid yang dipakai ulang).
   ========================================================================== */
new gLastLegacyDialog[MAX_PLAYERS] = {-1, ...};
new gLastLegacyDialogSession[MAX_PLAYERS] = {-1, ...};

stock RP_ShowPlayerDialog(playerid, dialogid, dialog_style, const caption[], const info[], const button1[], const button2[])
{
	gLastLegacyDialog[playerid] = dialogid;
	gLastLegacyDialogSession[playerid] = gPlayerSession[playerid];
	return ShowPlayerDialog(playerid, dialogid, DIALOG_STYLE:dialog_style, caption, info, button1, button2);
}

stock RP_DialogConsume(playerid) { gLastLegacyDialog[playerid] = -1; return 1; }

stock RP_DialogIsExpected(playerid, dialogid)
{
	if (dialogid < 0 || dialogid >= 32700) return 0;
	if (gLastLegacyDialogSession[playerid] != gPlayerSession[playerid]) return 0;
	return (gLastLegacyDialog[playerid] == dialogid) ? 1 : 0;
}

stock RP_IsAuthExemptDialog(dialogid)
{
	switch (dialogid) {
		case DIALOG_LOGIN, DIALOG_UCP_OTP_VERIFY, DIALOG_UCP_REG_PASS,
		     DIALOG_CHAR_SELECT, DIALOG_CHAR_CREATE_NAME,
		     DIALOG_CHAR_CREATE_DOB, DIALOG_CHAR_CREATE_GENDER: return 1;
	}
	return 0;
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	if (!response) {
		if (dialogid == DIALOG_LOGIN || dialogid == DIALOG_UCP_OTP_VERIFY || dialogid == DIALOG_CHAR_SELECT) {
			SendClientMessage(playerid, COLOR_ERROR, "[NUSANTARA RP] Sesi login dibatalkan.");
			Kick(playerid);
		}
		return 1;
	}

	// [SECURITY] Tolak response untuk dialog legacy yang tidak sedang ditampilkan
	// ke pemain ini (mencegah spoofing dialog-id dari client termodifikasi).
	if (!RP_IsAuthExemptDialog(dialogid)) {
		if (!RP_DialogIsExpected(playerid, dialogid)) {
			gLastLegacyDialog[playerid] = -1;
			return 1;
		}
		RP_DialogConsume(playerid);
	}

	switch (dialogid) {
		case DIALOG_LOGIN:              ProcessUCPLogin(playerid, inputtext);
		case DIALOG_UCP_OTP_VERIFY:     ProcessUCPOtpVerify(playerid, inputtext);
		case DIALOG_UCP_REG_PASS:       ProcessUCPRegisterPass(playerid, inputtext);
		case DIALOG_CHAR_SELECT:        ProcessCharSelect(playerid, listitem);
		case DIALOG_CHAR_CREATE_NAME:   ProcessCharCreateName(playerid, inputtext);
		case DIALOG_CHAR_CREATE_DOB:    ProcessCharCreateDOB(playerid, inputtext);
		case DIALOG_CHAR_CREATE_GENDER: ProcessCharCreateGender(playerid, listitem);
		case DIALOG_INVENTORY: {
			if (listitem < 0 || listitem >= MAX_INVENTORY_SLOTS) return 1;
			gSelectedInvSlot[playerid] = gInvSlotMap[playerid][listitem];
			RP_ShowPlayerDialog(playerid, DIALOG_ITEM_ACTION, DIALOG_STYLE_LIST,
				"TINDAKAN BARANG", "1. Gunakan Barang\n2. Buang Barang", "Pilih", "Kembali");
		}
		case DIALOG_ITEM_ACTION: {
			new slot = gSelectedInvSlot[playerid];
			if (slot < 0 || slot >= MAX_INVENTORY_SLOTS) return 1;
			if (listitem == 0) {
				UsePlayerItemBySlot(playerid, slot);
			} else if (listitem == 1) {
				TakePlayerItem(playerid, gPlayerItems[playerid][slot][itType], 1);
				SendInfo(playerid, "Barang telah dibuang dari kantong.");
			}
		}
		case DIALOG_GIVE_PLAYER: {
			new slot = gSelectedInvSlot[playerid];
			if (slot < 0 || slot >= MAX_INVENTORY_SLOTS) return 1;
			new targetid, amount;
			if (!sscanf(inputtext, "ud", targetid, amount)) {
				new itemType = gPlayerItems[playerid][slot][itType];
				if (itemType != ITEM_NONE) {
					ProcessGiveItem(playerid, targetid, itemType, amount);
				}
			} else {
				SendSyntax(playerid, "[ID/Nama Pemain] [Jumlah]");
			}
		}
		case DIALOG_STORE_BUY:         ProcessStorePurchase(playerid, listitem);
		case DIALOG_PHONE_MENU: {
			if (listitem == 0) RP_ShowPlayerDialog(playerid, DIALOG_PHONE_CALL, DIALOG_STYLE_INPUT, "PANGGILAN", "Nomor tujuan:", "Panggil", "Batal");
			else if (listitem == 1) RP_ShowPlayerDialog(playerid, DIALOG_PHONE_SMS, DIALOG_STYLE_INPUT, "SMS", "Format: [Nomor] [Pesan]", "Kirim", "Batal");
			else if (listitem == 2) {
				gPlayerData[playerid][pPhoneStatus] = !gPlayerData[playerid][pPhoneStatus];
				SendInfo(playerid, "Mode dering: %s.", gPlayerData[playerid][pPhoneStatus] ? "Aktif" : "Hening");
			}
		}
		case DIALOG_PHONE_CALL:        ProcessCall(playerid, inputtext);
		case DIALOG_PHONE_SMS: {
			new num, msg[128];
			if (!sscanf(inputtext, "ds[128]", num, msg)) ProcessSMS(playerid, num, msg);
			else SendSyntax(playerid, "[Nomor] [Pesan]");
		}
		case DIALOG_RADIO_FREQ: {
			new f = strval(inputtext);
			if (f >= 1 && f <= 999) {
				gPlayerData[playerid][pRadioFreq] = f;
				SendSucc(playerid, "Frekuensi HT disetel ke %d MHz.", f);
			} else SendErr(playerid, "Frekuensi 1 - 999 MHz.");
		}
		case DIALOG_GPS_MENU: {
			switch (listitem) {
				case 0:  { SetPlayerCheckpoint(playerid, BANK_X, BANK_Y, BANK_Z, 6.0); SendSucc(playerid, "GPS: Rute disetel ke Bank Nusantara (%.0fm).", GetPlayerDistanceFromPoint(playerid, BANK_X, BANK_Y, BANK_Z)); }
				case 1:  { SetPlayerCheckpoint(playerid, POLICE_X, POLICE_Y, POLICE_Z, 6.0); SendSucc(playerid, "GPS: Rute disetel ke Kantor Polisi LSPD (%.0fm).", GetPlayerDistanceFromPoint(playerid, POLICE_X, POLICE_Y, POLICE_Z)); }
				case 2:  { SetPlayerCheckpoint(playerid, MEDIC_X, MEDIC_Y, MEDIC_Z, 6.0); SendSucc(playerid, "GPS: Rute disetel ke RS All Saints ASGH (%.0fm).", GetPlayerDistanceFromPoint(playerid, MEDIC_X, MEDIC_Y, MEDIC_Z)); }
				case 3:  { SetPlayerCheckpoint(playerid, GOV_X, GOV_Y, GOV_Z, 6.0); SendSucc(playerid, "GPS: Rute disetel ke Balai Kota SAGOV (%.0fm).", GetPlayerDistanceFromPoint(playerid, GOV_X, GOV_Y, GOV_Z)); }
				case 4:  { SetPlayerCheckpoint(playerid, FUEL_STATION_X, FUEL_STATION_Y, FUEL_STATION_Z, 6.0); SendSucc(playerid, "GPS: Rute disetel ke SPBU Bahan Bakar Idlewood (%.0fm).", GetPlayerDistanceFromPoint(playerid, FUEL_STATION_X, FUEL_STATION_Y, FUEL_STATION_Z)); }
				case 5:  { SetPlayerCheckpoint(playerid, STORE_IDLEWOOD_X, STORE_IDLEWOOD_Y, STORE_IDLEWOOD_Z, 6.0); SendSucc(playerid, "GPS: Rute disetel ke Toko 24/7 Idlewood (%.0fm).", GetPlayerDistanceFromPoint(playerid, STORE_IDLEWOOD_X, STORE_IDLEWOOD_Y, STORE_IDLEWOOD_Z)); }
				case 6:  { SetPlayerCheckpoint(playerid, WORKSHOP_X, WORKSHOP_Y, WORKSHOP_Z, 6.0); SendSucc(playerid, "GPS: Rute disetel ke Bengkel Modifikasi Mekanik (%.0fm).", GetPlayerDistanceFromPoint(playerid, WORKSHOP_X, WORKSHOP_Y, WORKSHOP_Z)); }
				case 7:  { SetPlayerCheckpoint(playerid, DEALER_X, DEALER_Y, DEALER_Z, 6.0); SendSucc(playerid, "GPS: Rute disetel ke Showroom Dealership (%.0fm).", GetPlayerDistanceFromPoint(playerid, DEALER_X, DEALER_Y, DEALER_Z)); }
				case 8:  { SetPlayerCheckpoint(playerid, COURIER_X, COURIER_Y, COURIER_Z, 6.0); SendSucc(playerid, "GPS: Rute disetel ke Job Kurir Paket (%.0fm).", GetPlayerDistanceFromPoint(playerid, COURIER_X, COURIER_Y, COURIER_Z)); }
				case 9:  { SetPlayerCheckpoint(playerid, SWEEPER_X, SWEEPER_Y, SWEEPER_Z, 6.0); SendSucc(playerid, "GPS: Rute disetel ke Job Pembersih Jalan Sweeper (%.0fm).", GetPlayerDistanceFromPoint(playerid, SWEEPER_X, SWEEPER_Y, SWEEPER_Z)); }
				case 10: { SetPlayerCheckpoint(playerid, TRUCKER_X, TRUCKER_Y, TRUCKER_Z, 6.0); SendSucc(playerid, "GPS: Rute disetel ke Job Truk Hauler (%.0fm).", GetPlayerDistanceFromPoint(playerid, TRUCKER_X, TRUCKER_Y, TRUCKER_Z)); }
				case 11: { SetPlayerCheckpoint(playerid, BUS_DEPOT_X, BUS_DEPOT_Y, BUS_DEPOT_Z, 6.0); SendSucc(playerid, "GPS: Rute disetel ke Job Terminal Bus Umum (%.0fm).", GetPlayerDistanceFromPoint(playerid, BUS_DEPOT_X, BUS_DEPOT_Y, BUS_DEPOT_Z)); }
				case 12: { SetPlayerCheckpoint(playerid, FISH_PIER_X, FISH_PIER_Y, FISH_PIER_Z, 6.0); SendSucc(playerid, "GPS: Rute disetel ke Dermaga Mancing Santa Maria (%.0fm).", GetPlayerDistanceFromPoint(playerid, FISH_PIER_X, FISH_PIER_Y, FISH_PIER_Z)); }
				case 13: { SetPlayerCheckpoint(playerid, LUMBER_X, LUMBER_Y, LUMBER_Z, 6.0); SendSucc(playerid, "GPS: Rute disetel ke Hutan Penebangan Kayu (%.0fm).", GetPlayerDistanceFromPoint(playerid, LUMBER_X, LUMBER_Y, LUMBER_Z)); }
				case 14: { SetPlayerCheckpoint(playerid, SAWMILL_X, SAWMILL_Y, SAWMILL_Z, 6.0); SendSucc(playerid, "GPS: Rute disetel ke Pabrik Pengolahan Kayu Sawmill (%.0fm).", GetPlayerDistanceFromPoint(playerid, SAWMILL_X, SAWMILL_Y, SAWMILL_Z)); }
				case 15: { SetPlayerCheckpoint(playerid, TRASH_BUYER_X, TRASH_BUYER_Y, TRASH_BUYER_Z, 6.0); SendSucc(playerid, "GPS: Rute disetel ke Daur Ulang Sampah (%.0fm).", GetPlayerDistanceFromPoint(playerid, TRASH_BUYER_X, TRASH_BUYER_Y, TRASH_BUYER_Z)); }
				case 16: { DisablePlayerCheckpoint(playerid); SendInfo(playerid, "GPS: Tanda navigasi telah dimatikan."); }
			}
			PlayerPlaySound(playerid, 1083, 0.0, 0.0, 0.0);
		}
		case DIALOG_BANK_MENU:         OnBankDialogResponse(playerid, listitem);
		case DIALOG_BANK_DEPOSIT:      OnBankDepositResponse(playerid, inputtext);
		case DIALOG_BANK_WITHDRAW:     OnBankWithdrawResponse(playerid, inputtext);
		case DIALOG_BANK_TRANSFER:     OnBankTransferResponse(playerid, inputtext);
		case DIALOG_ATM_MENU:          OnATMMenuResponse(playerid, listitem);
		case DIALOG_ATM_WITHDRAW:      OnATMWithdrawResponse(playerid, inputtext);
		case DIALOG_ATM_DEPOSIT:       OnATMDepositResponse(playerid, inputtext);
		case DIALOG_JOB_MENU:          OnJobMenuResponse(playerid, listitem);
		case DIALOG_DEALERSHIP:        OnDealershipResponse(playerid, listitem);
		case DIALOG_MYVEH: {
			if (listitem >= 0 && listitem < MAX_OWNED_VEHICLES) {
				new realSlot = gPlayerVehListSlot[playerid][listitem];
				ShowVehicleActionDialog(playerid, realSlot);
			}
		}
		case DIALOG_MYVEH_ACTION:      OnMyVehActionResponse(playerid, listitem);
		case DIALOG_TRUNK:             OnVehicleTrunkResponse(playerid, listitem);
		case DIALOG_TRUNK_DEPOSIT:      OnTrunkDepositCashResponse(playerid, listitem);
		case DIALOG_TRUNK_DEPOSIT_CASH: OnTrunkSaveCashInput(playerid, inputtext);
		case DIALOG_TRUNK_WITHDRAW:     OnTrunkSaveCashInput(playerid, inputtext);
		case DIALOG_TRUNK_TAKE_CASH:    OnTrunkTakeCashInput(playerid, inputtext);
		case DIALOG_TRUNK_DEPOSIT_ITEM: OnTrunkDepositItemResponse(playerid, listitem);
		case DIALOG_TRUNK_WITHDRAW_ITEM:OnTrunkWithdrawItemResponse(playerid, inputtext);
		case DIALOG_911_SERVICE: {
			gPending911Service[playerid] = (listitem == 0) ? 1 : 2;
			new title[48];
			format(title, sizeof(title), "LAPORAN DARURAT %s", (listitem == 0) ? "SAPD POLISI" : "SAMD MEDIS");
			RP_ShowPlayerDialog(playerid, DIALOG_911_MESSAGE, DIALOG_STYLE_INPUT,
				title, "Jelaskan situasi darurat dan kondisi Anda secara singkat:", "Kirim", "Batal");
		}
		case DIALOG_911_MESSAGE: {
			if (strlen(inputtext) < 3) {
				SendErr(playerid, "Pesan laporan terlalu singkat.");
				return 1;
			}
			CreateDispatchReport(playerid, gPending911Service[playerid], inputtext);
		}
		case DIALOG_LANGUAGE_SELECT: {
			if (!response) return 1;
			if (listitem == 0) {
				SetPlayerLang(playerid, LANG_ID);
				SendSucc(playerid, "Bahasa sistem berhasil diatur ke: Bahasa Indonesia.");
			} else {
				SetPlayerLang(playerid, LANG_EN);
				SendSucc(playerid, "System language successfully set to: English.");
			}
			UpdatePlayerHUD(playerid);
			PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
			return 1;
		}
		case DIALOG_ARMORY_MENU: OnFactionArmoryResponse(playerid, listitem);
		case DIALOG_RENTAL_MENU: OnRentalDialogResponse(playerid, listitem);
		case DIALOG_TRUNK_WEAPON: OnTrunkWeaponResponse(playerid, listitem);
		case DIALOG_BLACKMARKET_BUY: OnBlackMarketBuyResponse(playerid, listitem);
		case DIALOG_BLACKMARKET_SELL: OnBlackMarketSellResponse(playerid, listitem);
		case DIALOG_VENDING_SNACK: OnVendingDialogResponse(playerid, listitem);
		case DIALOG_DMV_THEORY: OnDMVDialogResponse(playerid, listitem);
		case DIALOG_AMMUNATION_BUY: OnAmmuNationDialogResponse(playerid, listitem);
		case DIALOG_BAR_MENU: OnBarMenuDialogResponse(playerid, listitem);
		case DIALOG_GYM_STYLES: OnGymStyleDialogResponse(playerid, listitem);
		case DIALOG_FASTFOOD_MENU: OnFastFoodDialogResponse(playerid, listitem);
		case DIALOG_ACCESSORIES: OnAccessoriesDialogResponse(playerid, listitem);
		case DIALOG_WORKSHOP_MENU: OnWorkshopMenuResponse(playerid, listitem);
		case DIALOG_ACTION_PLATE_INPUT: {
			if (response && strlen(inputtext) > 0) ProcessCustomPlate(playerid, inputtext);
		}
		case DIALOG_ACTION_TAG_INPUT: {
			if (response && strlen(inputtext) > 0) ProcessTagGraffiti(playerid, inputtext);
		}
		case DIALOG_ACTION_AD_INPUT: {
			if (response && strlen(inputtext) > 0) ProcessSendAdvertisement(playerid, inputtext);
		}
	}
	return 1;
}

// checkpoint enter
