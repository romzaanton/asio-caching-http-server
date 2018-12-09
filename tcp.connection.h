#pragma once
#include <memory>
#include <iostream>
#include <array>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <algorithm>
#include "http.client.request.h"
#include "http.client.response.h"
#include "boost/asio.hpp" 
#include "boost/algorithm/string.hpp"

#ifndef TCP_SERVER_CONNECTION
#define TCP_SERVER_CONNECTION
#define REQUEST_BUFFER_SIZE 4096

namespace tcp {
	namespace server {
		class connection : public std::enable_shared_from_this<connection> {
		public:
			connection(const connection&) = delete;
			connection& operator = (const connection&) = delete;
			connection(boost::asio::ip::tcp::socket _socket);
			~connection() {
				this->socket.close();
				std::cout << "Connection destructed " << std::endl;
			}
			void start();
			void stop();
		private:
			boost::asio::ip::tcp::socket socket;
			std::shared_ptr<http::client::request> request;
			std::shared_ptr<http::client::response> response;
			std::array<char, REQUEST_BUFFER_SIZE> requestBuffer;
			void do_read();
			void do_write();
		};
	}
}
#endif