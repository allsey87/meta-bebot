DESCRIPTION = "A basic console image for the BeBot Robot"
LICENSE = "MIT"
PR = "r0"

IMAGE_FEATURES += "splash package-management ssh-server-openssh"
IMAGE_LINGUAS = "en-us"

inherit core-image

# Gumstix machines individually RDEPEND on the firware they need but we repeat
# it here as we might want to use the same image on multiple different machines.
FIRMWARE_INSTALL = " \
  linux-firmware-sd8686 \
  linux-firmware-sd8787 \
"

SYSTEM_TOOLS_INSTALL = " \
  tzdata \
"

DEV_TOOLS_INSTALL = " \
  mtd-utils-ubifs \
  u-boot-mkimage \
"

NETWORK_TOOLS_INSTALL = " \
  init-ifupdown \
  iputils \
  iw \
  ntp \
"

MEDIA_TOOLS_INSTALL = " \
  media-ctl \
  v4l-utils \
  yavta \
"

UTILITIES_INSTALL = " \
  coreutils \
  grep \
  gzip \
  nano \
  sudo \
  tar \
  wget \
  zip \
"
 
IMAGE_INSTALL += " \
  ${FIRMWARE_INSTALL} \
  ${SYSTEM_TOOLS_INSTALL} \
  ${DEV_TOOLS_INSTALL} \
  ${NETWORK_TOOLS_INSTALL} \
  ${MEDIA_TOOLS_INSTALL} \
  ${UTILITIES_INSTALL} \
"

set_gumstix_user() {
	#To allow shutdown/restart
	echo "%sudo ALL=(ALL) ALL" >> ${IMAGE_ROOTFS}/etc/sudoers
}

ROOTFS_POSTPROCESS_COMMAND =+ "set_gumstix_user;"
