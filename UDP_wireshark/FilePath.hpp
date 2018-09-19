// -----------------------------------------------------------------------------
// Filename:    FilePath
// Revision:    None
// Date:        2018/07/09 - 04:33
// Author:      Haixiang HOU
// Email:       hexid26@outlook.com
// Website:     [NULL]
// Notes:       [NULL]
// -----------------------------------------------------------------------------
// Copyright:   2018 (c) Haixiang
// License:     GPL
// -----------------------------------------------------------------------------
// Version [1.0]
// C++ 简单的路径处理库

#include <string>
#include <iostream>

class FilePath
{
    // private:

  public:
    std::string full_path, dir_path, filename, filename_without_ext, ext_name;
    FilePath();
    void SetPath(std::string path);
    ~FilePath();
    void info();
};

FilePath::FilePath()
{
    full_path = "";
    dir_path = "";
    filename = "";
    filename_without_ext = "";
    ext_name = "";
}

void FilePath::SetPath(std::string path)
{
    full_path = path;
    int pos_slash = path.rfind('/');
    int pos_point = path.rfind('.');
    dir_path = path.substr(0, pos_slash + 1);
    filename = path.substr(pos_slash + 1, -1);
    filename_without_ext = path.substr(pos_slash + 1, pos_point - pos_slash - 1);
    ext_name = path.substr(pos_point + 1, -1);
}

FilePath::~FilePath()
{
}

void FilePath::info()
{
    std::cout << "Full Path : " << this->full_path << std::endl;
    std::cout << "Dir  Path : " << this->dir_path << std::endl;
    std::cout << "File Name : " << this->filename << std::endl;
    std::cout << "File Name no ext : " << this->filename_without_ext << std::endl;
    std::cout << "Ext  Name : " << this->ext_name << std::endl;
}
