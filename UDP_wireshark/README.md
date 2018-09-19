# UDP_wireshark

利用 `wireshark` 和 `tshark` 工具进行抓包。临时方案。  
两台服务器，`CentOS 7.5` + `Ubuntu 16.04 LTS server`。  
`Ubuntu 16.04 LTS server` 使用 USB 串口控制扫描装置开始扫描。
`Ubuntu 16.04 LTS server` 开启 Samba 服务让外部设备访问扫描结果。

## **文件说明**

`230` 和 `232` 文件夹是调用脚本，详见见文件夹下的 `README.md`。
`debug` 和 `release` 文件夹是编译输出路径，用来调试和编译。所有工作在 vscode 上完成。

### **define_const.hpp**

定义了各种常量的头文件

### **pcap_check.cpp**

用来读取 pcap 文件，把单个 pcap 文件缓存到内存中。
Packet 的顺序已经不用验证。

### **pcap_check.c**

C 语言的实现，已经放弃。