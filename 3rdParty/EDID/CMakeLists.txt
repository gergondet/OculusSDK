cmake_minimum_required(VERSION 2.8)
project (edid)
add_library(edid STATIC edid.cpp)
install(TARGETS edid
  DESTINATION ${INSTALL_LIB_PATH})
