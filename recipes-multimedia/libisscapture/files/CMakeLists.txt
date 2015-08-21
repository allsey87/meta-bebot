cmake_minimum_required(VERSION 2.8)
# Set project name
project(isscapture)

# Find dependencies
find_package(OpenCV)
include_directories(${OpenCV_INCLUDE_DIRS})

# Create and install the ISS capture library
add_library(isscapture SHARED src/iss_capture.cc)
target_link_libraries(isscapture ${OpenCV_LIBS})
set_target_properties(isscapture PROPERTIES SOVERSION 0.0.0
  VERSION 0.0.1)
install(TARGETS isscapture DESTINATION lib)
install(FILES src/iss_capture.h DESTINATION include)
