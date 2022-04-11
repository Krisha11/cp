#include "copier.h"
#include "file_descriptor.h"

#include <csignal>
#include <iostream>

// TODO Залить на git сделав CI

void signalHandler(int signum) {
   std::cout << "Interrupt signal (" << signum << ") received.\n";
   exit(signum);  
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage : ./main <src filename> <dst filename>\n";
        return EXIT_FAILURE;
    }

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    try {
        if (Copier::getInstance().MakeCopy(argv[1], argv[2]) != Copier::Error::OK) {
            std::cout << "Not successful\n";
            return EXIT_FAILURE;
        }

    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        std::cout << "Not successful\n";
        return EXIT_FAILURE;
    }


    std::cout << "Successful\n";

    return 0;
}
