#include <iostream>
#include <fstream>
#include <boost/asio.hpp>
#include <string>
#include <thread>

using namespace boost::asio;
using ip::tcp;

void session(tcp::socket socket, const std::string& request_path)
{
    try
    {
        std::string filename = "G:/Summer_Practise/src/html/" + request_path;

        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << "\n";
            std::string not_found_response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
            boost::asio::write(socket, boost::asio::buffer(not_found_response));
            return;
        }

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<char> content(size);
        file.read(content.data(), size);

        std::string response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Length: " + std::to_string(size) + "\r\n"
            "Content-Type: text/html\r\n" // You may want to set Content-Type appropriately
            "\r\n";

        boost::asio::write(socket, boost::asio::buffer(response));
        boost::asio::write(socket, boost::asio::buffer(content));
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception in session: " << e.what() << "\n";
    }
}

int main()
{
    try
    {
        boost::asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));

        std::cout << "Server running at http://127.0.0.1:8080\n";

        while (true)
        {
            tcp::socket socket(io_context);
            acceptor.accept(socket);

            boost::asio::streambuf request_buffer;
            boost::asio::read_until(socket, request_buffer, "\r\n\r\n");

            std::istream request_stream(&request_buffer);
            std::string request_line;
            std::getline(request_stream, request_line);

            std::string request_path;
            if (request_line.find("GET") != std::string::npos) {
                size_t start_pos = request_line.find(' ') + 1;
                size_t end_pos = request_line.find(' ', start_pos);
                request_path = request_line.substr(start_pos, end_pos - start_pos);
            }
            else {
                std::cerr << "Unsupported HTTP method\n";
                continue;
            }

            if (!request_path.empty() && request_path[0] == '/')
                request_path.erase(0, 1);

            if (request_path.empty())
                request_path = "index.html";

            std::thread(session, std::move(socket), request_path).detach();
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception in main: " << e.what() << "\n";
    }

    return 0;
}
