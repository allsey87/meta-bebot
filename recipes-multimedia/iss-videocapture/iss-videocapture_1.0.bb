SUMMARY = "ISS Video Capture Test Program"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=533f7826a9f41cd6b0cf1e8490d6a314"

DEPENDS = "opencv"

inherit cmake

PR = "r0"
PV = "1.0"

SRC_URI = "file://CMakeLists.txt \
           file://LICENSE \
           file://src/CMakeLists.txt \
           file://src/iss_video_capture.cxx \
           file://src/iss_video_capture.h \
           file://src/test.cxx \
          "

S = "${WORKDIR}"

