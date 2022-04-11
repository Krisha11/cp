#include "file_descriptor.h"

#include <iostream>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

FileDescriptor::FileDescriptor(const char* fn, int flags) {
    errno = 0;
    descr = open(fn, flags);
    if (descr < 0) {
        std::cout << "descr:" << fn << ' ' << flags << ' ' << descr << "\n";
        throw std::runtime_error(strerror(errno));
    }
}

FileDescriptor::~FileDescriptor() {
    close(descr);
}

int FileDescriptor::getDescriptor() const {
    return descr;
}

size_t FileDescriptor::getFileSize() const {
    struct stat statbuf = {};
    errno = 0;
    int ret_code = fstat(descr, &statbuf);

    if (ret_code != 0) {
        throw std::runtime_error(strerror(errno));
    }

    return statbuf.st_size;
}
