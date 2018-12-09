#include "pch.h"
#include "tcp.connection.h"


namespace tcp {
	namespace server {
		connection::connection(boost::asio::ip::tcp::socket _socket) 
			: socket(std::move(_socket)), response(nullptr), request(nullptr) {
			std::cout << "Connection constructed " << std::endl;
		}
		void connection::start() {
			this->do_read();
		};
		void connection::stop() {
			socket.close();
		}
		void connection::do_read() {
			auto self(shared_from_this());
			//** The boost::asio::buffer function is used to create a buffer object to represent raw memory, an array of POD elements, a vector of POD elements, or a std::string.
			this->socket.async_read_some(boost::asio::buffer(requestBuffer), [this, self](boost::system::error_code ec, size_t bytesTransfered) {
				if (!ec) {
					this->request = std::make_shared<http::client::request>(requestBuffer.data());
					this->request->log_headers();
					if (bytesTransfered < REQUEST_BUFFER_SIZE) {
						this->do_write();
					}
					else {
						this->do_read();
					}
				}
			/*	else if (ec == boost::asio::error::eof) {
					this->do_write();
				}*/
				else if (ec) {
					std::cout << "Err message: " << ec.value() << std::endl;
					this->stop();
				}

			});
		};
		void connection::do_write() {
			auto self(this->shared_from_this());
			this->response = std::make_shared<http::client::response>(this->request);
			std::string uri = this->request->get_uri();
			this->response->set_reply(uri);
			boost::asio::async_write(socket, this->response->reply_buf, [this, self](boost::system::error_code ec, size_t bytesSent) {
				if (!ec) {
					boost::system::error_code ignored_ec;
					this->socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
				}
				else {
					std::cout << "Err message: " << ec.value() << std::endl;
					this->socket.close();
				}
			});
		}
	}
}