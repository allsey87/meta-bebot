DESCRIPTION = "ARGoS3 plugins for the BeBot hardware."
HOMEPAGE = "https://github.com/allsey87/argos3-plugins-bebot"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=4f51bb496ef8872ccff73f440f2464a8"

BRANCH ?= "master"
SRCREV = "0b7e87a95438fde81610955421aecb25d8f78232"

SRC_URI = "git://github.com/allsey87/argos3-plugins-bebot.git;protocol=git;branch=${BRANCH}"

S = "${WORKDIR}/git"

inherit cmake

