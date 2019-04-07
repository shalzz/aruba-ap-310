ifdef ARUBA_MAKE_VERBOSE
__ARUBA_QUIET=
else
MAKEFLAGS += --no-print-directory -s
__ARUBA_QUIET=@
endif
