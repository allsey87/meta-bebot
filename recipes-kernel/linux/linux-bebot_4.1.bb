require recipes-kernel/linux/linux-yocto.inc

KERNEL_IMAGETYPE = "zImage"

COMPATIBLE_MACHINE = "bebot"

RDEPENDS_kernel_base += "kernel-devicetree"

KERNEL_DEVICETREE_bebot = "omap4-duovero-bebot.dtb"

LINUX_VERSION = "4.1"
LINUX_VERSION_EXTENSION = "-custom"

FILESEXTRAPATHS_prepend := "${THISDIR}/linux-bebot-4.1:${THISDIR}/files:"

S = "${WORKDIR}/git"

PR = "r4"

# v4.1.4  
SRCREV = "89e419960fb6a260f6a112821507d516117d5aa1"
SRC_URI = " \
    git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git;branch=linux-4.1.y \
    file://defconfig \
    file://0001-Added-support-for-the-BeBot.patch \
    file://0002-Support-multiple-SC16IS7XX-devices.patch \
 "

