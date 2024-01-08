#pragma once

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/socket.h>

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
    static constexpr unsigned long IO_LIMIT = 512;

    ssize_t readLoop(int fd, void* buffer, size_t nbytes);
    ssize_t recvLoop(int fd, void* buffer, size_t nbytes, int flags = 0);
    ssize_t readLoopSSL(SSL* ssl, void* buffer, size_t nbytes);
    ssize_t writeLoop(int fd, const void* buffer, size_t nbytes);
    ssize_t sendLoop(int fd, const void* buffer, size_t nbytes, int flags = 0);
    ssize_t writeLoopSSL(SSL* ssl, const void* buffer, size_t nbytes);

    std::string strip(const std::string& str);
    std::string encodeToSHA256(const std::string& input);
    std::string encodeToBase64(const std::string& input);
    std::string fileSizeToString(long double fileSize);
    std::time_t mailDateToUnixTimestamp(const std::string& dateString);

    std::string displayDisplayableHeader(const std::vector<std::string>& vec);
}