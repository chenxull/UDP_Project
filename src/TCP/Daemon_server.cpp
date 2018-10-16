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
    enum {
        BUF_SIZE = 1410
    };
    char buf[BUF_SIZE];
    string msg_;
    char *msg_buff = (char *)malloc(1400 * sizeof(char));
public:
    Connection(boost::asio::io_service& service): sock(service){
    }

     void start() {
        memset(msg_buff, 0, BUF_SIZE);
        boost::asio::async_read(sock, boost::asio::buffer(msg_buff,BUF_SIZE),
                boost::asio::transfer_at_least(1), boost::bind(
                        &Connection::handle_Read, // ＃1
                        shared_from_this(), boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    }
    tcp::socket& getSocket(){
        return sock;
    }

private:
    void handle_Read(const boost::system::error_code& error,std::size_t bytes_transferred){
         if (!error) {
            cout << "recv from: " << sock.remote_endpoint().address() << ":" << sock.remote_endpoint().port() << endl;
            cout << "接受到的数据："<< endl;
            cout << strlen(msg_buff) <<endl;
        /*TODO 数据包验证函数*/
            msg_=make_daytime_string();
            cout << "将要发送的数据"<<endl;
            cout << msg_<< endl;

            sock.async_write_some(boost::asio::buffer(msg_),
                    boost::bind(
                            &Connection::handle_Write, // ＃2
                            shared_from_this(),
                            boost::asio::placeholders::error));
        }
    }

    void handle_Write(const boost::system::error_code& error){
        if (!error) {
            memset(msg_buff, 0, BUF_SIZE); // 注意：重置buff
            sock.async_read_some(boost::asio::buffer(msg_buff,BUF_SIZE), boost::bind(
                    &Connection::handle_Read, // ＃3
                    shared_from_this(), boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }
    }

    std::string make_daytime_string()    //生成字符串的日期信息
    {
        time_t now = time(0);
        return ctime(&now);
    }
};

//创建Connection指针
typedef boost::shared_ptr<Connection> ConnectionPtr;

class Server{

private:
     tcp::acceptor acceptor;

public:
    Server(boost::asio::io_service& service):
    acceptor(service ,tcp::endpoint(tcp::v4(),9984)){
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
