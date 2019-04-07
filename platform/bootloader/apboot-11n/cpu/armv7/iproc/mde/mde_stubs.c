
/*
/projects/stbdev/ckder/northstar/iproc/u-boot/common/mde_main.c:139: undefined reference to `setupNewMdeWithCommonInfo'
/projects/stbdev/ckder/northstar/iproc/u-boot/common/mde_main.c:141: undefined reference to `setupNewMdeWithLocalSpecificInfo'
/projects/stbdev/ckder/northstar/iproc/u-boot/common/mde_main.c:147: undefined reference to `mdeInit'
/projects/stbdev/ckder/northstar/iproc/u-boot/common/mde_main.c:150: undefined reference to `mdeShowInfo'
/projects/stbdev/ckder/northstar/iproc/u-boot/common/mde_main.c:161: undefined reference to `mdeParseCmds'

 */
#if 0
sMde_t * setupNewMdeWithLocalSpecificInfo (sMde_t *newMde )
{
  return (sMde_t *)0;
}



MDE_CMD *getMdeCommonCommands ( unsigned *uCommandCount )
{
  return (MDE_CMD *)0;
}



void setupNewMdeWithCommonInfo (sMde_t *mde )
{
}

void  mdeInit(sMde_t *mde)
{
}

unsigned  mdeShowInfo(sMde_t *mde)
{
  return 0;
}


MDE_CMD *mdeParseCmds ( sMde_t *mde)
{
  return (MDE_CMD *) 0;
}
#endif

