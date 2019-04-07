/* Copyright (c) 2015, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <asm/stacktrace.h>
#include <linux/module.h>

#include "skbuff_debug.h"

static int skbuff_debugobj_enabled __read_mostly = 1;

/*
 * skbuff_debugobj_fixup():
 */
static int skbuff_debugobj_fixup(void *addr, enum debug_obj_state state)
{
	struct sk_buff *skb = (struct sk_buff *)addr;
	ftrace_dump(DUMP_ALL);
	pr_emerg("skbuff_debug: state = %d, skb = 0x%p last free = %pS\n",
			state, skb, skb->free_addr);
	BUG();

	return 0;
}

static struct debug_obj_descr skbuff_debug_descr = {
	.name		= "sk_buff_struct",
	.fixup_init	= skbuff_debugobj_fixup,
	.fixup_activate	= skbuff_debugobj_fixup,
	.fixup_destroy	= skbuff_debugobj_fixup,
	.fixup_free	= skbuff_debugobj_fixup,
};

inline void skbuff_debugobj_activate(struct sk_buff *skb)
{
	int ret;

	if (!skbuff_debugobj_enabled)
		return;

	ret = debug_object_activate(skb, &skbuff_debug_descr);

	if (ret) {
		ftrace_dump(DUMP_ALL);
		pr_emerg("skb_debug: failed to activate err = %d skb = 0x%p last free=%pS\n",
				ret, skb, skb->free_addr);
		BUG();
	}
}

inline void skbuff_debugobj_init_and_activate(struct sk_buff *skb)
{
	if (!skbuff_debugobj_enabled)
		return;

	debug_object_init(skb, &skbuff_debug_descr);
	skbuff_debugobj_activate(skb);
}

static int skbuff_debugobj_walkstack(struct stackframe *frame, void *d)
{
	u32 *pc = d;

	*pc = frame->pc;

	/* walk to outside kernel address space (e.g. module)
	 * for the time being, need a whitelist here in the
	 * future most likely */
	if (is_module_text_address(*pc))
		return -1;

	return 0;
}

#ifdef CONFIG_ARM
static void *skbuff_debugobj_get_free_addr(void)
{
	struct stackframe frame;
	register unsigned long current_sp asm ("sp");
	void *ret = NULL;

	frame.fp = (unsigned long)__builtin_frame_address(0);
	frame.sp = current_sp;
	frame.lr = (unsigned long)__builtin_return_address(0);
	frame.pc = (unsigned long)skbuff_debugobj_get_free_addr;

	walk_stackframe(&frame, skbuff_debugobj_walkstack, &ret);

	return ret;
}
#else
#error
static void *skbuff_debugobj_get_free_addr(void)
{
	/* not supported */
	return 0xdeadbeef;
}
#endif

inline void skbuff_debugobj_deactivate(struct sk_buff *skb)
{
	int obj_state;

	if (!skbuff_debugobj_enabled)
		return;

	obj_state = debug_object_get_state(skb);

	if (obj_state == ODEBUG_STATE_ACTIVE) {
		debug_object_deactivate(skb, &skbuff_debug_descr);
		skb->free_addr = skbuff_debugobj_get_free_addr();
		return;
	}

	ftrace_dump(DUMP_ALL);
	pr_emerg("skbuff_debug: deactivating inactive object skb=0x%p state=%d last free=%pS\n",
			skb, obj_state, skb->free_addr);
	BUG();
}

inline void skbuff_debugobj_destroy(struct sk_buff *skb)
{
	if (!skbuff_debugobj_enabled)
		return;

	debug_object_destroy(skb, &skbuff_debug_descr);
}

static int __init disable_object_debug(char *str)
{
	skbuff_debugobj_enabled = 0;

	pr_info("skbuff_debug: debug objects is disabled\n");
	return 0;
}

early_param("no_skbuff_debug_objects", disable_object_debug);
