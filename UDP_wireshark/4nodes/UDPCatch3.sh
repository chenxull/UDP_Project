#!/bin/bash
# -----------------------------------------------------------------------------Ï
# Filename:    UDPCatchDecode.sh
# Revision:    None
# Date:        2018/07/04 - 13:51
# Author:      Haixiang HOU
# Email:       hexid26@outlook.com
# Website:     [NULL]
# Notes:       [NULL]
# -----------------------------------------------------------------------------
# Copyright:   2018 (c) Haixiang
# License:     GPL
# -----------------------------------------------------------------------------
# Version [1.0]
# 生科院服务器抓包脚本
# 在 Ubuntu Server 上运行，调用 wireshark 进行抓包。

pause() {
#   echo -n "Press SPACE to continue or Ctrl+C to exit ... "
  while true; do
    read -n1 -r
    [[ $REPLY == ' ' ]] && break
  done
  echo "Continuing ..."
}

killall tshark

var_datadir=$HOME/PatientArchive/
var_datadir=$var_datadir$1\_$2/$3

mkdir $var_datadir -p
cd $var_datadir

echo "---------- Initialization Tshark on uct3----------"

# sleep 2
tshark -i enp101s0f0 -f 'udp src port 5048' -B 1024 -c 614400 -w 'DatPort16.pcap' &
tshark -i enp101s0f1 -f 'udp src port 5048' -B 1024 -c 614400 -w 'DatPort17.pcap' &
tshark -i enp101s0f2 -f 'udp src port 5048' -B 1024 -c 614400 -w 'DatPort18.pcap' &
tshark -i enp101s0f3 -f 'udp src port 5048' -B 1024 -c 614400 -w 'DatPort19.pcap' &
tshark -i enp179s0f0 -f 'udp src port 5048' -B 1024 -c 614400 -w 'DatPort20.pcap' &
tshark -i enp179s0f1 -f 'udp src port 5048' -B 1024 -c 614400 -w 'DatPort21.pcap' &
tshark -i enp179s0f2 -f 'udp src port 5048' -B 1024 -c 614400 -w 'DatPort22.pcap' &
tshark -i enp179s0f3 -f 'udp src port 5048' -B 1024 -c 614400 -w 'DatPort23.pcap' &
sleep 6
