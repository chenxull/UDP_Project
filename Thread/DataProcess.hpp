// -----------------------------------------------------------------------------
// Filename:    DataProcess.hpp
// Revision:    None
// Date:        2018/08/07 - 22:54
// Author:      Haixiang HOU
// Email:       hexid26@outlook.com
// Website:     [NULL]
// Notes:       [NULL]
// -----------------------------------------------------------------------------
// Copyright:   2018 (c) Haixiang
// License:     GPL
// -----------------------------------------------------------------------------
// Version [1.0]
// 根据病人的信息处理数据

#include <thread>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "Patient.hpp"
#include "Config.hpp"

#define PACKET_LENGTH 1452
#define PACKET_SUM_PER_INTERFACE 614400
#define VALID_BYTES_LENGTH_PER_PACKAGE 1400
#define FLAG_BITS_PER_PACKAGE 5

class DataProcess
{
  private:
    /* data */
    long long raw_data_length;
    long long index_data_length;
    long long decode_data_length;
    int start_file_index;
    int end_file_index;
    bool flag_raw_data_ready;
    bool flag_decode_data_ready;
    int order_map[2048] = {0, 64, 128, 192, 1, 65, 129, 193, 2, 66, 130, 194, 3, 67, 131, 195, 4, 68, 132, 196, 5, 69, 133, 197, 6, 70, 134, 198, 7, 71, 135, 199, 8, 72, 136, 200, 9, 73, 137, 201, 10, 74, 138, 202, 11, 75, 139, 203, 12, 76, 140, 204, 13, 77, 141, 205, 14, 78, 142, 206, 15, 79, 143, 207, 16, 80, 144, 208, 17, 81, 145, 209, 18, 82, 146, 210, 19, 83, 147, 211, 20, 84, 148, 212, 21, 85, 149, 213, 22, 86, 150, 214, 23, 87, 151, 215, 24, 88, 152, 216, 25, 89, 153, 217, 26, 90, 154, 218, 27, 91, 155, 219, 28, 92, 156, 220, 29, 93, 157, 221, 30, 94, 158, 222, 31, 95, 159, 223, 32, 96, 160, 224, 33, 97, 161, 225, 34, 98, 162, 226, 35, 99, 163, 227, 36, 100, 164, 228, 37, 101, 165, 229, 38, 102, 166, 230, 39, 103, 167, 231, 40, 104, 168, 232, 41, 105, 169, 233, 42, 106, 170, 234, 43, 107, 171, 235, 44, 108, 172, 236, 45, 109, 173, 237, 46, 110, 174, 238, 47, 111, 175, 239, 48, 112, 176, 240, 49, 113, 177, 241, 50, 114, 178, 242, 51, 115, 179, 243, 52, 116, 180, 244, 53, 117, 181, 245, 54, 118, 182, 246, 55, 119, 183, 247, 56, 120, 184, 248, 57, 121, 185, 249, 58, 122, 186, 250, 59, 123, 187, 251, 60, 124, 188, 252, 61, 125, 189, 253, 62, 126, 190, 254, 63, 127, 191, 255, 256, 320, 384, 448, 257, 321, 385, 449, 258, 322, 386, 450, 259, 323, 387, 451, 260, 324, 388, 452, 261, 325, 389, 453, 262, 326, 390, 454, 263, 327, 391, 455, 264, 328, 392, 456, 265, 329, 393, 457, 266, 330, 394, 458, 267, 331, 395, 459, 268, 332, 396, 460, 269, 333, 397, 461, 270, 334, 398, 462, 271, 335, 399, 463, 272, 336, 400, 464, 273, 337, 401, 465, 274, 338, 402, 466, 275, 339, 403, 467, 276, 340, 404, 468, 277, 341, 405, 469, 278, 342, 406, 470, 279, 343, 407, 471, 280, 344, 408, 472, 281, 345, 409, 473, 282, 346, 410, 474, 283, 347, 411, 475, 284, 348, 412, 476, 285, 349, 413, 477, 286, 350, 414, 478, 287, 351, 415, 479, 288, 352, 416, 480, 289, 353, 417, 481, 290, 354, 418, 482, 291, 355, 419, 483, 292, 356, 420, 484, 293, 357, 421, 485, 294, 358, 422, 486, 295, 359, 423, 487, 296, 360, 424, 488, 297, 361, 425, 489, 298, 362, 426, 490, 299, 363, 427, 491, 300, 364, 428, 492, 301, 365, 429, 493, 302, 366, 430, 494, 303, 367, 431, 495, 304, 368, 432, 496, 305, 369, 433, 497, 306, 370, 434, 498, 307, 371, 435, 499, 308, 372, 436, 500, 309, 373, 437, 501, 310, 374, 438, 502, 311, 375, 439, 503, 312, 376, 440, 504, 313, 377, 441, 505, 314, 378, 442, 506, 315, 379, 443, 507, 316, 380, 444, 508, 317, 381, 445, 509, 318, 382, 446, 510, 319, 383, 447, 511, 512, 576, 640, 704, 513, 577, 641, 705, 514, 578, 642, 706, 515, 579, 643, 707, 516, 580, 644, 708, 517, 581, 645, 709, 518, 582, 646, 710, 519, 583, 647, 711, 520, 584, 648, 712, 521, 585, 649, 713, 522, 586, 650, 714, 523, 587, 651, 715, 524, 588, 652, 716, 525, 589, 653, 717, 526, 590, 654, 718, 527, 591, 655, 719, 528, 592, 656, 720, 529, 593, 657, 721, 530, 594, 658, 722, 531, 595, 659, 723, 532, 596, 660, 724, 533, 597, 661, 725, 534, 598, 662, 726, 535, 599, 663, 727, 536, 600, 664, 728, 537, 601, 665, 729, 538, 602, 666, 730, 539, 603, 667, 731, 540, 604, 668, 732, 541, 605, 669, 733, 542, 606, 670, 734, 543, 607, 671, 735, 544, 608, 672, 736, 545, 609, 673, 737, 546, 610, 674, 738, 547, 611, 675, 739, 548, 612, 676, 740, 549, 613, 677, 741, 550, 614, 678, 742, 551, 615, 679, 743, 552, 616, 680, 744, 553, 617, 681, 745, 554, 618, 682, 746, 555, 619, 683, 747, 556, 620, 684, 748, 557, 621, 685, 749, 558, 622, 686, 750, 559, 623, 687, 751, 560, 624, 688, 752, 561, 625, 689, 753, 562, 626, 690, 754, 563, 627, 691, 755, 564, 628, 692, 756, 565, 629, 693, 757, 566, 630, 694, 758, 567, 631, 695, 759, 568, 632, 696, 760, 569, 633, 697, 761, 570, 634, 698, 762, 571, 635, 699, 763, 572, 636, 700, 764, 573, 637, 701, 765, 574, 638, 702, 766, 575, 639, 703, 767, 768, 832, 896, 960, 769, 833, 897, 961, 770, 834, 898, 962, 771, 835, 899, 963, 772, 836, 900, 964, 773, 837, 901, 965, 774, 838, 902, 966, 775, 839, 903, 967, 776, 840, 904, 968, 777, 841, 905, 969, 778, 842, 906, 970, 779, 843, 907, 971, 780, 844, 908, 972, 781, 845, 909, 973, 782, 846, 910, 974, 783, 847, 911, 975, 784, 848, 912, 976, 785, 849, 913, 977, 786, 850, 914, 978, 787, 851, 915, 979, 788, 852, 916, 980, 789, 853, 917, 981, 790, 854, 918, 982, 791, 855, 919, 983, 792, 856, 920, 984, 793, 857, 921, 985, 794, 858, 922, 986, 795, 859, 923, 987, 796, 860, 924, 988, 797, 861, 925, 989, 798, 862, 926, 990, 799, 863, 927, 991, 800, 864, 928, 992, 801, 865, 929, 993, 802, 866, 930, 994, 803, 867, 931, 995, 804, 868, 932, 996, 805, 869, 933, 997, 806, 870, 934, 998, 807, 871, 935, 999, 808, 872, 936, 1000, 809, 873, 937, 1001, 810, 874, 938, 1002, 811, 875, 939, 1003, 812, 876, 940, 1004, 813, 877, 941, 1005, 814, 878, 942, 1006, 815, 879, 943, 1007, 816, 880, 944, 1008, 817, 881, 945, 1009, 818, 882, 946, 1010, 819, 883, 947, 1011, 820, 884, 948, 1012, 821, 885, 949, 1013, 822, 886, 950, 1014, 823, 887, 951, 1015, 824, 888, 952, 1016, 825, 889, 953, 1017, 826, 890, 954, 1018, 827, 891, 955, 1019, 828, 892, 956, 1020, 829, 893, 957, 1021, 830, 894, 958, 1022, 831, 895, 959, 1023, 1024, 1088, 1152, 1216, 1025, 1089, 1153, 1217, 1026, 1090, 1154, 1218, 1027, 1091, 1155, 1219, 1028, 1092, 1156, 1220, 1029, 1093, 1157, 1221, 1030, 1094, 1158, 1222, 1031, 1095, 1159, 1223, 1032, 1096, 1160, 1224, 1033, 1097, 1161, 1225, 1034, 1098, 1162, 1226, 1035, 1099, 1163, 1227, 1036, 1100, 1164, 1228, 1037, 1101, 1165, 1229, 1038, 1102, 1166, 1230, 1039, 1103, 1167, 1231, 1040, 1104, 1168, 1232, 1041, 1105, 1169, 1233, 1042, 1106, 1170, 1234, 1043, 1107, 1171, 1235, 1044, 1108, 1172, 1236, 1045, 1109, 1173, 1237, 1046, 1110, 1174, 1238, 1047, 1111, 1175, 1239, 1048, 1112, 1176, 1240, 1049, 1113, 1177, 1241, 1050, 1114, 1178, 1242, 1051, 1115, 1179, 1243, 1052, 1116, 1180, 1244, 1053, 1117, 1181, 1245, 1054, 1118, 1182, 1246, 1055, 1119, 1183, 1247, 1056, 1120, 1184, 1248, 1057, 1121, 1185, 1249, 1058, 1122, 1186, 1250, 1059, 1123, 1187, 1251, 1060, 1124, 1188, 1252, 1061, 1125, 1189, 1253, 1062, 1126, 1190, 1254, 1063, 1127, 1191, 1255, 1064, 1128, 1192, 1256, 1065, 1129, 1193, 1257, 1066, 1130, 1194, 1258, 1067, 1131, 1195, 1259, 1068, 1132, 1196, 1260, 1069, 1133, 1197, 1261, 1070, 1134, 1198, 1262, 1071, 1135, 1199, 1263, 1072, 1136, 1200, 1264, 1073, 1137, 1201, 1265, 1074, 1138, 1202, 1266, 1075, 1139, 1203, 1267, 1076, 1140, 1204, 1268, 1077, 1141, 1205, 1269, 1078, 1142, 1206, 1270, 1079, 1143, 1207, 1271, 1080, 1144, 1208, 1272, 1081, 1145, 1209, 1273, 1082, 1146, 1210, 1274, 1083, 1147, 1211, 1275, 1084, 1148, 1212, 1276, 1085, 1149, 1213, 1277, 1086, 1150, 1214, 1278, 1087, 1151, 1215, 1279, 1280, 1344, 1408, 1472, 1281, 1345, 1409, 1473, 1282, 1346, 1410, 1474, 1283, 1347, 1411, 1475, 1284, 1348, 1412, 1476, 1285, 1349, 1413, 1477, 1286, 1350, 1414, 1478, 1287, 1351, 1415, 1479, 1288, 1352, 1416, 1480, 1289, 1353, 1417, 1481, 1290, 1354, 1418, 1482, 1291, 1355, 1419, 1483, 1292, 1356, 1420, 1484, 1293, 1357, 1421, 1485, 1294, 1358, 1422, 1486, 1295, 1359, 1423, 1487, 1296, 1360, 1424, 1488, 1297, 1361, 1425, 1489, 1298, 1362, 1426, 1490, 1299, 1363, 1427, 1491, 1300, 1364, 1428, 1492, 1301, 1365, 1429, 1493, 1302, 1366, 1430, 1494, 1303, 1367, 1431, 1495, 1304, 1368, 1432, 1496, 1305, 1369, 1433, 1497, 1306, 1370, 1434, 1498, 1307, 1371, 1435, 1499, 1308, 1372, 1436, 1500, 1309, 1373, 1437, 1501, 1310, 1374, 1438, 1502, 1311, 1375, 1439, 1503, 1312, 1376, 1440, 1504, 1313, 1377, 1441, 1505, 1314, 1378, 1442, 1506, 1315, 1379, 1443, 1507, 1316, 1380, 1444, 1508, 1317, 1381, 1445, 1509, 1318, 1382, 1446, 1510, 1319, 1383, 1447, 1511, 1320, 1384, 1448, 1512, 1321, 1385, 1449, 1513, 1322, 1386, 1450, 1514, 1323, 1387, 1451, 1515, 1324, 1388, 1452, 1516, 1325, 1389, 1453, 1517, 1326, 1390, 1454, 1518, 1327, 1391, 1455, 1519, 1328, 1392, 1456, 1520, 1329, 1393, 1457, 1521, 1330, 1394, 1458, 1522, 1331, 1395, 1459, 1523, 1332, 1396, 1460, 1524, 1333, 1397, 1461, 1525, 1334, 1398, 1462, 1526, 1335, 1399, 1463, 1527, 1336, 1400, 1464, 1528, 1337, 1401, 1465, 1529, 1338, 1402, 1466, 1530, 1339, 1403, 1467, 1531, 1340, 1404, 1468, 1532, 1341, 1405, 1469, 1533, 1342, 1406, 1470, 1534, 1343, 1407, 1471, 1535, 1536, 1600, 1664, 1728, 1537, 1601, 1665, 1729, 1538, 1602, 1666, 1730, 1539, 1603, 1667, 1731, 1540, 1604, 1668, 1732, 1541, 1605, 1669, 1733, 1542, 1606, 1670, 1734, 1543, 1607, 1671, 1735, 1544, 1608, 1672, 1736, 1545, 1609, 1673, 1737, 1546, 1610, 1674, 1738, 1547, 1611, 1675, 1739, 1548, 1612, 1676, 1740, 1549, 1613, 1677, 1741, 1550, 1614, 1678, 1742, 1551, 1615, 1679, 1743, 1552, 1616, 1680, 1744, 1553, 1617, 1681, 1745, 1554, 1618, 1682, 1746, 1555, 1619, 1683, 1747, 1556, 1620, 1684, 1748, 1557, 1621, 1685, 1749, 1558, 1622, 1686, 1750, 1559, 1623, 1687, 1751, 1560, 1624, 1688, 1752, 1561, 1625, 1689, 1753, 1562, 1626, 1690, 1754, 1563, 1627, 1691, 1755, 1564, 1628, 1692, 1756, 1565, 1629, 1693, 1757, 1566, 1630, 1694, 1758, 1567, 1631, 1695, 1759, 1568, 1632, 1696, 1760, 1569, 1633, 1697, 1761, 1570, 1634, 1698, 1762, 1571, 1635, 1699, 1763, 1572, 1636, 1700, 1764, 1573, 1637, 1701, 1765, 1574, 1638, 1702, 1766, 1575, 1639, 1703, 1767, 1576, 1640, 1704, 1768, 1577, 1641, 1705, 1769, 1578, 1642, 1706, 1770, 1579, 1643, 1707, 1771, 1580, 1644, 1708, 1772, 1581, 1645, 1709, 1773, 1582, 1646, 1710, 1774, 1583, 1647, 1711, 1775, 1584, 1648, 1712, 1776, 1585, 1649, 1713, 1777, 1586, 1650, 1714, 1778, 1587, 1651, 1715, 1779, 1588, 1652, 1716, 1780, 1589, 1653, 1717, 1781, 1590, 1654, 1718, 1782, 1591, 1655, 1719, 1783, 1592, 1656, 1720, 1784, 1593, 1657, 1721, 1785, 1594, 1658, 1722, 1786, 1595, 1659, 1723, 1787, 1596, 1660, 1724, 1788, 1597, 1661, 1725, 1789, 1598, 1662, 1726, 1790, 1599, 1663, 1727, 1791, 1792, 1856, 1920, 1984, 1793, 1857, 1921, 1985, 1794, 1858, 1922, 1986, 1795, 1859, 1923, 1987, 1796, 1860, 1924, 1988, 1797, 1861, 1925, 1989, 1798, 1862, 1926, 1990, 1799, 1863, 1927, 1991, 1800, 1864, 1928, 1992, 1801, 1865, 1929, 1993, 1802, 1866, 1930, 1994, 1803, 1867, 1931, 1995, 1804, 1868, 1932, 1996, 1805, 1869, 1933, 1997, 1806, 1870, 1934, 1998, 1807, 1871, 1935, 1999, 1808, 1872, 1936, 2000, 1809, 1873, 1937, 2001, 1810, 1874, 1938, 2002, 1811, 1875, 1939, 2003, 1812, 1876, 1940, 2004, 1813, 1877, 1941, 2005, 1814, 1878, 1942, 2006, 1815, 1879, 1943, 2007, 1816, 1880, 1944, 2008, 1817, 1881, 1945, 2009, 1818, 1882, 1946, 2010, 1819, 1883, 1947, 2011, 1820, 1884, 1948, 2012, 1821, 1885, 1949, 2013, 1822, 1886, 1950, 2014, 1823, 1887, 1951, 2015, 1824, 1888, 1952, 2016, 1825, 1889, 1953, 2017, 1826, 1890, 1954, 2018, 1827, 1891, 1955, 2019, 1828, 1892, 1956, 2020, 1829, 1893, 1957, 2021, 1830, 1894, 1958, 2022, 1831, 1895, 1959, 2023, 1832, 1896, 1960, 2024, 1833, 1897, 1961, 2025, 1834, 1898, 1962, 2026, 1835, 1899, 1963, 2027, 1836, 1900, 1964, 2028, 1837, 1901, 1965, 2029, 1838, 1902, 1966, 2030, 1839, 1903, 1967, 2031, 1840, 1904, 1968, 2032, 1841, 1905, 1969, 2033, 1842, 1906, 1970, 2034, 1843, 1907, 1971, 2035, 1844, 1908, 1972, 2036, 1845, 1909, 1973, 2037, 1846, 1910, 1974, 2038, 1847, 1911, 1975, 2039, 1848, 1912, 1976, 2040, 1849, 1913, 1977, 2041, 1850, 1914, 1978, 2042, 1851, 1915, 1979, 2043, 1852, 1916, 1980, 2044, 1853, 1917, 1981, 2045, 1854, 1918, 1982, 2046, 1855, 1919, 1983, 2047};
    std::string cur_index_id;

    /* function */
    inline unsigned int read_as_int(char *ptr);
    int read_pcap_2_memory(std::string filepath, char *packets_raw, char *index_raw);
    void check_udp_packets_order(std::string filepath);
    inline void convert_14bits_to_16bits(unsigned char *buffer, long long raw_data_index, long long decode_data_index);
    void process_tables(int start_table_index, int tables_per_thread, int board_sum);
    inline std::string get_data_part();

  public:
    /* data */
    Config config;
    Patient patient;
    char *raw_data;
    char *index_data;
    int16_t *decode_data;
    /* function */
    DataProcess(Config in_config, Patient in_patient);
    ~DataProcess();
    int load_slice(std::string index);
    int check_index_data();
    int map_raw_2_decode();
    int save_decode_data();
    int save_decode_tables();
};

DataProcess::DataProcess(Config in_config, Patient in_patient) : config(in_config), patient(in_patient)
{
    start_file_index = config.raw_data_ids[0];
    end_file_index = config.raw_data_ids[config.raw_data_ids.size() - 1];

    flag_raw_data_ready = false;
    flag_decode_data_ready = false;
    patient.print_info();
}

DataProcess::~DataProcess()
{
    free(raw_data);
    free(decode_data);
    free(index_data);
}

int DataProcess::load_slice(std::string index)
// 获得读取文件的范围 start_file_index 和 end_file_index
// 利用多线程把文件加载进入内存 *raw_data 和 *index_data
// 返回值 1 加载成功 其它：加载失败。
{
    cur_index_id = index;
    std::string filepath;
    raw_data_length = (end_file_index - start_file_index + 1) * (long long)PACKET_SUM_PER_INTERFACE * VALID_BYTES_LENGTH_PER_PACKAGE;
    index_data_length = (end_file_index - start_file_index + 1) * (long long)PACKET_SUM_PER_INTERFACE * FLAG_BITS_PER_PACKAGE;

    raw_data = (char *)std::calloc(raw_data_length, 1);
    index_data = (char *)std::calloc(index_data_length, 1);

    if (raw_data == NULL || index_data == NULL)
    {
        std::cout << "ERROR :: Malloc data for raw_data or index_data failed." << std::endl;
        exit(-1);
    }

    boost::asio::thread_pool threadpool(config.program_pcap_read_threads_sum);

    for (int i = start_file_index; i <= end_file_index; i++)
    {
        filepath = patient.path_of_pcapfile_from_slice(index, i);

        if (filepath == "error file")
        {
            exit(-1);
        }

        boost::asio::post(threadpool, boost::bind(&DataProcess::read_pcap_2_memory, this, filepath, &raw_data[(i - start_file_index) * (long long)PACKET_SUM_PER_INTERFACE * VALID_BYTES_LENGTH_PER_PACKAGE], &index_data[(i - start_file_index) * (long long)PACKET_SUM_PER_INTERFACE * FLAG_BITS_PER_PACKAGE]));
    }

    threadpool.join();

    return 1;
}

int DataProcess::check_index_data()
// 利用 read_pcap_2_memory 读取的 index_data 内容识别是否有 UDP 包乱序
// 检测完成，没有问题返回 1
{
    long long cnt = 0;
    long long check_no = 0;
    long long temp = 0;
    while (check_no < (long long)PACKET_SUM_PER_INTERFACE * (end_file_index - start_file_index + 1))
    {
        cnt = 0;
        temp = (int8_t)index_data[5 * check_no + 1];

        if (temp != check_no % 75)
        {
            return -1;
        }

        check_no++;
    }
    return 1;
}

inline unsigned int DataProcess::read_as_int(char *ptr)
// read 4 char as int
{
    unsigned int *int_ptr = (unsigned int *)ptr;
    return *int_ptr;
}

int DataProcess::read_pcap_2_memory(std::string filepath, char *packets_raw, char *index_raw)
// 把 filepath 指向的文件读取到 packets_raw
// packets_raw 保存的内容为 1400*614400 Bytes
// index_raw 保存的内容为 10*614400 Bytes
{
    std::ifstream openfile(filepath.c_str(), std::ifstream::binary | std::ifstream::ate);
    if (!openfile)
    {
        std::cout << "ERROR :: open pcap file error!" << std::endl;
        std::cout << "ERROR :: " << filepath << std::endl;
        return 0;
    }
    long long int filesize = openfile.tellg();
    openfile.seekg(0, openfile.beg);

    // 为 file_buffer 申请空间，并把 filepath 的数据载入内存
    char *file_buffer = (char *)std::malloc(filesize);
    if (file_buffer == NULL)
    {
        std::cout << "ERROR :: Malloc data for buffer failed." << std::endl;
        return 0;
    }
    openfile.read(file_buffer, filesize);
    if (openfile.peek() == EOF)
    {
        openfile.close();
    }
    else
    {
        std::cout << "ERROR :: Read file error." << std::endl;
    }

    // 在 file_buffer 中把实际数据和标记为分别复制到 packets_raw 和 index_raw
    unsigned int block_head_index = 0;
    unsigned int block_length = read_as_int(&file_buffer[block_head_index + 4]);
    int packet_cnt = 0;
    int packet_error_cnt = 0;

    while (block_head_index < filesize)
    {
        block_head_index += block_length;
        block_length = read_as_int(&file_buffer[block_head_index + 4]);

        if (read_as_int(&file_buffer[block_head_index]) != 6)
        {
            continue;
        }
        else if (read_as_int(&file_buffer[block_head_index]) == 6)
        {
            ++packet_cnt;
            if (read_as_int(&file_buffer[block_head_index + 20]) != PACKET_LENGTH)
            {
                ++packet_error_cnt;
            }
            // 把数据读取到 * packets_raw 和 * index_raw
            // From block_head_index + 74 To block_head_index + 1473
            // From block_head_index + 70 To block_head_index + 73
            // From block_head_index + 1474 To block_head_index + 1479
            memcpy(&packets_raw[(packet_cnt - 1) * VALID_BYTES_LENGTH_PER_PACKAGE], &file_buffer[block_head_index + 74], VALID_BYTES_LENGTH_PER_PACKAGE);
            memcpy(&index_raw[(packet_cnt - 1) * 5], &file_buffer[block_head_index + 71], 2);
            memcpy(&index_raw[(packet_cnt - 1) * 5 + 2], &file_buffer[block_head_index + 1475], 3);
        }
    }

    // 销毁 file_buffer
    std::free(file_buffer);
    if (packet_error_cnt == 0 && packet_cnt == PACKET_SUM_PER_INTERFACE)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

inline void DataProcess::convert_14bits_to_16bits(unsigned char *buffer, long long raw_data_index, long long decode_data_index)
// 从 pointer_7 读取 7 个字节（4个 samples，转换为 4个 int16_t （16bits）
// 循环8次，转换32个 samples，正好对应32个通道的 samples
// 调用一次转换 56 bytes
{
    unsigned char *ptr;
    unsigned char *pointer_7 = (unsigned char *)&raw_data[raw_data_index];
    int16_t *pointer_16 = &decode_data[decode_data_index];

    buffer[0] = pointer_7[5];
    buffer[1] = pointer_7[6];
    buffer[2] = pointer_7[7];
    buffer[3] = pointer_7[0];
    buffer[4] = pointer_7[1];
    buffer[5] = pointer_7[2];
    buffer[6] = pointer_7[3];
    buffer[7] = pointer_7[14];
    buffer[8] = pointer_7[15];
    buffer[9] = pointer_7[8];
    buffer[10] = pointer_7[9];
    buffer[11] = pointer_7[10];
    buffer[12] = pointer_7[11];
    buffer[13] = pointer_7[4];
    buffer[14] = pointer_7[23];
    buffer[15] = pointer_7[16];
    buffer[16] = pointer_7[17];
    buffer[17] = pointer_7[18];
    buffer[18] = pointer_7[19];
    buffer[19] = pointer_7[12];
    buffer[20] = pointer_7[13];
    buffer[21] = pointer_7[24];
    buffer[22] = pointer_7[25];
    buffer[23] = pointer_7[26];
    buffer[24] = pointer_7[27];
    buffer[25] = pointer_7[20];
    buffer[26] = pointer_7[21];
    buffer[27] = pointer_7[22];
    buffer[28] = pointer_7[33];
    buffer[29] = pointer_7[34];
    buffer[30] = pointer_7[35];
    buffer[31] = pointer_7[28];
    buffer[32] = pointer_7[29];
    buffer[33] = pointer_7[30];
    buffer[34] = pointer_7[31];
    buffer[35] = pointer_7[42];
    buffer[36] = pointer_7[43];
    buffer[37] = pointer_7[36];
    buffer[38] = pointer_7[37];
    buffer[39] = pointer_7[38];
    buffer[40] = pointer_7[39];
    buffer[41] = pointer_7[32];
    buffer[42] = pointer_7[51];
    buffer[43] = pointer_7[44];
    buffer[44] = pointer_7[45];
    buffer[45] = pointer_7[46];
    buffer[46] = pointer_7[47];
    buffer[47] = pointer_7[40];
    buffer[48] = pointer_7[41];
    buffer[49] = pointer_7[52];
    buffer[50] = pointer_7[53];
    buffer[51] = pointer_7[54];
    buffer[52] = pointer_7[55];
    buffer[53] = pointer_7[48];
    buffer[54] = pointer_7[49];
    buffer[55] = pointer_7[50];

    // TODO :: 需要根据条带化顺序修改 pointer_16 的索引位置
    for (int i = 0; i < 8; i++)
    {
        ptr = &buffer[i * 7];
        pointer_16[i * 4] = (int16_t)(((ptr[5] << 10) | (ptr[6] << 2)) & 0xfffc) / 4;
        pointer_16[i * 4 + 1] = (int16_t)(((ptr[3] << 12) | (ptr[4] << 4) | (ptr[5] >> 4)) & 0xfffc) / 4;
        pointer_16[i * 4 + 2] = (int16_t)(((ptr[1] << 14) | (ptr[2] << 6) | (ptr[3] >> 2)) & 0xfffc) / 4;
        pointer_16[i * 4 + 3] = (int16_t)(((ptr[0] << 8) | (ptr[1])) & 0xfffc) / 4;
    }

    return;
}

void DataProcess::process_tables(int start_table_index, int tables_per_thread, int board_sum)
{
    unsigned char *buffer;
    buffer = (unsigned char *)std::calloc(56, 1);
    int cur_table_ordered_index;
    for (int cur_table_index = start_table_index; cur_table_index < tables_per_thread + start_table_index; cur_table_index++)
    {
        // 使用 order_map 排序
        cur_table_ordered_index = order_map[cur_table_index];
        // 不使用 order_map 排序
        // cur_table_ordered_index = cur_table_index;
        for (int double_column_index = 0; double_column_index < 1875; double_column_index++)
        {

            for (int board_id = 0; board_id < board_sum; board_id++)
            {
                convert_14bits_to_16bits(&buffer[0], (long long)860160000 * (0 + board_id * 4) + cur_table_index * (long long)420000 + double_column_index * 56, cur_table_ordered_index * board_sum * (long long)960000 + double_column_index * board_sum * (long long)512 + board_id * 256);
                convert_14bits_to_16bits(&buffer[0], (long long)860160000 * (2 + board_id * 4) + cur_table_index * (long long)420000 + double_column_index * 56, cur_table_ordered_index * board_sum * (long long)960000 + double_column_index * board_sum * (long long)512 + board_id * 256 + 32);
                convert_14bits_to_16bits(&buffer[0], (long long)860160000 * (0 + board_id * 4) + cur_table_index * (long long)420000 + (double_column_index + 1875) * 56, cur_table_ordered_index * board_sum * (long long)960000 + double_column_index * board_sum * (long long)512 + board_id * 256 + 64);
                convert_14bits_to_16bits(&buffer[0], (long long)860160000 * (2 + board_id * 4) + cur_table_index * (long long)420000 + (double_column_index + 1875) * 56, cur_table_ordered_index * board_sum * (long long)960000 + double_column_index * board_sum * (long long)512 + board_id * 256 + 96);
                convert_14bits_to_16bits(&buffer[0], (long long)860160000 * (0 + board_id * 4) + cur_table_index * (long long)420000 + (double_column_index + 3750) * 56, cur_table_ordered_index * board_sum * (long long)960000 + double_column_index * board_sum * (long long)512 + board_id * 256 + 128);
                convert_14bits_to_16bits(&buffer[0], (long long)860160000 * (2 + board_id * 4) + cur_table_index * (long long)420000 + (double_column_index + 3750) * 56, cur_table_ordered_index * board_sum * (long long)960000 + double_column_index * board_sum * (long long)512 + board_id * 256 + 160);
                convert_14bits_to_16bits(&buffer[0], (long long)860160000 * (0 + board_id * 4) + cur_table_index * (long long)420000 + (double_column_index + 5625) * 56, cur_table_ordered_index * board_sum * (long long)960000 + double_column_index * board_sum * (long long)512 + board_id * 256 + 192);
                convert_14bits_to_16bits(&buffer[0], (long long)860160000 * (2 + board_id * 4) + cur_table_index * (long long)420000 + (double_column_index + 5625) * 56, cur_table_ordered_index * board_sum * (long long)960000 + double_column_index * board_sum * (long long)512 + board_id * 256 + 224);

                convert_14bits_to_16bits(&buffer[0], (long long)860160000 * (1 + board_id * 4) + cur_table_index * (long long)420000 + double_column_index * 56, cur_table_ordered_index * board_sum * (long long)960000 + double_column_index * board_sum * (long long)512 + board_sum * 256 + board_id * 256);
                convert_14bits_to_16bits(&buffer[0], (long long)860160000 * (3 + board_id * 4) + cur_table_index * (long long)420000 + double_column_index * 56, cur_table_ordered_index * board_sum * (long long)960000 + double_column_index * board_sum * (long long)512 + board_sum * 256 + board_id * 256 + 32);
                convert_14bits_to_16bits(&buffer[0], (long long)860160000 * (1 + board_id * 4) + cur_table_index * (long long)420000 + (double_column_index + 1875) * 56, cur_table_ordered_index * board_sum * (long long)960000 + double_column_index * board_sum * (long long)512 + board_sum * 256 + board_id * 256 + 64);
                convert_14bits_to_16bits(&buffer[0], (long long)860160000 * (3 + board_id * 4) + cur_table_index * (long long)420000 + (double_column_index + 1875) * 56, cur_table_ordered_index * board_sum * (long long)960000 + double_column_index * board_sum * (long long)512 + board_sum * 256 + board_id * 256 + 96);
                convert_14bits_to_16bits(&buffer[0], (long long)860160000 * (1 + board_id * 4) + cur_table_index * (long long)420000 + (double_column_index + 3750) * 56, cur_table_ordered_index * board_sum * (long long)960000 + double_column_index * board_sum * (long long)512 + board_sum * 256 + board_id * 256 + 128);
                convert_14bits_to_16bits(&buffer[0], (long long)860160000 * (3 + board_id * 4) + cur_table_index * (long long)420000 + (double_column_index + 3750) * 56, cur_table_ordered_index * board_sum * (long long)960000 + double_column_index * board_sum * (long long)512 + board_sum * 256 + board_id * 256 + 160);
                convert_14bits_to_16bits(&buffer[0], (long long)860160000 * (1 + board_id * 4) + cur_table_index * (long long)420000 + (double_column_index + 5625) * 56, cur_table_ordered_index * board_sum * (long long)960000 + double_column_index * board_sum * (long long)512 + board_sum * 256 + board_id * 256 + 192);
                convert_14bits_to_16bits(&buffer[0], (long long)860160000 * (3 + board_id * 4) + cur_table_index * (long long)420000 + (double_column_index + 5625) * 56, cur_table_ordered_index * board_sum * (long long)960000 + double_column_index * board_sum * (long long)512 + board_sum * 256 + board_id * 256 + 224);
            }
        }
    }
    free(buffer);
    return;
}

int DataProcess::map_raw_2_decode()
{
    int board_sum;
    int cur_start_table_index = 0;
    board_sum = (end_file_index - start_file_index + 1) / 4;

    decode_data_length = (end_file_index - start_file_index + 1) * (long long)PACKET_SUM_PER_INTERFACE * 1600 / 2;
    decode_data = (int16_t *)std::calloc(decode_data_length, 2);

    if ((end_file_index - start_file_index + 1) % 4 != 0)
    {
        std::cout << "ERROR :: The sum of ports must be times of 4.";
    }

    int tables_per_thread = 2048 / config.program_decode_pcap_threads_sum;
    int rest_tables = 2048 % config.program_decode_pcap_threads_sum;
    boost::asio::thread_pool threadpool(config.program_decode_pcap_threads_sum);

    for (int thread_index = 0; thread_index < config.program_decode_pcap_threads_sum; thread_index++)
    {
        // process_tables(thread_index * tables_per_thread, tables_per_thread, board_sum);

        if (thread_index < rest_tables)
        {
            boost::asio::post(threadpool, boost::bind(&DataProcess::process_tables, this, cur_start_table_index, tables_per_thread + 1, board_sum));
            cur_start_table_index += tables_per_thread + 1;
        }
        else
        {
            boost::asio::post(threadpool, boost::bind(&DataProcess::process_tables, this, cur_start_table_index, tables_per_thread, board_sum));
            cur_start_table_index += tables_per_thread;
        }
    }

    // 用线程池的时候要 join()
    threadpool.join();

    free(raw_data);
    free(index_data);
    flag_raw_data_ready = false;
    flag_decode_data_ready = true;

    return 1;
}

inline std::string DataProcess::get_data_part()
{
    switch (config.node_id)
    {
    case 1:
        return "A";
    case 2:
        return "B";
    case 3:
        return "C";
    case 4:
        return "D";
    default:
        return "";
    }
}

int DataProcess::save_decode_data()
// 将转码结果保存为一个 bin
{

    if (flag_decode_data_ready == false)
    {
        return 0;
    }

    
    int board_sum = (end_file_index - start_file_index + 1) / 4;
    std::string save_file = config.storage_path + patient.id + "_" + patient.name + "/" + cur_index_id + "/decode_data" + get_data_part() + ".bin";

    std::ofstream f_stream(save_file, std::fstream::out | std::fstream::binary);
    if (f_stream)
    {
        f_stream.write((char *)&decode_data[0], (long)3932160000 * board_sum);
        f_stream.close();
    }
    return 1;
}

int DataProcess::save_decode_tables()
// 将转码结果保存为 2048 个 bin
{

    if (flag_decode_data_ready == false)
    {
        return 0;
    }

    std::stringstream ss;

    int board_sum = (end_file_index - start_file_index + 1) / 4;

    std::string save_file = config.storage_path + patient.id + "_" + patient.name + "/" + cur_index_id + "/decode_data" + get_data_part();

    for (int table_index = 0; table_index < 2048; table_index++)
    {
        ss << std::setw(4) << std::setfill('0') << table_index;
        std::ofstream f_stream(save_file + ss.str() + ".bin", std::fstream::out);
        ss.str("");
        if (f_stream)
        {
            f_stream.write((char *)&decode_data[(long long)960000 * board_sum * table_index], 1920000 * board_sum);

            if (f_stream.good())
            {
                f_stream.close();
            }
            else
            {
                return 0;
            }
        }
    }
    return 1;
}
