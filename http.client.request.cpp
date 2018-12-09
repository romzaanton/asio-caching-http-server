#include "pch.h"
#include "http.client.request.h"



namespace http {
	namespace client {
		request::request(char* uri) {
			std::cout << "Request constructed: " << uri << std::endl;
			this->parse_uri(uri);
		};
		void request::log_headers() {
			std::cout << std::endl;
			std::cout << method << " " << uri << " " << version << std::endl;
			std::map<std::string, std::string>::iterator it = this->headers.begin();
			for (it = this->headers.begin(); it != this->headers.end(); ++it) {
				std::cout << it->first << " => " << it->second << '\n';
			}
			std::cout << std::endl;
		}
		std::string request::get_uri() {
			return this->uri;
		}
		void request::parse_uri(char* uri) {
			std::stringstream str_stream{ uri };
			std::string cr, cl, header_title, header_value;
			str_stream >> this->method >> this->uri >> this->version;
			header_title.reserve(100);
			header_value.reserve(250);
			std::map<std::string, std::string>::iterator it = this->headers.begin();
			while (str_stream) {
				std::getline(str_stream, header_title, ':');
				std::getline(str_stream, header_value);
				boost::algorithm::trim(header_title);
				boost::algorithm::trim(header_value);
				if (header_title.front() < 0) break;
				this->headers.insert(it, std::pair<std::string, std::string>(std::move(header_title), std::move(header_value)));
			}
		}
	}
}