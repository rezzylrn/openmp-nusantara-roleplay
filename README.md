# Nusantara Roleplay (open.mp)

Gamemode Roleplay modern dan modular yang dibangun dari nol untuk platform **open.mp** (Pawn 3.10) dengan standar komunitas roleplay Indonesia. Menggunakan database SQLite native bawaan open.mp (`omp_database.inc`), sistem inventaris modular berbasis slot, telekomunikasi interaktif, serta antarmuka TextDraw minimalis.

---

## Arsitektur & Struktur Direktori

Kode sumber diorganisasi secara modular untuk memudahkan pemeliharaan:

```text
Server/
├── config.json                     # Konfigurasi server open.mp
├── run-server.sh                   # Script runner otomatis untuk Linux
├── qawno/
│   └── include/                    # Include library resmi open.mp
└── gamemodes/
    └── roleplay/
        ├── roleplay.pwn            # Main entry point & routing callback
        └── modules/
            ├── header.inc          # Definisi, enums, dialog IDs, & konfigurasi global
            ├── database.inc        # Koneksi SQLite, skema tabel, & migrasi otomatis
            ├── textdraws.inc       # Modern Player HUD, Speedometer, & Alert UI
            ├── inventory.inc       # Sistem slot inventaris & penggunaan item
            ├── phone.inc           # Telekomunikasi (Ponsel /call /sms, HT /r, GPS)
            ├── shops.inc           # Toko 24/7 & katalog pembelian barang (/buy)
            ├── player_stats.inc    # Statistik (/stats), level/EXP, lapar & haus
            ├── economy.inc         # Cash, Bank, ATM, Payday per 60 menit, & /pay
            ├── factions.inc        # SAPD (Polisi), SAMD (Medis), SAGOV, & Gangs
            ├── vehicles.inc        # Kendaraan pribadi dinamis, bensin, mesin, bagasi
            ├── jobs.inc            # Sidejobs (Kurir, Sweeper, Trucker, Mekanik)
            ├── commands_rp.inc     # Perintah roleplay (/me, /do, /b, /w, /s, /l, /id)
            ├── admin.inc           # Manajemen staf admin (/a, /goto, /gethere, /ban)
            └── debug_tests.inc     # Test suite otomatis (11 pengujian modul)
```

---

## Fitur Utama

### 1. Autentikasi & Karakter
- Registrasi dan login berbasis salted SHA-256 (`player_auth.inc`).
- Pemain baru otomatis menerima Starter Pack di inventaris (Roti, Air Mineral, Ponsel, dan GPS).
- Penyimpanan posisi terakhir, interior, virtual world, jam bermain, dan atribut fisik.

### 2. Antarmuka Modern (TextDraw)
- **Player HUD (Kanan Atas):** Branding server, Nama & ID, saldo Cash & Bank, bar Lapar & Haus, serta Level karakter.
- **Speedometer (Kanan Bawah):** Otomatis aktif saat berada di kursi pengemudi. Menampilkan nama model kendaraan, kecepatan digital (KM/J), bar bahan bakar, status mesin (MENYALA / MATI), dan status pintu (TERKUNCI / BEBAS).
- **Banner Notifikasi:** Alert box floating di bagian bawah layar untuk notifikasi server penting.

### 3. Inventaris & Toko 24/7
- Sistem slot inventaris berkapasitas 12 barang (`/inv` atau `/items`).
- Item yang tersedia: Snack Roti, Air Mineral, Perban (+20 HP), Kotak P3K Medkit (+75 HP), Alat Perbaikan Mobil, Jerigen Bensin (20L), Rokok, Korek Api, Ponsel Pintar, Handy Talkie (HT), dan GPS Navigator.
- Toko serba ada 24/7 di Idlewood dan Commerce dengan menu pembelian interaktif (`/buy`).

### 4. Telekomunikasi & Navigasi
- **Ponsel Pintar:** Nomor telepon unik 6-digit. Mendukung panggilan suara interaktif (`/call`, `/p`, `/h`) dan SMS (`/sms`).
- **Handy Talkie (HT):** Mendukung frekuensi 1 - 999 MHz (`/setfreq` dan `/r`). Pesan radio didengar oleh semua pemain yang berada pada frekuensi yang sama.
- **GPS Navigator:** Menandai fasilitas umum di peta radar (Bank, Balai Kota, Rumah Sakit, Kepolisian, SPBU, Toko 24/7, Dealership).

### 5. Kendaraan Pribadi & SPBU
- Dealership kendaraan resmi (`/buyveh`) dengan registrasi plat nomor otomatis (`LS-XXXX`).
- Pengendalian penuh: `/engine` (atau tombol N), `/lights` (atau tombol H), `/hood`, `/trunk`, `/lock`, dan menu terpadu `/v` atau `/myveh`.
- Konsumsi bensin dinamis saat mesin hidup serta pengisian bahan bakar di SPBU (`/refuel`).
- Bagasi kendaraan (`/trunk`) untuk penyimpanan uang tunai dan barang.

### 6. Ekonomi & Gaji (Payday)
- Rekening Bank dan ATM (`/bank`, `/atm`) untuk setor, tarik tunai, dan transfer antar-pemain dengan biaya transaksi.
- Pembayaran tunai langsung antar-pemain (`/pay [id/nama] [jumlah]`).
- Siklus Payday otomatis setiap 60 menit dengan rincian gaji pokok, bonus level, tunjangan faksi, dan potongan pajak daerah.

### 7. Faksi Resmi (Factions)
- **SAPD (Kepolisian):** Perlengkapan dinas (`/duty`), borgol (`/cuff` & `/uncuff`), penggeledahan barang bawaan (`/frisk`), surat tilang (`/ticket`), penahanan sel penjara (`/arrest`), dan pengeras suara mobil patroli (`/m`).
- **SAMD (Medis):** Pertolongan medis (`/heal`), penyadaran kondisi kritis (`/revive`), dan ruang rawat inap UGD rumah sakit.
- **SAGOV (Pemerintah):** Pengaturan tarif pajak daerah (`/settax`) dan siaran pengumuman resmi (`/gov`).
- **Department Radio (`/d`):** Saluran komunikasi koordinasi antar-instansi (SAPD, SAMD, SAGOV).

### 8. Pekerjaan Sampingan (Sidejobs)
- **Kurir Paket:** Rute pengantaran logistik keliling Los Santos menggunakan mobil Bobcat.
- **Pembersih Jalan (Sweeper):** Pembersihan jalan raya kota menggunakan kendaraan Sweeper.
- **Pengangkut Muatan (Trucker):** Pengantaran trailer muatan antar-kota menggunakan truk Linerunner.
- **Mekanik Bengkel:** Memperbaiki mesin dan bodi kendaraan pemain lain.

---

## Kompilasi & Menjalankan Server

### Prasyarat
- open.mp server binary (v1.5.8 atau lebih baru)
- Pawn Compiler (Pawn 3.10)

### Kompilasi AMX
Jalankan perintah berikut dari direktori root server:

```bash
cd Server
qawno/pawncc gamemodes/roleplay/roleplay.pwn -iqawno/include -ogamemodes/roleplay.amx -d3
```

### Menjalankan Server (Linux)
Gunakan script runner yang disediakan:

```bash
chmod +x run-server.sh
./run-server.sh
```

---

## Lisensi
Proyek ini dilisensikan di bawah [MIT License](LICENSE).
Dikembangkan oleh Reja Irawan Alam Syah ([@rezzylrn](https://github.com/rezzylrn)).
