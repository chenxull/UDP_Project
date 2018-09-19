#!/bin/sh
#ps -ef | grep tshark| grep -v grep | awk '{print $2}' | xargs kill -9
#ps -ef | grep dumpcap| grep -v grep | awk '{print $2}' | xargs kill -9

NAME=tshark
echo $NAME
ID=`ps -ef | grep "$NAME" | grep -v "$0" | grep -v "grep" | awk '{print $2}'`
echo $ID
echo "---------------"
for id in $ID
do
kill -9 $id
echo "killed $id"
done
echo "---------------"

NAME=dumpcap
echo $NAME
ID=`ps -ef | grep "$NAME" | grep -v "$0" | grep -v "grep" | awk '{print $2}'`
echo $ID
echo "---------------"
for id in $ID
do
kill -9 $id
echo "killed $id"
done
echo "---------------"

mkdir /root/UCTDA/temp
sleep 2
tshark -i enp23s0f0 -f 'udp src port 5048' -B 1024 -c 614400 -w '/root/UCTDA/temp/DatPort0.pcap' &
tshark -i enp23s0f1 -f 'udp src port 5048' -B 1024 -c 614400 -w '/root/UCTDA/temp/DatPort1.pcap' &
tshark -i enp24s0f0 -f 'udp src port 5048' -B 1024 -c 614400 -w '/root/UCTDA/temp/DatPort2.pcap' &
tshark -i enp24s0f1 -f 'udp src port 5048' -B 1024 -c 614400 -w '/root/UCTDA/temp/DatPort3.pcap' &
tshark -i enp13s0f0 -f 'udp src port 5048' -B 1024 -c 614400 -w '/root/UCTDA/temp/DatPort4.pcap' &
tshark -i enp13s0f1 -f 'udp src port 5048' -B 1024 -c 614400 -w '/root/UCTDA/temp/DatPort5.pcap' &
tshark -i enp14s0f0 -f 'udp src port 5048' -B 1024 -c 614400 -w '/root/UCTDA/temp/DatPort6.pcap' &
tshark -i enp14s0f1 -f 'udp src port 5048' -B 1024 -c 614400 -w '/root/UCTDA/temp/DatPort7.pcap' &
tshark -i enp7s0f0 -f 'udp src port 5048' -B 1024 -c 614400 -w '/root/UCTDA/temp/DatPort16.pcap' &
tshark -i enp7s0f1 -f 'udp src port 5048' -B 1024 -c 614400 -w '/root/UCTDA/temp/DatPort17.pcap' &
tshark -i enp8s0f0 -f 'udp src port 5048' -B 1024 -c 614400 -w '/root/UCTDA/temp/DatPort18.pcap' &
tshark -i enp8s0f1 -f 'udp src port 5048' -B 1024 -c 614400 -w '/root/UCTDA/temp/DatPort19.pcap' &
tshark -i enp132s0f0 -f 'udp src port 5048' -B 1024 -c 614400 -w '/root/UCTDA/temp/DatPort20.pcap' &
tshark -i enp132s0f1 -f 'udp src port 5048' -B 1024 -c 614400 -w '/root/UCTDA/temp/DatPort21.pcap' &
tshark -i enp133s0f0 -f 'udp src port 5048' -B 1024 -c 614400 -w '/root/UCTDA/temp/DatPort22.pcap' &
tshark -i enp133s0f1 -f 'udp src port 5048' -B 1024 -c 614400 -w '/root/UCTDA/temp/DatPort23.pcap' &
sleep 8

