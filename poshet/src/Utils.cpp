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
            int bytesAmountToRead = std::min(nbytes - readSoFar, IO_LIMIT);
            int readCode = read(fd, (uint8_t*)buffer + readSoFar, bytesAmountToRead);
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
    ssize_t recvLoop(int fd, void* buffer, size_t nbytes, int flags) {
        if (nbytes < 0 || buffer == NULL) {
            return -1;
        }
        int readSoFar = 0;
        while (nbytes - readSoFar > 0) {
            int bytesAmountToRead = std::min(nbytes - readSoFar, IO_LIMIT);
            int readCode = recv(fd, (uint8_t*)buffer + readSoFar, bytesAmountToRead, flags);
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
            int bytesAmountToWrite = std::min(nbytes - writtenSoFar, IO_LIMIT);
            int writeCode = write(fd, (uint8_t*)buffer + writtenSoFar, bytesAmountToWrite);
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

    ssize_t sendLoop(int fd, const void* buffer, size_t nbytes, int flags) {
        if (nbytes < 0 || buffer == NULL) {
            return -1;
        }
        int writtenSoFar = 0;
        while (nbytes - writtenSoFar > 0) {
            int bytesAmountToWrite = std::min(nbytes - writtenSoFar, IO_LIMIT);
            int writeCode = send(fd, (uint8_t*)buffer + writtenSoFar, bytesAmountToWrite, flags);
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
    
    ssize_t readLoopSSL(SSL* ssl, void* buffer, size_t nbytes) {
        if (nbytes < 0 || buffer == NULL) {
            return -1;
        }
        int readSoFar = 0;
        while (nbytes - readSoFar > 0) {
            int bytesAmountToRead = std::min(nbytes - readSoFar, IO_LIMIT);
            int readCode = SSL_read(ssl, (uint8_t*)buffer + readSoFar, bytesAmountToRead);
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
            int bytesAmountToWrite = std::min(nbytes - writtenSoFar, IO_LIMIT);
            int writeCode = SSL_write(ssl, (uint8_t*)buffer + writtenSoFar, bytesAmountToWrite);
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

    std::string encodeToSHA256(const std::string& input) {
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
    std::string fileSizeToString(long double fileSize) {
        std::ostringstream stream;
        std::vector<std::string> sizes = { "B", "KB", "MB", "GB", "TB", "PB", "EB" };
        int i = 0;
        while (fileSize >= 1024 && i < sizes.size()) {
            fileSize /= 1024;
            i++;
        }
        stream << std::setprecision(3) <<  fileSize;
        std::string filesizeString = stream.str();
        return filesizeString + " " + sizes[i];
    }

    std::time_t mailDateToUnixTimestamp(const std::string& dateString) {
        std::tm timeStruct = {};
        std::istringstream ss(dateString);
        ss.imbue(std::locale::classic());
        ss >> std::get_time(&timeStruct, "%a, %d %b %Y %H:%M:%S");
        if (ss.fail()) {
            return -1;
        }

        char sign;
        int timezoneOffset;
        ss >> sign >> timezoneOffset;
        int offsetInSeconds = (sign == '+') ? timezoneOffset * 36 : -timezoneOffset * 36;
        std::time_t timestamp = std::mktime(&timeStruct) - offsetInSeconds;
        return timestamp;

    }

    std::string encodeToBase64(const std::string& input) {
        BIO *bio, *b64;
        BUF_MEM *bptr;
        b64 = BIO_new(BIO_f_base64());
        bio = BIO_new(BIO_s_mem());
        bio = BIO_push(b64, bio);
        BIO_write(bio, input.c_str(), input.length());
        BIO_flush(bio);
        BIO_get_mem_ptr(bio, &bptr);
        std::string result(bptr->data, bptr->length);
        BIO_free_all(bio);
        return result;
    }

    std::string displayDisplayableHeader(const std::vector<std::string>& vec) {
        std::string label = "";
        if (vec.size() != 0) {
            label += vec[0];
            if (vec.size() > 1) {
                label += " (" + vec[1] + ")";
            }
        }
        return label;
    }

    std::string generateUUID() {
        std::random_device rd;
        std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dis(0, 15);
        static std::uniform_int_distribution<> dis2(8, 11);
        const char* hex_chars = "0123456789abcdef";
        std::stringstream ss;
        int i;
        ss << std::hex;
        for (i = 0; i < 8; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 4; i++) {
            ss << dis(gen);
        }
        ss << "-4";
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        ss << dis2(gen);
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 12; i++) {
            ss << dis(gen);
        };
        return ss.str();
    }
    
    std::string hexEncode(const std::string &input) {
        std::ostringstream encoded;
        encoded << std::hex << std::setfill('0');

        for (unsigned char c : input) {
            encoded << std::setw(2) << static_cast<unsigned int>(c);
        }

        return encoded.str();
    }
    
    std::string hexDecode(const std::string &input) {
        std::string decoded;
        decoded.reserve(input.length() / 2);

        for (std::size_t i = 0; i < input.length(); i += 2) {
            std::istringstream iss(input.substr(i, 2));
            unsigned int value;
            iss >> std::hex >> value;

            decoded.push_back(static_cast<char>(value));
        }

        return decoded;
    }
}

