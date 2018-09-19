// -----------------------------------------------------------------------------
// Filename:    Patient.cpp
// Revision:    None
// Date:        2018/07/16 - 15:10
// Author:      Haixiang HOU
// Email:       hexid26@outlook.com
// Website:     [NULL]
// Notes:       [NULL]
// -----------------------------------------------------------------------------
// Copyright:   2018 (c) Haixiang
// License:     GPL
// -----------------------------------------------------------------------------
// Version [1.0]
// 一个病人的数据
// 内存里面一次只能 load 一张 slice，每个人有100个切面

#include <string>
#include <iostream>
#include <fstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

class Patient
{
  private:
    int creat_template(std::string storage_path, std::string id, std::string name);
    int load_info(std::string storage_path, std::string id, std::string name);

  public:
    // 个人信息的种类
    std::string name;
    int age;
    std::string id;
    std::string gender;
    std::string phone;
    std::string check_date;
    std::string comment;
    std::string data_path;
    int slice_sum;

    // std::string properties_name[9];
    // int pcap_loaded_sum;         //读入的 pcap 文件数量，最大 32
    // int slice_loaded_no;         //整理好的切片数量，最大 100
    // int matrix_loaded_sum;       //整理好的矩阵的数量，最大 2048
    // int matrix_current_no;       // 当前矩阵对应的编号，0~2047
    // char *matrixes[31457280000]; //一个人的一个切片的数据 3750*2048*2*2048 (2048 个矩阵)

    Patient();
    Patient(std::string storage_path, std::string id, std::string name);
    void print_info();
    bool path_of_pcapfile_from_slice(std::string slice_id, std::string paths[32]);
    std::string path_of_pcapfile_from_slice(std::string slice_id, int port_no);
    ~Patient();
};

Patient::Patient()
{
}

Patient::Patient(std::string storage_path, std::string id, std::string name)
{
    this->name = name;
    this->id = id;
    if (load_info(storage_path.c_str(), id, name) != 0)
    {
        creat_template(storage_path.c_str(), id, name);
        // exit(-1);
    }
}

int Patient::creat_template(std::string storage_path, std::string id, std::string name)
// 把病人信息保存到信息存储路径下
{
    std::string path = storage_path + id + "_" + name + "/patient.txt";
    boost::property_tree::ptree ptree;
    ptree.add("Name", name);
    ptree.add("Age", 0);
    ptree.add("ID", id);
    ptree.add("Gender", "NULL");
    ptree.add("Phone", "NULL");
    ptree.add("Check_Date", "NULL");
    ptree.add("Comment", "Template");
    ptree.add("Data_Path", storage_path);
    ptree.add("Slice_Sum", 60);
    boost::property_tree::ini_parser::write_ini(path.c_str(), ptree);
    std::cout << "Patient info file: [" << path << "]: has been created, please modify it." << std::endl;
    load_info(storage_path.c_str(), id, name);
    return 0;
}

int Patient::load_info(std::string storage_path, std::string id, std::string name)
// 根据 ID 和 Name 读取病人基本信息
{
    std::string path = storage_path + id + "_" + name;
    if (access(path.c_str(), 0) != 0)
    {
        std::cout << "ERROR :: no folder exsits, check the data folder -> " << path << std::endl;
        exit(-1);
    }
    path = path + "/patient.txt";
    if (access(path.c_str(), 0) != 0)
    {
        std::cout << "ERROR :: no patient info exsits." << std::endl;
        return -1;
    }
    boost::property_tree::ptree ptree;
    boost::property_tree::ini_parser::read_ini(path, ptree);

    age = ptree.get<int>("Age");
    gender = ptree.get<std::string>("Gender");
    phone = ptree.get<std::string>("Phone");
    check_date = ptree.get<std::string>("Check_Date");
    comment = ptree.get<std::string>("Comment");
    data_path = ptree.get<std::string>("Data_Path");
    slice_sum = ptree.get<int>("Slice_Sum");

    return 0;
}

void Patient::print_info()
{
    std::cout << "⌜---------- Patient Info Done. ----------⌝" << std::endl;
    std::cout << "Name = " << name << std::endl;
    std::cout << "Age = " << age << std::endl;
    std::cout << "ID = " << id << std::endl;
    std::cout << "Gender = " << gender << std::endl;
    std::cout << "Phone = " << phone << std::endl;
    std::cout << "Check_Date = " << check_date << std::endl;
    std::cout << "Comment = " << comment << std::endl;
    std::cout << "Data_Path = " << data_path << std::endl;
    std::cout << "Slice_Sum = " << slice_sum << std::endl;
    std::cout << "⌞---------- Patient Info Done. ----------⌟" << std::endl;
}

bool Patient::path_of_pcapfile_from_slice(std::string slice_id, std::string paths[32])
// 根据 slice_id 返回文件的列表: paths
{
    std::string path = data_path + id + "_" + name + "/" + slice_id;
    if (access(path.c_str(), 0) != 0)
    {
        std::cout << "ERROR :: Slice " << slice_id << " not exsited." << std::endl;
        return false;
    }

    path = path + "/DatPort";

    for (size_t i = 0; i < 32; i++)
    {
        paths[i] = path + std::to_string(i) + ".pcap";
    }
    return true;
}

std::string Patient::path_of_pcapfile_from_slice(std::string slice_id, int port_no)
// 根据 slice_id 和 port_no (0~31) 返回文件路径
{
    std::string path = data_path + id + "_" + name + "/" + slice_id;
    if (access(path.c_str(), 0) != 0)
    {
        std::cout << "ERROR :: Slice [" << path << "] not exsited." << std::endl;
        path = "error file";
        return path;
    }

    path = data_path + id + "_" + name + "/" + slice_id + "/DatPort" + std::to_string(port_no) + ".pcap";
    return path;
}

Patient::~Patient()
{
}
