

include $(TOPDIR)/config.mk
include $(TOPDIR)/board/mv_feroceon/config_kw/mvRules.mk

OBJS	= $(AOBJS) $(COBJS)

CFLAGS += -I$(TOPDIR)/diag 

all:	$(LIB)

$(LIB):	.depend $(OBJS)
	$(AR) crv $@ $(OBJS)

#########################################################################

.depend: Makefile $(AOBJS:.o=.S) $(COBJS:.o=.c)
	$(CC) -M $(CFLAGS) $(AOBJS:.o=.S) $(COBJS:.o=.c) > .depend

sinclude .depend

#########################################################################
