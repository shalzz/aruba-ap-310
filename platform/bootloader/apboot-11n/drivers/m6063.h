#ifndef __m6063_h__
#define __m6063_h__

#ifdef CONFIG_MARVELL_6063
extern void m6063_init(uint8_t *);
extern int m6063_detect(int *);
extern void m6063_led_init(void);
extern void m6063_announce(uint8_t *);
extern void m6063_set_port_state(int, int);
extern void m6063_set_macaddr(uint8_t *);
extern void m6063_init_cpu_port(void);
extern void m6063_shutdown_unused_ports(void);

#define	M6063_BASE_ADDR		0x10
#define	M6063_PHY_BASE		(M6063_BASE_ADDR + 0x0)
#define	M6063_PORT_BASE		(M6063_BASE_ADDR + 0x8)
#define	M6063_SWITCH_ADDR	(M6063_BASE_ADDR + 0xf)

#define	M6063_CPU_PORT	6

#define	M6063_PHY_ADDR(port)	(M6063_PHY_BASE + (port))
#define	M6063_PORT_ADDR(port)	(M6063_PORT_BASE + (port))

// PHY registers
#define	M6063_PHY_CONTROL_REG		0x0
#define	M6063_PHY_PARALLEL_LED_REG	0x16

// port (MAC) registers
#define	M6063_PORT_STATUS_REG		0x0
#define	M6063_PORT_ID_REG		0x3
#define	M6063_PORT_CONTROL_REG		0x4

// switch registers
#define	M6063_SWITCH_MAC_ADDRESS_REGISTER(x) ((x) + 0x1)

// misc. flags and things
#define	M6063_CPU_LINK_STATUS	((1<<13) | (1<<12) | (1<<11) | (1<<9) | (1<<8))

#define	M6063_PORT_STATE_DISABLED	0x0
#define	M6063_PORT_STATE_BLOCKING	0x1
#define	M6063_PORT_STATE_LEARNING	0x2
#define	M6063_PORT_STATE_FORWARDING	0x3
#endif	/* CONFIG_MARVEL_6063 */
#endif /* __m6063_h__ */
