#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "server.hpp"

using namespace std;

using boost::asio::ip::tcp;

int main()
{

    try
    {
        boost::asio::io_context service;
        Server *server;
        server = new Server(service);
        service.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}
