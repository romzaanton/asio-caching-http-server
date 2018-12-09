#include "pch.h"
#include <iostream>
#include <utility>
#include "tcp.server.h"

namespace tcp {
	namespace server {
		TCPServer::TCPServer(std::string& host, std::string& port) {
			this->setAcceptor(host, port);
		}
		void TCPServer::setAcceptor(std::string& host, std::string& port) {
			this->do_await_stop();
			//** A resolver takes a query object and turns it into a list of endpoints. 
			//** We construct a query using the name of the server, specified in argv[1], and the name of the service, in this case "daytime".
			//** tcp::resolver::results_type endpoints = resolver.resolve(argv[1], "daytime");
			//** The list of endpoints is returned using an iterator of type ip::tcp::resolver::iterator. //** (Note that a default constructed ip::tcp::resolver::iterator object can be used as an end iterator.)
			//** The list of endpoints obtained above could contain both IPv4 and IPv6 endpoints, so a program should try each of them until it finds one that works. 
			//** This keeps the client program independent of a specific IP version.
			//** IPv4 example - 209.173.53.167 
			//** IPv6 example - 0:0:0:0:0:ffff:d1ad:35a7
			//** You definitely need one acceptor per socket to listen and accept incoming connections.
			//** For instance, resolver convert '127.0.0.1' to the structured binary format '0x7F000001' 
			boost::asio::ip::tcp::resolver resolver(this->io_context);
			boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(host, port).begin();
			acceptor.open(endpoint.protocol());
			acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
			acceptor.bind(endpoint);
			acceptor.listen();
			std::cout << "Server listen on " << host << ":" << port << std::endl;
			this->do_accept();
		}
		int TCPServer::run() {
			io_context.run();
			return 0;
		}
		void TCPServer::do_await_stop() {
			this->signals.async_wait([this](boost::system::error_code ec, int _signal) {
				this->acceptor.close();
			});
		}
		void TCPServer::do_accept() {
			acceptor.async_accept([this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
				if (!acceptor.is_open()) {
					return;
				}
				if (!ec) {
					std::shared_ptr<connection> connection_ptr = std::make_shared<connection>(std::move(socket));
					connection_ptr->start();
				}
				this->do_accept();
			});
		}
	}
}