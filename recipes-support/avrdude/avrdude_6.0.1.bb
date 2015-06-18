DESCRIPTION = "AVRDUDE is software for programming Atmel AVR Microcontrollers."
HOMEPAGE = "http://www.nongnu.org/avrdude/"
SECTION = "console"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=4f51bb496ef8872ccff73f440f2464a8"

DEPENDS = "ncurses"
RDEPENDS_${PN} = "libusb1 libusb-compat readline libelf"

SRC_URI = "http://savannah.nongnu.org/download/avrdude/avrdude-${PV}.tar.gz"

inherit autotools

PR = "r0"

SRC_URI[md5sum] = "346ec2e46393a54ac152b95abf1d9850"
SRC_URI[sha256sum] = "c6804668dfa96b23185dfe2e8239089af4e4ae0b11aa7435bebb28c3260ede41"
