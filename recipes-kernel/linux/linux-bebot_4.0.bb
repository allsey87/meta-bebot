require recipes-kernel/linux/linux-yocto.inc

COMPATIBLE_MACHINE = "bebot"

# Pull in the devicetree files into the rootfs
RDEPENDS_kernel-base += "kernel-devicetree"

KERNEL_DEVICETREE_bebot = "omap4-duovero-bebot.dtb"

LINUX_VERSION = "4.0"
LINUX_VERSION_EXTENSION = "-custom"

#FILESEXTRAPATHS_prepend := "${THISDIR}/linux-bebot-4.0:${THISDIR}/files:"

S = "${WORKDIR}/git"

# v4.0.5 = be4cb235441a691ee63ba5e00843a9c210be5b8a
SRCREV = "be4cb235441a691ee63ba5e00843a9c210be5b8a"
SRC_URI = " \
    git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git;nocheckout=1;branch=linux-4.0.y \
    file://0001-Add-the-BeBot-device-tree.patch \
    file://0002-SC16IS7XX-Merged-upstream-changes-to-fix-multiple-is.patch \
    file://defconfig \
"
