#include "copier.h"

#include <iostream>
#include <fcntl.h>
#include <fstream>
#include <string.h>


constexpr size_t READ_CHUNK_SIZE = 4 * 1024;
// const int Error::DIFFERENT_FILESYSTEMS = 2;

Copier& Copier::getInstance() {
    static Copier instance;
    return instance;
}

Copier::~Copier() {
    std::error_code ec; // ~Copier must be noexcept
    if (!IsConfirmed) {
        if (SavedTemporary) {

            if (!ec) {
                fs::rename(TemporaryDst, Dst, ec); // restore temporary saving of old file
            }

            Created.push_back(TemporaryDst);
        }

        for (const auto& elem : Created) {
            if (ec) {
                continue;
            }

            std::cout << "Removing : " << elem << "\n";
            if (fs::exists(elem, ec) && !ec) {
                fs::remove_all(elem, ec);
            }
        }
    } else if (SavedTemporary) {
        std::cout << "Removing : " << TemporaryDst << "\n";
        if (fs::exists(TemporaryDst, ec) && !ec) {
            fs::remove_all(TemporaryDst, ec);
        }

    }

    if (ec) {
        std::cerr << "Problems with deleting extra files and directories \n";
    }
}

Copier::Error Copier::MakeCopy(const char* src, const char* dst) {
    Src = src;
    if (UpdateDstPath(src, dst, TemporaryDst, Dst, Created) != Error::OK) {
        return Error::FAIL;
    }

    if (fs::exists(Dst)) {
        std::cout << "Saving " << Dst << " to " << TemporaryDst << "\n";
        MakeHardLink(Dst, TemporaryDst); // save temporary old file

        fs::remove_all(Dst);

        SavedTemporary = true;
    }

    int exitCode = 0;
    bool copyLink = fs::is_symlink(Src);

    if (copyLink) {
        // - если src -- symlink, то копируется ссылка у дескриптора добавить метод is_symlink
        std::cout << "Making sym link to src PATH because src is a symlink\n";
        exitCode = MakeLinkCopy(Src, Dst);
    } else {
        // - Если dst находится в той же файловой системе, что и src, то делается hardlink (быстрая операция)
        std::cout << "Trying to make hard link because src is not a symlink\n";
        exitCode = MakeHardLink(Src, Dst);

        if (exitCode == Error::DIFFERENT_FILESYSTEMS) {
            CreateFile(Dst);

            std::cout << "Making real copy because of different file systems and src is not a symlink\n";
            exitCode = MakeRealCopy(Src, FileDescriptor(Src.c_str(), O_RDONLY), FileDescriptor(Dst.c_str(), O_WRONLY));
        }
    }

    if (exitCode != 0) {
        return Error::FAIL;
    }

    IsConfirmed = true;
    return Error::OK;
}

Copier::Copier() : IsConfirmed(false), SavedTemporary(false) {}

Copier::Error Copier::MakeRealCopy(const std::string &src, FileDescriptor srcDescr, FileDescriptor dstDescr) {
    size_t fsize = srcDescr.getFileSize();

    size_t current_size = 0;
    std::vector<char> file(fsize);

    while(current_size < fsize) {
        size_t want_to_read = std::min(READ_CHUNK_SIZE, fsize - current_size);

        errno = 0;
        ssize_t readed = read(srcDescr.getDescriptor(), file.data() + current_size, want_to_read);
        if (readed == -1) {
            switch (errno) {
                case EINTR:
                    continue;                    
                default:
                    std::cerr << "Something wrong with file: " << src << ' ' << strerror(errno) << "\n";
                    return Error::FAIL;
            }
        } else if (readed == 0) {
            std::cerr << "File trunkated!\nCurrent size: " << current_size << "\nExpected size: " << fsize << "\n";
            return Error::FAIL;
        }

        ssize_t wrote = write(dstDescr.getDescriptor(), file.data() + current_size, readed);
        if (wrote != readed) {
            std::cerr << wrote << ' ' << readed << ' ' << "Problems with copying " << strerror(errno) << "\n";
            return Error::FAIL;
        }

        current_size += readed;
        std::cout << "\r" << "Copied : " << current_size << "/" << fsize;
    }
    std::cout << "\n";

    return Error::OK;
}

Copier::Error Copier::MakeLinkCopy(const std::string &src, const std::string &dst) {
    // - если src -- symlink, то копируется ссылка
    fs::path symlink = fs::read_symlink(src);

    if (!symlink.has_parent_path()) {
        symlink = fs::path(src).parent_path() / symlink;
    }

    std::cout << "Creating symlink to " << symlink << "\n";
    fs::create_symlink(symlink, dst);

    return Error::OK;
}

Copier::Error Copier::MakeHardLink(const std::string &src, const std::string &dst) {
    // - Если dst находится в той же файловой системе, что и src, то делается hardlink (быстраа операция)
    std::error_code ec;
    fs::create_hard_link(src, dst, ec);

    if (ec.value() == EXDEV) {
        std::cerr << "Can't create hard link, different file systems\n";
        return Error::DIFFERENT_FILESYSTEMS;
    } else if (ec) {
        std::cerr << "Can't create hard link " << src << ' ' << dst << ' ' << ec << "\n";
        return Error::FAIL;
    }

    return Error::OK;
}

void Copier::CreateFile(const std::string& file) {
    std::cout << "Creating file " << file << "\n";
    std::ofstream ofs(file);
    ofs.close();
}

Copier::Error Copier::UpdateDstPath(const std::string src, const std::string dst, std::string &temporaryDst, std::string &realDst, std::vector<std::string> &created) {
    // simple src checks
    fs::path srcPath(src);    
    if (!fs::exists(srcPath)) {
        std::cerr << "Src file: " << src << " does not exist.\n";
        return Error::FAIL;
    }

    if (fs::is_directory(srcPath)) {
        std::cerr << "Src file: " << src << " is a directory, not ordinary file.\n";
        return Error::FAIL;
    }


    // - Если dst -- каталог, то создается файл dst/src
    fs::path dstPath(dst);
    if (fs::is_directory(dstPath)) {
        dstPath /= srcPath;
    }

    if (fs::is_directory(dstPath)) {
        std::cerr << "Dst file: " << dstPath << " is a directory, not ordinary file.\n";
        return Error::FAIL;
    }

    realDst = dstPath;

    // - Ecли dst не существует, то утилита пытается его создать (рекурсивно, со всеми путями)
    if (!fs::exists(dstPath)) {
        fs::path spoiltDstPath(std::string(dstPath));
        while (!fs::exists(dstPath) && dstPath.parent_path() != dstPath) {
            created.push_back(dstPath);
            dstPath = dstPath.parent_path();
        }

        // create directories
        for (int i = created.size() - 1; i >= 1; i--) {
            std::cout << "Creating directory : " << created[i] << "\n";
            fs::create_directory(created[i]);
        }
    }

    dstPath = fs::path(realDst + "_");
    while (fs::exists(dstPath)) {
        dstPath += "_";
    }

    // путь для бэкапа старого файла
    temporaryDst = dstPath;

    return Error::OK;
}
