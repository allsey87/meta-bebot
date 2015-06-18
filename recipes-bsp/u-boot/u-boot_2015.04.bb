require u-boot.inc

PV = "2015.04"

COMPATIBLE_MACHINE = "bebot"

# v2014.10 = f33cdaa4c3da4a8fd35aa2f9a3172f31cc887b35
SRCREV = "f33cdaa4c3da4a8fd35aa2f9a3172f31cc887b35"
SRC_URI = "git://git.denx.de/u-boot.git;protocol=git;branch=master \
	file://0001-BeBot-Add-Support.patch \
"
