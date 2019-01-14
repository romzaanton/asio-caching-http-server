#pragma once
#include <fstream>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <map>
#include <string>
#include <Windows.h>
#include <strsafe.h>
#include <utility>
#include <vector>

namespace assets {
	namespace cache {

		void load_predifined_resources(std::string& name, std::string& assets);
		
		class cache  {
		public:
			cache() = delete;
			cache(const cache&) = delete;
			cache& operator = (const cache&) = delete;
			cache(bool use_predefined);
			std::string find_resource(std::string& name) const;
		private:
			std::map<std::string, std::string>::iterator cache_iter;
			std::map<std::string, std::string> cache_storage;
			std::vector<std::string> predefined_resources = { "beach.jpeg", "video.mp4" };
		};
	}
}