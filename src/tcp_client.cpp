#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <string_view>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace net = boost::asio;
using net::ip::tcp;

using namespace std::literals;

std::string GetTimeStamp() {
    const auto now = std::chrono::system_clock::now();
    const auto t_c = std::chrono::system_clock::to_time_t(now);

    std::ostringstream oss;
        oss << "["s
            << std::put_time(std::gmtime(&t_c), "%F %T.") 
            << std::setfill('0') << std::setw(3) 
            << std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000
            << "] "s;
    return oss.str();
}

int main(int argc, char** argv) {

    if (argc != 4) {
        std::cout << "Invalid arguments"s << std::endl;
        return 1;
    }
    int port = std::stoi(argv[2]) ;
   
    boost::system::error_code ec;
    auto endpoint = tcp::endpoint(net::ip::make_address("127.0.0.1", ec), port);

    if (ec) {
        std::cout << "Wrong IP format"sv << std::endl;
        return 1;
    }
   
    net::io_context io_context;
    tcp::socket socket{io_context};
    socket.connect(endpoint, ec);

    if (ec) {
        std::cout << "Can't connect to server"sv << std::endl;
        return 1;
    }

    auto interval = std::chrono::seconds{std::stoi(argv[3])};

    while (true) {
        auto start1 = std::chrono::system_clock::now();
        
        socket.write_some(net::buffer(GetTimeStamp() + argv[1]), ec);
        if (ec) {
            std::cout << "Error sending data"sv << std::endl;
            return 1;
        }
            
        while (std::chrono::system_clock::now() - start1 < interval) {
        }
    }

}