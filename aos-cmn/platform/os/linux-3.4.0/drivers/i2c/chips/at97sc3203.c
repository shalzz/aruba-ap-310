// vim:set ts=4 sw=4:
/*
 * linux/drivers/i2c/chips/at97sc3203.c
 *
 * Copyright (C) 2007- Aruba Networks, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/miscdevice.h>
#include <linux/fcntl.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <asm/system.h>
//#include <asm/time.h>
#include <linux/delay.h>
#include <cmn/util/at97sc3203.h>		/* get the user-level API */

#ifdef CONFIG_RMI_PHOENIX
#include <asm/rmi/sim.h>
#endif

/* #define 	DEBUG_AT97SC3203S */


#ifdef DEBUG_AT97SC3203S		
#define dbg(fmt, args...) 	printk(KERN_DEBUG "%s: " fmt, __func__, ## args)
#define err(format, arg...) 	printk(KERN_ERR ": " format , ## arg)
#define info(format, arg...) 	printk(KERN_INFO ": " format , ## arg)
#else
#define dbg(fmt, args...)
#define err(format, arg...)
#define info(format, arg...)
#endif


#define DEVID_TPM    	   	0x29
#define TPM_KBUF_SZ         2048
/* default to 3204 on Arran Plan B */
#define DETECT_TPM          0
  
/* 
 *  Addresses to scan
 */

static int tpm_instance = 0;

static struct 	i2c_driver tpm_driver;
static struct 	i2c_client *tpm_clients[MAX_TPM] = {NULL};

// TIS offsets */
#define TIS_ACCESS     0x0000
#define TIS_INT_ENABLE 0x0008   /* - 0x000B */
#define TIS_INT_VECTOR 0x000C
#define TIS_INT_STATUS 0x0010   /* - 0x0013 */
#define TIS_INTF_CAP   0x0014   /* - 0x0017 */
#define TIS_STATUS     0x0018   /* - 0x001A */
#define TIS_HASH_END   0x0020
#define TIS_FIFO       0x0024   /* - 0x0027 */
#define TIS_HASH_START 0x0028
#define TIS_DID_VID    0x0F00   /* - 0x0F03 */
#define TIS_RID        0x0F04 

#define ACS_ESTABLISHED 0x01
#define ACS_REQUESTUSE  0x02
#define ACS_PENDINGREQ  0x04
#define ACS_SIEZE       0x08
#define ACS_BEENSIEZED  0x10
#define ACS_ACTIVELOC   0x20
#define ACS_RESERVED    0x40
#define ACS_VALID       0x80

#define STS_RSV0        0x01
#define STS_RETRY       0x02
#define STS_RSV1        0x04
#define STS_EXPECT      0x08
#define STS_DATAAVAIL   0x10
#define STS_GO          0x20
#define STS_CMDREADY    0x40
#define STS_VALID       0x80

#define NO_LOCALITY -1
#define LOCALITY_0   0
#define LOCALITY_1   1
#define LOCALITY_2   2
#define LOCALITY_3   3
#define LOCALITY_4   4

static struct tpm_dev_t {
  int currLocality;
  int opened;
  int delay;       // in milliseconds 
  int tpm_major;
  int tpm_minor;
  unsigned int manuf;
  unsigned int vers;

  unsigned char *iobuf;
  dev_t tpmdev;
//  void *remap;        // we are i2c - this doesn't apply to us
//  struct cdev cdev;   // may we don't need to pack this around - besides - we're a misc device now
  wait_queue_head_t tpmTimeout;
} tpm;

#ifdef CONFIG_ARUBA_AP
#include <linux/aruba-ap.h>
#include <cmn/oem/aphw.h>
#include <cmn/include/ap-tpm.h>
#endif

static int tpm_remove(struct i2c_client *client)
{
	struct tpm_dev_t *data = i2c_get_clientdata(client);
	kfree(data);
	return 0;
}

/* default to 3204 on Arran Plan B */
static int is_3204 = 1;
#if defined(CONFIG_ARUBA_AP) || defined(DETECT_TPM)
static int tpm_autodetect = 1;
#endif
#if DETECT_TPM    
int my_atpm_read(char *buff, size_t count);
int my_atpm_write(char *buff, size_t count);

static int do_tpm_cmd(unsigned char* cmd_buf, unsigned char* resp_buf)
{
    int     ret = 0, len;
    
    len = (cmd_buf[2]<<24)+(cmd_buf[3]<<16)+(cmd_buf[4]<<8)+cmd_buf[5];
    ret = my_atpm_write(cmd_buf, len);
    mdelay(3000);
    if (ret < 0) {
        printk("do_tpm_cmd: write returned error\n");
    } else {
        ret = my_atpm_read(resp_buf, 32);
        //if(ret < 0) printk("TPM read fails\n");
    }
    
    ret = (resp_buf[2]<<24)+(resp_buf[3]<<16)+(resp_buf[4]<<8)+resp_buf[5];    
    return ret;
}
#endif

static int tpm_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    struct i2c_client *this_client = NULL;

	if (tpm_instance >= (MAX_TPM)) {
	    // only 1 device for now...
		return -EBUSY;
	}

	this_client = kmalloc(sizeof *this_client, GFP_KERNEL);
	if (this_client == NULL) {
		return -ENOMEM;
	}
	memset(this_client, 0, sizeof(*this_client));

	this_client->adapter = client->adapter;

    tpm_clients[tpm_instance] = this_client;
    tpm_instance++;

	i2c_set_clientdata(client, this_client);

	printk("Found AT97SC3203 on %s\n", this_client->adapter->name);
    // Currently this is registered with the "misc devices" 
    tpm.tpm_major = 10;
    tpm.tpm_minor = TPM_MINOR;
  
  // initialize our wait queue 
  init_waitqueue_head(&tpm.tpmTimeout);

  // and fetch the DID/VID 
  tpm.currLocality = 0;
/* i2c device doesn't support localities at this time. Since we want the code we port to work ASAP
 * we will keep it in the tpm struct and just set it to 0 for now.
 */

  tpm.manuf = 0;
  tpm.vers = 0;
  // and intialize some variables 
  tpm.opened = 0;
  tpm.delay = HZ/2;    // a half second is the normal delay 

  return 0;
}

/*
 * The strategy is to send commands in the 3203 format because we don't want to
 * screw it up if it is a 3203 chip by sending it commands 3204 style.  
 * However, the 3204 will resend it's response to the last valid command when
 * it is sent a command 3203 style. If the last command had a long response we
 * need to be ready for the data returned. As it happens the 3204 will often
 * respond correctly to the start command so we have to follow up with GetCapabilities
 * to really make sure.  
 */
#if DETECT_TPM
void detect_tpm_chip_type(void)
{
    unsigned char   start_cmd[12]    = {0, 0xC1, 0, 0, 0, 12, 0, 0, 0, 0x99, 0x00, 0x01};
    unsigned char   getCapVer[18]    = {0, 0xC1, 0, 0, 0, 18, 0, 0, 0, 101, 0x00, 0x00,0x00, 0x1A, 0x00, 0x00, 0x00, 0x00 }; 
    unsigned char   respbuf[64];
    int             ret, retry;

    retry = 3;
    do {
        memset(respbuf, 0, sizeof(respbuf));
        ret = do_tpm_cmd(start_cmd, respbuf);
        if(ret == 10) { // length was right...
			mdelay(500);
			memset(respbuf, 0, sizeof(respbuf));
			ret = do_tpm_cmd(getCapVer, respbuf);
			if(ret == 29) {
			// check for 3203 revision values
				if((respbuf[18] <= 0x11)&&(respbuf[18] >= 0x0d)) {
					printk("TPM driver running in 3203 mode\n");
					break;
				} else if((respbuf[18] == 0x23) && (respbuf[19] == 0x10)) {
					printk("TPM driver running in 3203 mode with 3204S\n");
					break;
				}
			} else {
				is_3204 = 1;
				memset(respbuf, 0, sizeof(respbuf));
				mdelay(500);
				ret = do_tpm_cmd(getCapVer, respbuf);
				if(respbuf[18] >= 0x23) {
					printk("TPM driver running in 3204 mode\n");
					break;
				}
			}
        }
		mdelay(1000);
        retry--;
    } while(retry);
    /* Set driver to 3204 mode? */
    if(retry == 0) {
        is_3204 = 1;
        printk("TPM driver running in 3204 mode\n");
    }
}
#endif

int
tpm_open(struct inode *minode, struct file *mfile)
{
	/*if(MOD_IN_USE) */
	if (tpm.opened != 0) {
		return -EBUSY;
	}
    tpm.iobuf = kmalloc(TPM_KBUF_SZ, GFP_KERNEL);
    if (tpm.iobuf == (unsigned char *)0)
	{
	  printk(KERN_WARNING "atpm: io buffer allocation failed!\n");
	  return -ENOMEM;
	}
    tpm.opened = 1;
    tpm.delay = 30000;  /* 5 minutes */
    tpm.currLocality = 0;

#ifdef CONFIG_ARUBA_AP
	switch (__ap_hw_info[xml_ap_model].has_tpm) {
	case ARUBA_TPM_NONE:
		return 0;
	case ARUBA_TPM_DETECT:
		tpm_autodetect = 1;
		break;
	case ARUBA_TPM_3203S:
		tpm_autodetect = 0;
		is_3204 = 0;
		break;
	case ARUBA_TPM_3204T:
		tpm_autodetect = 0;
		is_3204 = 1;
		break;
	}
#endif

#if DETECT_TPM    
    if(tpm_autodetect) {
        tpm_autodetect = 0;
        detect_tpm_chip_type();
    }
#endif    
	return 0;
}

int
tpm_release(struct inode *minode, struct file *mfile)
{
/* 	MOD_DEC_USE_COUNT; */
    if (tpm.iobuf)
      kfree(tpm.iobuf);

    tpm.opened = 0;
	return 0;
}

ssize_t atpm_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp)
{
    unsigned long   len = 0;
    int             ret = 0; 
	struct i2c_msg tpm_write;

    // Saved the best part of the Atmel driver: the comment below...
   // saddle up and send the whole buffer 

    if ((ret = copy_from_user(tpm.iobuf, buff, count))) {
		return ret;
	}

    len = (tpm.iobuf[2]<<24)+(tpm.iobuf[3]<<16)+(tpm.iobuf[4]<<8)+tpm.iobuf[5];

    // Always send the first 10 bytes as a seperate Xaction.
    if(len < 10) {
		err("cmd length too short, len=%lu\n", len);
		ret = -ENXIO;
    }
    
	tpm_write.addr = DEVID_TPM;			    /* 0x29 */
	tpm_write.flags = I2C_M_NO_OFFSET;      /* I2C Write Command */
    tpm_write.offset = 0;
	tpm_write.len =	(is_3204) ? len : 10;	/* 3203 commands sent in 2 chunks */
//	tpm_write.len =	10;			            /* Write 10 Bytes */
	tpm_write.buf =	&tpm.iobuf[0];			/* Data to be written */
    //printk("%x %x %x %x %x %x \n",tpm.iobuf[0],tpm.iobuf[1],tpm.iobuf[2],tpm.iobuf[3],tpm.iobuf[4],tpm.iobuf[5]);

	if ((ret = i2c_transfer(tpm_clients[0]->adapter, &tpm_write, 1)) != 1) {
		err("tpm_write preamble fails on adapter %s, ret=%d\n",tpm_clients[0]->adapter->name, ret);
		return -ENXIO;
	}
	if (!is_3204) {
        mdelay(1);
		// Send the remaining (if any) bytes of the command.
		tpm_write.addr = DEVID_TPM;		        /* 0x29 */
		tpm_write.flags = I2C_M_NO_OFFSET;      /* I2C Write Command */
		tpm_write.offset = 0;
		tpm_write.len =	len - 10;			    /* Write the rest of the Bytes */
		tpm_write.buf =	&tpm.iobuf[10];			/* Data to be written */
		if(len > 10) {
		    //printk("Sending %d more bytes\n",len);
			if ((ret = i2c_transfer(tpm_clients[0]->adapter, &tpm_write, 1)) != 1) {
				err("tpm_write fails on adapter %s, ret=%d\n",tpm_clients[0]->adapter->name, ret);
				return -ENXIO;
			}
		}
    } //else
    return len;
}

ssize_t atpm_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
    unsigned long len, retlen;
	int ret = 0;
	struct i2c_msg tpm_read;

    memset(tpm.iobuf, 0, TPM_KBUF_SZ);
    //printk("READ PREAMBLE\n");
    tpm_read.addr = DEVID_TPM;                  /* 0x29 */
    tpm_read.flags = I2C_M_RD|I2C_M_NO_OFFSET;  /* I2C Read Command - don't send offset and restart */
//    tpm_read.flags = I2C_M_RD|I2C_M_NO_OFFSET|I2C_M_ACK_LAST_BYTE;  /* I2C Read Command - don't send offset and restart */
    tpm_read.offset = 0;
    tpm_read.len =	10;                          /* Read 10 Bytes */
    tpm_read.buf =	&tpm.iobuf[0];               /* Read Data */
	if ((ret = i2c_transfer(tpm_clients[0]->adapter, &tpm_read, 1)) != 1) {
		err("read preamble fails on adapter %s, ret=%d\n", tpm_clients[0]->adapter->name, ret);
		ret = -ENXIO;
        return ret;
	}
    len = (tpm.iobuf[2]<<24)+(tpm.iobuf[3]<<16)+(tpm.iobuf[4]<<8)+tpm.iobuf[5];
    retlen = len;
    if (len == -1)  {
        err(
    "atpm_read: Len in the TPM resp hdr is -1, TPM still working...\n");
		ret = -ENXIO;
        return ret;
    }
    if(len > TPM_KBUF_SZ) {
		ret = -ENXIO;
        err("atpm_read: req length > TPM_KBUF_SZ (len=%d)\n",len);
        return ret;
    }
	if (!is_3204)
        len -= 10;
    mdelay(1);
    if(len > 0) {
        //printk("READ BODY, len: %d\n",len);
        tpm_read.addr = DEVID_TPM;                  /* 0x29 */
//        tpm_read.flags = I2C_M_RD|I2C_M_NO_OFFSET|I2C_M_ACK_LAST_BYTE;  /* I2C Read Command - don't send offset and restart */
        tpm_read.flags = I2C_M_RD|I2C_M_NO_OFFSET;  /* I2C Read Command - don't send offset and restart  */
        tpm_read.offset = 0;
        tpm_read.len =	len;			            /* Read more Bytes */
        if(!is_3204)
            tpm_read.buf =	&tpm.iobuf[10];			    /* Read Data */
        else
            tpm_read.buf =	&tpm.iobuf[0];			    /* Read Data */
	    if ((ret = i2c_transfer(tpm_clients[0]->adapter, &tpm_read, 1)) != 1) {
		    err("read fails on adapter %s, ret=%d\n", tpm_clients[0]->adapter->name, ret);
		    ret = -ENXIO;
            return ret;
	    } 
    }
	if ((ret = copy_to_user(buff, tpm.iobuf, retlen))) {
	    return ret;
	}
    return retlen;
}

#if DETECT_TPM
int my_atpm_write(char  *buff, size_t count)
{
    unsigned long   len = 0;
    int             ret = 0; 
	struct i2c_msg tpm_write;

    len = (buff[2]<<24)+(buff[3]<<16)+(buff[4]<<8)+buff[5];

    // Always send the first 10 bytes as a seperate Xaction.
    if(len < 10) {
		err("cmd length too short, len=%lu\n", len);
		ret = -ENXIO;
    }
    
	tpm_write.addr = DEVID_TPM;			    /* 0x29 */
	tpm_write.flags = I2C_M_NO_OFFSET;      /* I2C Write Command */
    tpm_write.offset = 0;
	tpm_write.len =	(is_3204) ? len : 10;	/* 3203 commands sent in 2 chunks */
//	tpm_write.len =	10;			            /* Write 10 Bytes */
	tpm_write.buf =	&buff[0];			/* Data to be written */
    //printk("%x %x %x %x %x %x \n",tpm.iobuf[0],tpm.iobuf[1],tpm.iobuf[2],tpm.iobuf[3],tpm.iobuf[4],tpm.iobuf[5]);
	if ((ret = i2c_transfer(tpm_clients[0]->adapter, &tpm_write, 1)) != 1) {
		err("tpm_write preamble fails on adapter %s, ret=%d\n",tpm_clients[0]->adapter->name, ret);
		return -ENXIO;
	}
    
    if(!is_3204) {    
        mdelay(1);
        // Send the remaining (if any) bytes of the command.
	    tpm_write.addr = DEVID_TPM;		        /* 0x29 */
	    tpm_write.flags = I2C_M_NO_OFFSET;      /* I2C Write Command */
        tpm_write.offset = 0;
	    tpm_write.len =	len - 10;			    /* Write the rest of the Bytes */
	    tpm_write.buf =	&buff[10];			/* Data to be written */
        if(len > 10) {
            //printk("Sending %d more bytes\n",len);
	        if ((ret = i2c_transfer(tpm_clients[0]->adapter, &tpm_write, 1)) != 1) {
		        err("tpm_write fails on adapter %s, ret=%d\n",tpm_clients[0]->adapter->name, ret);
		        return -ENXIO;
	        }
        }
    }

    return len;
}

int my_atpm_read(char *buff, size_t count)
{
    unsigned long len, retlen;
	int ret = 0;
	struct i2c_msg tpm_read;

    tpm_read.addr = DEVID_TPM;                  /* 0x29 */
    tpm_read.flags = I2C_M_RD|I2C_M_NO_OFFSET;  /* I2C Read Command - don't send offset and restart */
    tpm_read.offset = 0;
    tpm_read.len =	10;                          /* Read 10 Bytes */
    tpm_read.buf =	&tpm.iobuf[0];               /* Read Data */
	if ((ret = i2c_transfer(tpm_clients[0]->adapter, &tpm_read, 1)) != 1) {
		err("read preamble fails on adapter %s, ret=%d\n", tpm_clients[0]->adapter->name, ret);
		ret = -ENXIO;
        return ret;
	}
    len = (tpm.iobuf[2]<<24)+(tpm.iobuf[3]<<16)+(tpm.iobuf[4]<<8)+tpm.iobuf[5];
    retlen = len;
    if (len == -1)  {
        err(
    "atpm_read: Len in the TPM resp hdr is -1, TPM still working...\n");
		ret = -ENXIO;
        return ret;
    }
    if(len > TPM_KBUF_SZ) {
		ret = -ENXIO;
        err("atpm_read: req length > TPM_KBUF_SZ (len=%d)\n",len);
        return ret;
    }
    if(!is_3204)
        len -= 10;
    mdelay(1);
    if(len > 0) {
        tpm_read.addr = DEVID_TPM;                  /* 0x29 */
        tpm_read.flags = I2C_M_RD|I2C_M_NO_OFFSET;  /* I2C Read Command - don't send offset and restart  */
        tpm_read.offset = 0;
        tpm_read.len =	len;			            /* Read more Bytes */
        if(!is_3204)
            tpm_read.buf =	&tpm.iobuf[10];			    /* Read Data */
        else
            tpm_read.buf =	&tpm.iobuf[0];			    /* Read Data */
	    if ((ret = i2c_transfer(tpm_clients[0]->adapter, &tpm_read, 1)) != 1) {
		    err("read fails on adapter %s, ret=%d\n", tpm_clients[0]->adapter->name, ret);
		    ret = -ENXIO;
            return ret;
	    } 
    }
    // data returned in tpm.iobuf to avoid catastrophic overruns - only copy back what we have room for
    len = (retlen > count) ? count : retlen;
    memcpy(buff, tpm.iobuf, len);
    return retlen;
}
#endif /* DETECT_TPM */

int atpm_poll(struct file *filp, const char __user *buff, size_t count, loff_t *offp)
{
    unsigned long   len = 0;
    int             ret = 0; 
	struct i2c_msg tpm_write;

    // Saved the best part of the Atmel driver: the comment below...
   // saddle up and send the whole buffer 

	if ((ret = copy_from_user(tpm.iobuf, buff, count))) {
		return ret;
	}

    len = (tpm.iobuf[2]<<24)+(tpm.iobuf[3]<<16)+(tpm.iobuf[4]<<8)+tpm.iobuf[5];

    // Always send the first 10 bytes as a seperate Xaction.
    if(len < 10) {
		err("cmd length too short, len=%lu\n", len);
		ret = -ENXIO;
    }
    
	tpm_write.addr = DEVID_TPM;			    /* 0x29 */
	tpm_write.flags = I2C_M_NO_OFFSET;      /* I2C Write Command */
    tpm_write.offset = 0;
	tpm_write.len =	10;			            /* Write 10 Bytes */
	tpm_write.buf =	&tpm.iobuf[0];			/* Data to be written */
    //printk("%x %x %x %x %x %x \n",tpm.iobuf[0],tpm.iobuf[1],tpm.iobuf[2],tpm.iobuf[3],tpm.iobuf[4],tpm.iobuf[5]);
	if ((ret = i2c_transfer(tpm_clients[0]->adapter, &tpm_write, 1)) != 1) {
		err("tpm_write preamble fails on adapter %s, ret=%d\n",tpm_clients[0]->adapter->name, ret);
		return -ENXIO;
	}
    mdelay(1);
    // Send the remaining (if any) bytes of the command.
	tpm_write.addr = DEVID_TPM;		        /* 0x29 */
	tpm_write.flags = I2C_M_NO_OFFSET;      /* I2C Write Command */
    tpm_write.offset = 0;
	tpm_write.len =	len - 10;			    /* Write the rest of the Bytes */
	tpm_write.buf =	&tpm.iobuf[10];			/* Data to be written */
    if(len > 10) {
	    if ((ret = i2c_transfer(tpm_clients[0]->adapter, &tpm_write, 1)) != 1) {
		    err("tpm_write fails on adapter %s, ret=%d\n",tpm_clients[0]->adapter->name, ret);
		    return -ENXIO;
	    }
    }

    return len;
}

static long
tpm_ioctl(struct file *file, unsigned int cmd,
           unsigned long arg)
{
    struct i2c_client *this_client = NULL;
    int bus_num = 0;
    
    if(bus_num >= tpm_instance){
        return -EINVAL;
    }
    this_client = tpm_clients[bus_num];
    
    switch(cmd){
    
    case TPM_IS_3204:
        if(arg)
            is_3204 = 1;
        else
            is_3204 = 0;
#if DETECT_TPM
        tpm_autodetect = 0;
#endif
        return 0;
            
    default:
        return -EINVAL;
    }
}

static const struct i2c_device_id at97sc3203s_id[] = {
	{ "at97sc3203", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, at97sc3203s_id);

static struct i2c_driver tpm_driver = {
//	.id		= I2C_DRIVERID_AT97SC3203,
	.driver = {
		.name   = "at97sc3203s",
	},
	.probe   = tpm_probe,
	.remove	 = tpm_remove,
	.id_table = at97sc3203s_id,
};

static struct file_operations tpm_fops = {
	.owner 		= THIS_MODULE,
    .read       = atpm_read,
    .write      = atpm_write,
	.unlocked_ioctl 		= tpm_ioctl,
	.open 		= tpm_open,
	.release 	= tpm_release,
};

static struct miscdevice tpm_miscdev = {
	TPM_MINOR,
	"at97sc3203",
	&tpm_fops
};

static int __init tpm_init(void)
{
	int ret;

	ret = i2c_add_driver(&tpm_driver);
	if (ret) {
		printk("Unable to register Phoenix I2C based TPM driver\n");
		return ret;		
	}	

	ret = misc_register(&tpm_miscdev);
	if (ret) {
		err("Register misc driver failed, errno is %d\n", ret);
		(void)i2c_del_driver(&tpm_driver);
		return ret;
	}

	return 0;
}

static void __exit tpm_exit(void)
{
	//remove_proc_entry("driver/max6657", NULL);
	misc_deregister(&tpm_miscdev);
	i2c_del_driver(&tpm_driver);
}


module_init(tpm_init);
module_exit(tpm_exit);

MODULE_AUTHOR("ARUBA");
MODULE_DESCRIPTION("Atmel at97sc3203s Driver");
MODULE_LICENSE("GPL");
