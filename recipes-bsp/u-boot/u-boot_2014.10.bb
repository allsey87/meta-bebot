require u-boot.inc

PV = "2014.10"

COMPATIBLE_MACHINE = "bebot"

# v2014.10 = c43fd23cf619856b0763a64a6a3bcf3663058c49
SRCREV = "c43fd23cf619856b0763a64a6a3bcf3663058c49"
SRC_URI = "git://git.denx.de/u-boot.git;protocol=git;branch=master \
	file://0001-Add-bebot-support.patch \
"
