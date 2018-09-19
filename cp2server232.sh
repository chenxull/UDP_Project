#!/bin/zsh
# -----------------------------------------------------------------------------Ï
# Filename:    cp2server.sh
# Revision:    None
# Date:        2018/07/04 - 19:46
# Author:      Haixiang HOU
# Email:       hexid26@outlook.com
# Website:     [NULL]
# Notes:       [NULL]
# -----------------------------------------------------------------------------
# Copyright:   2018 (c) Haixiang
# License:     GPL
# -----------------------------------------------------------------------------
# Version [1.0]
# copy this folder to server

ssh haixiang@192.168.15.232 rm -rf "/home/haixiang/WorkSpace/\[Source\]/UDP_Project"
scp -r ../UDP_Project haixiang@192.168.15.232:"/home/haixiang/WorkSpace/\[Source\]/"
