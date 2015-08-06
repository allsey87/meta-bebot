# The 'meta-bebot' Yocto Layer

Yocto layer containing patches and configuration for the BeBot minature mobile robot platform

## Instructions

1. Clone the yocto project and checkout fido (the latest revision at the time of writing)

   ```bash
git clone http://git.yoctoproject.org/git/poky
cd poky
git checkout -b fido origin/fido
```
2. Checkout the bebot layer into the poky directory

   ```bash
git clone https://github.com/allsey87/meta-bebot.git
```
3. Point the template variable inside the meta-bebot layer

   ```bash
export TEMPLATECONF=meta-bebot/conf
```
4. Init the build environment

   ```bash
source oe-init-build-env
```
5. Build an image

   ```bash
bitbake bebot-console-image
```

The build process takes at least an hour on a fast machine. The files for the image after the build can be found under tmp/deploy/images/overo. Instructions on how to prepare an SD card can be found here: [Create a bootable micro-SD card](http://www.gumstix.org/create-a-bootable-microsd-card.html)


