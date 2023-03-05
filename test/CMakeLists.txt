cmake_minimum_required(VERSION 3.1)
project(
  tc_dns_tests
  VERSION 1.0
  LANGUAGES CXX)

macro(test_this_shit target_name)
  add_executable(${target_name} ${CMAKE_CURRENT_LIST_DIR}/${target_name}.cc)
  set_property(TARGET ${target_name} PROPERTY CXX_STANDARD 17)
  target_link_libraries(${target_name} PRIVATE tc_dns_lib)
endmacro()

test_this_shit(config)
test_this_shit(crypt)
test_this_shit(http)
test_this_shit(util)
