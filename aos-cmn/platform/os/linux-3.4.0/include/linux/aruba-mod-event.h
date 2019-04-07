#ifndef ARUBA_EVENT_H
#define ARUBA_EVENT_H
#include <linux/notifier.h>
#include <linux/module.h>

/* Aruba module IDs */
enum {
    ARUBA_MODULE_ID_ASAP,
    ARUBA_MODULE_ID_ANUL,
    ARUBA_MODULE_ID_UOL,
};

/* Aruba module events */
enum {
    ARUBA_MODULE_EVENT_READY,
    ARUBA_MODULE_EVENT_REGISTER_INTERFACE,
};

struct aruba_module_event {
    u32 event;
    u32 target_module;
    u32 source_module;
    void *ctx;
};

/* APIs for sender */
void aruba_module_event_notify(u32 event, u32 target_module, u32 source_module, void *ctx);
void aruba_module_event_ready(u32 target_module, u32 source_module, void *ctx);
void aruba_module_event_register_interface(u32 target_module, u32 source_module, void *ctx);

/* APIs for listener */
void aruba_module_event_notifier_register(struct notifier_block *n);
#endif

