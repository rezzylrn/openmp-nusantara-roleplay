# Nusantara Roleplay (open.mp)

Gamemode Roleplay modern dan modular yang dibangun untuk platform **open.mp** (Pawn 3.10) dengan standar komunitas roleplay Indonesia. Dilengkapi plugin **MySQL (R41-4)**, parser **sscanf2**, command processor cepat **izcmd**, 8 variasi pekerjaan unik, serta antarmuka TextDraw minimalis.

---

## Arsitektur & Struktur Direktori

Kode sumber diorganisasi secara modular langsung di dalam direktori `gamemodes/`:

```text
├── config.json                     # Konfigurasi server open.mp (legacy_plugins: mysql.so)
├── database.sql                    # Skema database MySQL / MariaDB resmi
├── run-server.sh                   # Script runner otomatis untuk Linux
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
        ├── database.inc            # Koneksi MySQL R41-4 & threaded queries
        ├── textdraws.inc           # Modern Player HUD, Speedometer, & Alert UI
        ├── inventory.inc           # Sistem slot inventaris & penggunaan item
        ├── phone.inc               # Telekomunikasi (Ponsel /call /sms, HT /r, GPS)
        ├── shops.inc               # Toko 24/7 & katalog belanja (/buy)
        ├── player_stats.inc        # Statistik (/stats), level/EXP, lapar & haus
        ├── economy.inc             # Saldo Cash, Bank, ATM, Payday 60 menit, & /pay
        ├── factions.inc            # SAPD (Polisi), SAMD (Medis), SAGOV, & Gangs
        ├── vehicles.inc            # Kendaraan pribadi dinamis, bensin, mesin, bagasi
        ├── jobs.inc                # 8 Pekerjaan (Pemulung, Tebang Kayu, Mancing, Bus, dll.)
        ├── commands_rp.inc         # Perintah roleplay berbasis izcmd (CMD:*)
        ├── admin.inc               # Manajemen staf admin berbasis izcmd (CMD:*)
        └── debug_tests.inc         # Test suite otomatis (11 pengujian sistem)
```

---

## Library & Dependensi Terintegrasi

Gamemode ini mengadopsi standar library populer komunitas SA-MP & open.mp:
- **`izcmd` (Improved ZCMD):** Pemroses perintah O(1) tercepat tanpa iterasi `strcmp`.
- **`sscanf2`:** Parser parameter perintah berkinerja tinggi.
- **`a_mysql` (BlueG / maddinat0r R41-4):** Threaded asynchronous MySQL queries non-blocking.
- **`open.mp`:** C-API component SDK generasi baru untuk San Andreas Multiplayer.

---

## 8 Pekerjaan (Sidejobs)

1. **Pemulung Sampah (Scavenger):** Mengaduk tong sampah di gang kota (`/searchtrash`) untuk mencari botol, kaleng, dan barang bekas, lalu menjualnya ke pengepul besi tua (`/selltrash`).
2. **Penebang Pohon (Lumberjack):** Membawa gergaji mesin (Chainsaw) ke hutan Shady Creeks untuk menebang pohon (`/cutwood`), lalu menjual kayu gelondongan ke Pabrik Sawmill (`/sellwood`).
3. **Nelayan Dermaga (Fisherman):** Membawa joran & umpan ke Dermaga Santa Maria Pier untuk memancing ikan (`/fish`), menimbang hasil tangkapan, dan menjualnya di pasar ikan (`/sellfish`).
4. **Supir Bus Trans-LS (Bus Driver):** Mengemudikan bus kota singgah di 8 halte ikonik Los Santos (Pershing Square, Market, Unity Station, Idlewood).
5. **Kurir Paket (Courier):** Mengantar paket keliling kota menggunakan mobil pick-up Bobcat.
6. **Pembersih Jalan (Sweeper):** Menyapu rute jalan raya kota menggunakan kendaraan Sweeper.
7. **Pengangkut Muatan (Trucker Hauler):** Mengantar gandengan kargo kontainer antarkota dengan truk Linerunner.
8. **Mekanik Kendaraan (Mechanic):** Memperbaiki mesin dan bodi kendaraan pemain lain (`/service`).

---

## Fitur Lainnya

- **Autentikasi & Karakter:** Registrasi dan login berbasis salted SHA-256. Pemain baru otomatis menerima Starter Pack (Snack, Air, Ponsel, GPS).
- **TextDraw Modern:** Player HUD elegan di pojok kanan atas dan Speedometer digital terintegrasi di kanan bawah (KM/J, bahan bakar, status mesin, kunci pintu).
- **Inventaris 12 Slot:** Item fungsional seperti Snack, Air Mineral, Perban, Medkit, Jerigen Bensin, Toolkit Perbaikan, Rokok, Korek, Ponsel, HT, dan GPS.
- **Telekomunikasi:** Panggilan suara interaktif (`/call`, `/p`, `/h`), SMS (`/sms`), Handy Talkie frekuensi 1 - 999 MHz (`/setfreq`, `/r`), serta pengeras suara mobil patroli (`/m`).
- **Faksi Kedinasan:** SAPD (borgol `/cuff`, geledah `/frisk`, tilang `/ticket`, sel `/arrest`), SAMD (rawat `/heal`, pingsan darurat `/revive`), SAGOV (`/settax`, `/gov`), dan Department Radio (`/d`).

---

## Kompilasi & Menjalankan Server

### 1. Import Database MySQL
Jalankan perintah SQL dari file `database.sql`:
```bash
mysql -u root -p < database.sql
```

### 2. Kompilasi AMX
Jalankan kompilasi menggunakan Pawn Compiler (Pawn 3.10):
```bash
qawno/pawncc gamemodes/main.pwn -iqawno/include -ogamemodes/main.amx -d3
```

### 3. Menjalankan Server (Linux)
```bash
chmod +x run-server.sh
./run-server.sh
```

---

## Lisensi
Proyek ini dilisensikan di bawah [MIT License](LICENSE).
Dikembangkan oleh Reja Irawan Alam Syah ([@rezzylrn](https://github.com/rezzylrn)).
