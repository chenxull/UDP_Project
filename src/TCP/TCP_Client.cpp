// -----------------------------------------------------------------------------
// Filename:    TCP_Client.cpp
// Revision:    None
// Date:        2018/10/15 - 17:30
// Author:      ChenXu
// Email:       cx94758428042@gmail.com
// Website:     [NULL]
// Notes:       [NULL]
// -----------------------------------------------------------------------------
// Copyright:   2018 (c) ChenXu
// License:     GPL
// -----------------------------------------------------------------------------
// Version [1.0]
// TCP 客户端，发送特定的package给服务器
//初步实现功能，采用曳光代码的方式

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "DataProcess.hpp"

using boost::asio::ip::tcp;
using namespace std;

class TCP_Client
{
  private:
    char *msg_buff = (char *)malloc(1000 * sizeof(char));
    char *package = (char *)malloc(1400 * sizeof(char));
    short *msg_length = NULL;
    tcp::socket sock;

    void handle_connect(const boost::system::error_code &error, tcp::resolver::iterator endpointIterator)
    {
        if (!error)
        {

            memset(package, 0, 1400);
            memset(msg_buff, 0, 1000); // 注意：重置buf

            msg_length = (short *)package;
            msg_buff = package + 2;
            rand_msg_buff(&msg_buff, msg_length);

            cout << "将要发送的数据包：" << endl;
            printf("%d %s\n", *(short *)package, package + 2); //打印发送的数据包

            boost::asio::async_write(sock, boost::asio::buffer(package, strlen(package)), boost::bind(&TCP_Client::handle_send, this, boost::asio::placeholders::error));
        }
        else if (endpointIterator != tcp::resolver::iterator())
        {
            sock.close();
            tcp::endpoint endpoint = *endpointIterator;
            sock.async_connect(endpoint, boost::bind(&TCP_Client::handle_connect, this, boost::asio::placeholders::error, ++endpointIterator));
        }
    };
  
    void handle_send(const boost::system::error_code &error)
    {
        if (!error)
        {

            memset(package, 0, 1400);
            sock.async_read_some(boost::asio::buffer(package, 1400), boost::bind(&TCP_Client::handle_recive, this, boost::asio::placeholders::error));
        }
    };

    void handle_recive(const boost::system::error_code &error)
    {
        //TODO 根据返回的状态码，打印出不同信息
        if (!error)
        {

            cout << "recv from: " << sock.remote_endpoint().address() << ":" << sock.remote_endpoint().port() << endl;
            cout << "接受到的数据：" << endl;
            cout << package << endl; // print received message
            memset(package, 0, 1400);
            memset(msg_buff, 0, 1000);

            msg_length = (short *)package;
            msg_buff = package + 2;
            rand_msg_buff(&msg_buff, msg_length);

            cout << "发送的数据：" << endl;

            printf("%d %s\n", *(short *)package, package + 2); //打印发送的数据包
            usleep(40000);
            boost::asio::async_write(sock, boost::asio::buffer(package, strlen(package)), boost::bind(&TCP_Client::handle_send, this, boost::asio::placeholders::error));
        }
    };

  public:
    TCP_Client(boost::asio::io_context &service, tcp::resolver::iterator endpointIterator) : sock(service)
    {
        tcp::endpoint endpoint = *endpointIterator;
        sock.async_connect(endpoint, boost::bind(&TCP_Client::handle_connect, this, boost::asio::placeholders::error, ++endpointIterator));
    }
};

int main()
{
    try
    {
        boost::asio::io_context service;

        tcp::resolver resolver(service);
        tcp::resolver::query query("127.0.0.1", "9930");
        tcp::resolver::iterator iterator = resolver.resolve(query);

        TCP_Client TCP_Client(service, iterator);
        service.run();
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}
