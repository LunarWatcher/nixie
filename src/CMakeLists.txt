add_library(nixiesrc STATIC 
    nixie/config/Config.cpp

    nixie/apps/MessageApps.cpp

    nixie/apps/math/Units.cpp

    nixie/util/Preprocessor.cpp
)
add_executable(nixie nixie/Main.cpp)

target_include_directories(nixiesrc PUBLIC ./)
target_link_libraries(nixiesrc
PUBLIC
    dpp
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    utf8cpp
)

target_link_libraries(nixie PUBLIC nixiesrc)

# vim:ft=cmake
