#!/bin/bash

x4_tc_top=$1
install_script=$x4_tc_top/fsl-networking-uclibc-x86_64-ppce5500-toolchain-QorIQ-SDK-V1.6.sh
install_arg_file=$x4_tc_top/arg_file
sysroots_path=$x4_tc_top/sysroots/ppce5500-fsl-linux
env_file=$x4_tc_top/environment-setup-ppce5500-fsl-linux
uclibc_dir=$x4_tc_top/uClibc-0.9.33.2
patch_dir=$x4_tc_top/aruba-patch

function install_toolchain 
{
    # install toolchain
    echo "Installing toolchain ..."
    echo $x4_tc_top > $install_arg_file
    echo Y >> $install_arg_file
    $install_script < $install_arg_file > /dev/null

    cd $x4_tc_top
    # patch aruba fix for sysinfo.h
    patch -p0 < $patch_dir/aruba_sysinfo.patch > /dev/null
    cp -f $x4_tc_top/Makefile $x4_tc_top/sysroots

    # hack! FIXME_X4
    cd $patch_dir
    patch -p0 < features.patch > /dev/null

    rm -f $install_arg_file
    echo "Toolchain installed!"
    exit 0
}

# check if sysroots is installed 
if [ ! -d $x4_tc_top/sysroots ]; then
    install_toolchain
fi
