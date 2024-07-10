#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/make_shared.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include "handlers.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

void fail(beast::error_code ec, char const* what) {
    std::cerr << what << ": " << ec.message() << "\n";
}

void do_session(tcp::socket socket) {
    auto buffer = std::make_shared<beast::flat_buffer>();
    auto req = std::make_shared<http::request<http::string_body>>();

    http::async_read(socket, *buffer, *req, [socket = std::move(socket), buffer, req](beast::error_code ec, std::size_t) mutable {
        if (!ec) {
            handle_request(std::move(socket), *req);
        }
        else {
            fail(ec, "read");
        }
        });
}

void server(net::io_context& ioc, tcp::endpoint endpoint) {
    auto acceptor = std::make_shared<tcp::acceptor>(ioc, endpoint);

    acceptor->async_accept([acceptor, &ioc](beast::error_code ec, tcp::socket socket) {
        if (!ec) {
            // «апускаем новую сессию в новом потоке
            std::thread{ do_session, std::move(socket) }.detach();
        }
        server(ioc, acceptor->local_endpoint());
        });
}

int main() {
    try {
        net::io_context ioc{ 1 };
        tcp::endpoint endpoint{ tcp::v4(), 8080 };

        server(ioc, endpoint);

        ioc.run();
    }
    catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
