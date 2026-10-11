#if defined _INC_rp_advertisements
	#endinput
#endif
#define _INC_rp_advertisements

/*
==============================================================================
 advertisements.inc
 NUSANTARA ROLEPLAY - SANews Commercial Advertisement & Classifieds Network
==============================================================================
*/

#define AD_COST                 50
#define AD_COOLDOWN             45

new gPlayerAdCooldown[MAX_PLAYERS];

stock Advertisements_Init()
{
	for (new i = 0; i < MAX_PLAYERS; i++) {
		gPlayerAdCooldown[i] = 0;
	}
	print("[ADVERTISEMENTS] Jaringan iklan baris komersial SANews Los Santos aktif.");
	return 1;
}

stock ProcessSendAdvertisement(playerid, const adText[])
{
	if (!gPlayerData[playerid][pLoggedIn]) return 0;

	if (gPlayerData[playerid][pPhone] <= 0) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Anda harus memiliki ponsel untuk mencantumkan kontak iklan.");
		return 0;
	}

	new now = gettime();
	if (now < gPlayerAdCooldown[playerid]) {
		new sLeft = gPlayerAdCooldown[playerid] - now;
		new waitMsg[128];
		format(waitMsg, sizeof(waitMsg), "ERROR: Harap tunggu %d detik sebelum memasang iklan SANews berikutnya.", sLeft);
		SendClientMessage(playerid, COLOR_ERROR, waitMsg);
		return 0;
	}

	if (gPlayerData[playerid][pMoney] < AD_COST && gPlayerData[playerid][pBank] < AD_COST) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Biaya publikasi iklan baris SANews ($50) tidak mencukupi.");
		return 0;
	}

	if (strlen(adText) < 5 || strlen(adText) > 100) {
		SendClientMessage(playerid, COLOR_ERROR, "ERROR: Panjang teks iklan harus antara 5 hingga 100 karakter.");
		return 0;
	}

	if (gPlayerData[playerid][pMoney] >= AD_COST) {
		TakePlayerMoneyEx(playerid, AD_COST, "biaya iklan baris SANews");
	} else {
		TakePlayerBankEx(playerid, AD_COST, "biaya iklan baris SANews");
	}

	gPlayerAdCooldown[playerid] = now + AD_COOLDOWN;

	new broadcastAd[220];
	format(broadcastAd, sizeof(broadcastAd),
		"[IKLAN SANEWS] %s | Kontak: %s (No. Telp: %d)",
		adText, gPlayerData[playerid][pName], gPlayerData[playerid][pPhone]);

	for (new p = 0; p < MAX_PLAYERS; p++) {
		if (IsPlayerConnected(p) && gPlayerData[p][pLoggedIn]) {
			SendClientMessage(p, 0x4CD964FF, broadcastAd);
		}
	}

	PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
	SendClientMessage(playerid, COLOR_SUCCESS, "SUKSES: Ikan baris Anda berhasil dipublikasikan ke seluruh penjuru kota ($50).");
	return 1;
}
