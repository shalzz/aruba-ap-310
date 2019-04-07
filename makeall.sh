#!/bin/sh 

export ARUBA_SRC=`pwd`
export ARUBA_MAKE_VERBOSE=1

# Build the bootloader for AP 325
echo "Building GPL bootloader for AP 325"
export BOOTLOADER_LIST='apboot-11n'
make all_prom
unset BOOTLOADER_LIST
echo ""

# Build OpenSource GPL packages
echo "Building GPL packages for OCTOMORE AP 325"
export OCTOMORE=yes
export OEM=siemens
export FAT_AP=yes

if [ ! -d ${ARUBA_SRC}/cyassl/wolfssl-3.6.8-fips ]; then
    echo "Warning: Required module wolfssl-3.6.8-fips is missing from the build environment."
    echo "         Without this module there will be build errors because of this missing module."
    echo ""
    echo "Please refer to the README file for instructions on where to procure the commercial version of the wolfssl FIPS 3.6.8 module."
    echo ""
    echo "Proceeding with the build..."
    echo ""
fi

echo "Building libusb"
(cd utils/usb_modeswitch/libusb-0.1.12/; ./BUILD_OCTOMORE)

echo "Building ntp-4.2.8p9"
(cd utils/ntp-4.2.8p9/ARUBA_READMES/; ./BUILD.glenm)

make V=1 -i clean cleaninstall
mkdir -p usr/lib/aruba/octomore_obj
make V=1 -i image
unset OCTOMORE
echo ""

exit
