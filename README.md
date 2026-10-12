# Nusantara Roleplay (open.mp)

Gamemode open.mp Roleplay modern dan modular berbahasa Indonesia dengan arsitektur dialog oldschool, ekonomi dinamis, serta proteksi anti-cheat dan anti-bot terintegrasi.

---

## Fitur Utama

- **Autentikasi & Akun:** UCP register/login via bcrypt, OTP verifikasi, multi-karakter (maks. 3 per UCP), direct spawn tanpa macet di class selection.
- **UI & Interaksi:** Dialog berbasis `easyDialog.inc`, N-Key Radial Action Menu (`/menu`), dan real-time Progress Bar TextDraw.
- **Pekerjaan & Anti-Bot Minigames:**
  - Nelayan (`/fish`), Penebang Kayu (`/cutwood`), Kurir Paket (`/job`), Supir Bus & Sweeper, Mekanik Bengkel (`/service`), dan Pemulung Sampah.
  - Setiap pekerjaan dilengkapi puzzle interaktif (Strike QTE, Cut Angle, Captcha Resi, Diagnostik Mesin) untuk mencegah bot farming.
- **Sistem Produksi & Sumber Daya:**
  - **Pertanian Flint County (`/plantcrop`, `/harvestcrop`, `/sellcrops`):** Budidaya gandum & jagung legal.
  - **Pertambangan Hunter Quarry (`/buypickaxe`, `/mine`, `/smelt`, `/sellore`):** Penambangan bijih besi & batu bara serta tungku peleburan batangan besi.
  - **Penyelaman Laut Santa Maria (`/rentscuba`, `/dive`, `/sellpearl`):** Eksplorasi terumbu karang dasar laut untuk memanen mutiara alami.
- **Kendaraan & Transportasi:**
  - Dealership showroom, kepemilikan mobil pribadi (`/v`, `/myveh`), sistem bahan bakar, speedometer, kunci pintu (`/lock`), dan bagasi (`/trunk`).
  - **Laci Dashboard & STNK (`/glovebox`, `/stnk`, `/checkreg`):** Penyimpanan dokumen resmi kepemilikan kendaraan dan razia kepolisian.
  - Dinas Truk Sampah Trashmaster (`/starttrash`, `/pickuptrash`, `/unloaddump`).
- **Properti & Fasilitas:**
  - Pembelian rumah (`/buyhouse`, `/lockhouse`), delegasi kunci sewa (`/sharekey`), pintu interior dinamis, jaringan ATM, SPBU, minimarket 24/7, dan vending machine.
- **Faksi & Penegakan Hukum:**
  - **SAPD:** MDC kepolisian (`/mdc`), radar kecepatan, loker senjata dinas, borgol, penilangan, penahanan sel, dan forensik balistik selongsong peluru (`/casings`, `/forensik`).
  - **SAMD:** Sistem ambulans, perawatan medis darurat (`/treat`), resusitasi defibrillator (`/revive`), dan trauma knockout.
  - **SAGOV:** Penetapan tarif pajak kota (`/settax`) dan pengumuman resmi pemerintah (`/gov`).
- **Aktivitas & Kriminal Bawah Tanah:**
  - Pembobolan ATM memakai bor listrik (`/drillatm`), penyelundupan kontraband, chop shop kendaraan curian, kebun ganja ilegal, pasar gelap, speaker portabel boombox, grafiti semprot, dan dadu jalanan.
- **Anti-Cheat Terintegrasi:**
  - Deteksi otomatis Weapon Hack, No-Reload, Rapid Fire, Silent Aim, Teleport, Speedhack, Car Jacker, Mod Crasher, dan Fake Kill dengan sanksi kick instan tanpa false positive pada aksi resmi server.

---

## Arsitektur Modular Codebase

```
gamemodes/
├── core/         # Header, database MySQL R41-4, auth, event callbacks & dialog router
├── commands/     # Modul perintah per domain (chat, general, vehicle, jobs, factions, property, illegal, activities)
├── systems/      # Sistem dunia (economy, inventory, properties, vehicles, mining, scuba, farming, atms, dll.)
├── gameplay/     # Faksi, pekerjaan kota, minigames anti-bot, forensik balistik, telepon
├── ui/           # Inventory dialog, radial menu N-key, progress bar, speedometer
└── tests/        # Automated diagnostic test runner (67 Unit Tests)
```

---

## Kompilasi & Menjalankan Server

### 1. Database Setup
```bash
mysql -u samp -p roleplay < database.sql
```

### 2. Kompilasi Gamemode
```bash
qawno/pawncc gamemodes/main.pwn -iqawno/include -ogamemodes/main.amx -d3
```

### 3. Menjalankan Server
```bash
chmod +x run-server.sh
./run-server.sh
```

---

## Status Pengujian
- **Automated Tests:** `67 / 67 PASS (100%)`
- **Compiler Status:** `0 Errors, 0 Warnings`

---

## Kontak & Lisensi
- Pengembang: [@rezzylrn](https://github.com/rezzylrn)
- Lisensi: MIT
