#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "client.hpp"

using boost::asio::ip::tcp;
using namespace std;


int main()
{
    try
    {
        char *msg_buff;
        char *package;
        boost::asio::io_context service;

        tcp::resolver resolver(service);
        tcp::resolver::query query("127.0.0.1", "9972");
        tcp::resolver::iterator iterator = resolver.resolve(query);

        TCP_Client *tcp_client;
        tcp_client =  new TCP_Client(service,iterator,msg_buff,package);
        service.run();
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}