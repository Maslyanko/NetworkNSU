#include <iostream>
#include "netlab.hpp"

using namespace boost::asio;

namespace netlab {
    MyFtpClient::MyFtpClient(const std::string& serverAddress, int port) {
        try {
            this->server = ip::tcp::endpoint(ip::address::from_string(serverAddress), port);
        } catch (...) {
            try {
                io_service service;
                ip::tcp::resolver resolver(service);
    
                ip::tcp::resolver::query query(serverAddress, std::to_string(port));
                this->server = *(resolver.resolve(query));
            } catch (...) {
                throw NetLabException("Failed to resolve server address.");
            }
        }
    }

    void MyFtpClient::send(const std::string& file) {
        io_context context;
        ip::tcp::socket socket(context);
        socket.connect(server);

        socket.write_some(buffer(file));
    }
}