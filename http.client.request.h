#pragma once
#include <string>
#include <map>
#include <iostream>
#include <utility>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include "boost/algorithm/string.hpp"
#include "http.client.request.h"

namespace http {
	namespace client {
		class request {
		public:
			request() = delete;
			request(const request&) = delete;
			request& operator = (const request&) = delete;
			request(char *uri);
			~request() {
				std::cout << "Request destructed" << std::endl;
			}
			void log_headers();
			std::string get_uri();
		private:
			std::string method;
			std::string uri;
			std::string version;
			std::map<std::string, std::string> headers;
			void parse_uri(char* uri);
		};
	}
}