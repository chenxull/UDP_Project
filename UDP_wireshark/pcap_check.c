// -----------------------------------------------------------------------------
// Filename:    pcap_check.c
// Revision:    None
// Date:        2018/07/04 - 19:02
// Author:      Haixiang HOU
// Email:       hexid26@outlook.com
// Website:     [NULL]
// Notes:       [NULL]
// -----------------------------------------------------------------------------
// Copyright:   2018 (c) Haixiang
// License:     GPL
// -----------------------------------------------------------------------------
// Version [1.0]
// 检查 pcap 文件

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "define_const.hpp"

// 测试用的文件 /Users/haixiang/WorkSpace/20180704-162553/DatPort0.pcap

int check_file_complete(char *filename)
// 检查文件的完整性
{
    FILE *openfile_ptr;
    openfile_ptr = fopen(filename, "rb");

    if (openfile_ptr == NULL)
    {
        printf("ERROR: Can't open file!");
        exit(1);
    }

    // TODO: check the file with fseek
    unsigned int block_head_buf[7];
    unsigned int packet_buf[353];
    int ret_code = 0;
    int packet_cnt = 0;
    int packet_error_cnt = 0;
    fseek(openfile_ptr, 0, SEEK_SET);
    ret_code = fread(block_head_buf, 28, 1, openfile_ptr);

    while (ret_code != 0)
    {
        // printf("%d :: %08X, %u %08X\n", ++packet_cnt, block_head_buf[0], block_head_buf[1], block_head_buf[1]);

        if (block_head_buf[0] != 6)
        {
            fseek(openfile_ptr, block_head_buf[1] - 28, SEEK_CUR);
        }
        else if (block_head_buf[0] == 6)
        {
            ++packet_cnt;
            if (block_head_buf[5] != PACKET_LENGTH)
            {
                ++packet_error_cnt;
            }
            fseek(openfile_ptr, 42, SEEK_CUR);
            ret_code = fread(packet_buf, 1410, 1, openfile_ptr);
            fseek(openfile_ptr, block_head_buf[1] - 1410 - 70, SEEK_CUR);
        }
        ret_code = fread(block_head_buf, 28, 1, openfile_ptr);
    }
    // printf("Packet Sum = %d\n", packet_cnt);
    // printf("ERROR Packet Sum = %d\n", packet_error_cnt);
    fclose(openfile_ptr);

    if (packet_error_cnt == 0 && packet_cnt == PACKET_SUM_PER_INTERFACE)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int main(int argc, char *argv[])
{
    clock_t start_time, end_time;
    start_time = clock();
    char *filename;

    if (strcmp(argv[1], "mac") == 0)
    {
        filename = "/Users/haixiang/WorkSpace/20180704-162553/DatPort0.pcap";
    }
    else if (strcmp(argv[1], "ubuntu") == 0)
    {
        filename = "/home/haixiang/WorkSpace/20180704-162553/DatPort0.pcap";
    }
    else
    {
        filename = argv[1];
    }

    check_file_complete(filename) == 1 ? printf("INFO :: Packet checked successful.\n") : printf("ERROR :: Packet error! Date invalid!");
    end_time = clock();
    printf("Duration == %lf s.\n", (end_time - start_time) / (double)pow(10, 6));
    exit(0);
}