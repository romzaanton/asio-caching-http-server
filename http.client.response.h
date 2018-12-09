#pragma once
#include <chrono>
#include <functional>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <memory>
#include <regex>
#include <sstream>
#include <string>
#include <time.h>
#include <utility>
#include <vector>
#include <boost/asio.hpp>
#include "http.client.request.h"
#include "assets.caching.h"

namespace http {
	namespace client {
		
		std::string get_stock_reply();
		std::string get_index_page();
		std::string get_index_page_string();
		std::string get_date_header();

		class response {
		public:
			response() = delete;
			response(const response&) = delete;
			response& operator = (const response&) = delete;
			response(std::shared_ptr<http::client::request>);
			~response();
			void set_reply(std::string&);
			std::string reply_string;
			std::vector<boost::asio::const_buffer> reply_buf;
		private:
			static assets::cache::cache _cache;
			std::shared_ptr<http::client::request> request;
			std::map<std::string, std::function<std::string()>> routes;
			std::map<std::string, std::function<std::string()>>::iterator it;
			void set_routes();
			bool is_assets_url(std::string& url);
			void set_stock_reply(std::string& url);
			void set_assets_reply(std::string&);
			void set_asset_from_file(std::string& url);
			void upload_asset_from_disk(std::string& asset, std::string& name);
			void upload_asset_from_cache(std::string& asset);
			std::string get_asset_mime_type(std::string& url);
		};
	}
}