#!/bin/bash

clang++ -std=c++11 -Wall -O3 Pcap2bin.cpp -o ../release/Pcap2bin_cpp -lboost_system -lpthread