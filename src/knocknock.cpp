#include <glog/logging.h>

#include <memory>

#include "cartridge.h"

int main(int argc, char *argv[]) {
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();

    std::unique_ptr<Cartridge> cartridge;
    if (!Cartridge::from_file(argv[1], &cartridge)) {
        LOG(ERROR) << "Unable to load cartridge";
    }

    return 0;
}
