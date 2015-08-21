cmake_minimum_required(VERSION 2.8)
# Set project name
project(blocktracker)

# Set the module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

# Find dependencies
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})
find_package(ISSCapture REQUIRED)
include_directories(${ISSCapture_INCLUDE_DIR})
find_package(AprilTags REQUIRED)
include_directories(${AprilTags_INCLUDE_DIR})

# Build the application
add_executable(blocktracker src/block_demo.cc)
target_link_libraries(blocktracker ${ISSCapture_LIB} ${AprilTags_LIB} ${OpenCV_LIBS})
install(TARGETS blocktracker DESTINATION bin)


