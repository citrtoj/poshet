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
#include <sstream>
#include <iomanip>

#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/ssl.h>

namespace Utils {
    ssize_t readLoop(int fd, void* buffer, size_t nbytes);
    ssize_t readLoopSSL(SSL* ssl, void* buffer, size_t nbytes);
    ssize_t writeLoop(int fd, const void* buffer, size_t nbytes);
    ssize_t writeLoopSSL(SSL* ssl, const void* buffer, size_t nbytes);

    std::string strip(const std::string& str);
    std::string sha256(const std::string& input);
    std::string fileSizeToString(unsigned long long fileSize);
}