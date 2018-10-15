// -----------------------------------------------------------------------------
// Filename:    Daemon_server.cpp
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
// Daemon服务端，用来处理client发来的package，对其进行验证并返回相应的状态码
//需要二个class分别为Connection,Server
//初步实现功能，采用曳光代码的方式
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

using namespace std;

using boost::asio::ip::tcp;

//创建一个指针类型的类
class Connection: public boost::enable_shared_from_this<Connection> {
private:
    tcp::socket sock;
    char buf[512];
public:
    Connection(boost::asio::io_service& service): sock(service){
    }

     void start() {
        boost::asio::async_read(sock, boost::asio::buffer(buf),
                boost::asio::transfer_at_least(1), boost::bind(
                        &Connection::handle_Read, // ＃1
                        shared_from_this(), boost::asio::placeholders::error));
    }
    tcp::socket& getSocket(){
        return sock;
    }

private:
    void handle_Read(const boost::system::error_code& error){
         if (!error) {
            cout << "recv from: " << sock.remote_endpoint().address() << ":" << sock.remote_endpoint().port() << endl;
        /*TODO 数据包验证函数*/
            sock.async_write_some(boost::asio::buffer(buf),
                    boost::bind(
                            &Connection::handle_Write, // ＃2
                            shared_from_this(),
                            boost::asio::placeholders::error));
        }
    }

    void handle_Write(const boost::system::error_code& error){
        if (!error) {
            memset(buf, 0, 512); // 注意：重置buff
            sock.async_read_some(boost::asio::buffer(buf), boost::bind(
                    &Connection::handle_Read, // ＃3
                    shared_from_this(), boost::asio::placeholders::error));
        }
    }
};

//创建Connection指针
typedef boost::shared_ptr<Connection> ConnectionPtr;

class Server{

private:
     tcp::acceptor acceptor;

public:
    Server(boost::asio::io_service& service):
    acceptor(service ,tcp::endpoint(tcp::v4(),9997)){
        start();
    }

private:
    void start(){
        ConnectionPtr  conn(new Connection(acceptor.get_io_service()));
        acceptor.async_accept(conn->getSocket(),boost::bind(
                    &Server::handle_Accept,this,conn,
                    boost::asio::placeholders::error));

    };

    void handle_Accept(ConnectionPtr con,const boost::system::error_code& error)
    {
        if(!error){
            con->start();
            start();
        }
    };
};



int main(){
    
    try
    {
        boost::asio::io_service service;
        Server Server(service);
        service.run();

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}
