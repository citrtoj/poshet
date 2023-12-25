#pragma once

#include <string>

class Config {
public:
    static Config& getInstance();

    std::string _appName;

    // open config at startup... presume that there's a config.txt file right next to the executable
private:
    Config();
};
