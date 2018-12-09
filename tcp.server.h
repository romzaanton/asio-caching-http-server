#pragma once
//#define BOOST_ASIO_ENABLE_HANDLER_TRACKING
#define BOOST_ASIO_NO_DEPRECATED
#include "boost/asio.hpp"
#include <memory>
#include <vector>
#include <string>
#include "tcp.connection.h"


namespace tcp {
	namespace server {
		class TCPServer {
		public:
			TCPServer() = delete;
			TCPServer(const TCPServer&) = delete;
			TCPServer(const TCPServer&&) = delete;
			TCPServer& operator = (const TCPServer&) = delete;
			TCPServer& operator = (const TCPServer&&) = delete;
			TCPServer(std::string& host, std::string& port);
			~TCPServer() = default;
			int run();
			int stop();
		private:
			//** We need to create a server object to accept incoming client connections. The io_context object provides I/O services, such as sockets, that the server object will use.
			boost::asio::io_context io_context{ 4 };
			boost::asio::signal_set signals{ io_context, SIGINT, SIGTERM };
			//** The constructor initializes an acceptor to listen on TCP.
			boost::asio::ip::tcp::acceptor acceptor{ io_context };
			void setAcceptor(std::string& host, std::string& port);
			//** Do async await for system signal
			void do_await_stop();
			void do_accept();
		};
	}
}