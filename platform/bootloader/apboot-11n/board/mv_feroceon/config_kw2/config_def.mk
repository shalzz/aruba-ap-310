#
# image should be loaded at ${MV_BASE_ADDR}
#

# not a variable; sed will replace MV_BASE_ADDR with the right value
# variable expansion is not working here for some reason I can't quite
# figure out
TEXT_BASE = MV_BASE_ADDR
