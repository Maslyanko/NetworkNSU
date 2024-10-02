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

class SenderTcpClient {
private:
    
public:
    
};

class ReceiverTcpServer {
private:
    int port;
    std::string pathToUploads;
public:
    ReceiverTcpServer();
    ReceiverTcpServer(int port);
    ReceiverTcpServer(const std::string& pathToUploads);
    ReceiverTcpServer(int port, const std::string& pathToUploads);

    void start();
};
}