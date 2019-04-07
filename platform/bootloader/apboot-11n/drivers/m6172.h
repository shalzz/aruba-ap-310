#ifndef __m6172_h__
#define __m6172_h__

#ifdef CONFIG_MARVELL_6172
extern void m6172_init(uint8_t *);
extern int m6172_detect(int *);
extern void m6172_led_init(void);
extern void m6172_announce(uint8_t *);
extern void m6172_set_port_state(int, int);
extern void m6172_set_macaddr(uint8_t *);
extern void m6172_init_cpu_port(void);
extern void m6172_shutdown_unused_ports(void);

#define	M6172_BASE_ADDR		0x10
#define	M6172_PHY_BASE		(M6172_BASE_ADDR - M6172_BASE_ADDR)
#define	M6172_PORT_BASE		(M6172_BASE_ADDR + 0x0)
#define	M6172_GLOBAL1_ADDR	(M6172_BASE_ADDR + 0xb)
#define	M6172_GLOBAL2_ADDR	(M6172_BASE_ADDR + 0xc)

#define	M6172_CPU_PORT	5

#define	M6172_PHY_ADDR(port)	(M6172_PHY_BASE + (port))
#define	M6172_PORT_ADDR(port)	(M6172_PORT_BASE + (port))

/* PHY registers */
#define	M6172_PHY_CONTROL_REG		0x0

/* port (MAC) registers */
#define	M6172_PORT_STATUS_REG		0x0
#define	M6172_PORT_PHYSICAL_CONTROL_REG 0x1
#define	M6172_PORT_ID_REG		0x3
#define	M6172_PORT_CONTROL_REG		0x4
#define	M6172_PORT_LED_CONTROL_REG	0x16

/* switch registers */
#define	M6172_SWITCH_CONTROL_REGISTER		0x4
#define	M6172_SWITCH_MAC_ADDRESS_REGISTER	0xd

/* misc. flags and things */
#define	M6172_CPU_LINK_STATUS	((1<<11) | (1<<10) | (1<<9))

#define	M6172_PORT_STATE_DISABLED	0x0
#define	M6172_PORT_STATE_BLOCKING	0x1
#define	M6172_PORT_STATE_LEARNING	0x2
#define	M6172_PORT_STATE_FORWARDING	0x3
#endif	/* CONFIG_MARVEL_6172 */

#endif /* __m6172_h__ */
