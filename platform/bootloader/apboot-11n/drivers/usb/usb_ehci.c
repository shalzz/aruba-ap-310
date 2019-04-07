#ifndef powerpc
/* Copyright (c) 2007-2008, Juniper Networks, Inc.
 * All rights reserved. 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <pci.h>
#include <usb.h>
#include "usb_ehci.h"

#undef EHCI_DEBUG
#ifdef EHCI_DEBUG
#define	DBG(format, arg...)	\
	printf("\rEHCI_DEBUG: %s: " format "\n", __func__, ## arg)
#else
#define	DBG(format, arg...)	\
	do {} while(0)
#endif /* EHCI_DEBUG */

static struct {
	uint8_t hub[8];
	uint8_t device[18];
	uint8_t config[9];
	uint8_t interface[9];
	uint8_t endpoint[7];
} descr = {
    {	/* HUB */
	sizeof(descr.hub),	/* bDescLength */
	0x29,		/* bDescriptorType: hub descriptor */
	1,		/* bNrPorts -- runtime modified */
	0, 0,		/* wHubCharacteristics -- runtime modified */
	0xff,		/* bPwrOn2PwrGood */
	0,		/* bHubCntrCurrent */
	0		/* DeviceRemovable XXX at most 7 ports! XXX */
    },
    {	/* DEVICE */
	sizeof(descr.device),	/* bLength */
	1,		/* bDescriptorType: UDESC_DEVICE */
	0x00, 0x02,	/* bcdUSB: v2.0 */
	9,		/* bDeviceClass: UDCLASS_HUB */
	0,		/* bDeviceSubClass: UDSUBCLASS_HUB */
	1,		/* bDeviceProtocol: UDPROTO_HSHUBSTT */
	64,		/* bMaxPacketSize: 64 bytes */
	0x00, 0x00,	/* idVendor */
	0x00, 0x00,	/* idProduct */
	0x00, 0x01,	/* bcdDevice */
	1,		/* iManufacturer */
	2,		/* iProduct */
	0,		/* iSerialNumber */
	1		/* bNumConfigurations: 1 */
    },
    {	/* CONFIG */
	sizeof(descr.config),	/* bLength */
	2,		/* bDescriptorType: UDESC_CONFIG */
	sizeof(descr.config)+sizeof(descr.interface)+sizeof(descr.endpoint), 0,
			/* wTotalLength */
	1,		/* bNumInterface */
	1,		/* bConfigurationValue */
	0,		/* iConfiguration */
	0x40,		/* bmAttributes: UC_SELF_POWERED */
	0		/* bMaxPower */
    },
    {	/* INTERFACE */
	sizeof(descr.interface),	/* bLength */
	4,		/* bDescriptorType: UDESC_INTERFACE */
	0,		/* bInterfaceNumber */
	0,		/* bAlternateSetting */
	1,		/* bNumEndpoints */
	9,		/* bInterfaceClass: UICLASS_HUB */
	0,		/* bInterfaceSubClass: UISUBCLASS_HUB */
	0,		/* bInterfaceProtocol: UIPROTO_HSHUBSTT */
	0		/* iInterface */
    },
    {	/* ENDPOINT */
	sizeof(descr.endpoint),		/* bLength */
	5,		/* bDescriptorType: UDESC_ENDPOINT */
	0x81,		/* bEndpointAddress: UE_DIR_IN | EHCI_INTR_ENDPT */
	3,		/* bmAttributes: UE_INTERRUPT */
	8, 0,		/* wMaxPacketSize */
	255		/* bInterval */
    }
};

static struct ehci_hccr *hccr;	/* R/O registers, not need for volatile */
static volatile struct ehci_hcor *hcor;
static struct QH qh_list __attribute__((aligned(32)));
static int rootdev;
static uint16_t portreset;

#ifdef EHCI_DEBUG
static void
dump_pci_reg(pci_dev_t dev, int ofs)
{
	uint32_t reg;

	pci_read_config_dword(dev, ofs, &reg);
	printf("\t0x%02x: %08x\n", ofs, reg);
}

static void
dump_pci(int enh, pci_dev_t dev)
{
	int ofs;

	DBG("\n%s", (enh) ? "EHCI" : "OHCI");
	for (ofs = 0; ofs < 0x44; ofs += 4)
		dump_pci_reg(dev, ofs);
	if (enh)
		dump_pci_reg(dev, 0x60);
	dump_pci_reg(dev, 0xdc);
	dump_pci_reg(dev, 0xe0);
	if (enh) {
		dump_pci_reg(dev, 0xe4);
		dump_pci_reg(dev, 0xe8);
	}
}

static void
dump_regs(void)
{

	DBG("usbcmd=%#x, usbsts=%#x, usbintr=%#x,\n\tfrindex=%#x, "
	    "ctrldssegment=%#x, periodiclistbase=%#x,\n\tasynclistaddr=%#x, "
	    "configflag=%#x,\n\tportsc[1]=%#x, portsc[2]=%#x, usbcmd=%#x",
	    swap_32(hcor->or_usbcmd), swap_32(hcor->or_usbsts),
	    swap_32(hcor->or_usbintr), swap_32(hcor->or_frindex),
	    swap_32(hcor->or_ctrldssegment),
	    swap_32(hcor->or_periodiclistbase),
	    swap_32(hcor->or_asynclistaddr), swap_32(hcor->or_configflag),
	    swap_32(hcor->or_portsc[0]), swap_32(hcor->or_portsc[1]),
	    swap_32(hcor->or_usbmode));
}

static void
dump_TD(struct qTD *td)
{

	DBG("%p: qt_next=%#x, qt_altnext=%#x, qt_token=%#x, "
	    "qt_buffer={%#x,%#x,%#x,%#x,%#x}", td, swap_32(td->qt_next),
	    swap_32(td->qt_altnext), swap_32(td->qt_token),
	    swap_32(td->qt_buffer[0]), swap_32(td->qt_buffer[1]),
	    swap_32(td->qt_buffer[2]), swap_32(td->qt_buffer[3]),
	    swap_32(td->qt_buffer[4]));
}

static void
dump_QH(struct QH *qh)
{

	DBG("%p: qh_link=%#x, qh_endpt1=%#x, qh_endpt2=%#x, qh_curtd=%#x",
	    qh, swap_32(qh->qh_link), swap_32(qh->qh_endpt1),
	    swap_32(qh->qh_endpt2), swap_32(qh->qh_curtd));
	dump_TD(&qh->qh_overlay);
}
#endif

static __inline int
min3(int a, int b, int c)
{

	if (b < a)
		a = b;
	if (c < a)
		a = c;
	return (a);
}

/*
 * Create the appropriate control structures to manage
 * a new EHCI host controller.
 */
int
usb_lowlevel_init(void)
{
	pci_dev_t dev;
	uint32_t addr, reg;

#ifndef CONFIG_MARVELL
	dev = pci_find_device(0x1131, 0x1561, 0);
	if (dev != -1) {
		volatile uint32_t *hcreg;

		pci_read_config_dword(dev, PCI_BASE_ADDRESS_0, &addr);
		hcreg = (uint32_t *)(addr + 8);
		*hcreg = swap_32(1);
		udelay(100);
	}

	dev = pci_find_device(0x1131, 0x1562, 0);
	if (dev == -1) {
		printf("EHCI host controller not found\n");
		return (-1);
	}

	pci_read_config_dword(dev, EHCI_PCICS_USBBASE, &addr);
#else
	addr = 0xF1050100;
#endif /* CONFIG_MARVELL */
	hccr = (void *)addr;
	addr += hccr->cr_caplength;
	hcor = (void *)addr;
	/* Reset the device */
	hcor->or_usbcmd |= swap_32(2);
	udelay(1000);
	while (hcor->or_usbcmd & swap_32(2))
		udelay(1000);

	reg = swap_32(hccr->cr_hcsparams);
	descr.hub[2] = reg & 0xf;
	if (reg & 0x10000)	/* Port Indicators */
		descr.hub[3] |= 0x80;
	if (reg & 0x10)		/* Port Power Control */
		descr.hub[3] |= 0x01;

	/* Marvell USB controller Host mode set */
	hcor->or_usbmode |= swap_32(3);

    /* take control over the ports */
//	hcor->or_configflag |= swap_32(1);

	/* Set head of reclaim list */
	memset(&qh_list, 0, sizeof(qh_list));
	qh_list.qh_link = swap_32((uint32_t)&qh_list | QH_LINK_TYPE_QH);
	qh_list.qh_endpt1 = swap_32((1 << 15) | (USB_SPEED_HIGH << 12));
	qh_list.qh_curtd = swap_32(QT_NEXT_TERMINATE);
	qh_list.qh_overlay.qt_next = swap_32(QT_NEXT_TERMINATE);
	qh_list.qh_overlay.qt_altnext = swap_32(QT_NEXT_TERMINATE);
	qh_list.qh_overlay.qt_token = swap_32(0x40);

	/* Set async. queue head pointer. */
	hcor->or_asynclistaddr = swap_32((uint32_t)&qh_list);

	/* Start the host controller. */
	hcor->or_usbcmd |= swap_32(1);

	rootdev = 0;

	return (0);
}

/*
 * Destroy the appropriate control structures corresponding
 * the the EHCI host controller.
 */
int
usb_lowlevel_stop(void)
{
	return (0);
}

static void *
ehci_alloc(size_t sz, size_t align)
{
	static struct QH qh __attribute__((aligned(32)));
	static struct qTD td[4] __attribute__((aligned(32)));
	static int ntds = 0;
	void *p;

	switch (sz) {
	case sizeof(struct QH):
		p = &qh;
		ntds = 0;
		break;
	case sizeof(struct qTD):
		if (ntds == 3) {
			DBG("out of TDs");
			return (NULL);
		}
		p = &td[ntds];
		ntds++;
		break;
	default:
		DBG("unknown allocation size");
		return (NULL);
	}

	memset(p, sz , 0);
	return (p);
}

static void
ehci_free(void *p, size_t sz)
{
}

static int
ehci_td_buffer(struct qTD *td, void *buf, size_t sz)
{
	uint32_t addr, delta, next;
	int idx;

	addr = (uint32_t)buf;
	idx = 0;
	while (idx < 5) {
		td->qt_buffer[idx] = swap_32(addr);
		next = (addr + 4096) & ~4095;
		delta = next - addr;
		if (delta >= sz)
			break;
		sz -= delta;
		addr = next;
		idx++;
	}

	if (idx == 5) {
		DBG("out of buffer pointers (%u bytes left)", sz);
		return (-1);
	}

	return (0);
}

static int
ehci_submit_async(struct usb_device *dev, unsigned long pipe, void *buffer,
    int length, struct devrequest *req)
{
	struct QH *qh;
	struct qTD *td;
	volatile struct qTD *vtd;
	unsigned long ts;
	uint32_t *tdp;
	uint32_t endpt, token, usbsts;
	uint32_t c, toggle;

	DBG("dev=%p, pipe=%lx, buffer=%p, length=%d, req=%p", dev, pipe,
	    buffer, length, req);
	if (req != NULL)
		DBG("req=%u (%#x), type=%u (%#x), value=%u (%#x), index=%u",
		    req->request, req->request,
		    req->requesttype, req->requesttype,
		    swap_16(req->value), swap_16(req->value),
		    swap_16(req->index), swap_16(req->index));

	qh = ehci_alloc(sizeof(struct QH), 32);
	if (qh == NULL) {
		DBG("unable to allocate QH");
		return (-1);
	}
	qh->qh_link = swap_32((uint32_t)&qh_list | QH_LINK_TYPE_QH);
	c = (usb_pipespeed(pipe) != USB_SPEED_HIGH &&
	    usb_pipeendpoint(pipe) == 0) ? 1 : 0;
	endpt = (8 << 28) |
	    (c << 27) |
	    (usb_maxpacket(dev, pipe) << 16) |
	    (0 << 15) |
	    (1 << 14) |
	    (usb_pipespeed(pipe) << 12) |
	    (usb_pipeendpoint(pipe) << 8) |
	    (0 << 7) |
	    (usb_pipedevice(pipe) << 0);
	qh->qh_endpt1 = swap_32(endpt);
	endpt = (1 << 30) |
	    (dev->portnr << 23) |
	    (dev->parent->devnum << 16) |
	    (0 << 8) |
	    (0 << 0);
	qh->qh_endpt2 = swap_32(endpt);
	qh->qh_overlay.qt_next = swap_32(QT_NEXT_TERMINATE);
	qh->qh_overlay.qt_altnext = swap_32(QT_NEXT_TERMINATE);

	td = NULL;
	tdp = &qh->qh_overlay.qt_next;

	toggle = usb_gettoggle(dev, usb_pipeendpoint(pipe), usb_pipeout(pipe));

	if (req != NULL) {
		td = ehci_alloc(sizeof(struct qTD), 32);
		if (td == NULL) {
			DBG("unable to allocate SETUP td");
			goto fail;
		}
		td->qt_next = swap_32(QT_NEXT_TERMINATE);
		td->qt_altnext = swap_32(QT_NEXT_TERMINATE);
		token = (0 << 31) |
		    (sizeof(*req) << 16) |
		    (0 << 15) |
		    (0 << 12) |
		    (3 << 10) |
		    (2 << 8) |
		    (0x80 << 0);
		td->qt_token = swap_32(token);
		if (ehci_td_buffer(td, req, sizeof(*req)) != 0) {
			DBG("unable construct SETUP td");
			ehci_free(td, sizeof(*td));
			goto fail;
		}
		*tdp = swap_32((uint32_t)td);
		tdp = &td->qt_next;
		toggle = 1;
	}

	if (length > 0 || req == NULL) {
		td = ehci_alloc(sizeof(struct qTD), 32);
		if (td == NULL) {
			DBG("unable to allocate DATA td");
			goto fail;
		}
		td->qt_next = swap_32(QT_NEXT_TERMINATE);
		td->qt_altnext = swap_32(QT_NEXT_TERMINATE);
		token = (toggle << 31) |
		    (length << 16) |
		    ((req == NULL ? 1 : 0) << 15) |
		    (0 << 12) |
		    (3 << 10) |
		    ((usb_pipein(pipe) ? 1 : 0) << 8) |
		    (0x80 << 0);
		td->qt_token = swap_32(token);
		if (ehci_td_buffer(td, buffer, length) != 0) {
			DBG("unable construct DATA td");
			ehci_free(td, sizeof(*td));
			goto fail;
		}
		*tdp = swap_32((uint32_t)td);
		tdp = &td->qt_next;
	}

	if (req != NULL) {
		td = ehci_alloc(sizeof(struct qTD), 32);
		if (td == NULL) {
			DBG("unable to allocate ACK td");
			goto fail;
		}
		td->qt_next = swap_32(QT_NEXT_TERMINATE);
		td->qt_altnext = swap_32(QT_NEXT_TERMINATE);
		token = (toggle << 31) |
		    (0 << 16) |
		    (1 << 15) |
		    (0 << 12) |
		    (3 << 10) |
		    ((usb_pipein(pipe) ? 0 : 1) << 8) |
		    (0x80 << 0);
		td->qt_token = swap_32(token);
		*tdp = swap_32((uint32_t)td);
		tdp = &td->qt_next;
	}

	qh_list.qh_link = swap_32((uint32_t)qh | QH_LINK_TYPE_QH);

	usbsts = swap_32(hcor->or_usbsts);
	hcor->or_usbsts = swap_32(usbsts & 0x3f);

	/* Enable async. schedule. */
	hcor->or_usbcmd |= swap_32(0x20);
	while ((hcor->or_usbsts & swap_32(0x8000)) == 0)
		udelay(1);

    /* Wait for TDs to be processed. */
	ts = get_timer(0);
	vtd = td;
	do {
		token = swap_32(vtd->qt_token);
		if (!(token & 0x80))
			break;
	} while (get_timer(ts) < CFG_HZ);

	/* Disable async schedule. */
	hcor->or_usbcmd &= ~swap_32(0x20);
	while ((hcor->or_usbsts & swap_32(0x8000)) != 0)
		udelay(1);

	qh_list.qh_link = swap_32((uint32_t)&qh_list | QH_LINK_TYPE_QH);

	token = swap_32(qh->qh_overlay.qt_token);
    /*printf("TOKEN=%#x\n", token);*/
	if (!(token & 0x80)) {
		switch (token & 0xfc) {
		case 0:
			toggle = token >> 31;
			usb_settoggle(dev, usb_pipeendpoint(pipe),
			    usb_pipeout(pipe), toggle);
			dev->status = 0;
			break;
		case 0x40:
			dev->status = USB_ST_STALLED;
			break;
		case 0xa0:
		case 0x20:
			dev->status = USB_ST_BUF_ERR;
			break;
		case 0x50:
		case 0x10:
			dev->status = USB_ST_BABBLE_DET;
			break;
		default:
			dev->status = USB_ST_CRC_ERR;
			break;
		}
		dev->act_len = length - ((token >> 16) & 0x7fff);
	} else {
		printf("T ");
		dev->act_len = 0;
		DBG("dev=%u, usbsts=%#x, p[1]=%#x, p[2]=%#x",
		    dev->devnum, swap_32(hcor->or_usbsts),
		    swap_32(hcor->or_portsc[0]), swap_32(hcor->or_portsc[1]));
	}

	return ((dev->status != USB_ST_NOT_PROC) ? 0 : -1);

 fail:
	td = (void *)swap_32(qh->qh_overlay.qt_next);
	while (td != (void *)QT_NEXT_TERMINATE) {
		qh->qh_overlay.qt_next = td->qt_next;
		ehci_free(td, sizeof(*td));
		td = (void *)swap_32(qh->qh_overlay.qt_next);
	}
	ehci_free(qh, sizeof(*qh));
	return (-1);
}

static int
ehci_submit_root(struct usb_device *dev, unsigned long pipe, void *buffer,
    int length, struct devrequest *req)
{
	uint8_t tmpbuf[4];
	void *srcptr;
	int len, srclen;
	uint32_t reg;

	srclen = 0;
	srcptr = NULL;

	DBG("req=%u (%#x), type=%u (%#x), value=%u, index=%u",
	    req->request, req->request,
	    req->requesttype, req->requesttype,
	    swap_16(req->value), swap_16(req->index));

#define C(a,b)	(((b) << 8) | (a))

	switch (C(req->request, req->requesttype)) {
	case C(USB_REQ_GET_DESCRIPTOR, USB_DIR_IN | USB_RECIP_DEVICE):
		switch(swap_16(req->value) >> 8) {
		case USB_DT_DEVICE:
			srcptr = descr.device;
			srclen = sizeof(descr.device);
			break;
		case USB_DT_CONFIG:
			srcptr = descr.config;
			srclen = sizeof(descr.config) +
			    sizeof(descr.interface) + sizeof(descr.endpoint);
			break;
		case USB_DT_STRING:
			switch (swap_16(req->value) & 0xff) {
			case 0:		/* Language */
				srcptr = "\4\3\1\0";
				srclen = 4;
				break;
			case 1:		/* Vendor */
				srcptr = "\20\3M\0a\0r\0v\0e\0l\0l\0";
				srclen = 16;
				break;
			case 2:		/* Product */
				srcptr = "\12\3E\0H\0C\0I\0";
				srclen = 10;
				break;
			default:
				goto unknown;
			}
			break;
		default:
			DBG("+unknown value %x", swap_16(req->value));
			goto unknown;
		}
		break;
	case C(USB_REQ_GET_DESCRIPTOR, USB_DIR_IN | USB_RT_HUB):
		switch (swap_16(req->value) >> 8) {
		case USB_DT_HUB:
			srcptr = descr.hub;
			srclen = sizeof(descr.hub);
			break;
		default:
			DBG("-unknown value %x", swap_16(req->value));
			goto unknown;
		}
		break;
	case C(USB_REQ_SET_ADDRESS, USB_RECIP_DEVICE):
		rootdev = swap_16(req->value);
		break;
	case C(USB_REQ_SET_CONFIGURATION, USB_RECIP_DEVICE):
		/* Nothing to do */
		break;
	case C(USB_REQ_GET_STATUS, USB_DIR_IN | USB_RT_HUB):
		tmpbuf[0] = 1; /* USB_STATUS_SELFPOWERED */
		tmpbuf[1] = 0;
		srcptr = tmpbuf;
		srclen = 2;
		break;
	case C(USB_REQ_GET_STATUS, USB_DIR_IN | USB_RT_PORT):
		memset(tmpbuf, 0, 4);
		reg = swap_32(hcor->or_portsc[swap_16(req->index) - 1]);
		if (reg & EHCI_PS_CS)
			tmpbuf[0] |= USB_PORT_STAT_CONNECTION;
		if (reg & EHCI_PS_PE)
			tmpbuf[0] |= USB_PORT_STAT_ENABLE;
		if (reg & EHCI_PS_SUSP)
			tmpbuf[0] |= USB_PORT_STAT_SUSPEND;
		if (reg & EHCI_PS_OCA)
			tmpbuf[0] |= USB_PORT_STAT_OVERCURRENT;
		if (reg & EHCI_PS_PR)
			tmpbuf[0] |= USB_PORT_STAT_RESET;
		if (reg & EHCI_PS_PP)
			tmpbuf[1] |= USB_PORT_STAT_POWER >> 8;
		if (reg & EHCI_PS_PSPD == 0x08000000)
            tmpbuf[1] |= USB_PORT_STAT_HIGH_SPEED;// >> 8;
		else if (reg & EHCI_PS_PSPD == 0x04000000 || reg & EHCI_PS_PSPD == 0x00000000)
            tmpbuf[1] |= USB_PORT_STAT_LOW_SPEED;// >> 8;

		if (reg & EHCI_PS_CSC)
			tmpbuf[2] |= USB_PORT_STAT_C_CONNECTION;
		if (reg & EHCI_PS_PEC)
			tmpbuf[2] |= USB_PORT_STAT_C_ENABLE;
		if (reg & EHCI_PS_OCC)
			tmpbuf[2] |= USB_PORT_STAT_C_OVERCURRENT;
		if (portreset & (1 << swap_16(req->index)))
			tmpbuf[2] |= USB_PORT_STAT_C_RESET;
		srcptr = tmpbuf;
		srclen = 4;
		break;
	case C(USB_REQ_SET_FEATURE, USB_DIR_OUT | USB_RT_PORT):
		reg = swap_32(hcor->or_portsc[swap_16(req->index) - 1]);
		reg |= EHCI_PS_CLEAR;
		switch (swap_16(req->value)) {
		case USB_PORT_FEAT_POWER:
			reg |= EHCI_PS_PP;
			break;
		case USB_PORT_FEAT_RESET:
			if (EHCI_PS_IS_LOWSPEED(reg)) {
				/* Low speed device, give up ownership. */
				reg |= EHCI_PS_PO;
				break;
			}
			/* Start reset sequence. */
			reg &= ~EHCI_PS_PE;
			reg |= EHCI_PS_PR;
			hcor->or_portsc[swap_16(req->index) - 1] = swap_32(reg);
			/* Wait for reset to complete. */
			udelay(250000);
			/* Terminate reset sequence. */
		/*	reg &= ~EHCI_PS_PR;
			hcor->or_portsc[swap_16(req->index) - 1] = swap_32(reg);*/
			/* Wait for HC to complete reset. */
			udelay(2000);

			reg = swap_32(hcor->or_portsc[swap_16(req->index) - 1]);
			reg |= EHCI_PS_CLEAR;
			//reg &= ~EHCI_PS_CLEAR;
			if ((reg & EHCI_PS_PE) == 0) {
				/* Not a high speed device, give up ownership.*/
				reg |= EHCI_PS_PO;
				break;
			}
			portreset |= 1 << swap_16(req->index);
			break;
		default:
			DBG("unknown feature %x", swap_16(req->value));
			goto unknown;
		}
		hcor->or_portsc[swap_16(req->index) - 1] = swap_32(reg);
		break;
	case C(USB_REQ_CLEAR_FEATURE, USB_DIR_OUT | USB_RT_PORT):
		reg = swap_32(hcor->or_portsc[swap_16(req->index) - 1]);
		reg &= ~EHCI_PS_CLEAR;
		switch (swap_16(req->value)) {
		case USB_PORT_FEAT_ENABLE:
			reg &= ~EHCI_PS_PE;
			break;
		case USB_PORT_FEAT_C_CONNECTION:
			reg |= EHCI_PS_CSC;
			break;
		case USB_PORT_FEAT_C_RESET:
			portreset &= ~(1 << swap_16(req->index));
			break;
		default:
			DBG("unknown feature %x", swap_16(req->value));
			goto unknown;
		}
		hcor->or_portsc[swap_16(req->index) - 1] = swap_32(reg);
		break;
	default:
		DBG("Unknown request %x", C(req->request, req->requesttype));
		goto unknown;
	}

#undef C

	len = min3(srclen, swap_16(req->length), length);
	if (srcptr != NULL && len > 0)
		memcpy(buffer, srcptr, len);
	dev->act_len = len;
	dev->status = 0;
	return (0);

 unknown:
	DBG("requesttype=%x, request=%x, value=%x, index=%x, length=%x",
	    req->requesttype, req->request, swap_16(req->value),
	    swap_16(req->index), swap_16(req->length));

	dev->act_len = 0;
	dev->status = USB_ST_STALLED;
	return (-1);
}

int
submit_bulk_msg(struct usb_device *dev, unsigned long pipe, void *buffer,
    int length)
{

	if (usb_pipetype(pipe) != PIPE_BULK) {
		DBG("non-bulk pipe (type=%lu)", usb_pipetype(pipe));
		return (-1);
	}
	return (ehci_submit_async(dev, pipe, buffer, length, NULL));
}

int
submit_control_msg(struct usb_device *dev, unsigned long pipe, void *buffer,
    int length, struct devrequest *setup)
{

	if (usb_pipetype(pipe) != PIPE_CONTROL) {
		DBG("non-control pipe (type=%lu)", usb_pipetype(pipe));
		return (-1);
	}

	if (usb_pipedevice(pipe) == rootdev) {
		if (rootdev == 0)
        {
            dev->speed = (0x0c000000 & (*(volatile unsigned int *)(0xf1050184))) >> 26/*USB_SPEED_HIGH*/;
        }
		return (ehci_submit_root(dev, pipe, buffer, length, setup));
	}
	return (ehci_submit_async(dev, pipe, buffer, length, setup));
}

int
submit_int_msg(struct usb_device *dev, unsigned long pipe, void *buffer,
    int length, int interval)
{

	DBG("dev=%p, pipe=%lu, buffer=%p, length=%d, interval=%d", dev, pipe,
	    buffer, length, interval);
	return (-1);
}
#endif /* powerpc */
