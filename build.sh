#!/bin/bash

clang++ -std=c++11 -Wall -O3 Thread/Pcap2Bin.cpp -o release/Pcap2Bin_cpp -lboost_system -lpthread
clang++ `pkg-config --cflags --libs opencv` -std=c++11 -Wall -lpthread -O3 GPU/lashenimage.cpp -o release/2image_cpp
nvcc --std=c++11 -O3 GPU/origin.cu -o release/origin_cpp
nvcc --std=c++11 -O3 GPU/multicalc.cu -o release/multicalc_cpp
