cmake_minimum_required(VERSION 2.8)

find_package(OpenCV REQUIRED)

# ISS video capture test executable
add_executable(iss-videocapture test.cxx iss_video_capture.cxx)
target_link_libraries(iss-videocapture ${OpenCV_LIBS})
install(TARGETS iss-videocapture DESTINATION bin)


