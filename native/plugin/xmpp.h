#ifndef __GUARD_XMPP_H_
#define __GUARD_XMPP_H_
#include <weechat-plugin.h>
#include <jni.h>
#include <assert.h>

#define UNUSED(X) (void) X;

extern struct t_weechat_plugin *weechat_plugin;

static inline jlong pointer_to_id(void *ptr)
{
	return (jlong) ptr;
}

static inline void *id_to_ptr(jlong id)
{
	// Use a hashmap here in the long term if uintptr_t > jlong
	static_assert(sizeof(jlong) >= sizeof(void *), "Can't store ptrs in jlongs");
	return (void *) id;
}

// Marshalling of pointers between representation levels
static inline struct t_gui_buffer *bufferid_to_pointer(jlong id)
{
	return (struct t_gui_buffer *) id_to_ptr(id);
}

static inline jlong pointer_to_bufferid(struct t_gui_buffer *ptr)
{
	return pointer_to_id(ptr);
}

// Marshalling of pointers between representation levels
static inline struct t_gui_nick *nickid_to_pointer(jlong id)
{
	return (struct t_gui_nick *) id_to_ptr(id);
}

static inline jlong pointer_to_nickid(struct t_gui_nick *ptr)
{
	return pointer_to_id(ptr);
}


int xmpp_input_cb (const void *, void *, struct t_gui_buffer *, const char *);
int xmpp_command_cb (const void *, void *, struct t_gui_buffer *, int, char **, char **);
int xmpp_close_cb (const void *, void *, struct t_gui_buffer *);
int xmpp_pending_operations_cb(const void *, void *, int);
#endif
