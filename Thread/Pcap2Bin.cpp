// -----------------------------------------------------------------------------
// Filename:    DataProcessTest.cpp
// Revision:    None
// Date:        2018/08/07 - 23:08
// Author:      Haixiang HOU
// Email:       hexid26@outlook.com
// Website:     [NULL]
// Notes:       [NULL]
// -----------------------------------------------------------------------------
// Copyright:   2018 (c) Haixiang
// License:     GPL
// -----------------------------------------------------------------------------
// Version [1.0]
// 测试 DataProcess 类

// #include <time.h>
#include <chrono>
#include "DataProcess.hpp"

int main(int argc, char const *argv[])
{
    auto start_time_main = std::chrono::high_resolution_clock::now();
    auto end_time = std::chrono::high_resolution_clock::now();
    std::string cfg_file, id, name;
    std::string slice_index;
    int save_type = 0;

    switch (argc)
    {
    case 6:
        cfg_file = argv[1];
        id = argv[2];
        name = argv[3];
        slice_index = argv[4];
        save_type = atoi(argv[5]);
        if (save_type != 1 && save_type != 2048 && save_type != 0)
        {
            std::cout << "ERROR :: the last parameter must be 0 or 1 or 2048." << std::endl;
            std::cout << "0: no save. 1: save one bin. 2048: save 2048 bins." << std::endl;
            exit(-1);
        }

        break;
    default:
        std::cout << "Args are not correct, must have 5 args:" << std::endl
                  << "[config file] [ID] [Name] [Slice ID] [0 or 1 or 2048]" << std::endl;
        return -1;
        break;
    }

    // 初始化相关参数
    Config config(cfg_file);
    Patient *pptr = new Patient(config.storage_path, id, name);
    DataProcess *ptr = new DataProcess(config, *pptr);

    // 读取 Pcap 文件
    if (ptr->load_slice(slice_index) != 1)
    {
        std::cout << "ERROR :: Slice " << slice_index << " : "
                  << "pcap from " << config.raw_data_ids[0] << " to " << config.raw_data_ids[config.raw_data_ids.size() - 1] << " load failed." << std::endl;
    }
    std::cout << "INFO :: UCT" << ptr->config.node_id << " -> Slice " << slice_index << " : "
              << "pcap from " << config.raw_data_ids[0] << " to " << config.raw_data_ids[config.raw_data_ids.size() - 1] << " load successful." << std::endl;
    // end_time = std::chrono::high_resolution_clock::now();
    // std::cout << "INFO :: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time_main).count() << "ms for load pcap files." << std::endl;

    // 检查 Pcap 文件中包的顺序
    // auto start_time_check = std::chrono::high_resolution_clock::now();
    if (ptr->check_index_data() != 1)
    {
        std::cout << "ERROR :: slice " << slice_index << " order error." << std::endl;
        return 0;
    }
    // end_time = std::chrono::high_resolution_clock::now();
    // std::cout << "INFO :: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time_check).count() << "ms for check package order." << std::endl;

    // 解码
    auto start_time_decode = std::chrono::high_resolution_clock::now();
    if (ptr->map_raw_2_decode() != 1)
    {
        std::cout << "ERROR :: slice " << slice_index << " decode error." << std::endl;
        return 0;
    }
    end_time = std::chrono::high_resolution_clock::now();
    std::cout << "INFO :: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time_decode).count() << "ms for decode data." << std::endl;

    // auto start_time_save = std::chrono::high_resolution_clock::now();

    // 保存解码后的文件
    switch (save_type)
    {
    case 1:
        if (ptr->save_decode_data() != 1)
        {
            std::cout << "ERROR :: save decode data error." << std::endl;
            return 0;
        }
        break;
    case 2048:
        if (ptr->save_decode_tables() != 1)
        {
            std::cout << "ERROR :: save decode data error." << std::endl;
            return 0;
        }
        break;
    default:
        break;
    }

    end_time = std::chrono::high_resolution_clock::now();
    // std::cout << "INFO :: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time_save).count() << "ms for save decode data." << std::endl;
    std::cout << "Finish :: UCT" << ptr->config.node_id << " save decode data in " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time_main).count() << "ms (total)." << std::endl;
    // 释放指针
    free(pptr);
    free(ptr);

    // end_time = std::chrono::high_resolution_clock::now();
    // std::cout << "INFO :: Total running " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time_main).count() << "ms" << std::endl;

    exit(0);
}
