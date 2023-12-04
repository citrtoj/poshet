#include "Config.hpp"

Config& Config::getInstance() {
    static Config instance;
    return instance;
}

Config::Config() : RES_WIDTH(800),
                   RES_HEIGHT(600),
                   DB_PATH("../mail.db"),
                   APP_NAME("Poshet") {}