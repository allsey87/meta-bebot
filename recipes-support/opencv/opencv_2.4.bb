SUMMARY = "Opencv : The Open Computer Vision Library"
HOMEPAGE = "http://opencv.willowgarage.com/wiki/"
SECTION = "libs"

LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://include/opencv2/opencv.hpp;endline=41;md5=6d690d8488a6fca7a2c192932466bb14"

DEPENDS = "libtool bzip2 zlib libeigen libpng jpeg"

SRCREV = "2c9547e3147779001811d01936aed38f560929fc"
SRC_URI = "git://github.com/Itseez/opencv.git;branch=2.4"

PV = "2.4.11+git${SRCPV}"

S = "${WORKDIR}/git"

# Do an out-of-tree build
OECMAKE_SOURCEPATH = "${S}"
OECMAKE_BUILDPATH = "${WORKDIR}/build-${TARGET_ARCH}"

EXTRA_OECMAKE = "-DBUILD_PYTHON_SUPPORT=OFF \
                 -DWITH_CUDA=OFF \
                 -DWITH_GTK=OFF \
                 -DWITH_OPENCL=OFF \
                 -DWITH_OPENEXR=OFF \
                 -DWITH_EIGEN=ON \
                 -DWITH_PNG=ON \
                 -DWITH_JPEG=ON \
                 -DWITH_TIFF=OFF \
                 -DWITH_GSTREAMER=OFF \
                 -DWITH_FFMPEG=OFF \
                 -DWITH_JASPER=OFF \
		 -DWITH_V4L=OFF \
                 -DWITH_LIBV4L=OFF \
                 -DWITH_1394=OFF \
                 -DCMAKE_SKIP_RPATH=ON \
"

inherit distutils-base pkgconfig cmake

export BUILD_SYS
export HOST_SYS
#export PYTHON_CSPEC="-I${STAGING_INCDIR}/${PYTHON_DIR}"
#export PYTHON="${STAGING_BINDIR_NATIVE}/python"

TARGET_CC_ARCH += "-I${S}/include "

PACKAGES += "${PN}-apps"

python populate_packages_prepend () {
    cv_libdir = d.expand('${libdir}')
    cv_libdir_dbg = d.expand('${libdir}/.debug')
    do_split_packages(d, cv_libdir, '^lib(.*)\.so$', 'lib%s-dev', 'OpenCV %s development package', extra_depends='${PN}-dev', allow_links=True)
    do_split_packages(d, cv_libdir, '^lib(.*)\.la$', 'lib%s-dev', 'OpenCV %s development package', extra_depends='${PN}-dev')
    do_split_packages(d, cv_libdir, '^lib(.*)\.a$', 'lib%s-dev', 'OpenCV %s development package', extra_depends='${PN}-dev')
    do_split_packages(d, cv_libdir, '^lib(.*)\.so\.*', 'lib%s', 'OpenCV %s library', extra_depends='', allow_links=True)

    pn = d.getVar('PN', 1)
    metapkg =  pn + '-dev'
    d.setVar('ALLOW_EMPTY_' + metapkg, "1")
    blacklist = [ metapkg ]
    metapkg_rdepends = [ ] 
    packages = d.getVar('PACKAGES', 1).split()
    for pkg in packages[1:]:
        if not pkg in blacklist and not pkg in metapkg_rdepends and pkg.endswith('-dev'):
            metapkg_rdepends.append(pkg)
    d.setVar('RRECOMMENDS_' + metapkg, ' '.join(metapkg_rdepends))
}

PACKAGES_DYNAMIC += "^libopencv-.*"

FILES_${PN} = ""
FILES_${PN}-apps = "${bindir}/* ${datadir}/OpenCV"
FILES_${PN}-dbg += "${libdir}/.debug"
FILES_${PN}-dev = "${includedir} ${libdir}/pkgconfig"
FILES_${PN}-doc = "${datadir}/OpenCV/doc"

ALLOW_EMPTY_${PN} = "1"

do_install_append() {
    cp ${S}/include/opencv/*.h ${D}${includedir}/opencv/
    sed -i '/blobtrack/d' ${D}${includedir}/opencv/cvaux.h

}
