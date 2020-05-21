add_subdirectory(knocknock)

add_executable(knocknock-cli knocknock.cpp)
target_link_libraries(knocknock-cli PRIVATE knocknock)
target_compile_features(knocknock-cli PRIVATE cxx_std_17)

