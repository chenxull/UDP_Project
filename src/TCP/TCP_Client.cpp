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

using boost::asio::ip::tcp;
using namespace std;



class TCP_Client {
 private:

tcp::socket sock;
 enum {
			BUF_SIZE = 512
		};
char buf[BUF_SIZE];

 void handle_connect(const boost::system::error_code& error,tcp::resolver::iterator endpointIterator)
 {
     if (!error) {
				memset(buf, 0, 512);
				cin.getline(buf, BUF_SIZE);
				boost::asio::async_write(sock,boost::asio::buffer(buf, strlen(buf)), boost::bind(&TCP_Client::handle_send,this,boost::asio::placeholders::error));
			} else if (endpointIterator != tcp::resolver::iterator()) {
				sock.close();
				tcp::endpoint endpoint = *endpointIterator;
				sock.async_connect(endpoint, boost::bind(&TCP_Client::handle_connect,this, boost::asio::placeholders::error, ++endpointIterator));
			}
 };



 void handle_send(const boost::system::error_code& error)
 {
     if (!error) {
         //TODO 生成package函数
				memset(buf,0, 512); // 注意：重置buf
				sock.async_read_some(boost::asio::buffer(buf),boost::bind(&TCP_Client::handle_recive, this,boost::asio::placeholders::error));
			}

 };

 void handle_recive(const boost::system::error_code& error)
 {
     //TODO 根据返回的状态码，打印出不同信息
     if (!error) {
				cout << buf << endl; // print received message
				memset(buf, 0, 512);
				cin.getline(buf, BUF_SIZE);
				boost::asio::async_write(sock,boost::asio::buffer(buf, strlen(buf)), boost::bind(&TCP_Client::handle_send,this,boost::asio::placeholders::error));
			}
 };

 //void produce_package();



 public:
 TCP_Client(boost::asio::io_service& service,tcp::resolver::iterator endpointIterator): sock(service)
 {
     tcp::endpoint endpoint = *endpointIterator;
     sock.async_connect(endpoint,boost::bind(&TCP_Client::handle_connect,this,boost::asio::placeholders::error,++endpointIterator));
 }

};


int main(){
    try{
        boost::asio::io_service service;

        tcp::resolver resolver(service);
        tcp::resolver::query query("127.0.0.1","9997");
        tcp::resolver::iterator iterator = resolver.resolve(query);

        TCP_Client TCP_Client(service,iterator);
        service.run();


    }catch (std::exception& e){
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;

}


