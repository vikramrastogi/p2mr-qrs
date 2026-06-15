cmake_minimum_required(VERSION 3.22)
project(qrs_native_bench LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(QRS_BENCH_USE_OPENSSL "Enable OpenSSL SLH-DSA backend" ON)
option(QRS_BENCH_USE_BUNDLED_SECP256K1 "Build bundled libsecp256k1 Schnorr backend" ON)

add_executable(qrs_native_bench
  src/main.cpp
  src/bench_timer.cpp
  src/block_model.cpp
  src/core_validation_model.cpp
  src/environment.cpp
  src/experimental_batch_schnorr.c
  src/report_json.cpp
  src/report_markdown.cpp
  src/schnorr_secp256k1.cpp
  src/slh_dsa_openssl.cpp
  src/stats.cpp
)

target_include_directories(qrs_native_bench PRIVATE include)
target_compile_options(qrs_native_bench PRIVATE -O3 -Wall -Wextra -Wpedantic)
set_source_files_properties(src/experimental_batch_schnorr.c PROPERTIES
  COMPILE_OPTIONS "$<$<COMPILE_LANG_AND_ID:C,AppleClang,Clang,GNU>:-Wno-unused-function>"
)

if(APPLE AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  execute_process(
    COMMAND xcrun --show-sdk-path
    OUTPUT_VARIABLE QRS_MACOS_SDK_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
  )
  set(QRS_MACOS_CXX_INCLUDE_CANDIDATES
    "${QRS_MACOS_SDK_PATH}/usr/include/c++/v1"
  )
  file(GLOB QRS_MACOS_KNOWN_CXX_INCLUDES
    "/Library/Developer/CommandLineTools/SDKs/MacOSX*.sdk/usr/include/c++/v1"
  )
  list(APPEND QRS_MACOS_CXX_INCLUDE_CANDIDATES ${QRS_MACOS_KNOWN_CXX_INCLUDES})
  foreach(QRS_MACOS_CXX_INCLUDE IN LISTS QRS_MACOS_CXX_INCLUDE_CANDIDATES)
    if(EXISTS "${QRS_MACOS_CXX_INCLUDE}/cstddef")
      target_include_directories(qrs_native_bench SYSTEM PRIVATE "${QRS_MACOS_CXX_INCLUDE}")
      message(STATUS "Using explicit macOS libc++ include path: ${QRS_MACOS_CXX_INCLUDE}")
      break()
    endif()
  endforeach()
endif()

if(QRS_BENCH_USE_OPENSSL)
  find_package(OpenSSL REQUIRED)
  target_link_libraries(qrs_native_bench PRIVATE OpenSSL::Crypto)
  target_compile_definitions(qrs_native_bench PRIVATE QRS_BENCH_HAVE_OPENSSL=1)
endif()

if(QRS_BENCH_USE_BUNDLED_SECP256K1 AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/secp256k1/CMakeLists.txt")
  set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
  set(SECP256K1_INSTALL OFF CACHE BOOL "" FORCE)
  set(SECP256K1_ENABLE_MODULE_ECDH OFF CACHE BOOL "" FORCE)
  set(SECP256K1_ENABLE_MODULE_RECOVERY OFF CACHE BOOL "" FORCE)
  set(SECP256K1_ENABLE_MODULE_EXTRAKEYS ON CACHE BOOL "" FORCE)
  set(SECP256K1_ENABLE_MODULE_SCHNORRSIG ON CACHE BOOL "" FORCE)
  set(SECP256K1_ENABLE_MODULE_MUSIG OFF CACHE BOOL "" FORCE)
  set(SECP256K1_ENABLE_MODULE_ELLSWIFT OFF CACHE BOOL "" FORCE)
  set(SECP256K1_BUILD_BENCHMARK OFF CACHE BOOL "" FORCE)
  set(SECP256K1_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(SECP256K1_BUILD_EXHAUSTIVE_TESTS OFF CACHE BOOL "" FORCE)
  set(SECP256K1_BUILD_CTIME_TESTS OFF CACHE BOOL "" FORCE)
  set(SECP256K1_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  add_subdirectory(third_party/secp256k1 EXCLUDE_FROM_ALL)
  target_link_libraries(qrs_native_bench PRIVATE secp256k1)
  target_include_directories(qrs_native_bench PRIVATE
    third_party/secp256k1
    third_party/secp256k1/src
    third_party/secp256k1/include
  )
  target_compile_definitions(qrs_native_bench PRIVATE
    QRS_BENCH_HAVE_SECP256K1=1
    QRS_BENCH_SECP256K1_COMMIT="aea86bc3509f3ceb5c047fe6567bf8910f72a728"
    ECMULT_WINDOW_SIZE=15
    COMB_BLOCKS=43
    COMB_TEETH=6
  )
endif()
