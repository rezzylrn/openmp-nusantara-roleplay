-- Nusantara Roleplay - Database Schema (MySQL R41-4 / MariaDB)
-- Standard: UTF8MB4, InnoDB

CREATE DATABASE IF NOT EXISTS `roleplay` DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
USE `roleplay`;

-- ---------------------------------------------------------------------------
-- Tabel: ucp_accounts (Akun Master UCP Pemain)
-- ---------------------------------------------------------------------------
DROP TABLE IF EXISTS `ucp_accounts`;
CREATE TABLE `ucp_accounts` (
  `id` INT AUTO_INCREMENT PRIMARY KEY,
  `ucp_name` VARCHAR(24) NOT NULL UNIQUE,
  `player_name` VARCHAR(24) NOT NULL DEFAULT '',
  `password` VARCHAR(64) NOT NULL DEFAULT '',
  `salt` VARCHAR(64) NOT NULL DEFAULT '',
  `security_code` VARCHAR(16) NOT NULL DEFAULT '',
  `pin_used` TINYINT DEFAULT 0,
  `created_at` TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ---------------------------------------------------------------------------
-- Tabel: characters (Karakter Roleplay Multi-Character terikat ke UCP)
-- ---------------------------------------------------------------------------
DROP TABLE IF EXISTS `characters`;
CREATE TABLE `characters` (
  `id` INT AUTO_INCREMENT PRIMARY KEY,
  `ucp_id` INT NOT NULL,
  `name` VARCHAR(24) NOT NULL UNIQUE,
  `birthdate` VARCHAR(16) DEFAULT '01/01/2000',
  `gender` INT DEFAULT 1,
  `money` INT DEFAULT 500,
  `bank` INT DEFAULT 1000,
  `level` INT DEFAULT 1,
  `exp` INT DEFAULT 0,
  `admin` INT DEFAULT 0,
  `faction` INT DEFAULT 0,
  `factionrank` INT DEFAULT 0,
  `duty` INT DEFAULT 0,
  `job` INT DEFAULT 0,
  `hunger` FLOAT DEFAULT 100.0,
  `thirst` FLOAT DEFAULT 100.0,
  `skin` INT DEFAULT 0,
  `lastx` FLOAT DEFAULT 1481.0,
  `lasty` FLOAT DEFAULT -1748.0,
  `lastz` FLOAT DEFAULT 13.5,
  `lasta` FLOAT DEFAULT 270.0,
  `interior` INT DEFAULT 0,
  `vworld` INT DEFAULT 0,
  `cuffed` INT DEFAULT 0,
  `warnings` INT DEFAULT 0,
  `kills` INT DEFAULT 0,
  `deaths` INT DEFAULT 0,
  `playhours` INT DEFAULT 0,
  `muted` INT DEFAULT 0,
  `phone` INT DEFAULT 0,
  `phonestatus` INT DEFAULT 1,
  `radiofreq` INT DEFAULT 0,
  `radiostatus` INT DEFAULT 0,
  `injured` INT DEFAULT 0,
  `language` INT DEFAULT 0,
  `tickets` INT DEFAULT 0,
  `ticket_amount` INT DEFAULT 0,
  `drive_lic` INT DEFAULT 0,
  `gun_lic` INT DEFAULT 0,
  `fight_style` INT DEFAULT 4,
  `created_at` TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
  INDEX (`ucp_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ---------------------------------------------------------------------------
-- Tabel: vehicles (Kendaraan Pribadi Dinamis)
-- ---------------------------------------------------------------------------
DROP TABLE IF EXISTS `vehicles`;
CREATE TABLE `vehicles` (
  `id` INT AUTO_INCREMENT PRIMARY KEY,
  `ownerid` INT NOT NULL,
  `model` INT NOT NULL,
  `posx` FLOAT DEFAULT 0.0,
  `posy` FLOAT DEFAULT 0.0,
  `posz` FLOAT DEFAULT 0.0,
  `posa` FLOAT DEFAULT 0.0,
  `color1` INT DEFAULT 1,
  `color2` INT DEFAULT 1,
  `fuel` FLOAT DEFAULT 100.0,
  `health` FLOAT DEFAULT 1000.0,
  `plate` VARCHAR(16) DEFAULT '',
  `engine` INT DEFAULT 0,
  `locked` INT DEFAULT 0,
  `trunk` INT DEFAULT 0,
  `trunk_items` VARCHAR(255) DEFAULT '',
  `trunk_gun` INT DEFAULT 0,
  `trunk_ammo` INT DEFAULT 0,
  `impounded` INT DEFAULT 0,
  INDEX (`ownerid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ---------------------------------------------------------------------------
-- Tabel: bans (Daftar Blokir Akun & IP)
-- ---------------------------------------------------------------------------
DROP TABLE IF EXISTS `bans`;
CREATE TABLE `bans` (
  `id` INT AUTO_INCREMENT PRIMARY KEY,
  `name` VARCHAR(24) NOT NULL,
  `ip` VARCHAR(20) NOT NULL,
  `reason` VARCHAR(96),
  `admin` VARCHAR(24),
  `created` INT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ---------------------------------------------------------------------------
-- Tabel: settings (Konfigurasi Server Global)
-- ---------------------------------------------------------------------------
DROP TABLE IF EXISTS `settings`;
CREATE TABLE `settings` (
  `key` VARCHAR(64) PRIMARY KEY,
  `value` TEXT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ---------------------------------------------------------------------------
-- Tabel: doors (Pintu & Teleportasi Gedung Dinamis Indoor / Outdoor)
-- ---------------------------------------------------------------------------
DROP TABLE IF EXISTS `doors`;
CREATE TABLE `doors` (
  `id` INT AUTO_INCREMENT PRIMARY KEY,
  `name` VARCHAR(48) NOT NULL,
  `ext_x` FLOAT NOT NULL,
  `ext_y` FLOAT NOT NULL,
  `ext_z` FLOAT NOT NULL,
  `ext_a` FLOAT DEFAULT 0.0,
  `ext_interior` INT DEFAULT 0,
  `ext_vw` INT DEFAULT 0,
  `int_x` FLOAT NOT NULL,
  `int_y` FLOAT NOT NULL,
  `int_z` FLOAT NOT NULL,
  `int_a` FLOAT DEFAULT 0.0,
  `int_interior` INT DEFAULT 0,
  `int_vw` INT DEFAULT 0,
  `locked` TINYINT DEFAULT 0,
  `admin_level` INT DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ---------------------------------------------------------------------------
-- Tabel: atms (Mesin ATM Dinamis)
-- ---------------------------------------------------------------------------
DROP TABLE IF EXISTS `atms`;
CREATE TABLE `atms` (
  `id` INT AUTO_INCREMENT PRIMARY KEY,
  `x` FLOAT NOT NULL,
  `y` FLOAT NOT NULL,
  `z` FLOAT NOT NULL,
  `rx` FLOAT DEFAULT 0.0,
  `ry` FLOAT DEFAULT 0.0,
  `rz` FLOAT DEFAULT 0.0,
  `interior` INT DEFAULT 0,
  `vw` INT DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ---------------------------------------------------------------------------
-- Tabel: trash_bins (Tong Sampah Dinamis & Scavenge Rongsokan)
-- ---------------------------------------------------------------------------
DROP TABLE IF EXISTS `trash_bins`;
CREATE TABLE `trash_bins` (
  `id` INT AUTO_INCREMENT PRIMARY KEY,
  `x` FLOAT NOT NULL,
  `y` FLOAT NOT NULL,
  `z` FLOAT NOT NULL,
  `rz` FLOAT DEFAULT 0.0,
  `interior` INT DEFAULT 0,
  `vw` INT DEFAULT 0,
  `capacity` INT DEFAULT 100
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ---------------------------------------------------------------------------
-- Tabel: etle_cameras (Kamera Tilang Elektronik Dinamis / Speed Trap)
-- ---------------------------------------------------------------------------
DROP TABLE IF EXISTS `etle_cameras`;
CREATE TABLE `etle_cameras` (
  `id` INT AUTO_INCREMENT PRIMARY KEY,
  `name` VARCHAR(64) NOT NULL,
  `posx` FLOAT NOT NULL,
  `posy` FLOAT NOT NULL,
  `posz` FLOAT NOT NULL,
  `speed_limit` FLOAT NOT NULL DEFAULT 100.0,
  `ticket_fee` INT NOT NULL DEFAULT 150,
  `created_at` TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ---------------------------------------------------------------------------
-- Tabel: weed_plants (Sistem Pertanian Ganja Bawah Tanah Dinamis)
-- ---------------------------------------------------------------------------
DROP TABLE IF EXISTS `weed_plants`;
CREATE TABLE `weed_plants` (
  `id` INT AUTO_INCREMENT PRIMARY KEY,
  `ownerid` INT NOT NULL,
  `posx` FLOAT NOT NULL,
  `posy` FLOAT NOT NULL,
  `posz` FLOAT NOT NULL,
  `growth_stage` INT NOT NULL DEFAULT 1,
  `plant_time` INT NOT NULL DEFAULT 0,
  `created_at` TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ---------------------------------------------------------------------------
-- Tabel: store_robberies (Log & Cooldown Perampokan Minimarket Kasir)
-- ---------------------------------------------------------------------------
DROP TABLE IF EXISTS `store_robberies`;
CREATE TABLE `store_robberies` (
  `store_id` INT PRIMARY KEY,
  `store_name` VARCHAR(64) NOT NULL,
  `posx` FLOAT NOT NULL,
  `posy` FLOAT NOT NULL,
  `posz` FLOAT NOT NULL,
  `last_robbed` INT NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
