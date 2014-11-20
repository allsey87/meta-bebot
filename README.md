# The 'meta-bebot' Yocto Layer

Yocto layer containing patches and configuration for the BeBot minature mobile robot platform

## Instructions

1. Clone the yocto project and checkout dizzy (the latest revision at the time of writing)

   ```bash
git clone http://git.yoctoproject.org/git/poky
cd poky
git checkout -b dizzy origin/dizzy
```
2. Checkout the additional layers

   ```bash
git clone https://github.com/gumstix/meta-gumstix.git
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

