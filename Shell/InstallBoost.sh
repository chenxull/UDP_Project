#!/bin/bash
# -----------------------------------------------------------------------------Ï
# Filename:    InstallBoost.sh
# Revision:    None
# Date:        2018/08/14 - 19:38
# Author:      Haixiang HOU
# Email:       hexid26@outlook.com
# Website:     [NULL]
# Notes:       [NULL]
# -----------------------------------------------------------------------------
# Copyright:   2018 (c) Haixiang
# License:     GPL
# -----------------------------------------------------------------------------
# Version [1.0]
#  下载、安装 boost 库

# wget https://dl.bintray.com/boostorg/release/1.68.0/source/boost_1_68_0.tar.gz
tar zxf boost_1_68_0.tar.gz
rm -f boost_1_68_0.tar.gz
cd boost_1_68_0
./bootstrap.sh --prefix=/usr
./b2 stage threading=multi link=shared
sudo ./b2 install threading=multi link=shared
echo "Boost installation finished!"
