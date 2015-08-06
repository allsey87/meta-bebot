DESCRIPTION = "A basic console image for the DuoVero based BeBot."
HOMEPAGE = "http://github.com/allsey87/meta-bebot/"
LICENSE = "MIT"

IMAGE_FEATURES += "package-management ssh-server-openssh"
# Uncomment below to include dev tools and packages
# IMAGE_FEATURES += "tools-sdk dev-pkgs"

IMAGE_LINGUAS = "en-us"

inherit core-image

SYSTEM_TOOLS_INSTALL = " \
  cpufrequtils \
  tzdata \
  i2c-tools \
"

DEV_TOOLS_INSTALL = " \
  mtd-utils-ubifs \
  u-boot-mkimage \
"

NETWORK_TOOLS_INSTALL = " \
  curl \
  iproute2 \
  iputils \
  iw \
  ntp \
"

HAL_TOOLS_INSTALL = " \
  argos3plugins \
"

# does media-ctl need to be built here?

MEDIA_TOOLS_INSTALL = " \
  raw2rgbpnm \
  v4l-utils \
  yavta \
"

UTILITIES_INSTALL = " \
  avrdude \
  coreutils \
  diffutils \
  findutils \
  grep \
  gzip \
  nano \
  picocom \
  sudo \
  tar \
  wget \
  zip \
"
 
IMAGE_INSTALL += " \
  ${SYSTEM_TOOLS_INSTALL} \
  ${DEV_TOOLS_INSTALL} \
  ${NETWORK_TOOLS_INSTALL} \
  ${MEDIA_TOOLS_INSTALL} \
  ${UTILITIES_INSTALL} \
"

# Create a generic 'gumstix' user account, part of the gumstix group,
# using '/bin/sh' and with a home directory '/home/gumstix' (see
# /etc/default/useradd).  We set the password to 'gumstix' and add them
# to the 'sudo' group.
inherit extrausers
EXTRA_USERS_PARAMS = " \
    useradd -P gumstix -G sudo gumstix; \
"
