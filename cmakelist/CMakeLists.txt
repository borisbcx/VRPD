cmake_minimum_required (VERSION 2.8)

project (vrpd1.0)

SET(PROJECT_ROOT_PATH "${CMAKE_SOURCE_DIR}/../") 
SET(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/../bin/") 
SET(LIBRARY_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/../lib/") 

INCLUDE_DIRECTORIES("${PROJECT_ROOT_PATH}/VRPD/header")
INCLUDE_DIRECTORIES("${PROJECT_ROOT_PATH}/VRPH-1.0.0/inc")
LINK_DIRECTORIES("${CMAKE_SOURCE_DIR}/../lib/") 

ADD_SUBDIRECTORY(src)       
