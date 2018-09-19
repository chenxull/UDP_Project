#!/bin/bash
# -----------------------------------------------------------------------------Ï
# Filename:    UDPCatch.sh
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
# 在 Ubuntu Server 上运行，调用 wireshark 进行抓包

pause() {
#   echo -n "Press SPACE to continue or Ctrl+C to exit ... "
  while true; do
    read -n1 -r
    [[ $REPLY == ' ' ]] && break
  done
  echo
  echo "Continuing ..."
}

var_datadir=$(date +"%Y%m%d-%H%M%S")

cd ~
killall tshark
mkdir /home/samba/anonymous_shares/$var_datadir
cd /home/samba/anonymous_shares/$var_datadir

echo "---------- Initialization Tshark on 230----------"
ssh root@192.168.15.230 mkdir /root/UCTDA/temp
ssh root@192.168.15.230 "/root/UCTDA/tsharkudp25M_temp.sh" &

echo "---------- Initialization Tshark on 232----------"

sleep 2
tshark -i enp23s0f0 -f 'udp src port 5048' -B 1024 -c 614400 -w 'DatPort8.pcap' &
tshark -i enp23s0f1 -f 'udp src port 5048' -B 1024 -c 614400 -w 'DatPort9.pcap' &
tshark -i enp24s0f0 -f 'udp src port 5048' -B 1024 -c 614400 -w 'DatPort10.pcap' &
tshark -i enp24s0f1 -f 'udp src port 5048' -B 1024 -c 614400 -w 'DatPort11.pcap' &
tshark -i enp13s0f0 -f 'udp src port 5048' -B 1024 -c 614400 -w 'DatPort12.pcap' &
tshark -i enp13s0f1 -f 'udp src port 5048' -B 1024 -c 614400 -w 'DatPort13.pcap' &
tshark -i enp14s0f0 -f 'udp src port 5048' -B 1024 -c 614400 -w 'DatPort14.pcap' &
tshark -i enp14s0f1 -f 'udp src port 5048' -B 1024 -c 614400 -w 'DatPort15.pcap' &
tshark -i enp132s0f0 -f 'udp src port 5048' -B 1024 -c 614400 -w 'DatPort24.pcap' &
tshark -i enp132s0f1 -f 'udp src port 5048' -B 1024 -c 614400 -w 'DatPort25.pcap' &
tshark -i enp133s0f0 -f 'udp src port 5048' -B 1024 -c 614400 -w 'DatPort26.pcap' &
tshark -i enp133s0f1 -f 'udp src port 5048' -B 1024 -c 614400 -w 'DatPort27.pcap' &
tshark -i enp7s0f0 -f 'udp src port 5048' -B 1024 -c 614400 -w 'DatPort28.pcap' &
tshark -i enp7s0f1 -f 'udp src port 5048' -B 1024 -c 614400 -w 'DatPort29.pcap' &
tshark -i enp8s0f0 -f 'udp src port 5048' -B 1024 -c 614400 -w 'DatPort30.pcap' &
tshark -i enp8s0f1 -f 'udp src port 5048' -B 1024 -c 614400 -w 'DatPort31.pcap' &
sleep 10

echo "Ready for capture UDP packets"
echo "Press <Space> to send reset instruction"
pause
echo -en '\x52\x53' > /dev/ttyUSB0
echo "Press <Space> to send transmission instruction"
pause
echo -en '\x46\x41' > /dev/ttyUSB0

# After capturing all packets
sleep 10

echo "WARNING: If the transmission has done, press <Space> to continue!"
pause
killall tshark

# Close tshark on 192.168.15.230
ssh root@192.168.15.230 killall tshark
ssh root@192.168.15.230 sleep 2
sleep 2

# copy files from 192.168.15.230
scp root@192.168.15.230:/root/UCTDA/temp/DatPort*.pcap ./
ssh root@192.168.15.230 rm -rf ~/UCTDA/temp

# 修改权限
chmod -R 0755 ./*
chown -R nobody:nogroup ./*
ls -l DatPort*.pcap

