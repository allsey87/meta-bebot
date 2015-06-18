require recipes-kernel/linux/linux-yocto.inc

COMPATIBLE_MACHINE = "bebot"

# Pull in the devicetree files into the rootfs
RDEPENDS_kernel-base += "kernel-devicetree"

KERNEL_DEVICETREE_bebot = "omap4-duovero-bebot.dtb"

LINUX_VERSION = "3.17"
LINUX_VERSION_EXTENSION = "-custom"

BOOT_SPLASH ?= "logo_linux_clut224-generic.ppm"

FILESEXTRAPATHS_prepend := "${THISDIR}/linux-gumstix-3.17:${THISDIR}/files:"

S = "${WORKDIR}/git"

# v3.17.8 = bc15d4627aa8f562a1c5ec9d84076b8db25bab31
SRCREV = "bc15d4627aa8f562a1c5ec9d84076b8db25bab31"
SRC_URI = " \
    git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git;nocheckout=1;branch=linux-3.17.y \
    file://defconfig \
    file://0001-Added-initial-support-for-the-BeBot-Mobile-Robotics-.patch \
    file://0002-Work-in-progress-commit-updating-the-OMAP4ISS-driver.patch \
    file://0003-Fixed-getting-V4L2_CID_PIXEL_RATE-issue-added-DMA-ch.patch \
    file://0004-BeBot-Clean-up-device-tree-to-add-LEDs-and-SC16IS7XX.patch \
    file://${BOOT_SPLASH} \
"


