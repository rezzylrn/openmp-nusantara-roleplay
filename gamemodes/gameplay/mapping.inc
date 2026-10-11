#if defined _INC_mapping
	#endinput
#endif
#define _INC_mapping

/*
==============================================================================
 mapping.inc
 NUSANTARA ROLEPLAY - Clean Modular World Mapping (Zero Traffic Collisions)
==============================================================================
 Menambahkan mapping realistis tanpa menutup jalan atau menghalangi kendaraan:
 1. Pershing Square Civic Plaza & Bank Nusantara (Center Plaza)
 2. All Saints General Hospital (ASGH) Emergency Driveway & Ambulance Bay
 3. Los Santos Police Department (LSPD) Headquarters Secure Compound
 4. Nusantara Mechanical Workshop & Tuning Plaza (Idlewood/Commerce)
 5. Unity Station Public Park & Newbie Arrival Hub
==============================================================================
*/

stock LoadNusantaraMapping()
{
	print("[MAPPING] Memuat mapping dunia Nusantara Roleplay (Clean ground-aligned props)...");

	// =========================================================================
	// 1. PERSHING SQUARE CIVIC PLAZA & BANK NUSANTARA (1481.0, -1748.0, 13.5)
	// =========================================================================
	// Mesin ATM outdoor di dinding Bank
	CreateObject(2942, 1496.8, -1738.5, 13.54, 0.0, 0.0, 0.0, 150.0);
	CreateObject(2942, 1498.2, -1738.5, 13.54, 0.0, 0.0, 0.0, 150.0);

	// Bangku taman warga (Pedestrian walkways)
	CreateObject(1280, 1475.0, -1762.0, 13.50, 0.0, 0.0, 0.0, 150.0);
	CreateObject(1280, 1485.0, -1762.0, 13.50, 0.0, 0.0, 0.0, 150.0);
	CreateObject(1280, 1475.0, -1740.0, 13.50, 0.0, 0.0, 180.0, 150.0);
	CreateObject(1280, 1485.0, -1740.0, 13.50, 0.0, 0.0, 180.0, 150.0);

	// Lampu jalan & pedestrian plaza
	CreateObject(1224, 1470.0, -1750.0, 13.50, 0.0, 0.0, 90.0, 200.0);
	CreateObject(1224, 1490.0, -1750.0, 13.50, 0.0, 0.0, 270.0, 200.0);

	// Tanaman hias / Planters di plaza pejalan kaki
	CreateObject(627, 1472.0, -1755.0, 13.50, 0.0, 0.0, 0.0, 150.0);
	CreateObject(627, 1488.0, -1755.0, 13.50, 0.0, 0.0, 0.0, 150.0);
	CreateObject(628, 1472.0, -1745.0, 13.50, 0.0, 0.0, 0.0, 150.0);
	CreateObject(628, 1488.0, -1745.0, 13.50, 0.0, 0.0, 0.0, 150.0);

	// Tempat sampah pedestrian
	CreateObject(1337, 1479.0, -1762.0, 13.50, 0.0, 0.0, 0.0, 100.0);
	CreateObject(1337, 1481.0, -1740.0, 13.50, 0.0, 0.0, 180.0, 100.0);

	// Tiang bendera Nusantara Center
	CreateObject(1665, 1480.0, -1750.0, 13.50, 0.0, 0.0, 0.0, 200.0);

	// =========================================================================
	// 2. ALL SAINTS GENERAL HOSPITAL (ASGH) EMERGENCY BAY (1172.8, -1323.2, 14.0)
	// =========================================================================
	// Bollard pembatas trotoar jalur ambulans darurat
	CreateObject(1425, 1178.0, -1318.0, 13.80, 0.0, 0.0, 90.0, 150.0);
	CreateObject(1425, 1184.0, -1318.0, 13.80, 0.0, 0.0, 90.0, 150.0);
	CreateObject(1425, 1190.0, -1318.0, 13.80, 0.0, 0.0, 90.0, 150.0);

	// Safety cones di zona darurat
	CreateObject(1238, 1176.0, -1328.0, 13.80, 0.0, 0.0, 0.0, 100.0);
	CreateObject(1238, 1182.0, -1328.0, 13.80, 0.0, 0.0, 0.0, 100.0);

	// Perlengkapan medis luar / Emergency stretcher & storage
	CreateObject(19971, 1174.0, -1315.0, 13.80, 0.0, 0.0, 0.0, 150.0);
	CreateObject(1431, 1173.0, -1313.0, 13.80, 0.0, 0.0, 0.0, 150.0);
	CreateObject(1280, 1188.0, -1312.0, 13.80, 0.0, 0.0, 180.0, 150.0);

	// =========================================================================
	// 3. LSPD HEADQUARTERS SECURE COMPOUND (1545.0, -1675.0, 13.5)
	// =========================================================================
	// Pos jaga gerbang belakang polisi
	CreateObject(3055, 1545.0, -1684.0, 13.50, 0.0, 0.0, 90.0, 200.0);

	// Portal gerbang barrier otomatis
	CreateObject(968, 1544.5, -1680.0, 13.50, 0.0, 0.0, 90.0, 200.0);

	// Menara lampu sorot pengamanan malam
	CreateObject(3878, 1535.0, -1660.0, 13.50, 0.0, 0.0, 0.0, 250.0);
	CreateObject(3878, 1585.0, -1660.0, 13.50, 0.0, 0.0, 0.0, 250.0);

	// Pembatas parkir armada cruiser polisi
	CreateObject(1425, 1550.0, -1665.0, 13.50, 0.0, 0.0, 0.0, 150.0);
	CreateObject(1425, 1555.0, -1665.0, 13.50, 0.0, 0.0, 0.0, 150.0);

	// =========================================================================
	// 4. NUSANTARA MECHANICAL WORKSHOP (1995.0, -2040.0, 13.5)
	// =========================================================================
	// Struktur lift hidrolik bengkel (di samping stall, tidak nabrak spawn kendaraan)
	CreateObject(19969, 1990.0, -2035.0, 13.50, 0.0, 0.0, 0.0, 200.0);

	// Derek mesin / Engine hoist
	CreateObject(19974, 1988.0, -2040.0, 13.50, 0.0, 0.0, 45.0, 150.0);

	// Lemari toolkit baja mekanik
	CreateObject(19968, 1985.0, -2035.0, 13.50, 0.0, 0.0, 90.0, 150.0);

	// Tumpukan ban balap & spare-parts
	CreateObject(19966, 1985.0, -2045.0, 13.50, 0.0, 0.0, 0.0, 150.0);
	CreateObject(19967, 1987.0, -2045.0, 13.50, 0.0, 0.0, 0.0, 150.0);

	// Drum oli & kompresor angin industri
	CreateObject(1430, 2005.0, -2045.0, 13.50, 0.0, 0.0, 0.0, 150.0);
	CreateObject(19972, 2007.0, -2045.0, 13.50, 0.0, 0.0, 0.0, 150.0);

	// =========================================================================
	// 5. UNITY STATION PUBLIC PARK & NEWBIE ARRIVAL HUB (1745.0, -1860.0, 13.5)
	// =========================================================================
	// Papan peta informasi kota Los Santos
	CreateObject(19978, 1740.0, -1865.0, 13.50, 0.0, 0.0, 0.0, 150.0);

	// Mesin penjual minuman & camilan otomatis
	CreateObject(1775, 1742.0, -1855.0, 13.50, 0.0, 0.0, 90.0, 150.0);
	CreateObject(1776, 1744.0, -1855.0, 13.50, 0.0, 0.0, 90.0, 150.0);

	// Telepon umum outdoor
	CreateObject(1216, 1738.0, -1855.0, 13.50, 0.0, 0.0, 90.0, 150.0);

	// Bangku peristirahatan & tempat sampah stasiun
	CreateObject(1280, 1750.0, -1860.0, 13.50, 0.0, 0.0, 180.0, 150.0);
	CreateObject(1337, 1753.0, -1860.0, 13.50, 0.0, 0.0, 0.0, 100.0);

	// =========================================================================
	// 6. PERSHING SQUARE CENTRAL FOUNTAIN & GARDEN (Outdoor Park Center)
	// =========================================================================
	// Air mancur tengah plaza (di taman rumput, tidak di jalan)
	CreateObject(2944, 1481.0, -1750.0, 14.20, 0.0, 0.0, 0.0, 200.0);
	CreateObject(627, 1475.0, -1750.0, 13.50, 0.0, 0.0, 0.0, 150.0);
	CreateObject(628, 1487.0, -1750.0, 13.50, 0.0, 0.0, 0.0, 150.0);
	CreateObject(1280, 1476.0, -1745.0, 13.50, 0.0, 0.0, 135.0, 150.0);
	CreateObject(1280, 1486.0, -1745.0, 13.50, 0.0, 0.0, 225.0, 150.0);

	// =========================================================================
	// 7. BALAI KOTA LOS SANTOS / SAGOV INTERIOR (Interior 3, VW 101)
	// =========================================================================
	// Meja sidang utama & kursi pimpinan
	CreateObject(2185, 384.80, 176.50, 1007.40, 0.0, 0.0, 180.0, 150.0);
	CreateObject(1720, 384.80, 177.50, 1007.40, 0.0, 0.0, 0.0, 150.0);
	CreateObject(1720, 383.50, 175.50, 1007.40, 0.0, 0.0, 180.0, 150.0);
	CreateObject(1720, 386.10, 175.50, 1007.40, 0.0, 0.0, 180.0, 150.0);
	// Tiang bendera kenegaraan di ruang rapat
	CreateObject(1665, 382.00, 178.00, 1007.40, 0.0, 0.0, 0.0, 150.0);
	CreateObject(1665, 387.60, 178.00, 1007.40, 0.0, 0.0, 0.0, 150.0);

	// =========================================================================
	// 8. ALL SAINTS GENERAL HOSPITAL WARD INTERIOR (Interior 5, VW 102)
	// =========================================================================
	// Meja resepsionis dokter & suster
	CreateObject(2185, 1145.00, -1308.00, 1005.00, 0.0, 0.0, 90.0, 150.0);
	CreateObject(1720, 1144.00, -1308.00, 1005.00, 0.0, 0.0, 90.0, 150.0);
	// Kursi tunggu pasien di koridor rawat
	CreateObject(1280, 1148.00, -1302.00, 1005.00, 0.0, 0.0, 180.0, 150.0);
	CreateObject(1280, 1154.00, -1302.00, 1005.00, 0.0, 0.0, 180.0, 150.0);
	// Kotak suplai obat medis & tempat sampah klinis
	CreateObject(1431, 1146.00, -1312.00, 1005.00, 0.0, 0.0, 0.0, 150.0);
	CreateObject(1337, 1144.00, -1314.00, 1005.00, 0.0, 0.0, 0.0, 100.0);

	// =========================================================================
	// 9. LSPD CENTRAL POLICE STATION INTERIOR (Interior 6, VW 103)
	// =========================================================================
	// Meja resepsionis piket pengaduan warga
	CreateObject(2185, 246.50, 68.00, 1002.70, 0.0, 0.0, 180.0, 150.0);
	CreateObject(1720, 246.50, 69.20, 1002.70, 0.0, 0.0, 0.0, 150.0);
	// Kursi saksi / pelapor
	CreateObject(1720, 245.20, 66.80, 1002.70, 0.0, 0.0, 180.0, 150.0);
	CreateObject(1720, 247.80, 66.80, 1002.70, 0.0, 0.0, 180.0, 150.0);
	// Papan pengumuman & tempat sampah
	CreateObject(1337, 243.00, 67.00, 1002.70, 0.0, 0.0, 0.0, 100.0);

	// =========================================================================
	// 10. TOKO SERBA ADA 24/7 IDLEWOOD INTERIOR (Interior 17, VW 104)
	// =========================================================================
	// Counter kasir minimarket
	CreateObject(2185, -28.50, -188.00, 1002.60, 0.0, 0.0, 90.0, 150.0);
	CreateObject(1720, -29.60, -188.00, 1002.60, 0.0, 0.0, 90.0, 150.0);
	// Mesin minuman dingin & camilan
	CreateObject(1775, -24.00, -183.00, 1002.60, 0.0, 0.0, 180.0, 150.0);
	CreateObject(1776, -22.50, -183.00, 1002.60, 0.0, 0.0, 180.0, 150.0);

	print("[MAPPING] 65+ Objek modular Nusantara Roleplay (Indoor & Outdoor) sukses dimuat.");
	return 1;
}
