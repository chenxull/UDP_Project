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

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <time.h>
#include <cmath>
#include <vector>

#include "define_const.hpp"
#include "FilePath.hpp"

// 测试用的文件 /Users/haixiang/WorkSpace/20180704-162553/DatPort0.pcap

inline void wait_on_enter()
{
    std::string dummy;
    std::cout << "Enter to continue..." << std::endl;
    std::getline(std::cin, dummy);
}

inline unsigned int read_as_int(char *ptr)
// read 4 char as int
{
    unsigned int *int_ptr = (unsigned int *)ptr;
    return *int_ptr;
}

int read_pcap_2_memory(FilePath filepath, char **packets_raw, char **index_raw)
// 把 filepath 指向的文件读取到 packets_raw
// packets_raw 保存的内容为 1400*614400 Bytes
// index_raw 保存的内容为 10*614400 Bytes
{
    std::ifstream openfile(filepath.full_path.c_str(), std::ifstream::binary | std::ifstream::ate);
    if (!openfile)
    {
        std::cout << "ERROR :: open pcap file error!" << std::endl;
        std::cout << "ERROR :: " << filepath.full_path << std::endl;
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
            std::memcpy(*packets_raw + (packet_cnt - 1) * VALID_BYTES_LENGTH_PER_PACKAGE, &file_buffer[block_head_index + 74], VALID_BYTES_LENGTH_PER_PACKAGE);
            std::memcpy(*index_raw + (packet_cnt - 1) * 5, &file_buffer[block_head_index + 71], 2);
            // std::cout << std::hex << std::showbase << std::uppercase << read_as_int(&file_buffer[block_head_index + 70]) << std::endl;
            std::memcpy(*index_raw + (packet_cnt - 1) * 5 + 2, &file_buffer[block_head_index + 1475], 3);
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

void save_index_raw(std::string filename, char *index_raw)
// 把 index_raw 保存成文件
{
    std::ofstream openfile(filename.c_str(), std::ostream::out | std::ostream::trunc);

    if (!openfile)
    {
        std::cout << "ERROR :: save index file error!" << std::endl;
        return;
    }

    std::ostringstream membuf(std::ios::in | std::ios::out);
    membuf << std::hex << std::uppercase;
    int grp = 0;
    int cnt = 0;
    while (grp < PACKET_SUM_PER_INTERFACE)
    {
        cnt = 0;
        membuf << std::setw(1) << std::setfill('0') << (index_raw[5 * grp + (cnt++)] & 0xff) << " ";
        membuf << std::setw(1) << std::setfill('0') << (index_raw[5 * grp + (cnt++)] & 0xff) << " ";
        membuf << std::setw(1) << std::setfill('0') << (index_raw[5 * grp + (cnt++)] & 0xff) << " ";
        membuf << std::setw(1) << std::setfill('0') << (index_raw[5 * grp + (cnt++)] & 0xff) << " ";
        membuf << std::setw(1) << std::setfill('0') << (index_raw[5 * grp + (cnt++)] & 0xff) << std::endl;
        grp++;
    }
    openfile << membuf.str();
    membuf.clear();
    openfile.close();
    return;
}

void check_index_raw(char *index_raw)
{
    int grp = 0;
    int cnt = 0;
    int check_no = 0;
    int temp = 0;
    while (grp < PACKET_SUM_PER_INTERFACE)
    {
        cnt = 0;
        temp = (int)index_raw[5 * grp + 1];
        
        if (temp != check_no % 75) {
            std::cout << "ERROR :: The order is wrong." << std::endl;
        }

        check_no++;
        grp++;
    }
}

int sort_packets_per_lan(FilePath filepath)
// 把 wireshark 抓到的包（单根网线上的包）排序
{
    char *packets_raw;
    char *index_raw;

    packets_raw = (char *)std::malloc(PACKET_SUM_PER_INTERFACE * VALID_BYTES_LENGTH_PER_PACKAGE);
    index_raw = (char *)std::malloc(PACKET_SUM_PER_INTERFACE * 10);

    if (packets_raw == NULL || index_raw == NULL)
    {
        std::cout << "ERROR :: Malloc data for packets_raw or index_raw failed." << std::endl;
        return 0;
    }

    if (read_pcap_2_memory(filepath, &packets_raw, &index_raw) == 0)
    {
        std::cout << "ERROR :: read_pcap_2_memory error!" << std::endl;
        return 0;
    }
    // save_index_raw(filepath.filename_without_ext + (std::string) "_index.txt", index_raw);
    check_index_raw(index_raw);
    return 1;
}

int main(int argc, char *argv[])
{
    clock_t start_time, end_time;
    start_time = clock();
    FilePath filepath;

    if (strcmp(argv[1], "mac") == 0)
    {
        filepath.SetPath(MAC_DATA_PATH);
    }
    else if (strcmp(argv[1], "ubuntu") == 0)
    {
        filepath.SetPath(UBUNTU_DATA_PATH);
    }
    else
    {
        filepath.SetPath(argv[1]);
    }

    // std::cout << "File path : " << filepath.full_path << std::endl;

    (sort_packets_per_lan(filepath) == 1) ? std::cout << "INFO :: Packet checked successful. " : std::cout << "ERROR :: Packet error! Date invalid! ";

    end_time = clock();
    printf("Duration == %lf s.\n", (end_time - start_time) / (double)pow(10, 6));
    exit(0);
}