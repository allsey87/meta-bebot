SUMMARY = "OpenCV Compatible Capture Library for the OMAP4 ISS"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=533f7826a9f41cd6b0cf1e8490d6a314"

DEPENDS = "opencv"

inherit cmake

PR = "r0"
PV = "1.0"

SRC_URI = "file://LICENSE \
           file://src \
           file://CMakeLists.txt \
          "

S = "${WORKDIR}"
