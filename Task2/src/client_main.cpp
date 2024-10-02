#include <iostream>
#include <boost/asio.hpp>

int main(int argc, char** argv) {
    try {
        boost::asio::io_context io_context;
        boost::asio::ip::tcp::resolver resolver(io_context);

        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}