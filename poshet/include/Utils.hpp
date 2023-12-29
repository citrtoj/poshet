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
#include <openssl/bio.h>

namespace Utils {
    ssize_t readLoop(int fd, void* buffer, size_t nbytes);
    ssize_t readLoopSSL(SSL* ssl, void* buffer, size_t nbytes);
    ssize_t writeLoop(int fd, const void* buffer, size_t nbytes);
    ssize_t writeLoopSSL(SSL* ssl, const void* buffer, size_t nbytes);

    std::string strip(const std::string& str);
    std::string encodeToSHA256(const std::string& input);
    std::string encodeToBase64(const std::string& input);
    std::string fileSizeToString(unsigned long long fileSize);
    std::time_t mailDateToUnixTimestamp(const std::string& dateString);
}