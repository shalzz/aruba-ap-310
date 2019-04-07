#ifndef SOS_SLOT_CONV
#define SOS_SLOT_CONV

#ifdef CONFIG_MSWITCH
/* Map between internal (sos/scci) and external slot numbering */
#define SOS_INT_SLOT(Slot) \
    (((Slot) < 2)? ((Slot)+1): ((Slot)-2))
#define SOS_EXT_SLOT(Slot) \
    (((Slot) < 2)? ((Slot)+2): ((Slot)-1))
#elif defined(CONFIG_NEBBIOLO)
/* Map between internal (sibyte/scci) and external slot numbering */
#define SOS_INT_SLOT(Slot) \
    (((Slot) < 2)? (3-(Slot)): ((Slot)-2))
#define SOS_EXT_SLOT(Slot) \
    (((Slot) < 2)? ((Slot)+2): (3-(Slot)))
#elif defined(CONFIG_GRENACHE)
#define SOS_INT_SLOT(Slot) Slot
#define SOS_EXT_SLOT(Slot) Slot
#elif defined(CONFIG_PORFIDIO) || defined(CONFIG_MILAGRO)
/*
 * On Porfidio, we just have single physical slot.
 * So the slot is 0 for both CP and DP.
 */
#define SOS_INT_SLOT(Slot) Slot
#define SOS_EXT_SLOT(Slot) Slot
#elif defined(X86_TARGET_PLATFORM)
#define SOS_INT_SLOT(Slot) 0
#define SOS_EXT_SLOT(Slot) 0
#else /* CONFIG_MSWITCH */
/* all other switches only have one slot, but use GMAC 1 */
#define SOS_INT_SLOT(Slot) 1
#define SOS_EXT_SLOT(Slot) 1
#endif /* CONFIG_MSWITCH */

#endif /* SOS_SLOT_CONV */


