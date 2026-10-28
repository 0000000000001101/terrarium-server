cmake_minimum_required(VERSION 3.20...4.4)

project(TerrariumServer
    VERSION 0.0.0
    LANGUAGES CXX
)

option(TERRARIUM_WARNINGS_AS_ERRORS
    "Treat warnings as errors"
    OFF
)

option(TERRARIUM_USE_STATIC_MSVC_RUNTIME
    "Use the static MSVC runtime (/MT, /MTd) instead of the dynamic runtime (/MD, /MDd)"
    ON
)

# Set before creating targets so it initializes the runtime setting for targets
# created here and in lib/. When the option is OFF, CMake's default is dynamic.
if(MSVC AND TERRARIUM_USE_STATIC_MSVC_RUNTIME)
    set(CMAKE_MSVC_RUNTIME_LIBRARY
        "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
endif()

add_executable(TerrariumServer
    src/main.cpp
    src/Server.cpp
    src/Server.hpp
)

target_include_directories(TerrariumServer PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# Terrarium Server's C++ version
target_compile_features(TerrariumServer PRIVATE cxx_std_23)

# Ask for standard C++. No compiler-specific extensions. (Keeps things portable!)
set_target_properties(TerrariumServer PROPERTIES
    CXX_EXTENSIONS OFF
)

target_compile_definitions(TerrariumServer PRIVATE
    "$<$<CONFIG:Debug>:TERRARIUM_DEBUG>"

    "$<$<CONFIG:RelWithDebInfo>:TERRARIUM_DEBUG>"
    "$<$<CONFIG:RelWithDebInfo>:TERRARIUM_REL_WITH_DEB_INFO>"

    "$<$<CONFIG:Release>:TERRARIUM_RELEASE>"

    "$<$<CONFIG:MinSizeRel>:TERRARIUM_RELEASE>"
    "$<$<CONFIG:MinSizeRel>:TERRARIUM_MIN_SIZE_REL>"
)

if(MSVC)
    target_compile_options(TerrariumServer PRIVATE /W4)
    if(TERRARIUM_WARNINGS_AS_ERRORS)
        target_compile_options(TerrariumServer PRIVATE /WX)
    endif()
else()
    target_compile_options(TerrariumServer PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
    if(TERRARIUM_WARNINGS_AS_ERRORS)
        target_compile_options(TerrariumServer PRIVATE -Werror)
    endif()
endif()

add_subdirectory(lib)

target_link_libraries(TerrariumServer PRIVATE spdlog::spdlog)
target_link_libraries(TerrariumServer PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(TerrariumServer PRIVATE websocketpp::websocketpp)

# TODO: CTest
