#pragma once
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

namespace Utils {
    static const char* APP_NAME = "Poshet";

    int readLoop(int fd, void* buffer, int nbytes);
    int writeLoop(int fd, const void* buffer, int nbytes);

    
}