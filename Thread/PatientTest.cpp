// -----------------------------------------------------------------------------
// Filename:    PatientTest.cpp
// Revision:    None
// Date:        2018/08/07 - 14:28
// Author:      Haixiang HOU
// Email:       hexid26@outlook.com
// Website:     [NULL]
// Notes:       [NULL]
// -----------------------------------------------------------------------------
// Copyright:   2018 (c) Haixiang
// License:     GPL
// -----------------------------------------------------------------------------
// Version [1.0]
// 测试 Patient 类

#include "Patient.hpp"

int main(int argc, char const *argv[])
{
    Patient test_patient("/home/haixiang/WorkSpace/PatientArchieve/", "1", "TEST");
    std::string paths[32];
    test_patient.path_of_pcapfile_from_slice("1", paths);

    for (size_t i = 0; i < 32; i++)
    {
        std::cout << paths[i] << std::endl;
    }

    std::string path = test_patient.path_of_pcapfile_from_slice("1", 3);
    std::cout << std::endl << path << std::endl;
    return 0;
}
