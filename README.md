# Nusantara Roleplay (open.mp)

Gamemode Roleplay modern dan modular yang dibangun untuk platform **open.mp** (Pawn 3.10) dengan standar server roleplay profesional. Dilengkapi plugin **MySQL (R41-4)**, parser **sscanf2**, command processor cepat **izcmd**, autentikasi **UCP Otomatis & Multi-Character**, sistem registrasi aman berbasis **OTP/PIN Bot**, antarmuka **Full Clickable TextDraws (Minim Command)**, serta interaksi cepat menggunakan **Tombol Y & N**.

---

## Arsitektur & Struktur Direktori

Kode sumber diorganisasi secara modular langsung di dalam direktori `gamemodes/`:

```text
├── config.json                     # Konfigurasi server open.mp (legacy_plugins: mysql.so)
├── database.sql                    # Skema database MySQL / MariaDB (ucp_accounts & characters)
├── run-server.sh                   # Script runner otomatis untuk Linux (qemu-i386-static & ramdisk)
├── plugins/
│   ├── mysql.so                    # Plugin MySQL R41-4 (statically linked)
│   └── samp_bcrypt.so              # Plugin Bcrypt 0.4.1 (asynchronous hashing)
├── components/
│   └── sscanf.so                   # Komponen native sscanf open.mp
├── qawno/
│   └── include/                    # Include library (open.mp, a_mysql, sscanf2, izcmd, samp_bcrypt)
└── gamemodes/
    ├── main.pwn                    # Master entry point, click handler & callback router
    ├── core/                       # Inti gamemode & autentikasi
    │   ├── header.inc              # Definisi, warna, enums, & model data master
    │   ├── database.inc            # Pool koneksi MySQL R41-4 & skema database
    │   └── player_auth.inc         # Autentikasi UCP & multi-char bcrypt
    ├── anticheat/                  # Server-side Anti-Cheat Engine (Modular)
    │   ├── anticheat.inc           # Master controller & administrative alerts
    │   ├── ac_weapons.inc          # Anti-Weapon Spawner, Rapid Fire, Silent-Aim Range
    │   ├── ac_movement.inc         # Anti-Airbreak, Teleport, Noclip, Jetpack
    │   └── ac_vehicles.inc         # Anti-Car Slapper, Crasher, Speedhack, Carjack
    ├── systems/                    # Sistem dunia & ekonomi dinamis
    │   ├── economy.inc             # Saldo Cash, Bank, ATM, Payday 60 menit, & /pay
    │   ├── inventory.inc           # Sistem 12 slot inventaris, use item, & give
    │   ├── properties.inc          # Real Estate & Housing dinamis (Grove St, Idlewood, dll.)
    │   ├── vehicles.inc            # Kendaraan pribadi dinamis, bensin, mesin, bagasi
    │   ├── shops.inc               # Toko 24/7 & katalog belanja
    │   └── workshop.inc            # Bengkel tuning mekanik (Velg, Hidrolik, Nitro, Cat)
    ├── gameplay/                   # Alur permainan & faksi
    │   ├── factions.inc            # SAPD (Polisi), SAMD (Medis), SAGOV, & Gangs
    │   ├── jobs.inc                # 8 Pekerjaan & handler interaksi tombol Y
    │   ├── police_mdc.inc          # SAPD Mobile Data Computer (MDC), Cek Plat, & Speed Radar
    │   ├── phone.inc               # Telekomunikasi (Ponsel, SMS, HT /r, GPS)
    │   └── player_stats.inc        # Statistik (/stats), level/EXP, lapar & haus
    ├── ui/                         # Grafis & TextDraw Interaktif
    │   └── textdraws.inc           # Full Clickable TextDraws (HUD, Speedo, Phone, Inv, Veh, Bank)
    ├── commands/                   # Command processor (izcmd)
    │   ├── commands_rp.inc         # Perintah roleplay warga (/inv, /give, /showid, dll.)
    │   └── admin.inc               # Manajemen staf admin & moderasi
    └── tests/                      # Pengujian & Diagnostik Otomatis
        └── debug_tests.inc         # Automated unit test suite (14 pengujian)
```

---

## Ekosistem Full Clickable TextDraws (Minim Command)

Server ini mengadopsi antarmuka grafis interaktif berbasis kursor mouse (`SelectTextDraw`), sehingga pemain tidak perlu lagi menghafal banyak command teks di chat:

### 1. Smartphone OS Interaktif (Full Click)
- Dibuka dengan `/phone` atau `/hp` atau via menu interaksi.
- Tampilan overlay smartphone elegan di sebelah kanan layar:
  - **Status Bar:** Jam real-time, sinyal 5G, indikator baterai `[||||]`.
  - **Banner Info:** Nomor telepon aktif (`No: 554-xxx`) & status mode suara.
  - **Aplikasi Dapat Diklik Langsung:**
    1. `[ 1. Panggilan ]` -> Membuka dialog input nomor tujuan.
    2. `[ 2. Pesan SMS ]` -> Membuka dialog format SMS.
    3. `[ 3. GPS Peta ]` -> Membuka navigasi 16 titik penting Los Santos (Fasilitas Publik, 8 Sidejobs, Bisnis) lengkap dengan kalkulasi jarak dinamis real-time (`DIALOG_STYLE_TABLIST_HEADERS`).
    4. `[ 4. Cek Pulsa ]` -> Menampilkan alert status pulsa & kuota.
    5. `[ 5. Mode Suara ]` -> Mengubah mode dering / hening secara instan.
    6. `[ 6. Tutup Ponsel ]` -> Menutup tampilan ponsel.

### 2. Modal Inventaris 25 Slot Interaktif (5x5 Grid & 3D Preview Models)
- Dibuka dengan `/inv` atau via Quick Action Menu (tombol **N** on-foot).
- **Desain Modern Dark Slate:** Menggunakan kisi 5x5 (25 Slot Barang) dengan header nama pemain dan bar kapasitas beban (`0.0 / 50.0 kg`).
- **3D Preview Models:** Setiap slot menampilkan model objek 3D interaktif yang berputar halus sesuai tipe barang.
- **25 Slot Grid Interaktif:** Klik pada salah satu slot untuk memilih barang (slot yang dipilih akan disorot warna cyan).
- **Panel Aksi Terintegrasi di Samping Kanan Grid:**
  - `[ Amount ]` -> Menampilkan dan memisahkan kuantitas barang dalam stack.
  - `[ Use ]` -> Menggunakan item yang sedang dipilih (makan snack, minum, pakai medkit, dll.).
  - `[ Give ]` -> Membuka dialog cepat untuk menyerahkan item ke pemain terdekat.
  - `[ Drop ]` -> Membuang barang yang sedang dipilih dari kantong.
  - `[ Close ]` -> Menutup inventaris.

### 3. Dashboard Kontrol Kendaraan & Sistem Bagasi (Trunk Storage)
- Muncul otomatis saat mengemudi dan menekan tombol **N** (atau perintah `/vehpanel`).
- Tombol panel interaktif:
  - `[ 1. MESIN: HIDUP / MATI ]` -> Menyalakan atau mematikan mesin.
  - `[ 2. KUNCI PINTU: KUNCI / BUKA ]` -> Mengunci atau membuka pintu kendaraan.
  - `[ 3. LAMPU: NYALA / MATI ]` -> Mengatur sakelar lampu utama.
  - `[ 4. KAP MESIN: BUKA / TUTUP ]` -> Membuka kap mesin depan.
  - `[ 5. BAGASI: BUKA / TUTUP ]` -> Membuka pintu bagasi belakang.
  - `[ X. TUTUP PANEL ]` -> Menutup panel kontrol kendaraan.
- **Sistem Penyimpanan Bagasi (`/trunk` saat on-foot):**
  - Berdiri di dekat belakang kendaraan (radius 4.0 meter) dan buka bagasi.
  - Menampilkan menu tablist interaktif:
    - **Saldo Kas Bagasi:** Menyimpan atau mengambil uang tunai hingga $5.000.000.
    - **6 Slot Barang Dinamis:** Menyimpan dan mengambil item (makanan, minuman, medkit, toolkit, jerigen bensin, rokok, kayu, rongsokan, dll.).
    - Seluruh item dan uang bagasi tersimpan permanen di database MySQL (kolom `trunk` dan `trunk_items`).

### 4. Mesin ATM & Perbankan TextDraw
- Tekan **Y** saat berada di depan ATM atau Bank Nusantara.
- Tampilan kartu ATM modern:
  - Informasi saldo real-time (Saldo Rekening Bank & Uang Tunai di Dompet).
  - Tombol transaksi:
    - `[ 1. TARIK TUNAI ]` -> Penarikan instan $1 - $1.000.000 (ATM) / $50.000.000 (Bank).
    - `[ 2. SETOR TUNAI ]` -> Setoran instan uang tunai.
    - `[ 3. TRANSFER DANA ]` -> Transfer antar rekening warga dengan resolusi ID/Nama dan proteksi fee admin $10.
    - `[ X. KELUARKAN KARTU ]`

### 5. Menu Interaksi Cepat Warga (Quick Action Menu)
- Tekan tombol **N** saat berjalan kaki (on-foot) atau ketik `/menu`:
  - `[ 1. INVENTARIS BARANG ]`
  - `[ 2. SMARTPHONE / HP ]`
  - `[ 3. KARTU IDENTITAS KTP ]`
  - `[ 4. KONTROL KENDARAAN ]`
  - `[ 5. STATISTIK KARAKTER ]`
  - `[ 6. PANDUAN KOTA ]`
  - `[ X. TUTUP MENU ]`

---

## Layanan Darurat 911 & Sistem Medis Trauma
- **Panggilan Darurat 911 (`/call 911` via Smartphone):**
  - Pemain dapat memilih divisi: **1. Kepolisian (SAPD)** atau **2. Medis / Ambulans (SAMD)**.
  - Mengirimkan deskripsi kejadian darurat lengkap dengan koordinat pelapor real-time.
  - Seluruh petugas yang bertugas dinas (`/duty`) menerima radio dispatch dengan rute GPS instan.
  - Petugas dapat merespons panggilan via `/acceptdispatch [ID]` untuk otomatis memasang checkpoint rute GPS ke lokasi pelapor.
  - Command `/dispatches` untuk melihat seluruh antrean panggilan darurat yang masih aktif.
- **Sistem Medis & Pertolongan Pertama (Trauma System):**
  - **Kondisi Kritis (Knockout / Injured):** Saat HP mencapai 0, karakter masuk mode sekarat (animasi pingsan 90 detik countdown darurat).
  - **Pertolongan Pertama Warga (`/treat [id]`):** Menggunakan Perban (`ITEM_BANDAGE`) untuk membalut luka dan menyadarkan korban kembali dengan 35 HP.
  - **Resusitasi Paramedis (`/revive [id]`):** Khusus tim medis SAMD on-duty atau Admin menggunakan Kotak P3K (`ITEM_MEDKIT`) untuk memulihkan kondisi vital hingga 100% HP lengkap dengan animasi CPR dan audio defibrilator.

---

## Autentikasi UCP & Multi-Character (Sistem OTP Bot)

- **Identifikasi Akun Otomatis:** Nama UCP diambil langsung dari nama player SA-MP saat terhubung (tanpa dialog input nama tambahan).
- **Verifikasi Status Akun:**
  - **Belum terdaftar:** Server otomatis menolak koneksi dan mengarahkan pemain untuk mendaftar melalui Bot resmi Discord/Telegram untuk memperoleh Kode OTP/PIN.
  - **Pendaftaran via Bot (Password Kosong):** Server meminta **Kode Verifikasi (OTP/PIN)** yang dikirim bot. Setelah PIN cocok, pemain diarahkan membuat kata sandi baru (dienkripsi dengan **Bcrypt 12-Round**).
  - **Akun Aktif:** Pemain langsung diminta kata sandi dan diverifikasi asinkron via `samp_bcrypt`.
- **Multi-Character Selection (Maks. 3 Karakter):**
  - Kamera spectate sinematik di atas kota, menonaktifkan tombol native `<< >> Spawn` SA-MP sepenuhnya.
  - Klik slot karakter -> **Langsung load dan spawn ke dunia permainan** (anti-tenggelam dengan `SetPlayerPosFindZ` dan auto-unfreeze 750ms).
- **Pembuatan Karakter Baru:**
  - **Validasi Format Nama (`First_Last`):** Wajib format `Nama_Belakang` (contoh: `Javier_Travors`, `Bryan_Calister`).
  - **Data Kelahiran & Gender:** Input tanggal lahir (`DD/MM/YYYY`) dan opsi gender Pria/Wanita.

---

## Fitur Administrator
- **Teleportasi Klik Peta (Click-on-Map Teleport):** Admin dapat membuka peta (ESC > Map) dan langsung klik kanan di mana saja untuk berteleportasi seketika ke lokasi tujuan. Proteksi anti-cheat otomatis tersinkronisasi.
- **Perintah Staf Modern (`/a`, `/goto`, `/gethere`, `/setskin`, `/veh`, dll.)**: Berbasis `izcmd` O(1) cepat.

---

## Interaksi Tombol 'Y' (`KEY_YES`)

Pemain cukup mendekati objek di dunia lalu menekan tombol **Y**:
- **Tong Sampah:** Langsung memulung sampah.
- **Hutan Shady Creeks:** Langsung menebang pohon menggunakan Chainsaw.
- **Dermaga Santa Maria Pier:** Langsung memancing ikan.
- **Pengepul Rongsokan:** Langsung menjual barang bekas.
- **Pabrik Sawmill:** Langsung menjual balok kayu.
- **Pasar Ikan:** Langsung menjual ikan hasil tangkapan.
- **Pusat Buruh / Job Depot:** Membuka menu pemilihan pekerjaan.
- **ATM & Bank:** Membuka antarmuka ATM TextDraw.
- **Toko 24/7:** Membuka katalog belanja kebutuhan pokok.
- **Dealership:** Membuka katalog pembelian kendaraan.
- **SPBU:** Langsung mengisi bahan bakar kendaraan.

---

## 8 Pekerjaan (Sidejobs)

1. **Pemulung Sampah (Scavenger):** Mengaduk tong sampah gang kota untuk mencari rongsokan, lalu menjualnya ke pengepul.
2. **Penebang Pohon (Lumberjack):** Membawa Chainsaw ke hutan Shady Creeks untuk menebang pohon, lalu menjual kayu ke Sawmill.
3. **Nelayan Dermaga (Fisherman):** Membawa joran & umpan ke Dermaga Santa Maria Pier untuk memancing ikan, lalu menjual tangkapan di pasar ikan.
4. **Supir Bus Trans-LS (Bus Driver):** Mengemudikan bus kota singgah di 8 halte ikonik Los Santos.
5. **Kurir Paket (Courier):** Mengantar paket keliling kota menggunakan pick-up Bobcat.
6. **Pembersih Jalan (Sweeper):** Menyapu rute jalan raya kota menggunakan Sweeper.
7. **Pengangkut Muatan (Trucker Hauler):** Mengantar kargo trailer kontainer dengan truk Linerunner.
8. **Mekanik Kendaraan (Mechanic):** Memperbaiki mesin dan bodi kendaraan pemain lain.

---

## Server-Side Anti-Cheat Engine (`gamemodes/anticheat/`)
Gamemode ini mengadopsi arsitektur anti-cheat server-side modular yang dirancang khusus untuk menangkal berbagai eksploit dari cheat open-source terpopuler (seperti **mod_sa / s0beit**, **UltraFuck MoonLoader**, skrip **CLEO**, dan **RakSAMP packet flooders**):

### 1. Senjata & Amunisi (`ac_weapons.inc`)
- **Anti-Weapon Spawner:** Memverifikasi status kepemilikan senjata resmi di server. Senjata ilegal (Minigun, RPG, AK-47 cheat) langsung di-strip seketika dan admin menerima alert otomatis.
- **Anti-Rapid Fire:** Membatasi interval fisik tembakan per-senjata (contoh: Deagle min. 600ms, Shotgun min. 800ms, Sniper min. 800ms) untuk menggagalkan modifikasi rate-of-fire.
- **Anti-No Reload & Infinite Clip:** Melacak kapasitas magasin nyata per senjata (Deagle 7 butir, M4 50 butir, Shotgun 8 butir). Menembak melampaui kapasitas tanpa jeda animasi reload (> 1.25s) otomatis dibatalkan.
- **Anti-Crasher Bullets & Melee Exploit:** Memblokir paket tembakan berpeluru (`OnPlayerWeaponShot`) yang dikirim dari senjata non-peluru (tinju, pisau, tongkat golf, kamera) serta memfilter koordinat tembakan korup / NaN.

### 2. Pergerakan & Posisi (`ac_movement.inc`)
- **Anti-Airbreak & Foot Speedhack:** Memantau kecepatan gerak horizontal jalan kaki (maksimal 22 m/s). Pemain yang bergerak abnormal otomatis dikembalikan ke posisi sah terakhir (*rubberband*).
- **Anti-Teleport & Click Warp:** Mendeteksi perpindahan posisi instan (> 60 meter per detik) dan langsung membatalkan perpindahan posisi.
- **Anti-Flyhack / Air Walk:** Mendeteksi kenaikan elevasi Z tanpa tangga, parasut, air, atau melompat, serta memulihkan posisi ke darat.
- **Anti-Jetpack:** Memblokir penggunaan jetpack tanpa otorisasi staf admin.

### 3. Kendaraan (`ac_vehicles.inc`)
- **Anti-Car Jacker / Remote Vehicle Warp:** Memvalidasi jarak pemain saat memasuki kendaraan sebagai pengemudi. Jika jarak pemain terhadap pintu mobil > 7.5 meter, pemain dikeluarkan paksa dan dikembalikan ke posisi awal.
- **Anti-Vehicle Speedhack & Nitro Multiplier:** Mengawasi kecepatan fisik mobil darat (> 230 KM/J) dan meredam kecepatan fisik kendaraan seketika.
- **Anti-Car Fly (Mobil Terbang):** Mencegah mobil darat terbang di udara tanpa ramp/stunt dengan menetralkan vektor kecepatan vertikal Z.
- **Anti-Car Slapper & Crasher:** Memvalidasi paket `OnUnoccupiedVehicleUpdate` (mobil kosong). Lompatan posisi mobil kosong > 12m atau manipulasi dari jarak jauh (> 55m) langsung ditolak.

### 4. Pertarungan & Combat (`ac_combat.inc`)
- **Anti-Silent Aim (Camera FOV Vector):** Menghitung *dot product* antara vektor hadap kamera pemain (`GetPlayerCameraFrontVector`) dengan vektor posisi sasaran. Jika tembakan mengarah di luar sudut pandang (< 0.1 dot product / > 84 derajat) atau mengenai punggung lawan tanpa membidik, peluru otomatis dibatalkan.
- **Anti-Hitbox & Offset Manipulation:** Menolak peluru dengan offset model di luar batas wajar hitbox karakter (> 3.8 meter).
- **Anti-Out of Stream Damager:** Menolak damage terhadap pemain yang berada di luar streaming radius penembak.
- **Anti-Fake Kill / Death Flooder:** Mencegah spam paket `OnPlayerDeath` berulang (< 3 detik antar kematian) dan memvalidasi jarak pembunuh (jarak melee maks. 5.5m).
- **Anti-Godmode / Health Lock:** Melacak konsistensi pengurangan HP setelah menerima damage terverifikasi server.

### 5. Tindakan Troll & Exploit (`ac_troll.inc`)
- **Anti-Pickup Teleport:** Memverifikasi jarak pemain saat mengambil pickup (`OnPlayerPickUpPickup`). Pengambilan dari jarak > 5.5 meter otomatis dibatalkan.
- **Anti-Vehicle Tuning Crasher:** Memvalidasi `CARMODTYPE` pada `OnVehicleMod` untuk mencegah injeksi komponen tuning palsu yang dapat meng-crash client pemain lain di sekitar bengkel.

---

## Properti Rumah & Real Estate (`properties.inc`)
- 6 properti awal tersebar di Los Santos (Grove Street, Ganton, Idlewood, Verona Beach, Vinewood Hills, Jefferson).
- Dilengkapi 3D Text Label status dan pickup masuk/keluar interaktif.
- Tekan **Y** di depan pintu: Beli rumah (`/buyhouse`), masuk ke interior, atau kunci/buka pintu (`/lockhouse`).

---

## Kepolisian SAPD: MDC & Speed Radar (`gameplay/police_mdc.inc`)
- **Mobile Data Computer (`/mdc`):** Komputer taktis polisi berbasis easyDialog untuk akses cepat pencarian database warga, cek plat nomor kendaraan, respons antrean panggilan 911 (pasang GPS otomatis), dan pantau surat tilang ETLE.
- **Speed Radar Otomatis:** Radar di dashboard mobil patroli memantau kecepatan kendaraan di depan. Jika melaju > 100 KM/J, radar otomatis menampilkan peringatan visual nama kendaraan, kecepatan, dan plat nomor.

---

## Antarmuka Oldschool & Activity Progress Bar (JGRP Style)
- **Migrasi ke easyDialog (`qawno/include/easyDialog.inc`):**
  - Mengeliminasi ID collision dan bug menu tertukar.
  - Setiap dialog dideklarasikan dengan penamaan unik (`Dialog:Name`), crash-proof dan modular.
- **Radial Action Menu N-Key (`ui/radial_menu.inc`):**
  - Tekan tombol **N** (atau `/menu` / `/radial`):
    - **Di Mobil:** Kontrol mesin, kunci, lampu, kap, bagasi, storage bagasi, argometer taksi, towtruck derek, garasi `/v`, isi bensin, dan plat DMV.
    - **Jalan Kaki:** Inventaris barang `/items`, handphone `/phone`, kendaraan `/v`, statistik `/stats`, dompet & lisensi, lowongan `/job`, darurat 911, bantuan `/help`, serta deteksi otomatis fasilitas terdekat.
- **Activity Progress Bar TextDraw (`ui/progress_bar.inc`):**
  - TextDraw progress bar di tengah bawah layar dengan animasi dan visual feedback persentase saat beraktivitas:
    - Mengisi bensin, menambal ban kempes, mempreteli mobil di chop shop, membobol kunci lockpick, memanen ganja, menggali pasir pantai, memasak ikan bakar, membersihkan grafiti, dan mengais tong sampah.
- **Inventaris Tablist Dialog (`ui/inventory_dialog.inc`):**
  - Menggantikan TextDraw inventaris dengan dialog tablist header klasik JGRP style:
    - Menampilkan daftar 25 slot barang, jumlah, dan opsi Gunakan (Use), Berikan (Give), atau Buang (Drop).
- **Admin Control Panel (`commands/admin_panel.inc`):**
  - Perintah `/admin` / `/adminpanel` untuk staf server: Teleportasi lokasi/pemain, heal, freeze, spawn kendaraan, kick, ban, serta atur cuaca dan jam server.
- **Anti-Cheat Terintegrasi & Auto-Kick:**
  - Auto-kick pemain yang terdeteksi menggunakan program ilegal (Weapon Spawner, Teleport/Warp, Airbreak, Silent Aim, Flyhack, Car Jacker).
  - Integrasi penuh dengan seluruh fitur server sah untuk mencegah false positive.
- **Clean World Mapping:**
  - Mapping custom dinonaktifkan sepenuhnya untuk menjamin integritas geometri dan collision default GTA San Andreas.
- **Smartphone TextDraw OS Modern (OLED Screen & Glassmorphism):**
  - Tampilan visual smartphone realistis di kanan layar:
    - Titanium bezel frame & OLED deep dark screen.
    - Dynamic Island / Notch bar & status bar (Jam digital, sinyal cellular 5G, indikator baterai 100%).
    - Header OS & nomor telepon pemilik.
    - 6 Kartu Aplikasi Interaktif Clickable:
      - `[1] Telepon / Call`: Dial pad panggilan suara (`/call`).
      - `[2] Pesan / SMS`: Inbox & kirim SMS instan (`/sms`).
      - `[3] GPS Maps`: Navigasi rute ke 16 POI kota (`/gps`).
      - `[4] M-Banking`: Mobile banking transfer dana & cek saldo.
      - `[5] Darurat 911`: Panggilan dispatch polisi & paramedis ambulans.
      - `[6] Mode & Bahasa`: Alihkan mode dering/hening & switch bahasa (ID/EN).
    - Tombol navigasi bawah: `[ --- HOME / TUTUP --- ]` untuk menutup ponsel.
- **Street Zone & Compass HUD (Navigasi Wilayah):**
  - Terletak rapi di atas radar peta (sudut kiri bawah layar).
  - Menampilkan arah mata angin (`[ N ]`, `[ NE ]`, `[ S ]`, `[ SW ]`, dll.) dan nama distrik/wilayah secara real-time (Pershing Square, Idlewood, Ganton, Commerce, Market, ASGH Hospital, Jefferson, Unity Station, Willowfield, Ocean Docks, dll.).
- **Overlay Kondisi Kritis (Critical Trauma State):**
  - Muncul otomatis saat karakter sekarat/pingsan (`pInjured`):
    - Overlay gelap kemerahan di tengah layar.
    - Header: `[ KONDISI KRITIS (INJURED) ]` / `[ CRITICAL INJURY STATE ]`.
    - Countdown darurat 90 detik real-time.
    - Panduan darurat bagi korban (`/call 911`) maupun warga sekitar (`/treat [id]`).

---

## Sistem Dinamis Dunia (`systems/doors.inc`, `atms.inc`, `trash.inc`)
- **Pintu Gedung Dinamis (Indoor / Outdoor Door & Entrance System):**
  - Akses masuk/keluar gedung menggunakan tombol **F / ENTER** atau perintah `/enter` & `/exit`.
  - Pickup penanda (1239 exterior / 1318 interior) dan 3D Text Label status pintu.
  - Dilengkapi proteksi kunci (`dLocked`), level otoritas admin/faksi, dan kalibrasi otomatis anti-teleport.
  - Perintah Admin: `/createdoor [nama]`, `/setdoorint [id]`, `/destroydoor [id]`, `/lockdoor [id]`, `/doors`.
  - Preset bawaan: Balai Kota Los Santos (SAGOV), RS All Saints (SAMD ASGH), Kantor Polisi Pusat (SAPD LSPD), Minimarket 24/7 Idlewood, Kantor Arsip Warga.
- **Jaringan Mesin ATM Dinamis (Dynamic ATMs):**
  - Mesin ATM realistis (model 2942) dengan 3D Text Label interaktif.
  - Tekan **Y** atau ketik `/atm` untuk membuka menu perbankan (tarik, setor, transfer, saldo).
  - Perintah Admin: `/createatm`, `/destroyatm [id]`, `/atms`.
  - Tersebar di titik strategis: Pershing Square Bank Wall, ASGH Drop-off, Idlewood Store, Unity Station, Santa Maria Beach Pier.
- **Tong Sampah Dinamis & Pemulung Rongsokan (Dynamic Trash & Scavenge):**
  - Objek tong sampah kota (model 1337) dengan 3D Text Label.
  - Interaksi warga:
    - `/rongsok` / `/scavenge` / `/ngais` / Tekan **Y**: Mengais tong sampah (animasi membungkuk, cooldown 25 detik).
    - Berpeluang mendapatkan: Barang Rongsokan (`ITEM_RECYCLED` x1-x3 untuk dijual ke pengepul `/selltrash`), Uang receh ($5 - $35), Makanan/Minuman kaleng.
    - `/buang` / `/trash`: Membuang barang yang tidak terpakai dari inventaris.
  - Perintah Admin: `/createtrash`, `/destroytrash [id]`, `/trashes`.

---

## Mapping Dunia Modular Los Santos (`gameplay/mapping.inc`)
- 65+ Objek detail yang diselaraskan rapi dengan ketinggian tanah (ground-aligned) dan diletakkan di trotoar/plaza tanpa memotong atau menghalangi jalan lalu lintas kendaraan (**zero collision / tidak nabrak**):
  - **Pershing Square Civic Plaza & Garden:** Air mancur tengah taman plaza (`2944`), tanaman pot bunga besar, mesin ATM outdoor di dinding bank, bangku santai pedestrian, lampu jalan, tempat sampah, dan tiang bendera pusat.
  - **All Saints General Hospital (ASGH) Bay & Medical Ward:** Jalur bollard drop-off ambulans, kerucut pengaman (traffic cones), brankar/stretcher darurat, peti suplai medis, bangku tunggu, meja dokter resepsionis, kursi periksa, dan tirai medis.
  - **LSPD Headquarters Secure Yard & Station Interior:** Pos jaga gerbang belakang polisi, portal gate otomatis, menara lampu sorot pengamanan malam, pembatas parkir armada cruiser, meja resepsionis piket, komputer arsip, kursi interogasi.
  - **Nusantara Mechanical Workshop (Idlewood/Commerce):** Struktur hidrolik mobil, derek mesin, lemari toolkit baja, tumpukan ban balap, drum oli, dan kompresor angin.
  - **Unity Station Public Park & Arrival Hub:** Papan peta informasi kota, mesin penjual snack & minuman otomatis, telepon umum luar ruangan, bangku peristirahatan stasiun.
  - **Toko 24/7 Idlewood Interior:** Counter kasir minimarket, rak belanjaan snack, mesin pendingin minuman otomatis.

---

## Bengkel Tuning Mekanik (`workshop.inc`)
- Area bengkel Idlewood dengan katalog modifikasi kendaraan (`/tune` / `/modveh`):
  - Velg mobil sport (Rimshine, Mega, Shadow, dll.).
  - Sistem suspensi hidrolik (Hydraulics).
  - Tabung Nitrous Oxide 10x.
  - Spoiler balap & knalpot kustom.
  - Pengecatan ulang bodi kendaraan.

---

## Sistem Kamera Tilang Elektronik Dinamis ETLE (`systems/traffic_camera.inc`)
- **Jaringan Kamera Speed Trap Dinamis Berbasis Database (Tabel `etle_cameras`):**
  - Kamera tilang sepenuhnya dinamis — admin dapat memasang atau menghapus kamera langsung dari dalam game.
  - Perintah Admin:
    - `/createetle [batas_kmh] [denda] [nama_lokasi...]`: Menaruh kamera ETLE baru (objek 18880 + 3D Text Label).
    - `/destroyetle [camera_id]`: Menghapus kamera dari dunia dan database MySQL.
    - `/etles`: Menampilkan jumlah kamera tilang aktif.
  - Preset bawaan: Pershing Square (100 KM/J), Commerce Freeway (120 KM/J), Idlewood Boulevard (90 KM/J), Santa Maria Highway (80 KM/J).
- **Mekanisme Pelanggaran Otomatis:**
  - Deteksi kecepatan real-time saat berkendara.
  - Audio shutter jepretan kamera tilang (Sound 1133) & toast notification bilingual (ID/EN).
  - Denda langsung tercatat di profil karakter (`pTickets`, `pTicketAmount`).
  - Broadcast darurat otomatis ke radio polisi SAPD jika melaju kencang (> 135 KM/J).
  - Armada darurat dinas kepolisian dan paramedis ambulans otomatis dikecualikan.
  - Perintah Warga: `/tickets` (cek tagihan surat tilang) dan `/payticket` / `/bayartilang` (pelunasan via bank/cash).

---

## Ekonomi Bawah Tanah & Pekerjaan Ilegal (`gameplay/illegal_activities.inc`)
- **1. Penyelundupan Paket Kontraband Gelap (Contraband Runner Sidejob):**
  - Markas sindikat rahasia di Ocean Docks (`/smuggle` atau `/selundup`).
  - Mengambil paket kontraband gelap (`ITEM_CONTRABAND`) dan mengantarkannya ke titik drop acak di pinggiran kota (El Corona, Alhambra Backalley, Commerce Underground, Ocean Docks Pier).
  - Hadiah pengantaran: Uang tunai kotor ($800 - $1.400) dan cooldown pengantaran 3 menit.
  - Risiko: Jika tertangkap dan digeledah polisi (`/frisk` / `/confiscate`), barang bukti disita.
- **2. Mutilasi Suku Cadang Kendaraan Curian (Illegal Chop Shop):**
  - Lokasi bengkel rahasia peleburan rangka di Ocean Docks (`/chopveh` atau `/chopshop`).
  - Membawa kendaraan apa saja dan membongkarnya menggunakan Linggis Baja (`ITEM_CROWBAR`) atau Toolkit.
  - Hasil mutilasi: Uang tunai peleburan ($750 - $1.600) + 2x-4x Suku Cadang Curian (`ITEM_CARPARTS`).
  - 35% Chance membunyikan alarm dan memicu laporan 911 ke patroli kepolisian SAPD.
- **3. Pembobolan Kunci Kendaraan (Vehicle Lockpicking):**
  - Berdiri di samping mobil terkunci mana saja dan gunakan `/lockpick` atau `/bobol`.
  - Memerlukan Obeng Lockpick (`ITEM_LOCKPICK`). Animasi membongkar kunci, peluang keberhasilan 65%.
  - Jika berhasil, kunci pintu terbuka, alarm mobil berbunyi nyaring, dan sinyal alarm 911 otomatis dikirim ke polisi terdekat.
- **4. Pasar Gelap Bawah Tanah (Black Market Dealer):**
  - Lokasi tersembunyi di gang kumuh Ocean Docks (`/blackmarket`, `/bm`, `/pasargelap`).
  - Katalog Beli:
    - Obeng Lockpick Pembobol Kunci ($150)
    - Linggis Baja / Crowbar ($100)
    - Pistol Rakitan Gelap Colt .45 + 50 Peluru ($1.200)
    - Paket Kontraband Mentah ($250)
  - Penadah Hasil Kejahatan (Menu Jual):
    - Jual Suku Cadang Curian (`ITEM_CARPARTS`): $280 / pcs
    - Jual Paket Kontraband (`ITEM_CONTRABAND`): $650 / pcs
    - Jual Logam Daur Ulang (`ITEM_RECYCLED`): $35 / pcs
- **5. Penyitaan Barang Bukti Kepolisian (`/confiscate` atau `/sita`):**
  - Khusus petugas SAPD on-duty (`/duty`):
  - Menggeledah dan menyita seluruh paket kontraband, suku cadang curian, dan lockpick dari tersangka kejahatan.

---

## Sistem Interaksi & Fitur Kebebasan Kota Lanjutan

### 1. Perampokan Kasir Minimarket 24/7 (`systems/store_robbery.inc`)
- **Mekanisme Perampokan Toko Bersenjata:**
  - Pemain membawa senjata api ke meja kasir toko 24/7 (Idlewood, Commerce, Mulholland) dan menggunakan `/robstore` atau `/rampok`.
  - Kasir panik, memulai timer pembongkaran brankas selama 35 detik.
  - Sinyal darurat diam-diam otomatis dikirim ke seluruh radio petugas kepolisian SAPD (`[ALARM 911 PERAMPOKAN]`).
  - Pemain yang bertahan di kasir hingga timer tuntas mendapatkan tas uang jarahan ($1.200 - $2.500).
  - Cooldown toko 15 menit agar tidak terjadi spamming.

### 2. Pertanian Ganja Bawah Tanah Dinamis (`systems/weed_farm.inc`)
- **Penanaman & Pertumbuhan Ganja (MySQL `weed_plants`):**
  - Pemain membeli benih ganja (`ITEM_WEEDSEED`) di Black Market.
  - Menanam pot di tanah terbuka atau pekarangan rahasia via `/plantweed` atau `/tanam`.
  - Objek pot tanaman hijau (`3409`/`861`) dengan visual 3 tahap pertumbuhan:
    - Tahap 1: Bibit Baru (0–90 detik)
    - Tahap 2: Tunas Daun (90–180 detik)
    - Tahap 3: Siap Panen (180+ detik)
  - Pemanenan via `/harvestweed` / `/panen` menghasilkan 2x–4x Daun Ganja Kering (`ITEM_WEED`).
  - Daun ganja dapat dijual ke penadah Black Market ($110 / pcs) atau dilinting.
  - Petugas kepolisian SAPD on-duty dapat memusnahkan tanaman ilegal dengan `/burnweed` atau `/hanguskan`.

### 3. Detektor Logam & Harta Karun Pantai (`systems/metal_detector.inc`)
- **Pencarian Harta Karun Terpendam:**
  - Pemain membeli Detektor Logam (`ITEM_DETECTOR`) dan Sekop (`ITEM_SHOVEL`) di toko 24/7.
  - Menelusuri pasir pantai (Santa Maria Beach, Verona Beach, Flint Waters) via `/detect` atau `/deteksi`.
  - Audio beeping proximity sound berbunyi semakin kencang saat mendekati titik harta karun.
  - Menggali titik temuan via `/dig` atau `/gali` (animasi menggali + cooldown 15 detik).
  - Tabel Hadiah: Harta Karun Kuno (`ITEM_TREASURE`, laku $250 di Black Market), uang tunai receh tercecer di pasir ($40–$120), kotak peluru militer tua (Desert Eagle 20 peluru), atau logam rongsokan.

### 4. Pendaftaran Plat Nomor Kustom DMV (`systems/dmv_plates.inc`)
- **Kustomisasi Identitas Kendaraan Pribadi:**
  - Pemain yang mengendarai mobil pribadi dapat mendaftarkan plat nomor kustom via `/customplate [plat_baru]` atau `/setplate`.
  - Biaya pendaftaran $500 (dibayar tunai atau via rekening bank).
  - Validasi teks 3–8 karakter alphanumeric kapital dan sensor kata terlarang.
  - Plat langsung ter-render live di kendaraan fisik (`SetVehicleNumberPlate`) dan tersimpan permanen di database MySQL `vehicles`.

### 5. Mesin Vending Trotoar Interaktif (`systems/vending_machines.inc`)
- **Mesin Sprunk & Snack Otomatis di Titik Ramai Pedestrian:**
  - Tersebar di Unity Station, Pershing Square Plaza, RS All Saints, Dermaga Santa Maria, dan SPBU Idlewood.
  - Tekan tombol **Y** atau gunakan `/vending`:
    - Minuman Kaleng Sprunk ($3): Memulihkan dahaga Haus +40%, animasi minum kaleng.
    - Makanan Ringan Cokelat Energi ($4): Memulihkan Lapar +35%, animasi makan.

### 6. Truk Derek Towtruck & Depo Sitaan Kendaraan (`systems/impound.inc`)
- **Derek & Penertiban Kendaraan Terlantar:**
  - Truk derek Towtruck (`525`) untuk Mekanik atau Polisi SAPD on-duty.
  - Mengaitkan kabel derek ke mobil di belakangnya via `/tow` atau `/derek`, dan melepas kaitan via `/untow`.
  - Membawa mobil pelanggar ke Depo Sitaan LSPD dan menyita via `/impound` (petugas mendapat $100 upah jasa derek).
  - Pemilik kendaraan dapat menebus mobil sitaan di loket LSPD via `/claimveh` atau `/tebus` ($200 denda sita).

### 7. Dinding Grafiti & Pembersihan Dinas Kota (`systems/graffiti.inc`)
- **Tagging Wilayah & Grafiti Jalanan:**
  - Beli Kaleng Cat Semprot (`ITEM_SPRAYCAN`) di toko 24/7.
  - Menggambar grafiti wilayah di 6 dinding kota (Ganton, Idlewood, East LS, Glen Park, Verona, El Corona) via `/tag [teks]`.
  - Teks grafiti dan nama penyemprot terpampang live di dinding (3D Label).
  - Warga atau pembersih jalan (Sweeper) dapat membersihkan coretan via `/cleantag` dan mendapat upah kebersihan kota $75.

### 8. Api Unggun & Masak Ikan Luar Ruangan (`systems/cooking.inc`)
- **Perapian Santai & Kuliner Ikan:**
  - Nyalakan api unggun via `/campfire` atau `/apiunggun` (butuh Balok Kayu `ITEM_WOOD` dan Korek `ITEM_LIGHTER`).
  - Api unggun bertahan selama 10 menit di alam terbuka/pantai.
  - Bakar ikan mentah hasil pancingan via `/cook` atau `/bakar` (menghasilkan `ITEM_COOKEDFISH`).
  - Santap ikan bakar via `/makanikan` (memulihkan Lapar +60% dan HP +25%).

### 9. Permainan Dadu Kasino Jalanan (`systems/dice_game.inc`)
- **Judi Taruhan Dadu Adil:**
  - Beli Dadu Kasino (`ITEM_DICE`) di toko 24/7.
  - Tantang lawan taruhan via `/dice [targetid] [nominal_taruhan]` (taruhan $5–$5,000).
  - Kedua pemain melempar dadu 1-6 dengan animasi kasino. Pemenang otomatis menerima uang taruhan dari yang kalah secara aman tanpa rawan penipuan.

### 10. Ujian Surat Izin Mengemudi (SIM) & Razia SAPD (`systems/dmv_exam.inc`)
- **Lisensi Berkendara Resmi Kota:**
  - Ikuti ujian teori keselamatan berkendara di Balai Kota SAGOV via `/dmv` atau `/examsim` ($150 biaya ujian).
  - Jika lulus, karakter resmi memperoleh Surat Izin Mengemudi (SIM) aktif tersimpan di MySQL (`drive_lic`).
  - Petugas kepolisian SAPD on-duty dapat merazia kelayakan berkendara via `/checklic [targetid]`.

### 11. Toko Senjata Resmi Ammu-Nation & Lisensi Senjata Api (`systems/ammunation.inc`)
- **Penjualan Senjata Legal Berizin Resmi:**
  - Kasir resmi Ammu-Nation Willowfield (`/buygun` atau `/ammunation`).
  - Memerlukan Lisensi Senjata Api Resmi (`gun_lic`) yang diterbitkan oleh kepolisian SAPD via `/issuegunlic [targetid]`.
  - Katalog Resmi: Pistol Colt .45 ($600), Desert Eagle .50 ($1.500), Shotgun Berburu ($1.200), Kotak Amunisi Resmi ($200), Rompi Kevlar ($300).

### 12. Layanan Perbengkelan Jalanan & Tambal Ban (`systems/tire_service.inc`)
- **Penanganan Darurat Kendaraan di Pinggir Jalan:**
  - Tambal ban bocor/kempes via `/fixpuncture` atau `/tambalban` (memerlukan `ITEM_REPAIRKIT`). Tekanan angin keempat roda pulih normal.
  - Tuang bahan bakar darurat dari jerigen via `/pourfuel` atau `/tuangbensin` (memerlukan `ITEM_JERRYCAN`). Menambahkan 20 Liter bensin langsung ke tangki mobil.

### 13. Jaringan Iklan Baris SANews (`systems/advertisements.inc`)
- **Publikasi Komersial & Jual-Beli Publik:**
  - Pasang iklan baris via `/ad [teks]` atau `/iklan` (biaya $50, cooldown 45 detik).
  - Otomatis mencantumkan nomor ponsel dan nama pemilik iklan ke seluruh warga kota secara live.

### 14. Pembagian Kunci Rumah & Roommate (`systems/house_sharing.inc`)
- **Akses Delegasi Kunci Hunian:**
  - Pemilik rumah pribadi dapat membagikan kunci cadangan kepada teman via `/sharekey [targetid]` atau `/bagikunci`.
  - Teman pemegang kunci dapat keluar-masuk rumah secara bebas tanpa perlu menunggu pemilik.
  - Pemilik dapat mencabut kembali kunci cadangan kapan saja via `/unsharekey [targetid]`.

### 15. Siklus Cuaca Dinamis & Jam Dunia (`systems/weather_system.inc`)
- **Rotasi Cuaca & Sinkronisasi Waktu BMKG:**
  - 5 Tahap cuaca berkala: Cerah Berawan, Panas Tropis, Hujan Gerimis, Kabut Lembut, dan Hujan Badai Kilat.
  - Waktu server berputar otomatis setiap siklus 10 menit.
  - Warga dapat mengecek prakiraan cuaca terkini via `/weather` atau `/cuaca`.

### 16. Bar Klub Malam & Tes Alkohol Breathalyzer (`systems/bar_alcohol.inc`)
- **Minuman Alkohol & Efek Mabuk:**
  - Bar Alhambra, Ten Green Bottles Ganton, dan Pig Pen (`/bar` atau `/orderdrink`).
  - Menu: Kaleng Bir Dingin ($8), Sloki Whiskey ($25), Shot Vodka Keras ($40).
  - Efek mabuk: Layar goyang realistis (`pDrunkLevel`), animasi sempoyongan.
  - Razia Polisi: Petugas SAPD dapat melakukan tes napas pengemudi mabuk via `/breathalyzer [targetid]` atau `/tesalkohol`.

### 17. Gym Kebugaran & Seni Bela Diri Fight Styles (`systems/gym_fitness.inc`)
- **Pelatihan Gaya Bertarung & Workouts:**
  - Pusat Gym Pantai Santa Maria & Ganton (`/gym` atau `/fightstyle`).
  - Belajar jurus bela diri ($150): Tinju Klasik Boxing, Seni Kung Fu, Kickboxing Muay Thai, Grab Kick. Tersimpan di database MySQL (`fight_style`).
  - Latihan fisik angkat beban barbell via `/workout` atau `/angkatbesi` untuk memulihkan kebugaran & stamina.

### 18. Restoran Cepat Saji Burger Shot, Cluckin' Bell & Pizza (`systems/fast_food.inc`)
- **Kuliner Cepat Saji Ikonik:**
  - Burger Shot Marina, Cluckin' Bell Willowfield, dan Well Stacked Pizza Idlewood (`/orderfood` atau `/pesanmakan`).
  - Menu Kombo: Burger Double Meat ($12, Lapar +50%, HP +20), Cluckin' Fried Chicken ($15, Lapar +60%, HP +25), Pizza Slice ($14, Lapar +55%, HP +25).

### 19. Aksesoris Karakter & Attachable Objects (`systems/accessories.inc`)
- **Fashion & Perlengkapan Fisik Menempel di Tubuh:**
  - Menu kustomisasi via `/accessories` atau `/aksesoris`.
  - Pilihan: Topi Fedora (Kepala), Kacamata Hitam Aviator (Mata), Tas Ransel Petualang (Punggung), serta opsi lepas aksesoris.

### 20. Argometer Tarif Taksi & Panggilan Taksi Warga (`systems/taxi_meter.inc`)
- **Layanan Transportasi Taksi Resmi:**
  - Khusus armada Taxi & Cabbie (Model 420 & 438).
  - Pengemudi taksi menyalakan argometer tarif per kilometer via `/taximeter [tarif]` (default $5/km).
  - Warga yang membutuhkan tumpangan dapat memesan taksi via `/calltaxi` atau `/pesantaksi` yang langsung mengirimkan titik koordinat GPS ke pengemudi aktif.

---

## Stasiun Rental Sepeda & Kendaraan Pemula (`systems/rental.inc`)
- **3 Stasiun Rental Terbuka:**
  - Unity Station (Pintu kedatangan stasiun).
  - Pershing Square Civic Center.
  - Santa Maria Beach Promenade.
- **Katalog Sewa:**
  - Sepeda BMX ($15).
  - Sepeda Gunung MTB ($20).
  - Motor Bebek Faggio ($50).
  - Mobil Pemula Previon ($150).
- Plat nomor dinas `SEWA`, proteksi otomatis 1 kendaraan sewa per warga, dan pembersihan otomatis saat player offline atau via `/unrentveh`.

---

## Loker Dinas Taktis & Persenjataan Faksi (`gameplay/armory.inc`)
- **Loker Kepolisian (SAPD):**
  - Titik loker di dalam markas LSPD HQ (Interior 6) dan halaman luar.
  - Pilihan perlengkapan dinas:
    - 1. Perlengkapan Patroli: Nightstick, Deagle (140 peluru), Shotgun (60 peluru), Rompi Kevlar 100%, Handy Talkie HT.
    - 2. Perlengkapan Berat SWAT: Senapan M4 (300 peluru), MP5 (200 peluru), Sniper Rifle (30 peluru), Rompi Kevlar 100% (Khusus Rank 2+ / Admin).
    - 3. Isi ulang rompi anti-peluru Kevlar 100%.
    - 4. Senjata kejut listrik non-mematikan (Taser Silenced Pistol).
    - 5. Pengembalian & penyimpanan seluruh persenjataan dinas ke loker.
- **Loker Medis (SAMD):**
  - Titik loker di dalam Rumah Sakit ASGH (Interior 5) dan IGD luar.
  - Pilihan pasokan: 5x Perban, 3x Kotak P3K Medkit, Handy Talkie radio darurat (911 MHz), serta pemulihan kondisi fisik & stamina darah ke 100%.

---

## Bagasi Senjata Api Kendaraan (Vehicle Trunk Firearms)
- Integrasi penyimpanan senjata api berpeluru di bagasi mobil (`/trunk`):
  - Simpan senjata yang sedang dipegang ke dalam bagasi mobil (`vTrunkGun` & `vTrunkAmmo`).
  - Ambil senjata dari bagasi mobil ke tangan secara aman.
  - Serialisasi permanen ke tabel MySQL `vehicles` kolom `trunk_gun` dan `trunk_ammo`.

---

## Fitur Gameplay Baru: Tani, Kebersihan, Bobol ATM, Boombox & Forensik
- **Sistem Pertanian Flint County (`systems/farming.inc`):**
  - Toko Tani: Beli bibit gandum, bibit jagung, dan gembor air (`/buyseed`).
  - Menanam di ladang tanah subur (`/plantcrop`), menyiram air (`/watercrop` mempercepat tumbuh 50%), dan memanen (`/harvestcrop`) dengan progress bar.
  - Jual hasil bumi di Lumbung Padi Flint (`/sellcrops`) untuk memperoleh uang tunai bersih ($45 - $60 per karung).
- **Dinas Kebersihan Kota Trashmaster (`gameplay/trash_job.inc`):**
  - Mengemudikan truk sampah Trashmaster (Model 408) dari Depot TPA (`/starttrash`).
  - Mengambil kantong sampah dari perumahan warga (`/pickuptrash`), memasukkan ke mesin belakang truk (`/throwtrash`) dengan progress bar.
  - Setor muatan penuh ke TPA Los Santos (`/unloaddump`) untuk upah $450 tunai.
- **Pembobolan Mesin ATM / Drill Skimming (`systems/atm_robbery.inc`):**
  - Membutuhkan Bor Listrik (`ITEM_DRILL`) atau Lockpick dari Pasar Gelap.
  - Bobol brankas ATM jalanan (`/drillatm`), memicu alarm sensor ke radio kepolisian SAPD, jarahan $700 - $1.450 tunai (cooldown 20 menit).
- **Portable Live Audio Boombox (`systems/boombox.inc`):**
  - Pasang speaker radio di tanah (`/boombox`), putar stasiun radio online atau URL audio stream MP3 real-time ke warga dalam radius 35 meter.
  - Ambil kembali speaker boombox ke inventaris kapan saja (`/pickupboombox`).
- **Forensik Balistik & Selongsong Peluru SAPD (`gameplay/forensics.inc`):**
  - Setiap tembakan senjata api otomatis mencatat selongsong peluru di TKP.
  - Detektif kepolisian SAPD dapat memeriksa TKP (`/casings` / `/forensik`) untuk melacak kaliber peluru, waktu tembakan, dan mengamankan barang bukti (`/collectcasing`).

---

## Mini-Games Interaktif Pekerjaan & Proteksi Anti-Bot Farming (`gameplay/job_minigames.inc`)
- **Tujuan Sistem:** Menghilangkan kebosanan saat grinding pekerjaan dan membasmi script AFK bot farming otomatis secara 100%.
- **Nelayan (Fishing Strike QTE):** Saat umpan disambar ikan, muncul minigame arah tarikan joran (Kiri, Kanan, Sentak Atas). Reaksi seimbang menangkap ikan monster 4 - 13 Kg dan bonus uang tunai.
- **Penebang Pohon (Lumberjack Felling Cut):** Analisa kemiringan batang pohon untuk menentukan sudut potongan gergaji (Undercut 45°, Felling Cut 90°, Cross-cut). Potongan presisi menghasilkan 2x Balok Kayu Kualitas Super.
- **Kurir Paket (Delivery Receipt Captcha):** Saat tiba di alamat rumah pemesan, kurir harus memasukkan 4-Digit Kode Resi unik yang muncul di layar untuk konfirmasi serah terima paket dan mencairkan bonus ketelitian +$35.
- **Mekanik Bengkel (Diagnostic Puzzle):** Saat servis mesin `/service engine`, mekanik menginspeksi gejala suara mesin untuk menentukan penanganan tepat (Busi/Injektor, Radiator, Suspensi) demi memulihkan kendaraan 100% dan mendapat uang tip pelanggan.
- **Pemulung (Scavenger Sorting):** Memilah tumpukan sampah (kardus, kantong tertutup, dasar keranjang) untuk menemukan uang tunai tercecer atau barang antik berharga.
- **Bus Kota & Sweeper (Traffic Safety QTE):** Setiap interval halte rute kota, muncul evaluasi rambu lalu lintas untuk melatih disiplin berkendara dan memberikan bonus keselamatan jalan.

---

## Kompilasi & Menjalankan Server

### 1. Import Database MySQL
```bash
mysql -u root -p < database.sql
```

### 2. Kompilasi AMX (Pawn 3.10)
```bash
qawno/pawncc gamemodes/main.pwn -iqawno/include -ogamemodes/main.amx -d3
```

### 3. Menjalankan Server
```bash
chmod +x run-server.sh
./run-server.sh
```

---

## Kontak & Pengembang
- **GitHub**: [@rezzylrn](https://github.com/rezzylrn)
- **Discord**: `@rezz.910` (`Gracious`)
- **Telegram**: [@gracxq](https://t.me/gracxq)

---

## Lisensi
Proyek ini dilisensikan di bawah [MIT License](LICENSE).
Dikembangkan oleh **rezzylrn**.
