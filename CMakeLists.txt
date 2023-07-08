cmake_minimum_required(VERSION 3.5)
project(Encfs)

message(STATUS "src this is source dir " ${CMAKE_SOURCE_DIR})

# set different program names
SET(ENCFS "encfs")
SET(ENCFSCTL "encfsctl")
SET(MAKEKEY "makekey")

# set property
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -w -O0 -Wall -g -ggdb -Wunused -D_FILE_OFFSET_BITS=64 -D_GNU_SOURCE -D_SVID_SOURCE -D_DEFAULT_SOURCE -fpermissive")
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/build/lib)

# find source cpp files
file(GLOB_RECURSE source_list_echo_server "src/*.cpp")

# find include files
include_directories(
    include/
    intl/
)

# generate executable files
add_library(${ENCFS} SHARED ${source_list_echo_server})

add_executable(${MAKEKEY}
    ${CMAKE_CURRENT_SOURCE_DIR}/main/main.cpp)
target_link_libraries(${MAKEKEY} -lpthread -lssl -ltinyxml2 -ltinyxml2 -lfuse)

#add_executable(${ENCFSCTL}
#    ${CMAKE_CURRENT_SOURCE_DIR}/main/encfsctl.cpp)
#target_link_libraries(${ENCFSCTL} PRIVATE ${ENCFS} -lpthread -lssl -ltinyxml2
#    -lfuse)
#add_executable(${MAKEKEY}
#    ${CMAKE_CURRENT_SOURCE_DIR}/main/makeKey.cpp)
#target_link_libraries(${MAKEKEY} -lpthread -lssl -ltinyxml2 -ltinyxml2 -lfuse)
