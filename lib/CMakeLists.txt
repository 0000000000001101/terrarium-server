include(FetchContent)

#- SPDLOG -------------------------------------------------------------------

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.17.0
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)

FetchContent_MakeAvailable(spdlog)

#- JSON ---------------------------------------------------------------------

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.12.0
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)

FetchContent_MakeAvailable(json)

#- ASIO ---------------------------------------------------------------------

find_package(Threads REQUIRED)

FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG        asio-1-32-0 # asio-1-38-2
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)

FetchContent_MakeAvailable(asio)

add_library(asio_target INTERFACE)

set_target_properties(asio_target PROPERTIES
    CXX_STANDARD 11
)

target_include_directories(asio_target SYSTEM INTERFACE
    "${asio_SOURCE_DIR}/asio/include"
)

target_compile_definitions(asio_target INTERFACE
    ASIO_STANDALONE
)

target_link_libraries(asio_target INTERFACE
    Threads::Threads
)

#- WEBSOCKET++ --------------------------------------------------------------

FetchContent_Declare(
    websocketpp
    GIT_REPOSITORY https://github.com/zaphoyd/websocketpp.git
    GIT_TAG        0.8.2
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

FetchContent_MakeAvailable(websocketpp)

add_library(websocketpp_headers INTERFACE)
target_compile_definitions(websocketpp_headers INTERFACE
    _WEBSOCKETPP_CPP11_TYPE_TRAITS_
)
target_include_directories(websocketpp_headers SYSTEM INTERFACE "${websocketpp_SOURCE_DIR}")
target_link_libraries(websocketpp_headers INTERFACE asio_target)
add_library(websocketpp::websocketpp ALIAS websocketpp_headers)
