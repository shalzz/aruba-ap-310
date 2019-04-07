#ifndef __aruba_manuf_h__
#define __aruba_manuf_h__

#ifndef __TLV_INVENTORY__
typedef struct {
  char format;
  char part_number[8];
  char major_revision[2];
  char minor_revision[2];
  char box_serial_number[9];
  char date[6];
  unsigned char network_address[6];
  unsigned char network_address_count[2];
  char system_serial_number[9];
#ifdef AP_HAS_TWO_BOARDS
  char radio_card_assembly_number[8];
  char radio_card_major_revision[2];
  char radio_card_variant[2];
  char radio_card_serial_number[9];
#endif
  unsigned char checksum;
} __attribute__ ((packed)) std_manuf_t;

typedef struct {
  char format;
  char part_number[8];
  char major_revision[2];
  char minor_revision[2];
  char box_serial_number[9];
  char date[6];
  unsigned char network_address[6];
  unsigned char network_address_count[2];
  char system_serial_number[9];
  unsigned char wireless_address[6];
  unsigned char wireless_address_count[2];
  unsigned char checksum;
} __attribute__ ((packed)) moscato_manuf_t;

extern unsigned char manuf_compute_checksum(nvram_manuf_t *);
#endif

extern void manuf_recover_ethaddr(int);
extern int manuf_is_iap(void);

#endif /* __aruba_manuf_h__ */
