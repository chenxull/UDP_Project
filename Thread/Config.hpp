// -----------------------------------------------------------------------------
// Filename:    Config.hpp
// Revision:    None
// Date:        2018/08/15 - 12:17
// Author:      Haixiang HOU
// Email:       hexid26@outlook.com
// Website:     [NULL]
// Notes:       [NULL]
// -----------------------------------------------------------------------------
// Copyright:   2018 (c) Haixiang
// License:     GPL
// -----------------------------------------------------------------------------
// Version [1.0]
// 读取配置文件

#include <string>
#include <iostream>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <sstream>

class Config
{
  private:
    /* data */
  public:
    int node_id;
    int system_cpu_cores;
    int program_check_slices_threads_sum;
    int program_pcap_read_threads_sum;
    int program_decode_pcap_threads_sum;
    std::string storage_path;
    std::vector<int> raw_data_ids;

    Config(std::string config_file);
    void create_template();
    ~Config();
};

Config::Config(std::string config_file)
{
    boost::property_tree::ptree ptree;
    boost::property_tree::ini_parser::read_ini(config_file, ptree);
    node_id = ptree.get<int>("system.Node_ID");
    system_cpu_cores = ptree.get<int>("system.CPU_Cores");
    program_check_slices_threads_sum = ptree.get<int>("system.Check_Slices_Threads_Sum");
    program_pcap_read_threads_sum = ptree.get<int>("system.Pcap_Read_Threads_Sum");
    program_decode_pcap_threads_sum = ptree.get<int>("system.Decode_Pcap_Threads_Sum");
    storage_path = ptree.get<std::string>("system.Storage_Path");
    std::stringstream ss(ptree.get<std::string>("system.Raw_Data_Ids"));
    int temp;

    while (ss >> temp)
    {
        raw_data_ids.push_back(temp);

        if (ss.peek() == ',')
            ss.ignore();
    }
}

void Config::create_template()
{
    boost::property_tree::ptree ptree;
    ptree.add("system.Node_ID", 0);
    ptree.add("system.CPU_Cores", 0);
    ptree.add("system.Check_Slices_Threads_Sum", 0);
    ptree.add("system.Pcap_Read_Threads_Sum", 0);
    ptree.add("system.Decode_Pcap_Threads_Sum", 0);
    ptree.add("system.Storage_Path", "");
    ptree.add("system.Raw_Data_Ids", "");
    boost::property_tree::ini_parser::write_ini("config.ini", ptree);
}

Config::~Config()
{
}
