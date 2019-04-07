#
# Copyright (C) 2002-2011 by Aruba Networks, Inc.
# All Rights Reserved.
# 
# This software is an unpublished work and is protected by copyright and 
# trade secret law.  Unauthorized copying, redistribution or other use of 
# this work is prohibited.
#
# The above notice of copyright on this source code product does not indicate
# any actual or intended publication of such source code.
#
# Top level makefile.
#
# Control the build, like a boss!
ARUBA_TOP_LEVEL_MAKE=1

ifeq ($(origin ARUBA_SRC), undefined)
export ARUBA_SRC := $(shell pwd)
endif

include $(ARUBA_SRC)/mk/Makefile.top

#
# first we need to know what to build
#
ifeq "$(filter ${TARGETS_WITH_NO_IMAGE_TYPE},$(MAKECMDGOALS))" ""
include ${ARUBA_BUILD_DIRS}
endif

#
# this is here to handle one-offs like AMP and UPGRADER (other-platform)
# this is the only optional section for the top level
#
-include ${ARUBA_MK_IMAGE_TYPES}/${ARUBA_IMAGE_TYPE}/build-dirs
#
# no directories can be set after this point
#

# next, the top-level rules for cleaning things
include ${ARUBA_MK_CORE}/clean-rules

TARGETS_NO_PARALLEL_SUPPORT := savesymbols clean cleaninstall arubainstall
ifeq "$(filter ${TARGETS_WITH_NO_IMAGE_TYPE},$(MAKECMDGOALS))" ""
ifeq "$(filter ${TARGETS_NO_PARALLEL_SUPPORT},$(MAKECMDGOALS))" ""

export ENABLE_PARALLEL:=1

endif # filter savesymbols
endif # filter targets_with_no_image_type

#
# end of first section
#
include ${ARUBA_MK_ROOT}/Makefile.bottom

# Local targets below this point.

#
# This provides build order dependencies.
#
ifeq "$(filter ${TARGETS_WITH_NO_IMAGE_TYPE},$(MAKECMDGOALS))" ""
include ${ARUBA_MK_PLATFORM_TYPES}/${ARUBA_PLATFORM_TYPE}/dependencies
endif

#
# This will build images for all images defined in BUILD_LIST
#
include ${ARUBA_MK_DATA}/build-list

# get the core build rules for allworlds and allimage
include ${ARUBA_MK_CORE}/oem-rules

# get the core build rules for world and cleanworld
include ${ARUBA_MK_CORE}/world-rules

# get the kernel module rules
ifeq "$(filter ${TARGETS_WITH_NO_IMAGE_TYPE},$(MAKECMDGOALS))" ""
include ${ARUBA_MK_IMAGE_TYPES}/${ARUBA_IMAGE_TYPE}/modules
endif

# get the rules for how things are named and installed
include ${ARUBA_MK_CORE}/install-rules
include ${ARUBA_MK_CORE}/version-info

# get kernel rules
ifeq "$(filter ${TARGETS_WITH_NO_IMAGE_TYPE},$(MAKECMDGOALS))" ""
include ${ARUBA_MK_PLATFORM_TYPES}/${ARUBA_PLATFORM_TYPE}/kernel-build
endif

# get image rules
include ${ARUBA_MK_CORE}/image-rules

#
# Random other stuff that needs to be available
# at the top-level.
# Order of these includes isn't expected to matter,
# so we put them alphabetically.
#
include ${ARUBA_MK_MISC}/bootloader
include ${ARUBA_MK_MISC}/check-symbols
include ${ARUBA_MK_MISC}/codesigner
include ${ARUBA_MK_MISC}/fips
include ${ARUBA_MK_MISC}/fs-and-kernel
include ${ARUBA_MK_MISC}/gdbinit
include ${ARUBA_MK_MISC}/log-msgs
include ${ARUBA_MK_MISC}/nnm
include ${ARUBA_MK_MISC}/save-symbols
include ${ARUBA_MK_MISC}/sign
