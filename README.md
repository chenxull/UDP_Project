# 生科院的 UDP 收包项目

## ●　UDP_wireshark

`VSCode` 项目，调试编译均用 `VSCode` 完成。

利用 `wireshark` 和 `tshark` 工具进行抓包。临时方案。  
两台服务器，`CentOS 7.5` + `Ubuntu 16.04 LTS server`。  
`Ubuntu 16.04 LTS server` 使用 USB 串口控制扫描装置开始扫描。
`Ubuntu 16.04 LTS server` 开启 Samba 服务让外部设备访问扫描结果。

---

## Todolist

●　**UDP_wireshark**

- [x] 将 `wireshark` 抓取的 pcap 文件读取到内存
- [x] 提取原始 UDP 数据包中的有效数据
- [x] 把有效数据转从 14bit 为最小单位换成为 16bit，数据类型为 signed int
- [x] 将所有数据按照 GPU 中读取矩阵的顺序保存成文件

●　**UDP_Receiver**

- [x] 多线程监听不同的 IP 地址
- [ ] 异常状态的判断
- [ ] 整合 *UDP_wireshark* 中的功能
- [ ] daemon 进程后台调度

●　**Daemon 进程**

- [ ] 和其它节点通信
- [ ] 和工控机通信
- [ ] 调用 *UDP_Receiver*
- [ ] 调用 *CUDA* 程序
- [ ] 节点状态检测管理
