#include "Config.hpp"

Config& Config::getInstance() {
    static Config instance;
    return instance;
}

Config::Config() {}