/*
 * pm_api.h - Power monitor API
 *
 * Standard API between IPM driver and platform code
 */

/* Assumptions: Cat 5 cable @12.5ohms/100m */
#define POE_AF_MAX_POWER 139    /* Scaled to 0.1W */
#define POE_AT_MAX_POWER 255    /* Scaled to 0.1W */
#define PSE_AF_MAX_POWER 154    /* Scaled to 0.1W */
#define PM_MAX_RADIOS	2
#define PM_MAX_ANTENNAS	8

enum pm_state { PM_DISABLE, PM_ENABLE };
enum pm_radio_power { PM_RADIO_POWER_FULL, PM_RADIO_REDUCED_3DB, PM_RADIO_REDUCED_6DB };
enum pm_radio_chains { PM_RADIO_CHAIN_1X1, PM_RADIO_CHAIN_2X2,
		 PM_RADIO_CHAIN_3X3, PM_RADIO_CHAIN_4X4, PM_RADIO_CHAIN_5X5,
		 PM_RADIO_CHAIN_6X6, PM_RADIO_CHAIN_7X7, PM_RADIO_CHAIN_8X8 };
enum pm_radios { RADIO_5G, RADIO_2G, RADIO_ALL };
enum pm_status { PM_UNRESTRICTED, PM_RESTRICTED };

/* DC Power threshold */
#define PM_POWER_DC 1000

#define EFF_SCALE_FACTOR     1000 /* scale up by 10^3 */

/* Structure to hold the PoE efficiency versus power points */
struct eff_entry {
    int pwr_out_1;  /* in mW */
    int pwr_out_2;  /* in mW */
    int eff_1;      /* eff (e.g: 0.795)  * EFF_SCALE_FACTOR
                                to make it a fixed point number */
    int eff_2;      /* likewise as eff_1 */
};

/* Functions defined in the platform code */
struct pm_functions {
	int (*set_usb)(enum pm_state state);
	int (*set_alt_ethernet_port)(enum pm_state state);
	int (*set_pse)(enum pm_state state);
	enum pm_state (*get_pse)(void);
	enum pm_state (*get_pse_request)(void);
	void (*set_radio_2ghz_power)(enum pm_radio_power power);
	void (*set_radio_5ghz_power)(enum pm_radio_power power);
	void (*set_radio_2ghz_chain)(enum pm_radio_chains chains);
	void (*set_radio_5ghz_chain)(enum pm_radio_chains chains);
	int (*update_radios)(enum pm_radios radios);
	int (*throttle_cpu_100)(void);
	int (*throttle_cpu_75)(void);
	int (*throttle_cpu_50)(void);
	int (*throttle_cpu_25)(void);
	int (*start_static_power_management)(void);
	int (*stop_static_power_management)(void);
	void (*set_power_status)(enum pm_status status);

	/* Power threshold value (scaled to 0.1 watts) */
	int (*get_power_threshold)(void);

	/* Get real PoE power from measured power */
	uint32_t (*get_poe_power)(uint32_t measured_power);

	/* Get measured power from real PoE power */
	uint32_t (*get_measured_power)(uint32_t poe_power);

	/* Get PSE port power consumption (AP-303H etc.)*/
	uint32_t (*get_pse_port_power)(void);

	/* Index 0: 5G 1: 2G, 1x1, 2x2, 3x3, 4x4 */
	int available_radio_chain_masks[PM_MAX_RADIOS][PM_MAX_ANTENNAS];

	/* Nominal VBUS voltage measurement (mV) for workarounds */
	int nominal_vbus_voltage;
};

/* Accessor function to get the the PM functions */
const struct pm_functions *aruba_pm_functions_get(void);

/* Get APBOOT environment variable */
char *aruba_get_apboot_env(char *s, char *buf, int buf_size);

/* Get input pin value */
int aruba_get_gpio_pin(char *);
