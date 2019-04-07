#include <linux/aruba-mod-event.h>

static BLOCKING_NOTIFIER_HEAD(aruba_module_event_notifier_list);

/* APIs for sender */
void aruba_module_event_notify(u32 event, u32 target_module, u32 source_module, void *ctx)
{
    struct aruba_module_event e = {
        .event = event,
        .target_module = target_module,
        .source_module = source_module,
        .ctx = ctx
    };

    blocking_notifier_call_chain(&aruba_module_event_notifier_list, event, &e);
}
EXPORT_SYMBOL(aruba_module_event_notify);

void aruba_module_event_ready(u32 target_module, u32 source_module, void *ctx)
{
    aruba_module_event_notify(ARUBA_MODULE_EVENT_READY, target_module, source_module, ctx);
}
EXPORT_SYMBOL(aruba_module_event_ready);

void aruba_module_event_register_interface(u32 target_module, u32 source_module, void *ctx)
{
    aruba_module_event_notify(ARUBA_MODULE_EVENT_REGISTER_INTERFACE, target_module, source_module, ctx);
}
EXPORT_SYMBOL(aruba_module_event_register_interface);

/* API for listener */
void aruba_module_event_notifier_register(struct notifier_block *n)
{
    blocking_notifier_chain_register(&aruba_module_event_notifier_list, n);
}
EXPORT_SYMBOL(aruba_module_event_notifier_register);

