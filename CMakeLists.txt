project(ntpwedit_clone)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

add_executable(ntpwedit_clone WIN32
    src/main.cpp
    src/ntreg.cpp
    src/sam.cpp
    src/crypto.cpp
    src/ntpwedit.rc
)

target_include_directories(ntpwedit_clone PRIVATE src)
target_link_libraries(ntpwedit_clone comctl32)
