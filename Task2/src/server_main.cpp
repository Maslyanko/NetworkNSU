#include <iostream>

#include "netLab/includes/netlab.hpp"

int main (int argc, char** argv) {
    try {
        netlab::ReceiverTcpServer server(8888);
        server.start();
    } catch (const netlab::NetLabException& e) {
        std::cerr << "Error: " << e.getMessage() << std::endl;
        return 1;
    }

    return 0;
}