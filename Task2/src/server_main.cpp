#include <iostream>

#include "netlab/netlab.hpp"

int main (int argc, char** argv) {
    try {
        netlab::MyFtpServer server(8888, "../sources");
        server.receive();
    } catch (const netlab::NetLabException& e) {
        std::cerr << "Error: " << e.getMessage() << std::endl;
        return 1;
    }

    return 0;
}