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

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

int main(int argc, char const *argv[])
{
    std::string filepath;
    switch (argc)
    {
    case 1:
        filepath = "UCTDat_2048T2048R_25M.bin";
        break;
    case 2:
        filepath = argv[1];
        break;
    default:
        std::cout << "请输入表格编号 0 ~ 2047" << std::endl;
        return -1;
        break;
    }
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

    std::stringstream ss;
    std::string save_file;

    for (int table_index = 0; table_index < 2048; table_index++)
    {
        save_file = "small";
        ss << std::setw(4) << std::setfill('0') << table_index;
        std::ofstream f_stream(save_file + ss.str() + ".bin", std::fstream::out);
        ss.str("");

        if (f_stream)
        {
            f_stream.write((char *)&file_buffer[(long long)15360000 * table_index], 15360000);

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

    return 0;
}
