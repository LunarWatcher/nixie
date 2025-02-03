#include "Config.hpp"
#include <fstream>

#include <nlohmann/json.hpp>

namespace nixie {

Config::Config(const std::string& confSource) {
    std::ifstream f(confSource);
    if (!f) {
        throw std::runtime_error("Failed to find config file");
    }

    nlohmann::json j;
    f >> j;

    j.at("token").get_to(token);
}

}
