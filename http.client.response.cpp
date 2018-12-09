#include "pch.h"
#include "http.client.response.h"

namespace http {
	namespace client {

		const std::string crlf{ "\r\n" };

		namespace stirng_status {
			const std::string ok = "HTTP/1.1 200 OK\r\n";
		}
		namespace mime_types {
			const std::string text_plain = "Content-Type: text/plain\r\n";
			const std::string text_html = "Content-Type: text/html\r\n";
			const std::string text_js = "Content-Type: text/javascript\r\n";
			const std::string text_css = "Content-Type: text/css\r\n";
			const std::string video_mp4 = "Content-Type: video/mp4\r\n";
			const std::string image_jpeg = "Content-Type: image/jpeg\r\n";
			const std::string image_png = "Content-Type: image/png\r\n";
		}
		namespace common_headres {
			const std::string connection = "Connection: keep-alive\r\n";
			const std::string access_control_allow_origin = "access-control-allow-origin: *\r\n";
			const std::string accept_ranges = "Accept-Ranges: bytes\r\n";
			const std::string status_200 = "status: 200\r\n";
			const std::string host = "Host: 127.0.0.1:3100\r\n";
			const std::string origin = "Origin: 127.0.0.1:3100\r\n";
		}

		std::string get_stock_reply() {
			return "HTTP/1.1 200 OK\r\n"
				"Content-Type: text/html\r\n"
				//"Date: Sat, 01 Dec 2018 20:40:06 GMT\r\n"
				"Connection: keep-alive\r\n"
				"Content-Length: 16\r\n"
				"\r\n"
				"Hi, i am server";
		}

		std::string get_date_header() {
			time_t now = std::time(nullptr);
			std::tm tm;
			gmtime_s(&tm, &now);
			std::stringstream stringstream;
			stringstream << std::put_time(&tm, "%a, %d %b %Y %H:%M:%S GMT");
			std::string http_date;
			std::getline(stringstream, http_date);
			http_date += "\r\n";
			return "Date: " + http_date;
		}

		std::string get_index_page() {
			std::string reply;
			reply += http::client::stirng_status::ok;
			reply += http::client::mime_types::text_html;
			reply += get_date_header();
			std::ifstream file("index.html", std::ios::binary);
			if (!file.good()) {
				std::cout << "Couldn't open index.html" << std::endl;
			}
			std::string file_string;
			char buf[512];
			while (file.read(buf, sizeof(buf)).gcount()) {
				file_string.append(buf, file.gcount());
			}
			reply += "Content-Length: " + std::to_string(file_string.size()) + "\r\n";
			file.close();
			reply += "\r\n";
			reply += file_string;
			return reply;
		}

		std::string get_index_page_string() {
			std::string reply;
			reply += http::client::stirng_status::ok;
			reply += http::client::mime_types::text_html;
			reply += get_date_header();
			std::ifstream file("index.html");
			if (!file.good()) {
				std::cout << "Couldn't open index.html" << std::endl;
			}
			reply += "Content-Length: " + std::to_string(file.tellg()) + "\r\n";
			reply += "\r\n";
			std::string file_char;
			while (file >> file_char) {
				reply += file_char;
			}
			return reply;
		}

		assets::cache::cache response::_cache{true};

		response::response(std::shared_ptr<http::client::request> _request) : request(_request) {
			this->set_routes();
			std::cout << "Response constructed" << std::endl;
		}
		response::~response() {
			std::cout << "Response destructed" << std::endl;
		}
		void response::set_routes() {
			this->it = this->routes.begin();
			std::function<std::string()> reply_func = get_stock_reply;
			this->routes.insert(this->it, std::pair<std::string, std::function<std::string()>>("/", reply_func));
			reply_func = get_index_page;
			this->routes.insert(this->it, std::pair<std::string, std::function<std::string()>>("/index", reply_func));
		}
		void response::set_reply(std::string& url) {
			if (this->is_assets_url(url)) {
				this->set_assets_reply(url);
			}
			else {
				this->set_stock_reply(url);
			}
		}
		bool response::is_assets_url(std::string& url) {
			bool matched = false;
			std::regex regex{ "\/.*\\.(jpeg|mp4|js|html|jpg|png|css)$" };
			if (std::regex_match(url, regex)) {
				matched = true;
			}
			return matched;
		}
		void response::set_stock_reply(std::string& url) {
			this->it = this->routes.begin();
			this->it = this->routes.find(url);
			if (this->it != this->routes.end()) {
				this->reply_string = it->second();
			}
			else {
				this->reply_string = get_stock_reply();
			}
			this->reply_buf.push_back(boost::asio::buffer(this->reply_string));
		}
		void response::set_assets_reply(std::string& url) {
			this->reply_string += http::client::stirng_status::ok;
			this->reply_string += get_date_header();
			this->reply_string += http::client::common_headres::access_control_allow_origin;
			this->reply_string += http::client::common_headres::accept_ranges;
			this->reply_string += http::client::common_headres::status_200;
			this->reply_string += http::client::common_headres::host;
			this->reply_string += http::client::common_headres::origin;
			this->reply_string += "Content-Disposition: filename=" + url.substr(1, url.size()) + "\r\n";
			this->reply_string += this->get_asset_mime_type(url);
			this->set_asset_from_file(url);
			this->reply_buf.push_back(boost::asio::buffer(this->reply_string));
		}
		void response::set_asset_from_file(std::string& url) {
			std::string file_name = url.substr(1, url.size());
			std::string asset;
			asset.reserve(1000);
			asset = response::_cache.find_resource(file_name);
			if (asset.size() > 0) {
				this->upload_asset_from_cache(asset);
			}
			else {
				this->upload_asset_from_disk(asset, file_name);
			}
			
		}
		void response::upload_asset_from_disk(std::string& asset, std::string& name) {
			std::ifstream file_stream{ name, std::ios::binary | std::ios::ate };
			if (!file_stream.good()) {
				std::cerr << "Content was not found - " + name << std::endl;
				return;
			}
			this->reply_string += "Content-Length: " + std::to_string(file_stream.tellg()) + "\r\n";
			this->reply_string += "\r\n";
			file_stream.seekg(0);
			char read_buf[512];
			while (file_stream.read(read_buf, sizeof(read_buf)).gcount() > 0) {
				asset.append(read_buf, file_stream.gcount());
			}
			this->reply_string += asset;
			file_stream.close();
		}
		void response::upload_asset_from_cache(std::string& asset) {
			this->reply_string += "Content-Length: " + std::to_string(asset.size()) + "\r\n";
			this->reply_string += "\r\n";
			this->reply_string += asset;
		}
		std::string response::get_asset_mime_type(std::string& url) {
			std::string file_extension;
			std::regex regex{ "(jpeg|mp4|js|jpg)" };
			std::smatch matched;
			if (std::regex_search(url, matched, regex)) {
				file_extension = matched[0];
				if (file_extension == "html") {
					return http::client::mime_types::text_html;
				}
				else if (file_extension == "mp4") {
					return http::client::mime_types::video_mp4;
				}
				else if (file_extension == "jpg" || file_extension == "jpeg") {
					return http::client::mime_types::image_jpeg;
				}
				else {
					return http::client::mime_types::text_html;
				}
			}
			else {
				return http::client::mime_types::text_html;
			}
		}
	}
}