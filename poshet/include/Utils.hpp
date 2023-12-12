#pragma once

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include <string>
#include <algorithm>

namespace Utils {
    int readLoop(int fd, void* buffer, int nbytes);
    int writeLoop(int fd, const void* buffer, int nbytes);

    std::string strip(const std::string& str);
}