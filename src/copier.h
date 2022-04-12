#pragma once

#include "file_descriptor.h"

#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

class Copier {
public:
    enum Error {
        OK,
        FAIL,
        DIFFERENT_FILESYSTEMS
    };

    static Copier& getInstance();
    Copier(Copier const&) = delete;
    void operator=(Copier const&)  = delete;
    
    ~Copier();
    Error MakeCopy(const char* src, const char* dst);

private:
    Copier();

    static Error MakeRealCopy(const std::string &src, FileDescriptor srcDescr, FileDescriptor dstDescr);
    static Error MakeLinkCopy(const std::string &src, const std::string &dst);
    static Error MakeHardLink(const std::string &src, const std::string &dst);

    static Error UpdateDstPath(const std::string src, const std::string dst, std::string &temporaryDst, std::string &realDst, std::vector<std::string> &created);

    static void CreateFile(const std::string& file);

private:
    std::string Src;
    std::string TemporaryDst;
    std::string Dst;
    std::vector<std::string> Created;
    bool IsConfirmed;
    bool SavedTemporary;
};
