#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

using namespace std;

using boost::asio::ip::tcp;


class Connection: public boost::enable_shared_from_this<Connection>
{
  private:
    tcp::socket sock;
    enum
    {
        BUF_SIZE = 1410
    };
    char buf[BUF_SIZE];
    string msg_;
    char *msg_buff=(char *)malloc(1400 * sizeof(char));

    void handle_Read(const boost::system::error_code &error, std::size_t bytes_transferred);
    void handle_Write(const boost::system::error_code &error);
    string make_daytime_string();

  public:
    Connection(boost::asio::io_context &service);
    tcp::socket& getSocket(){
        return sock;
    }
    void start();
};

typedef boost::shared_ptr<Connection> ConnectionPtr;

class Server
{
  private:
    tcp::acceptor acceptor;
    void start();
    void handle_Accept(ConnectionPtr con, const boost::system::error_code &error);

  public:
    Server(boost::asio::io_context &service);
};

Connection::Connection(boost::asio::io_context& service): sock(service){
    }


void Connection::start() {
        memset(msg_buff, 0, BUF_SIZE);
        boost::asio::async_read(sock, boost::asio::buffer(msg_buff,BUF_SIZE),
                boost::asio::transfer_at_least(1), boost::bind(
                        &Connection::handle_Read, // ＃1
                        shared_from_this(), boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    }

void Connection::handle_Read(const boost::system::error_code& error,std::size_t bytes_transferred){
         if (!error) {
            cout << "recv from: " << sock.remote_endpoint().address() << ":" << sock.remote_endpoint().port() << endl;
            cout << "接受到的数据："<< endl;
            printf("%d %s\n", *(short *)msg_buff, msg_buff+2); //打印发送的数据包
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

void Connection::handle_Write(const boost::system::error_code& error){
        if (!error) {
            memset(msg_buff, 0, BUF_SIZE); // 注意：重置buff
            sock.async_read_some(boost::asio::buffer(msg_buff,BUF_SIZE), boost::bind(
                    &Connection::handle_Read, // ＃3
                    shared_from_this(), boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }
    }

string Connection::make_daytime_string() //生成字符串的日期信息
{
        time_t now = time(0);
        return ctime(&now);
}

Server::Server(boost::asio::io_context& service):
    acceptor(service ,tcp::endpoint(tcp::v4(),9972)){
        start();
    }

    void Server::start(){
        ConnectionPtr  conn(new Connection(acceptor.get_io_context()));
        acceptor.async_accept(conn->getSocket(),boost::bind(
                    &Server::handle_Accept,this,conn,
                    boost::asio::placeholders::error));

    }

    void Server::handle_Accept(ConnectionPtr con,const boost::system::error_code& error)
    {
        if(!error){
            con->start();
            start();
        }
    }