add_executable(knocknock-cli knocknock-cli.cpp)
target_link_libraries(knocknock-cli PRIVATE knocknock glog::glog)
target_compile_features(knocknock-cli PRIVATE cxx_std_17)
