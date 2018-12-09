#include "pch.h"
#include "assets.caching.h"


namespace assets {
	namespace cache {

		void load_predifined_resources(std::string& name, std::string& assets) {
			std::ifstream file(name, std::ios::binary);
			if (!file.good()) {
				std::cout << "Couldn't open " + name << std::endl;
			}
			std::string file_string;
			char buf[512];
			while (file.read(buf, sizeof(buf)).gcount() > 0) {
				file_string.append(buf, file.gcount());
			}
			assets.append(file_string);
		};

		cache::cache(bool use_predefined = true) {
			if (use_predefined) {
				for (auto resource : this->predefined_resources) {
					std::future<std::string> file_string = std::async(std::launch::deferred, [this](std::string name) {
						std::string async_assets;
						assets::cache::load_predifined_resources(name, async_assets);
						return async_assets;
					}, resource);
					if (cache::find_resource(resource).size() > 0) {
						continue;
					}
					std::cout << resource << " inda house" << std::endl;
					std::map<std::string, std::string>::iterator it = this->cache_storage.begin();
					this->cache_storage.insert(it, std::pair<std::string, std::string>(resource, file_string.get()));
				}
			}
		}
		std::string cache::find_resource(std::string& name) const {
			auto found = this->cache_storage.find(name);
			if (found != this->cache_storage.end()) {
				return found->second;
			}
			else {
				return "";
			}
		}
	}
}