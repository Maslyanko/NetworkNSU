#include "netlab.hpp"
#include <fstream>
#include <iostream>
#include <filesystem>

namespace {
    int charsToInt(const char* chars) {
        int result = 0;
    
        for (int i = 0; i < 4; ++i) {
            result <<= 8;
            result |= static_cast<int>(chars[i]);
        }
    
        return result;
    }

    void intToChars(int value, char* result) {
        for (int i = 0; i < 4; ++i) {
            result[3 - i] = static_cast<char>(value >> (i * 8));
        }
    }

    void copyFileName(const std::string& fileName, char* buffer) {
        char const* begin = fileName.c_str();

        for (int i = 0; i < fileName.length(); ++i) {
            buffer[i] = begin[i];
        }
    }
}

namespace netlab {
    std::mutex MyFtp::mutex;

    MyFtp::MyFtp(std::shared_ptr<boost::asio::ip::tcp::socket> socket): 
    socket(socket), bufferSize(8192) {}

    void MyFtp::sendFile(const std::string& filePath) const {
        char buffer[bufferSize];

        if (!std::filesystem::exists(filePath)) {
            buffer[0] = '?';
            socket.get()->write_some(boost::asio::buffer(buffer, 1));
            throw NetLabException("File does not exist.");
        }

        std::string fileName = std::filesystem::path(filePath).filename();

        std::ifstream file(filePath, std::ios::binary);

        file.seekg(0, file.end);
        int fileSize = file.tellg();
        file.seekg(0, file.beg);

        int fileNameLength = fileName.size();

        if (fileNameLength > 4096) {
            buffer[0] = '\0';
            socket.get()->write_some(boost::asio::buffer(buffer, 1));
            throw NetLabException("File name is too long.");
        }

        if (fileSize > 1024 * 1024 * 1024) {
            buffer[0] = '>';
            socket.get()->write_some(boost::asio::buffer(buffer, 1));
            throw NetLabException("File size is too large.");
        }

        copyFileName(fileName, buffer);
        buffer[fileNameLength] = '\0';
        intToChars(fileSize, &buffer[fileNameLength + 1]);

        int bytesRead = file.read(&buffer[fileNameLength + 5], bufferSize - fileNameLength - 4).gcount();
        socket.get()->write_some(boost::asio::buffer(buffer, bytesRead + fileNameLength + 5));

        socket.get()->read_some(boost::asio::buffer(buffer, 1));
        if (buffer[0] == 0) {
            std::cout << "File already exists on server." << std::endl;
            file.close();
            return;
        }

        while (file.good()) {
            bytesRead = file.read(buffer, bufferSize).gcount();
            socket.get()->write_some(boost::asio::buffer(buffer, bytesRead));
        }

        file.close();

        socket.get()->read_some(boost::asio::buffer(buffer, 1));
        
        if (buffer[0] == 1) {
            std::cout << "Success!" << std::endl;
        } else { 
            std::cerr << "Failed to receive file." << std::endl;
        }
    }

    void MyFtp::receiveFile(const std::string& uploadsDir) const {
        std::string fileName;
        int fileSize;

        char buffer[bufferSize];
        int alreadyRead = socket.get()->read_some(boost::asio::buffer(buffer, bufferSize));

        if (buffer[0] == '\0') {
            throw NetLabException("Invalid file name.");
        } else if (buffer[0] == '>') {
            throw NetLabException("Invalid file size.");
        } else if (buffer[0] == '?') {
            throw NetLabException("Client specified an incorrect file name.");
        }

        fileName = std::string(buffer);

        mutex.lock();
            if (std::filesystem::exists(uploadsDir + "/" + fileName)) {
                buffer[0] = 0;
                socket.get()->write_some(boost::asio::buffer(buffer, 1));
                throw NetLabException("File already exists.");
            } else {
                buffer[0] = 1;
                socket.get()->write_some(boost::asio::buffer(buffer, 1));
            }
    
            std::ofstream file(uploadsDir + "/" + fileName, std::ios::out | std::ios::binary);   
        mutex.unlock();

        int fileNameLength = fileName.size();

        fileSize = charsToInt(&buffer[fileNameLength + 1]);

        file.write(&buffer[fileNameLength + 5], alreadyRead - fileNameLength - 5);

        while (alreadyRead < fileSize) {
            int bytesRead = socket.get()->read_some(boost::asio::buffer(buffer, bufferSize));
            file.write(buffer, bytesRead);
            alreadyRead += bytesRead;
        }

        file.close();

        buffer[0] = 1;
        socket.get()->write_some(boost::asio::buffer(buffer, 1));
    }

    std::shared_ptr<boost::asio::ip::tcp::socket> MyFtp::getSocket() const {
        return socket;
    }

    MyFtp::~MyFtp() {
        socket.get()->close();
    }
}