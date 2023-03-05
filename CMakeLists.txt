cmake_minimum_required(VERSION 3.1)
project(
  tc_dns
  VERSION 1.0
  LANGUAGES CXX)

option(BUILD_TESTS "build tests" ON)

find_package(OpenSSL REQUIRED COMPONENTS Crypto SSL)
find_package(CURL REQUIRED COMPONENTS HTTP HTTPS SSL)
find_package(json-c CONFIG)

set(TC_DNS_LIB_INCLUDES ${CMAKE_CURRENT_LIST_DIR}/include/)
set(TC_DNS_LIB_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/src/tc/config.cc
    ${CMAKE_CURRENT_LIST_DIR}/src/tc/crypt.cc
    ${CMAKE_CURRENT_LIST_DIR}/src/tc/http.cc
    ${CMAKE_CURRENT_LIST_DIR}/src/tc/util.cc
)

add_library(tc_dns_lib STATIC ${TC_DNS_LIB_SRCS})
target_include_directories(tc_dns_lib PUBLIC ${TC_DNS_LIB_INCLUDES})
set_property(TARGET tc_dns_lib PROPERTY CXX_STANDARD 17)
target_link_libraries(tc_dns_lib PRIVATE OpenSSL::Crypto)
target_link_libraries(tc_dns_lib PRIVATE CURL::libcurl)
target_link_libraries(tc_dns_lib PRIVATE json-c::json-c)

set(TC_DNS_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/src/tc_dns.cc
)

add_executable(tc_dns ${TC_DNS_SRCS})
set_property(TARGET tc_dns PROPERTY CXX_STANDARD 17)
target_link_libraries(tc_dns PRIVATE tc_dns_lib)

if(BUILD_TESTS)
  add_subdirectory(test)
endif()
