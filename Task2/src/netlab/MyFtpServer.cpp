#include <iostream>
#include <thread>
#include "netlab.hpp"

using namespace boost::asio;

namespace {
    void handling(ip::tcp::socket socket) {
        std::cout << "New client" << socket.local_endpoint().address().to_string();
        std::string message;
        socket.read_some(buffer(message, 20));
        std::cout << ": " << message << std::endl;
    }
}

namespace netlab {
    MyFtpServer::MyFtpServer(): 
    MyFtpServer::MyFtpServer(8888, "server/uploads") {}

    MyFtpServer::MyFtpServer(int port): 
    MyFtpServer::MyFtpServer(port, "server/uploads") {}

    MyFtpServer::MyFtpServer(const std::string& pathToUploads): 
    MyFtpServer(8888, pathToUploads) {}

    MyFtpServer::MyFtpServer(int port, const std::string& pathToUploads) {
        if (port < 1024 || port > 65535) {
            throw NetLabException("Invalid port number. Please use a number between 1024 and 65535.");
        }
        this->port = port;
        this->pathToUploads = pathToUploads;
    }

    void MyFtpServer::receive() {
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

    void MyFtpServer::shutdown() {
        // TODO: Implement server shutdown
    }
}