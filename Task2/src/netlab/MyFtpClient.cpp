#include <iostream>
#include "netlab.hpp"

namespace netlab {
    MyFtpClient::MyFtpClient(const std::string& serverAddress, int port) {
        try {
            this->server = 
            boost::asio::ip::tcp::endpoint
            (
                boost::asio::ip::address::from_string(serverAddress), port
            );
        } catch (...) {
            try {
                boost::asio::io_service service;
                boost::asio::ip::tcp::resolver resolver(service);
    
                boost::asio::ip::tcp::resolver::query query(serverAddress, std::to_string(port));
                this->server = *(resolver.resolve(query));
            } catch (...) {
                throw NetLabException("Failed to resolve server address.");
            }
        }
    }

    void MyFtpClient::send(const std::string& file) const {
        boost::asio::io_context context;
        boost::asio::ip::tcp::socket socket(context);
        socket.connect(server);

        netlab::MyFtp session(std::make_shared<boost::asio::ip::tcp::socket>(std::move(socket)));

        session.sendFile(file);
    }
}