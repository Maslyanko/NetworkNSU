#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include "../includes/netlab.hpp"

using namespace boost::asio;

namespace {
    void handling(ip::tcp::socket socket) {
        std::cout << "New client" << socket.local_endpoint().address().to_string() << std::endl;
        while (true) {
            
        } 
    }
}

namespace netlab {
    ReceiverTcpServer::ReceiverTcpServer(): 
    ReceiverTcpServer::ReceiverTcpServer(8888, "server/uploads") {}

    ReceiverTcpServer::ReceiverTcpServer(int port): 
    ReceiverTcpServer::ReceiverTcpServer(port, "server/uploads") {}

    ReceiverTcpServer::ReceiverTcpServer(const std::string& pathToUploads): 
    ReceiverTcpServer(8888, pathToUploads) {}

    ReceiverTcpServer::ReceiverTcpServer(int port, const std::string& pathToUploads) {
        if (port < 1024 || port > 65535) {
            throw NetLabException("Invalid port number. Please use a number between 1024 and 65535.");
        }
        this->port = port;
        this->pathToUploads = pathToUploads;
    }

    void ReceiverTcpServer::start() {
        io_context context;
        ip::tcp::acceptor acceptor(context, ip::tcp::endpoint(ip::address::from_string("127.0.0.1"), port));

        acceptor.listen(10);

        while (true) {
            ip::tcp::socket socket(context);
            acceptor.accept(socket);

            std::thread handler(handling, std::move(socket));
            handler.detach();
        }
    }
}