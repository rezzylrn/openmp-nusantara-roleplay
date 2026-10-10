# Nusantara Roleplay (open.mp)

Gamemode Roleplay modern dan modular yang dibangun untuk platform **open.mp** (Pawn 3.10) dengan standar komunitas roleplay Indonesia (referensi Jogjagamers / JGRP & server roleplay terkemuka). Dilengkapi plugin **MySQL (R41-4)**, parser **sscanf2**, command processor cepat **izcmd**, autentikasi **UCP & Multi-Character**, antarmuka **Full Clickable TextDraws (Minim Command)**, serta interaksi cepat menggunakan **Tombol Y & N**.

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
    └── modules/
        ├── header.inc              # Definisi, warna, enums, & konfigurasi global
        ├── database.inc            # Koneksi MySQL R41-4, UCP auth & multi-character queries
        ├── textdraws.inc           # Full Clickable TextDraws (HUD, Speedo, Phone, Inv, Veh, Bank, Menu)
        ├── inventory.inc           # Sistem 12 slot inventaris, penggunaan item, & sistem give
        ├── phone.inc               # Telekomunikasi (Ponsel /call /sms, HT /r, GPS)
        ├── shops.inc               # Toko 24/7 & katalog belanja (/buy)
        ├── player_stats.inc        # Statistik (/stats), level/EXP, lapar & haus
        ├── economy.inc             # Saldo Cash, Bank, ATM, Payday 60 menit, & /pay
        ├── factions.inc            # SAPD (Polisi), SAMD (Medis), SAGOV, & Gangs
        ├── vehicles.inc            # Kendaraan pribadi dinamis, bensin, mesin, bagasi
        ├── jobs.inc                # 8 Pekerjaan & handler interaksi tombol Y (KEY_YES)
        ├── commands_rp.inc         # Perintah roleplay berbasis izcmd (/inv, /give, /showid, dll.)
        ├── admin.inc               # Manajemen staf admin berbasis izcmd (CMD:*)
        ├── anticheat.inc           # Server-side Anti-Cheat (Weapon, Speed, Airbreak, Money, Rapid-Fire)
        ├── properties.inc          # Real Estate & Housing dinamis (Grove St, Idlewood, Vinewood, dll.)
        ├── police_mdc.inc          # SAPD Mobile Data Computer (MDC), Cek Plat, & Speed Radar
        ├── workshop.inc            # Bengkel tuning mekanik (Velg, Hidrolik, Nitro, Spoiler, Cat)
        └── debug_tests.inc         # Test suite otomatis (14 pengujian sistem)
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
    3. `[ 3. GPS Peta ]` -> Membuka menu navigasi lokasi.
    4. `[ 4. Cek Pulsa ]` -> Menampilkan alert status pulsa & kuota.
    5. `[ 5. Mode Suara ]` -> Mengubah mode dering / hening secara instan.
    6. `[ 6. Tutup Ponsel ]` -> Menutup tampilan ponsel.

### 2. Modal Inventaris 12 Slot (Clickable Grid & Action Buttons)
- Dibuka dengan `/inv` atau tombol cepat **N**.
- **12 Slot Grid Interaktif:** Klik pada salah satu slot untuk memilih barang (slot yang dipilih akan disorot warna hijau).
- **Tombol Aksi di Bawah Grid:**
  - `[ GUNAKAN ]` -> Menggunakan item yang sedang dipilih (makan roti, minum, pakai medkit, dll.).
  - `[ BERIKAN ]` -> Membuka dialog cepat untuk menyerahkan item ke pemain terdekat.
  - `[ BUANG ]` -> Membuang barang yang sedang dipilih dari kantong.
  - `[ TUTUP ]` -> Menutup inventaris.

### 3. Dashboard Kontrol Kendaraan (Vehicle Control Panel)
- Muncul otomatis saat mengemudi dan menekan tombol **N** (atau perintah `/vehpanel`).
- Tombol panel interaktif:
  - `[ 1. MESIN: HIDUP / MATI ]` -> Menyalakan atau mematikan mesin.
  - `[ 2. KUNCI PINTU: KUNCI / BUKA ]` -> Mengunci atau membuka pintu kendaraan.
  - `[ 3. LAMPU: NYALA / MATI ]` -> Mengatur sakelar lampu utama.
  - `[ 4. KAP MESIN: BUKA / TUTUP ]` -> Membuka kap mesin depan.
  - `[ 5. BAGASI: BUKA / TUTUP ]` -> Membuka pintu bagasi belakang.
  - `[ X. TUTUP PANEL ]` -> Menutup panel kontrol kendaraan.

### 4. Mesin ATM & Perbankan TextDraw
- Tekan **Y** saat berada di depan ATM atau Bank Nusantara.
- Tampilan kartu ATM modern:
  - Informasi saldo real-time (Saldo Rekening Bank & Uang Tunai di Dompet).
  - Tombol transaksi:
    - `[ 1. TARIK TUNAI ]`
    - `[ 2. SETOR TUNAI ]`
    - `[ 3. TRANSFER DANA ]`
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

## Autentikasi UCP & Multi-Character (JGRP Standard)

- **Login Akun UCP:** Pemain memasukkan akun UCP master.
- **Pengecekan Registrasi Otomatis:**
  - Jika **belum terdaftar**: Meminta input **4-8 digit Kode Keamanan / PIN**, lalu dilanjutkan pembuatan kata sandi.
  - Jika **sudah terdaftar**: Meminta kata sandi UCP terenkripsi **Bcrypt 12-Round Asynchronous** (`samp_bcrypt.so`).
- **Multi-Character Selection (Maks. 3 Karakter):**
  - Menampilkan daftar karakter yang terikat ke akun UCP (Nama, Level, Faksi).
  - Opsi `[+] Buat Karakter Baru` jika slot masih tersedia (< 3 slot).
- **Pembuatan Karakter Baru:**
  - **Validasi Nama Ketat (`First_Last`):** Wajib format `Nama_Belakang` (contoh: `Javier_Travors`, `Bryan_Calister`). Tidak mengizinkan angka atau simbol aneh.
  - **Tanggal Lahir:** Format `DD/MM/YYYY` (contoh: `15/08/2000`).
  - **Jenis Kelamin & Karakter Awal:** Pria / Wanita beserta pemilihan skin starter pack.

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

## Server-Side Anti-Cheat Engine (`anticheat.inc`)
Gamemode ini dilengkapi sistem proteksi server-side aktif untuk mencegah eksploitasi dan cheat:
- **Anti-Weapon Hack:** Memverifikasi kepemilikan senjata resmi. Senjata ilegal (Minigun, RPG, AK47 cheat) langsung di-strip secara instan dan admin diberi notifikasi peringatan.
- **Anti-Teleport & Airbreak:** Memantau perpindahan posisi delta per detik. Pemain yang teleport/airbreak otomatis ditarik kembali ke koordinat sah terakhir.
- **Anti-Speed Hack:** Mendeteksi kecepatan abnormal kendaraan darat (> 240 KM/J) dan meredam kecepatan fisik kendaraan.
- **Anti-Money Hack:** Sinkronisasi saldo uang client secara periodik dengan otoritas server untuk menangkal S0beit / Cheat Engine.
- **Anti-Rapid Fire:** Membatasi interval fisik tembakan senjata (`OnPlayerWeaponShot`) untuk mencegah spam peluru abnormal.

---

## Properti Rumah & Real Estate (`properties.inc`)
- 6 properti awal tersebar di Los Santos (Grove Street, Ganton, Idlewood, Verona Beach, Vinewood Hills, Jefferson).
- Dilengkapi 3D Text Label status dan pickup masuk/keluar interaktif.
- Tekan **Y** di depan pintu: Beli rumah (`/buyhouse`), masuk ke interior, atau kunci/buka pintu (`/lockhouse`).

---

## Kepolisian SAPD: MDC & Speed Radar (`police_mdc.inc`)
- **Mobile Data Computer (`/mdc`):** Akses arsip kriminal dan data tilang di dalam mobil patroli polisi.
- **Cek Plat Nomor (`/checkplate [plat]`):** Mengetahui pemilik kendaraan, model mobil, dan status kepemilikan.
- **Speed Radar Otomatis:** Radar di dashboard mobil patroli memantau kecepatan kendaraan di depan. Jika melaju > 100 KM/J, radar otomatis menampilkan peringatan visual nama kendaraan, kecepatan, dan plat nomor.

---

## Bengkel Tuning Mekanik (`workshop.inc`)
- Area bengkel Idlewood dengan katalog modifikasi kendaraan (`/tune` / `/modveh`):
  - Velg mobil sport (Rimshine, Mega, Shadow, dll.).
  - Sistem suspensi hidrolik (Hydraulics).
  - Tabung Nitrous Oxide 10x.
  - Spoiler balap & knalpot kustom.
  - Pengecatan ulang bodi kendaraan.

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
- **Discord**: `@rezz` (`rezzylrn`)
- **Telegram**: [@rezzylrn](https://t.me/rezzylrn)

---

## Lisensi
Proyek ini dilisensikan di bawah [MIT License](LICENSE).
Dikembangkan oleh **rezzylrn**.
