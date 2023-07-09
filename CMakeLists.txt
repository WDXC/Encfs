cmake_minimum_required(VERSION 3.5)
project(Encfs)

message(STATUS "src this is source dir " ${CMAKE_SOURCE_DIR})

# set different program names
SET(ENCFS "encfs")
SET(ENCFS_MAIN "encfs_main")
SET(ENCFSCTL "encfsctl")
SET(MAKEKEY "makekey")

# set property
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
  "${CMAKE_CURRENT_LIST_DIR}/cmake")
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
  "${CMAKE_CURRENT_LIST_DIR}/third_party/easyloggingpp/cmake")

option (USE_INTERNAL_TINYXML "use built-in TinyXML2" ON)
option (USE_INTERNAL_EASYLOGGING "use built-in Easylogging++" ON)
if (APPLE)
  option (ENABLE_NLS "compile with Native Language Support (using gettext)" OFF)
else()
  option (ENABLE_NLS "compile with Native Language Support (using gettext)" ON)
endif()

if (CYGWIN)
  if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX /usr)
  endif()
endif()

# We need C++ 11
if (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 3.0)
  # CMake 3.1 has built-in CXX standard checks.
  set(CMAKE_CXX_STANDARD 11)
  set(CMAKE_CXX_STANDARD_REQUIRED on)
else ()
  if (CMAKE_COMPILER_IS_GNUCXX)
    message ("** Assuming that GNU CXX uses -std=c++11 flag for C++11 compatibility.")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message ("** Assuming that Clang uses -std=c++11 flag for C++11 compatibility.")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  else()
    message ("** No CMAKE C++11 check. If the build breaks, you're on your own.")
  endif()
endif ()

if (CYGWIN OR WIN32 OR APPLE)
  set(DEFAULT_CASE_INSENSITIVE TRUE)
else()
  set(DEFAULT_CASE_INSENSITIVE FALSE)
endif()

if (CYGWIN)
  find_program(PKILL NAMES "pkill")
  if(NOT PKILL)
    message(FATAL_ERROR "pkill not found, please install procps-ng package.")
  endif()
endif()

# Check for include files and stdlib properties.
include (CheckIncludeFileCXX)
if (NOT CYGWIN)
  check_include_file_cxx (attr/xattr.h HAVE_ATTR_XATTR_H)
  check_include_file_cxx (sys/xattr.h HAVE_SYS_XATTR_H)
endif()

# Check for FUSE.
find_package (FUSE REQUIRED)
include_directories (${FUSE_INCLUDE_DIR})
add_definitions (-D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=29)
if (CYGWIN)
  # Cygwin build is intended to use WinFsp
  add_definitions(-DCYGFUSE)
endif()

# Check for OpenSSL.
if (APPLE AND NOT DEFINED OPENSSL_ROOT_DIR)
  # Search both Brew and Macports
  set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl;/opt/local")
endif()
find_package (OpenSSL REQUIRED)
include_directories (${OPENSSL_INCLUDE_DIR})

find_program (POD2MAN pod2man)

# Check for include files and stdlib properties.
include (CheckIncludeFileCXX)
if (NOT CYGWIN)
  check_include_file_cxx (attr/xattr.h HAVE_ATTR_XATTR_H)
  check_include_file_cxx (sys/xattr.h HAVE_SYS_XATTR_H)
endif()

include(CheckStructHasMember)
check_struct_has_member("struct dirent" d_type dirent.h HAVE_DIRENT_D_TYPE LANGUAGE CXX)

# Check if xattr functions take extra arguments, as they do on OSX.
include (CheckCXXSourceCompiles)
check_cxx_source_compiles ("#include <sys/types.h>
  #include <sys/xattr.h>
  int main() { getxattr(0,0,0,0,0,0); return 1; }
  " XATTR_ADD_OPT)

# If awailable on current architecture (typically embedded 32-bit), link with it explicitly;
# GCC autodetection is faulty, see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=81358 and
# find_libray is no great help here since it is sometimes(!) not in standard paths.
set(CMAKE_REQUIRED_FLAGS "-latomic")
CHECK_CXX_SOURCE_COMPILES("int main() {return 0;}" COMPILER_NEEDS_LATOMIC)
if(COMPILER_NEEDS_LATOMIC)
        set(ATOMIC_LIBRARY atomic)
endif()
# compensate the effect of extra linking of libatomic on platforms where intrinsics are used
set(CMAKE_REQUIRED_FLAGS "-Wl,--as-needed")
CHECK_CXX_SOURCE_COMPILES("int main() {return 0;}" LINKER_SUPPORTS_WLASNEEDED)
if(LINKER_SUPPORTS_WLASNEEDED)
        SET(EXTRA_LINKER_FLAGS "-Wl,--as-needed")
endif()

set(CMAKE_REQUIRED_FLAGS)

# Check if we have some standard functions.
include (CheckFuncs)
check_function_exists_glibc (lchmod HAVE_LCHMOD)
check_function_exists_glibc (utimensat HAVE_UTIMENSAT)
if (APPLE)
  message ("-- There is no usable FDATASYNC on Apple")
  set(HAVE_FDATASYNC FALSE)
else()
  check_function_exists_glibc (fdatasync HAVE_FDATASYNC)
endif (APPLE)

set (CMAKE_THREAD_PREFER_PTHREAD)
find_package (Threads REQUIRED)

# Logging.
add_definitions (-DELPP_THREAD_SAFE -DELPP_DISABLE_DEFAULT_CRASH_HANDLING)
add_definitions (-DELPP_NO_DEFAULT_LOG_FILE)
add_definitions (-DELPP_CUSTOM_COUT=std::cerr)
check_include_file_cxx (syslog.h HAVE_SYSLOG_H)
if (HAVE_SYSLOG_H)
  message ("-- Enabled syslog logging support")
  add_definitions(-DELPP_SYSLOG)
endif (HAVE_SYSLOG_H)

# Compile-time configuration.
configure_file (${CMAKE_CURRENT_LIST_DIR}/config.h.cmake
  ${CMAKE_BINARY_DIR}/config.h)

find_program(CLANG_TIDY_EXE NAMES "clang-tidy-4.0" "clang-tidy40" "clang-tidy" DOC "Path to clang-tidy executable")
if(NOT CLANG_TIDY_EXE)
  message(STATUS "clang-tidy not found.")
else()
  message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
  string(CONCAT TIDY_OPTS "-checks=*"
    ",-cert-err58-cpp"
    ",-cppcoreguidelines-pro-*"
    ",-google-build-using-namespace"
    ",-google-readability-casting"
    ",-google-readability-todo"
    ",-google-runtime-int"
    ",-google-runtime-references"
    ",-modernize-loop-convert"
    ",-performance-inefficient-string-concatenation"
    ",-readability-inconsistent-declaration-parameter-name"
    ",-readability-named-parameter"
    )
  set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}" ${TIDY_OPTS})
  #set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}" "-fix" "-checks=-*,google-readability-redundant-smartptr-get")
endif()

# Set RPATH to library install path.
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}")


if (USE_INTERNAL_TINYXML)
  message("-- Using vendored TinyXML2")
  set(TINYXML_DIR third_party/tinyxml2)
  if (BUILD_SHARED_LIBS)
    set(BUILD_STATIC_LIBS OFF CACHE BOOL "build static libs")
    set(BUILD_SHARED_LIBS ON CACHE BOOL "build shared libs")
    set(TINYXML_LIBRARIES tinyxml2)
  else ()
    set(BUILD_STATIC_LIBS ON CACHE BOOL "build static libs")
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "build shared libs")
    set(TINYXML_LIBRARIES tinyxml2_static)
  endif ()
  set(BUILD_TESTS OFF CACHE BOOL "build tests")
  add_subdirectory(${TINYXML_DIR} EXCLUDE_FROM_ALL)
  include_directories(${CMAKE_CURRENT_LIST_DIR}/${TINYXML_DIR})
  link_directories(${CMAKE_BINARY_DIR}/${TINYXML_DIR})
else ()
  find_package (TinyXML REQUIRED)
  include_directories (${TINYXML_INCLUDE_DIR})
endif ()

if (USE_INTERNAL_EASYLOGGING)
  message("-- Using vendored Easylogging++")
  set(EASYLOGGING_DIR third_party/easyloggingpp)
  set(build_static_lib ON CACHE BOOL "build static libs")
  add_subdirectory(${EASYLOGGING_DIR} EXCLUDE_FROM_ALL)
  include_directories(${CMAKE_CURRENT_LIST_DIR}/${EASYLOGGING_DIR}/src)
  link_directories(${CMAKE_BINARY_DIR}/${EASYLOGGING_DIR})
  include_directories(${EASYLOGGING_DIR}/src)
  set(EASYLOGGINGPP_LIBRARY easyloggingpp)
else ()
  set(EASYLOGGINGPP_USE_SHARED_LIBS ON CACHE BOOL "look for shared lib")
  find_package (EASYLOGGINGPP REQUIRED)
  include_directories (${EASYLOGGINGPP_INCLUDE_DIR})
endif ()


LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/build/lib)
LINK_DIRECTORIES(/usr/lib/x86_64-linux-gnu/)

# find include files
include_directories (${CMAKE_BINARY_DIR})
include_directories(
    include/
    include/intl/
)

# find source cpp files
file(GLOB_RECURSE source_list_echo_server "src/*.cpp")

# generate executable files
add_library(${ENCFS} SHARED ${source_list_echo_server})
add_executable(${ENCFS_MAIN}
    ${CMAKE_CURRENT_SOURCE_DIR}/main/main.cpp)
target_link_libraries(${ENCFS_MAIN}
                      PRIVATE ${ENCFS}
                      ${EXTRA_LINKER_FLAGS}
                      ${FUSE_LIBRARIES}
                      ${OPENSSL_LIBRARIES}
                      ${TINYXML_LIBRARIES}
                      ${EASYLOGGINGPP_LIBRARY}
                      ${CMAKE_THREAD_LIBS_INIT}
                      ${Intl_LIBRARIES}
                      ${ATOMIC_LIBRARY}
)

add_executable(${ENCFSCTL}
    ${CMAKE_CURRENT_SOURCE_DIR}/main/encfsctl.cpp)
target_link_libraries(${ENCFSCTL}
                      PRIVATE ${ENCFS}
                      ${EXTRA_LINKER_FLAGS}
                      ${FUSE_LIBRARIES}
                      ${OPENSSL_LIBRARIES}
                      ${TINYXML_LIBRARIES}
                      ${EASYLOGGINGPP_LIBRARY}
                      ${CMAKE_THREAD_LIBS_INIT}
                      ${Intl_LIBRARIES}
                      ${ATOMIC_LIBRARY}
)

add_executable(${MAKEKEY}
    ${CMAKE_CURRENT_SOURCE_DIR}/main/makeKey.cpp)
target_link_libraries(${MAKEKEY}
                      PRIVATE ${ENCFS}
                      ${EXTRA_LINKER_FLAGS}
                      ${FUSE_LIBRARIES}
                      ${OPENSSL_LIBRARIES}
                      ${TINYXML_LIBRARIES}
                      ${EASYLOGGINGPP_LIBRARY}
                      ${CMAKE_THREAD_LIBS_INIT}
                      ${Intl_LIBRARIES}
                      ${ATOMIC_LIBRARY}
)
