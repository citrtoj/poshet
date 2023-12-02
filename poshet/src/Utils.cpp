#include "Utils.hpp"
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

namespace Utils {
    int readLoop(int fd, void* buffer, int nbytes) {
        // returns readSoFar at first error/close (if error on first read returns -1, or 0 on close) or when finished. also returns -1 on bad params.
        if (nbytes < 0 || buffer == NULL) {
            return -1;
        }
        int readSoFar = 0;
        // uint8_t -- ca sa fim siguri ca scriem in bytes. De unde stiu eu ca char-ul e mereu 1 byte?
        while (nbytes - readSoFar > 0) {
            int readCode = read(fd, (uint8_t*)buffer + readSoFar, nbytes - readSoFar);
            if (readCode < 1) {
                if (readSoFar == 0) {
                    return -1;
                }
                else {
                    return readSoFar;
                }
            }
            readSoFar += readCode;
        }
        return readSoFar;
    }

    int writeLoop(int fd, const void* buffer, int nbytes) {
        // returns writtenSoFar at first error/close (if error on first write returns -1, or 0 on close) or when finished. also returns -1 on bad params.
        if (nbytes < 0 || buffer == NULL) {
            return -1;
        }
        int writtenSoFar = 0;
        while (nbytes - writtenSoFar > 0) {
            int writeCode = write(fd, (uint8_t*)buffer + writtenSoFar, nbytes - writtenSoFar);
            if (writeCode <= 1) {
                if (writtenSoFar == 0) {
                    return writeCode;
                }
                else {
                    return writtenSoFar;
                }
            }
            writtenSoFar += writeCode;
        }
        return writtenSoFar;
    }

}
