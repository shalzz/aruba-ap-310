#include <common.h>
typedef struct sysmon_s sysmon_t;
typedef struct sysmon_table_s sysmon_table_t;

struct sysmon_s
{
    uchar   chip;
    void    (*init)(sysmon_t *);
    int    (*read)(sysmon_t *, uint8_t);
};



struct sysmon_table_s
{           
    char *          name;
    char *          unit_name;
    sysmon_t *      sysmon;
    void            (*exec_before)(sysmon_table_t *);
    void            (*exec_after)(sysmon_table_t *);

    int	    (*get_value)(sysmon_t *, uint8_t *, uint8_t *, char *);

//    int             unit_precision;
//    int             unit_div;
//    int             unit_min;
//    int             unit_max;
     uint8_t *	     addr;
     uint8_t *       mask;
 
//   uint             val_min;
//    uint             val_max;
//    int              val_valid;
//    uint             val_min_alt;
//    uint             val_max_alt;
//    int              val_valid_alt;
//    uint             addr;
}; 




