#include "pch.h"
#include "http.router.h"

namespace http {
	namespace router {
		void write_stock_reply(tcp_con& tcp_con) {

		}
		router::router(std::string&& _route) {
			std::string url{ "\\" };
			std::function<void(tcp_con&)> func = write_stock_reply;
			this->it = this->routes.begin();
			this->routes.insert(it, std::pair<std::string, std::function<void(tcp_con&)>>(url, func));
		}
	}
}
