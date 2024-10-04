#pragma once
#include <string>
#include <boost/asio.hpp>
#include <memory>
#include <mutex>

namespace netlab {
class NetLabException {
private:
    std::string message;
public:
    NetLabException(const std::string& message) : message(message) {}
    const std::string& getMessage() const {return message;}
};

class MyFtp {
private:
    std::shared_ptr<boost::asio::ip::tcp::socket> socket;
    int bufferSize;
    static std::mutex mutex;
public:
    MyFtp(std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    void sendFile(const std::string& filePath) const;
    void receiveFile(const std::string& uploadsDir) const;

    std::shared_ptr<boost::asio::ip::tcp::socket> getSocket() const;
    ~MyFtp();
};

class MyFtpClient {
private:
    boost::asio::ip::tcp::endpoint server;
public:
    MyFtpClient(const std::string& serverAddress, int port);

    void send(const std::string& file) const;
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

    void receive() const;
    void shutdown() const;
};
}