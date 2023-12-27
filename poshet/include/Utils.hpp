#pragma once

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include <string>
#include <iostream>
#include <algorithm>

#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/ssl.h>

namespace Utils {
    int readLoop(int fd, void* buffer, int nbytes);
    int readLoopSSL(SSL* ssl, void* buffer, int nbytes);
    int writeLoop(int fd, const void* buffer, int nbytes);
    int writeLoopSSL(SSL* ssl, const void* buffer, int nbytes);

    std::string strip(const std::string& str);
    std::string sha256(const std::string& input);
}