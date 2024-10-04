#include <iostream>
#include <thread>
#include "netlab.hpp"

using namespace boost::asio;

namespace {
    void handling(const std::string& path, ip::tcp::socket socket) {
        netlab::MyFtp session(std::make_shared<ip::tcp::socket>(std::move(socket)));

        try {
            session.receiveFile(path);
        } catch (netlab::NetLabException& e) {
            std::cout << "Client: " + e.getMessage() << std::endl;
        }
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

    void MyFtpServer::receive() const {
        io_context context;
        ip::tcp::acceptor acceptor(context, ip::tcp::endpoint(ip::address::from_string("127.0.0.1"), port));

        acceptor.listen(10);

        while (true) {
            ip::tcp::socket socket(context);
            acceptor.accept(socket);

            std::thread handler(handling, pathToUploads, std::move(socket));
            handler.detach();
        }
    }

    void MyFtpServer::shutdown() const {
        // TODO: Implement server shutdown
    }
}