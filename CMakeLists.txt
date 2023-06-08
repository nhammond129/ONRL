cmake_minimum_required(VERSION 3.21)
project(ONRL LANGUAGES CXX)

if (WIN32)
    set(CMAKE_GENERATOR_PLATFORM x64)
endif()

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.5.1)
FetchContent_MakeAvailable(SFML)

include_directories(ONRL
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_executable(ONRL
    src/main.cpp
    src/console.cpp
    src/map.cpp
    src/util.cpp
    src/player.cpp
)

target_link_libraries(ONRL PRIVATE sfml-graphics sfml-window sfml-system sfml-audio)
target_compile_features(ONRL PRIVATE cxx_std_20)
if (WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET ONRL POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:ONRL> $<TARGET_FILE_DIR:ONRL> COMMAND_EXPAND_LISTS)
endif()

install(TARGETS ONRL)