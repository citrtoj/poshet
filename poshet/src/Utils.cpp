#include "Utils.hpp"

namespace Utils {
    ssize_t readLoop(int fd, void* buffer, size_t nbytes) {
        // if returned number is positive: if equal to nbytes then succeeded writing, else it piped somewhere
        // if returned number is negative: returns -readSofar - 1 (to distinguish from error on first read)
        if (nbytes < 0 || buffer == NULL) {
            return -1;
        }
        int readSoFar = 0;
        while (nbytes - readSoFar > 0) {
            int readCode = read(fd, (uint8_t*)buffer + readSoFar, nbytes - readSoFar);
            if (readCode < 1) {
                if (readCode == 0) {
                    return readSoFar;
                }
                else {
                    return -readSoFar - 1;
                }
            }
            readSoFar += readCode;
        }
        return readSoFar;
    }

    ssize_t writeLoop(int fd, const void* buffer, size_t nbytes) {
        // same as readLoop
        if (nbytes < 0 || buffer == NULL) {
            return -1;
        }
        int writtenSoFar = 0;
        while (nbytes - writtenSoFar > 0) {
            int writeCode = write(fd, (uint8_t*)buffer + writtenSoFar, nbytes - writtenSoFar);
            if (writeCode < 1) {
                if (writeCode == 0) {
                    return writtenSoFar;
                }
                else {
                    return -writtenSoFar - 1;
                }
            }
            writtenSoFar += writeCode;
        }
        return writtenSoFar;
    }

    // it's the same logic from the above functions but really I didn't think it was worth it to DRY them
    ssize_t readLoopSSL(SSL* ssl, void* buffer, size_t nbytes) {
        if (nbytes < 0 || buffer == NULL) {
            return -1;
        }
        int readSoFar = 0;
        while (nbytes - readSoFar > 0) {
            int readCode = SSL_read(ssl, (uint8_t*)buffer + readSoFar, nbytes - readSoFar);
            if (readCode < 1) {
                if (readCode == 0) {
                    return readSoFar;
                }
                else {
                    return -readSoFar - 1;
                }
            }
            readSoFar += readCode;
        }
        return readSoFar;
    }

    ssize_t writeLoopSSL(SSL* ssl, const void* buffer, size_t nbytes) {
        if (nbytes < 0 || buffer == NULL) {
            return -1;
        }
        int writtenSoFar = 0;
        while (nbytes - writtenSoFar > 0) {
            int writeCode = SSL_write(ssl, (uint8_t*)buffer + writtenSoFar, nbytes - writtenSoFar);
            if (writeCode < 1) {
                if (writeCode == 0) {
                    return writtenSoFar;
                }
                else {
                    return -writtenSoFar - 1;
                }
            }
            writtenSoFar += writeCode;
        }
        return writtenSoFar;
    }


    std::string strip(const std::string& str) {
        auto start = std::find_if(str.begin(), str.end(), [](char c) {
            return !std::isspace(c);
        });

        auto end = std::find_if(str.rbegin(), str.rend(), [](char c) {
            return !std::isspace(c);
        }).base();

        return (start < end) ? std::string(start, end) : std::string();
    }

    std::string sha256(const std::string& input) {
        EVP_MD_CTX *mdctx;
        const EVP_MD *md;
        unsigned char hash[SHA256_DIGEST_LENGTH];
        unsigned int hashLen;
        OpenSSL_add_all_digests();
        md = EVP_get_digestbyname("sha256");
        if (!md) {
            return "";
        }
        mdctx = EVP_MD_CTX_new();
        EVP_DigestInit_ex(mdctx, md, NULL);
        EVP_DigestUpdate(mdctx, input.c_str(), input.length());
        EVP_DigestFinal_ex(mdctx, hash, &hashLen);
        EVP_MD_CTX_free(mdctx);
        std::string hashedString;
        for (int i = 0; i < hashLen; ++i) {
            char hex[3];
            sprintf(hex, "%02x", hash[i]);
            hashedString += hex;
        }

        return hashedString;
    }
}

std::string Utils::fileSizeToString(unsigned long long fileSize) {
    std::ostringstream stream;
    double doubleFileSize = fileSize;
    std::vector<std::string> sizes = { "B", "KB", "MB", "GB", "TB", "PB", "EB" };
    int i = 0;
    while (doubleFileSize >= 1024 && i < sizes.size()) {
        doubleFileSize /= 1024;
        i++;
    }
    stream << std::setprecision(3) <<  doubleFileSize;
    std::string filesizeString = stream.str();
    return filesizeString + " " + sizes[i];
}