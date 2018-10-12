// -----------------------------------------------------------------------------
// Filename:    UDP_Rcv.hpp
// Revision:    None
// Date:        2018/09/30 - 02:00
// Author:      Haixiang HOU
// Email:       hexid26@outlook.com
// Website:     [NULL]
// Notes:       [NULL]
// -----------------------------------------------------------------------------
// Copyright:   2018 (c) Haixiang
// License:     GPL
// -----------------------------------------------------------------------------
// Version [1.0]
// ! UDP 收包类，监听 172.16.0.1 ~ 172.16.31.1，同时每秒显示收包计数。
// TODO 加入解码和保存功能

#include <boost/thread/thread_pool.hpp>
#include <vector>

#include "Config.hpp"
#include "Error_Code.hpp"
#include "UDP_Socket.hpp"

class UDP_Rcv {
  private:
    Error_Code error_code;                    // 错误码
    int listen_port;                          // 监听端口号
    Config *config;                           // 配置信息
    char **data_buffer;                       // 保存的采集信息缓存空间
    char **check_buffer;                      // 保存的验证信息缓存空间
    std::vector<std::string> ip_address_list; // 监听的 IP 地址
    int *packets_cnt;                         // 收包数量计数器，每个线程一个
    unsigned long long *bytes_cnt;            // TODO 收包字节计数器，可能删除掉
    void init_buffer(int ip_count);           // 初始化缓存空间
    void listening(int ip_address_index);     // 开始监听

    /* data */
  public:
    UDP_Rcv(Config *config);
    ~UDP_Rcv();
};

void UDP_Rcv::init_buffer(int ip_count) {
    for (uint8_t index = 0; index < ip_count; index++) {
        // ! 监听地址 172.16.0.1 ~ 172.16.31.1 (可选，从 config 中读取 Raw_Data_Ids)
        ip_address_list.push_back("172.16." + std::to_string(config->raw_data_ids[index]) + ".1");
    }
    // ! 监听端口号
    listen_port = 58000;
    data_buffer = (char **)malloc(sizeof(char *) * ip_count +
                                  (long long)ip_count * 1400 * PACKET_SUM_PER_INTERFACE * sizeof(char));
    int tmp_index = ip_count;
    // 为 data_buffer 生成连续的二维数组空间，head 是一维数组的头
    if (data_buffer != NULL) {
        char *head = (char *)(data_buffer + ip_count * sizeof(char *));
        while (tmp_index--) {
            data_buffer[tmp_index] = (char *)(head + (int)tmp_index * 1400 * PACKET_SUM_PER_INTERFACE * sizeof(char));
        }
    } else {
        std::cout << error_code.Description(error_code.mem_malloc_error);
    }
    check_buffer =
        (char **)malloc(sizeof(char *) * ip_count + (long long)ip_count * 5 * PACKET_SUM_PER_INTERFACE * sizeof(char));
    tmp_index = ip_count;
    if (check_buffer != NULL) {
        char *head = (char *)(check_buffer + ip_count * sizeof(char *));
        while (tmp_index--) {
            check_buffer[tmp_index] = (char *)(head + (int)tmp_index * 5 * PACKET_SUM_PER_INTERFACE * sizeof(char));
        }
    } else {
        std::cout << error_code.Description(error_code.mem_malloc_error);
    }
}

void UDP_Rcv::listening(int ip_address_index) {
    UDP_Socket *socket;
    boost::asio::io_context io_context;
    printf("%s\n", ip_address_list[ip_address_index].c_str());
    // ! 生成一个 UDP_Socket 类来监听一个 ip 地址
    socket = new UDP_Socket(io_context, packets_cnt[ip_address_index], bytes_cnt[ip_address_index],
                            ip_address_list[ip_address_index], listen_port, &data_buffer[ip_address_index][0],
                            &check_buffer[ip_address_index][0]);
    io_context.run();
}

UDP_Rcv::UDP_Rcv(Config *cfg) {
    config = cfg;
    uint8_t ip_count = config->raw_data_ids.size();
    init_buffer(ip_count);
    // ! 初始化计数器
    packets_cnt = (int *)calloc(sizeof(int), ip_count);
    memset(&packets_cnt[0], 0, sizeof(int) * ip_count);
    bytes_cnt = (unsigned long long *)calloc(sizeof(unsigned long long), ip_count);
    memset(&bytes_cnt[0], 0, sizeof(unsigned long long) * ip_count);
    int *tmp_packets_cnt = new int[ip_count]();
    int *repeat_cnt = new int[ip_count]();

    boost::asio::thread_pool udp_rcv_thread_pool(ip_count);
    // ! index < 1 这里要改成 ip_count 才能监听所有端口，1 为测试用，监听一个端口
    for (int index = 0; index < ip_count; index++) {
        boost::asio::post(udp_rcv_thread_pool, boost::bind(&UDP_Rcv::listening, this, index));
    }
    // ! 等待40ms
    usleep(40000);
    std::cout << "INFO :: Ready done." << std::endl;
    // TODO 开始监听，告诉 Daemon 程序，可以开始发包
    // TODO 一秒钟之后，开始查找 ip_count 个计数器中的最小值。以 table 为单位开始处理
    // TODO inline private get_min_packets_cnt() 获取最小值 -> min_packets_cnt
    // TODO 最小值的上一轮计数器，last_packets_cnt
    // TODO 需要处理的 table 数量 (min_packets_cnt - last_packets_cnt)/300，每根网线300个包，32根网线正好构成一张 table
    // TODO 多线程处理 (min_packets_cnt - last_packets_cnt)/300 个 tables 的解码和放置
    //  TODO 单独一个线程用来保存解码后的数据字段
    while (true) {
        sleep(1);

        for (uint8_t index = 0; index < ip_count; index++) {
            if (tmp_packets_cnt[index] != packets_cnt[index]) {
                printf("Interface %d :: %d\n", index, packets_cnt[index]);
                tmp_packets_cnt[index] = packets_cnt[index];
                repeat_cnt[index] = 0;
            } else {
                repeat_cnt[index]++;
                if (repeat_cnt[index] > 3) {
                    packets_cnt[index] = 0;
                    if (tmp_packets_cnt[index] != 0) {
                        printf("Interface %d :: packets_cnt = %d\n", index, packets_cnt[index]);
                    }
                    tmp_packets_cnt[index] = 0;
                    repeat_cnt[index] = 0;
                    bytes_cnt[index] = 0;
                }
                if (packets_cnt[index] == 614400) {
                    printf("Interface %d :: Rcv done. %llu bytes\n", index, bytes_cnt[index]);
                    // printf("Interface 1 CNT :: %d\n", packets_cnt[1]);
                    // printf("Interface 2 CNT :: %d\n", packets_cnt[2]);
                    // printf("Interface 3 CNT :: %d\n", packets_cnt[3]);
                    // printf("Interface 4 CNT :: %d\n", packets_cnt[4]);
                    // printf("Interface 5 CNT :: %d\n", packets_cnt[5]);
                    // printf("Interface 6 CNT :: %d\n", packets_cnt[6]);
                    // printf("Interface 7 CNT :: %d\n", packets_cnt[7]);
                    break;
                }
            }
        }
    }
}

UDP_Rcv::~UDP_Rcv() {
    free(data_buffer);
    free(check_buffer);
}
