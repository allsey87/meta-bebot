SUMMARY = "Linux Kernel Module for the BeBot Range Finders on the MID Case"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=12f884d2ae1ff87c09e5b7ccc2c4ca7e"

inherit module

PR = "r0"
PV = "1.0"

SRC_URI = "file://Makefile \
           file://rf.h \
           file://rf-core.c \
           file://rf-class.c \	
           file://bebot-chassis-rfs.c \
           file://COPYING \
          "

S = "${WORKDIR}"

# The inherit of module.bbclass will automatically name module packages with
# "kernel-module-" prefix as required by the oe-core build environment.
