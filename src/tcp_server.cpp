//
#include <boost/asio/signal_set.hpp>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <sstream>
#include <fstream>

#include "listener.h"

namespace {
namespace net = boost::asio;
using namespace std::literals;
namespace sys = boost::system;

std::string HandleRequest(std::string&& req) {
        std::string res = std::move(req);
    return res;
}

// Запускает функцию fn на n потоках, включая текущий
template <typename Fn>
void RunWorkers(unsigned n, const Fn& fn) {
    n = std::max(1u, n);
    std::vector<std::jthread> workers;
    workers.reserve(n - 1);
    // Запускаем n-1 рабочих потоков, выполняющих функцию fn
    while (--n) {
        workers.emplace_back(fn);
    }
    fn();
}

}  // namespace

int main(int argc, char** argv) {

    if (argc != 2) {
        std::cout << "Invalid arguments"s << std::endl;
        return 1;
    }
    int port = std::stoi(argv[1]);

    const unsigned num_threads = std::thread::hardware_concurrency();

    net::io_context ioc(num_threads);

    // Подписываемся на сигналы и при их получении завершаем работу сервера
    net::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait([&ioc](const sys::error_code& ec, [[maybe_unused]] int signal_number) {
        if (!ec) {
            ioc.stop();
        }
    });

    std::ofstream log_file_( "txt.log"s );

    const auto address = net::ip::make_address("0.0.0.0");
    listener::Server(ioc, {address, port}, [&log_file_](auto&& req, auto&& sender) {
        sender(HandleRequest(std::forward<decltype(req)>(req)),log_file_);
    });

    // Эта надпись сообщает тестам о том, что сервер запущен и готов обрабатывать запросы
    std::cout << "Server has started..."sv << std::endl;

    RunWorkers(num_threads, [&ioc] {
        ioc.run();
    });
    

}
