DESCRIPTION = "ARGoS3 plugins for the BeBot hardware."
HOMEPAGE = "https://github.com/allsey87/argos3-plugins-bebot"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=4f51bb496ef8872ccff73f440f2464a8"

BRANCH ?= "master"
SRCREV = "eb14fd878048e21404e6b5c4d550c0fefe80b9f4"

SRC_URI = "git://github.com/allsey87/argos3-plugins-bebot.git;protocol=git;branch=${BRANCH}"

S = "${WORKDIR}/git"

PR = "r0"

do_compile() {
	${CXX} ${CXXFLAGS} -std=c++11 ${LDFLAGS} ${S}/leds.cxx -lpthread -o leds
}

