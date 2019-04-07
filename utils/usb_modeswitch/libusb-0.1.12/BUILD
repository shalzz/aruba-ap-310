#!/bin/sh

#
# Checkout
# - Makefile
# - config.log
# - config.status
#

# to build image
export ARUBA_TOOL_PREFIX="${ARUBA_SRC}/tools//nebbiolo/toolchains/mipscross-1.00/crosstool/gcc-3.4.3-glibc-2.3.6/mipsisa32-xlr-linux/bin/mipsisa32-xlr-linux"

export CC="${ARUBA_TOOL_PREFIX}-gcc"
export CXX="${ARUBA_TOOL_PREFIX}-c++"
export AR="${ARUBA_TOOL_PREFIX}-ar"
export LD="${ARUBA_TOOL_PREFIX}-ld"
#export LD="${ARUBA_TOOL_PREFIX}-ld -L${ARUBA_SRC}/tools/ppc-linux-gnu-uClibc-0.9.26 -L${ARUBA_SRC}/tools/ppc-linux-gnu-uClibc-0.9.26/lib"

#export CFLAGS="-g -B${ARUBA_SRC}/tools/ppc-linux-gnu-uClibc-0.9.26 -B${OPENSSL_DIR}/mswitch_obj -B${ARUBA_SRC}/tools/ppc-linux-gnu-uClibc-0.9.26/lib"

./configure --host=mips-linux --target=mips-linux 

