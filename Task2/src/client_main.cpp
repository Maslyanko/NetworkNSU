#include <iostream>
#include <boost/asio.hpp>

#include "netlab/netlab.hpp"

int main(int argc, char** argv) {
    try {
        netlab::MyFtpClient client("127.0.0.1", 8888);
        client.send("hello world!");
        std::cout << "Message sent successfully." << std::endl;
    } catch (netlab::NetLabException &e) {
        std::cerr << e.getMessage() << std::endl;
        return 1;
    }


    return 0;
}