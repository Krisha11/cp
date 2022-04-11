#pragma once

#include <unistd.h>

class FileDescriptor {
public:
    explicit FileDescriptor(const char* fn, int flags);

    ~FileDescriptor();

    int getDescriptor() const;

    size_t getFileSize() const;

private:
    int descr;
};
