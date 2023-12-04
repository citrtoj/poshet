#pragma once

#include <string>

class Config {
public:
    static Config& getInstance();

    std::string APP_NAME;
    std::string DB_PATH;

    int RES_HEIGHT;
    int RES_WIDTH;
private:
    Config();
};
