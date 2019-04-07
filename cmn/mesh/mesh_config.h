#ifndef _MESH_CONFIG_H
#define _MESH_CONFIG_H


#define MESH_SSID_CNT 2

#if defined(AP_PLATFORM)

#if !defined(CONFIG_ARDMORE) && !defined(CONFIG_SPRINGBANK) && !defined(CONFIG_MASTERSON) && !defined(CONFIG_LAGAVULIN)
#define MESH_P_VAP_ID(_aphw, _r) 0
#define MESH_C_VAP_ID(_aphw, _r) (MESH_P_VAP_ID(_aphw, _r)+1)
#else
//In Ardmore platform,
//We found the MeshC can't work well with aruba001.
//But aruba000 is ok, we guess it maybe because aruba000 has same MAC as radio. 
#define MESH_C_VAP_ID(_aphw, _r) 0
#define MESH_P_VAP_ID(_aphw, _r) (MESH_C_VAP_ID(_aphw, _r)+1)
#endif

#else 

#define MESH_C_VAP_ID(_aphw, _r) get_mesh_vap_id(_aphw, 1)
#define MESH_P_VAP_ID(_aphw, _r) get_mesh_vap_id(_aphw, 2)

#endif
typedef enum {
  MESH_ROLE_INVALID = 0,
  MESH_POINT        = 1,
  MESH_PORTAL       = 2,
  MESH_AUTO_ROLE    = 3
} mesh_role_t;
/*for mesh auto role feature*/
typedef enum {
  MESHAUTO_INVALID   = 0,
  MESHAUTO_POINT_LINKDOWN = 1,
  MESHAUTO_PORTAL         = 2,
  MESHAUTO_POINT_LINKUP   = 3
} mesh_auto_role_t;

typedef enum {
  MESH_SAE_DISABLE = 0,
  MESH_SAE_ENABLE  = 1
} mesh_sae_t;

/*This is dummy mesh_role for user to configure RMP,
 *internally only 2 mesh_role exists; mesh_portal and mesh_point.
 *when user configures remote-mesh-portal, mesh_role is actually
 *configured as mesh_portal; and rap credentials are checked;
 *is_rmp_enable in sapmisc_ap_prov_data_t is set to 1
 */
#define REMOTE_MESH_PORTAL_DUMMY_VAL 9

typedef enum _reselection_mode_type {
    RESELECT_NEVER=0,
    RESELECT_SUBTHRESHOLD,
    RESELECT_STARTUP_SUBTHRESHOLD,
    RESELECT_ANYTIME,
    RESELECTION_MODE_END
} reselection_mode_type;

typedef enum _metric_algorithm_type {
    METRIC_DISTRIBUTED_TREE_RSSI = 0,
    METRIC_BEST_LINK_RSSI,
    METRIC_ALGORITHM_END
} metric_algorithm_type;

#define MESH_RSSI_THRESHOLD_DFLT 12
#define MESH_MAX_CHILDREN_DFLT 64
#define MESH_MAX_HOP_COUNT_DFLT 8
#define MESH_METRIC_ALGORITHM_DFLT METRIC_DISTRIBUTED_TREE_RSSI
#define MESH_RESELECTION_MODE_DFLT RESELECT_STARTUP_SUBTHRESHOLD

enum _mesh_nbr_rows {MN_MAC=0, MN_PORTAL, MN_CHAN, MN_AGE, MN_HOPS,  
                     MN_COST, MN_RELATION, MN_FLAGS, MN_RSSI,MN_RATE, MN_AREQ, MN_ARESP, MN_AFAIL,
                     MN_HTDETAILS, MN_CLID,MN_LAST};

#ifndef __KERNEL__
#include "wifi/ap_tmp_files.h"

#define MESH_GET_ROLE(m) do {             \
        FILE *fp = fopen(MESH_ROLE_FILE, "r");  \
        if (fp) {                               \
            fscanf(fp, "%d", &m);          \
            fclose(fp);                        \
        } else                                 \
            m = MESH_ROLE_INVALID;       \
    } while(0);

#endif

#endif
