#include <glog/logging.h>

#include <optional>

#include <knocknock/cartridge.h>

int main(int argc, char *argv[]) {
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();

    std::optional<Cartridge> cartridge = Cartridge::from_file(argv[1]);
    if (!cartridge) {
        LOG(ERROR) << "Unable to load cartridge";
    }

    return 0;
}
