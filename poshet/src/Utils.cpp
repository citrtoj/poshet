#include "Utils.hpp"
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

namespace Utils {
    int readLoop(int fd, void* buffer, int nbytes) {
        // if returned number is positive -- if equal to nbytes then succeeded writing, else it piped somewhere
        // if returned number is negative then it's -readSofar-1 (to distinguish from error on first read)
        if (nbytes < 0 || buffer == NULL) {
            return -1;
        }
        int readSoFar = 0;
        while (nbytes - readSoFar > 0) {
            int readCode = read(fd, (uint8_t*)buffer + readSoFar, nbytes - readSoFar);
            if (readCode < 1) {
                if (readSoFar == 0) {
                    return readCode;
                }
                else {
                    return -readSoFar - 1;
                }
            }
            readSoFar += readCode;
        }
        return readSoFar;
    }

    int writeLoop(int fd, const void* buffer, int nbytes) {
        // same as writeLoop
        if (nbytes < 0 || buffer == NULL) {
            return -1;
        }
        int writtenSoFar = 0;
        while (nbytes - writtenSoFar > 0) {
            int writeCode = write(fd, (uint8_t*)buffer + writtenSoFar, nbytes - writtenSoFar);
            if (writeCode < 1) {
                if (writtenSoFar == 0) {
                    return writeCode;
                }
                else {
                    return -writtenSoFar - 1;
                }
            }
            writtenSoFar += writeCode;
        }
        return writtenSoFar;
    }

}
