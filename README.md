# Nusantara Roleplay (open.mp)

Gamemode Roleplay modern dan modular yang dibangun untuk platform **open.mp** (Pawn 3.10) dengan standar komunitas roleplay Indonesia (referensi Jogjagamers / JGRP & server roleplay terkemuka). Dilengkapi plugin **MySQL (R41-4)**, parser **sscanf2**, command processor cepat **izcmd**, autentikasi **UCP & Multi-Character**, antarmuka **TextDraw Smartphone & Inventaris Modal**, serta interaksi cepat menggunakan **Tombol Y**.

---

## Arsitektur & Struktur Direktori

Kode sumber diorganisasi secara modular langsung di dalam direktori `gamemodes/`:

```text
├── config.json                     # Konfigurasi server open.mp (legacy_plugins: mysql.so)
├── database.sql                    # Skema database MySQL / MariaDB (ucp_accounts & characters)
├── run-server.sh                   # Script runner otomatis untuk Linux (qemu-i386-static & ramdisk)
├── plugins/
│   └── mysql.so                    # Plugin MySQL R41-4 (statically linked)
├── components/
│   └── sscanf.so                   # Komponen native sscanf open.mp
├── qawno/
│   └── include/                    # Include library (open.mp, a_mysql, sscanf2, izcmd)
└── gamemodes/
    ├── main.pwn                    # Master entry point & callback router
    └── modules/
        ├── header.inc              # Definisi, warna, enums, & konfigurasi global
        ├── database.inc            # Koneksi MySQL R41-4, UCP auth & multi-character queries
        ├── textdraws.inc           # Modern Player HUD, Speedometer, Smartphone UI, & Modal Inventory
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
        └── debug_tests.inc         # Test suite otomatis (11 pengujian sistem)
```

---

## Fitur Utama

### 1. Autentikasi UCP & Multi-Character (JGRP Standard)
- **Login Akun UCP:** Pemain memasukkan akun UCP master.
- **Pengecekan Registrasi Otomatis:**
  - Jika **belum terdaftar**: Meminta input **4-8 digit Kode Keamanan / PIN**, lalu dilanjutkan pembuatan kata sandi.
  - Jika **sudah terdaftar**: Meminta kata sandi UCP terenkripsi salted SHA-256.
- **Multi-Character Selection (Maks. 3 Karakter):**
  - Menampilkan daftar karakter yang terikat ke akun UCP (Nama, Level, Faksi).
  - Opsi `[+] Buat Karakter Baru` jika slot masih tersedia (< 3 slot).
- **Pembuatan Karakter Baru:**
  - **Validasi Nama Ketat (`First_Last`):** Wajib format `Nama_Belakang` (contoh: `Javier_Travors`, `Bryan_Calister`). Tidak mengizinkan angka atau simbol aneh.
  - **Tanggal Lahir:** Format `DD/MM/YYYY` (contoh: `15/08/2000`).
  - **Jenis Kelamin & Karakter Awal:** Pria / Wanita beserta pemilihan skin starter pack.

### 2. Smartphone UI Modern (TextDraw)
- Dibuka dengan `/phone` atau `/hp` (memerlukan item Ponsel di inventaris).
- Tampilan overlay smartphone elegan di sebelah kanan layar:
  - Header status bar: Jam digital, indikator sinyal 5G, dan bar baterai.
  - Nomor telepon aktif & saldo pulsa.
  - Menu aplikasi interaktif:
    1. Panggilan Suara (`/call`)
    2. Pesan Singkat SMS (`/sms`)
    3. Peta Navigasi GPS (`/gps`)
    4. Cek Pulsa & Saldo
    5. Pengaturan Mode Suara (Dering / Hening)
    6. Tutup Ponsel

### 3. Modal Inventaris 12 Slot (TextDraw) & Sistem Give
- Dibuka dengan `/inv` atau `/inventory`.
- Tampilan modal dialog berbasis grid 12 slot di tengah layar dengan penanda slot, nama barang, dan badge jumlah (`x3`).
- **Gunakan Item:** `/use [nomor slot 1-12]`.
- **Fitur Berikan Barang / Uang (`/give`):**
  - `/give [id/nama] cash [jumlah]` -> Menyerahkan uang tunai ke pemain terdekat disertai aksi roleplay `/me`.
  - `/give [id/nama] [nama_barang] [jumlah]` (contoh: `/give 2 snack 1`, `/give 2 water 2`, `/give 2 wood 5`) -> Menyerahkan item dari inventaris ke pemain terdekat.
- **Kartu Identitas KTP (`/showid`):** Memperlihatkan KTP lengkap (Nama, Akun UCP, Tanggal Lahir, Jenis Kelamin, Level) kepada pemain terdekat.

### 4. Interaksi Cepat Dunia dengan Tombol 'Y' (`KEY_YES`)
Pemain tidak perlu repot mengetik perintah saat berada di tempat penting — cukup tekan tombol **Y**:
- **Tong Sampah:** Langsung mengaduk sampah untuk mencari barang rongsokan.
- **Hutan Shady Creeks:** Langsung menebang pohon menggunakan Chainsaw.
- **Dermaga Santa Maria Pier:** Langsung memancing ikan.
- **Pengepul Rongsokan:** Langsung menjual barang bekas.
- **Pabrik Sawmill:** Langsung menjual balok kayu.
- **Pasar Ikan:** Langsung menjual ikan hasil tangkapan.
- **Pusat Buruh / Job Depot:** Membuka menu pemilihan pekerjaan.
- **ATM & Bank:** Membuka menu transaksi perbankan.
- **Toko 24/7:** Membuka katalog belanja kebutuhan pokok.
- **Dealership:** Membuka menu pembelian kendaraan.
- **SPBU:** Langsung mengisi bahan bakar kendaraan.

---

## 8 Pekerjaan (Sidejobs)

1. **Pemulung Sampah (Scavenger):** Mengaduk tong sampah gang kota (`/searchtrash` atau tekan **Y**) untuk mencari rongsokan, lalu menjualnya ke pengepul (`/selltrash` atau tekan **Y**).
2. **Penebang Pohon (Lumberjack):** Membawa Chainsaw ke hutan Shady Creeks untuk menebang pohon (`/cutwood` atau tekan **Y**), lalu menjual kayu ke Sawmill (`/sellwood` atau tekan **Y**).
3. **Nelayan Dermaga (Fisherman):** Membawa joran & umpan ke Dermaga Santa Maria Pier untuk memancing ikan (`/fish` atau tekan **Y**), lalu menjual tangkapan di pasar ikan (`/sellfish` atau tekan **Y**).
4. **Supir Bus Trans-LS (Bus Driver):** Mengemudikan bus kota singgah di 8 halte ikonik Los Santos.
5. **Kurir Paket (Courier):** Mengantar paket keliling kota menggunakan pick-up Bobcat.
6. **Pembersih Jalan (Sweeper):** Menyapu rute jalan raya kota menggunakan Sweeper.
7. **Pengangkut Muatan (Trucker Hauler):** Mengantar kargo trailer kontainer dengan truk Linerunner.
8. **Mekanik Kendaraan (Mechanic):** Memperbaiki mesin dan bodi kendaraan pemain lain (`/service`).

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

## Lisensi
Proyek ini dilisensikan di bawah [MIT License](LICENSE).
Dikembangkan oleh Reja Irawan Alam Syah ([@rezzylrn](https://github.com/rezzylrn)).
