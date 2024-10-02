#pragma once
#include <string>
#include <boost/asio.hpp>

namespace netlab {
class NetLabException {
private:
    std::string message;
public:
    NetLabException(const std::string& message) : message(message) {}
    const std::string& getMessage() const {return message;}
};

class MyFtp {
public:
    MyFtp(boost::asio::ip::tcp::socket socket);
    void sendFile(const std::string& file);
    void receiveFile();
};

class MyFtpClient {
private:
    boost::asio::ip::tcp::endpoint server;
public:
    MyFtpClient(const std::string& serverAddress, int port);

    void send(const std::string& file);
};

class MyFtpServer {
private:
    int port;
    std::string pathToUploads;
public:
    MyFtpServer();
    MyFtpServer(int port);
    MyFtpServer(const std::string& pathToUploads);
    MyFtpServer(int port, const std::string& pathToUploads);

    void receive();
    void shutdown();
};
}