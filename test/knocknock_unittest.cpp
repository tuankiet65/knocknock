#define CATCH_CONFIG_RUNNER

#include <glog/logging.h>
#include <catch2/catch.hpp>

int main(int argc, char *argv[]) {
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();

    // Write log to stderr
    FLAGS_logtostderr = true;

    // Enable log color.
    FLAGS_colorlogtostderr = true;

    // Log messages at level WARNING and above by default.
    FLAGS_minloglevel = google::GLOG_WARNING;

    // Then run the test suite.
    return Catch::Session().run(argc, argv);
}
