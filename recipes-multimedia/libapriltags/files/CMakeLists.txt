cmake_minimum_required(VERSION 2.8)
# Set project name
project(apriltags)

# Set the module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

# Find dependencies
find_package(OpenCV)
include_directories(${OpenCV_INCLUDE_DIRS})
find_package(Eigen3)
include_directories(${EIGEN3_INCLUDE_DIR})

# Create and install the Apriltags library
file(GLOB APRILTAGS_SOURCE_FILES "src/*.cc")
file(GLOB APRILTAGS_HEADER_FILES "src/*.h")

# Add and install shared library
add_library(apriltags SHARED ${APRILTAGS_SOURCE_FILES})
target_link_libraries(apriltags ${OpenCV_LIBS})
set_target_properties(apriltags PROPERTIES SOVERSION 0.0.0
  VERSION 0.0.1)
install(TARGETS apriltags DESTINATION lib)

# Install header files
install (FILES ${APRILTAGS_HEADER_FILES} DESTINATION include/apriltags)


