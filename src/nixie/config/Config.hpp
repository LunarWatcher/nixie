#pragma once

#include <string>

namespace nixie {

struct Config {
    std::string token;

    Config(const std::string& confSource = "./nixie.conf");

};

}
