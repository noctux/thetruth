#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "checkcopiedconstants.h"
#include "xmpp.h"
#include "signaling.h"
#include "java/eu_rationality_thetruth_Weechat.h"

#define JGETSTRING(native, java) \
	const char *native = (*env)->GetStringUTFChars(env, java, 0);
#define JRELEASESTRING(native, java) \
	(*env)->ReleaseStringUTFChars(env, java, native);

JNIEXPORT void JNICALL Java_eu_rationality_thetruth_Weechat_trigger_1pending_1operations
  (JNIEnv *env, jclass class)
{
	UNUSED(env);
	UNUSED(class);

	trigger_fd_signaling();
}

JNIEXPORT void JNICALL Java_eu_rationality_thetruth_Weechat_print
  (JNIEnv *env, jclass class, jlong bufferid, jstring jstr)
{
	UNUSED(class);

	JGETSTRING(str, jstr);
	struct t_gui_buffer *buffer = bufferid_to_pointer(bufferid);
	weechat_printf(buffer, "%s", str);
	JRELEASESTRING(str, jstr);
}

JNIEXPORT void JNICALL Java_eu_rationality_thetruth_Weechat_print_1prefix
  (JNIEnv *env, jclass class, jlong bufferid, jstring jprefix, jstring jstr)
{
	UNUSED(class);

	JGETSTRING(str,    jstr);
	JGETSTRING(prefix, jprefix);
	struct t_gui_buffer *buffer = bufferid_to_pointer(bufferid);
	weechat_printf(buffer, "%s%s", weechat_prefix(prefix), str);
	JRELEASESTRING(str, jstr);
	JRELEASESTRING(prefix, jprefix);
}

JNIEXPORT void JNICALL Java_eu_rationality_thetruth_Weechat_print_1date_1tags
  (JNIEnv *env, jclass class, jlong bufferid, jlong date, jstring jtags, jstring jmessage)
{
	UNUSED(class);

	JGETSTRING(tags, jtags);
	JGETSTRING(message, jmessage);
	struct t_gui_buffer *buffer = bufferid_to_pointer(bufferid);
	weechat_printf_date_tags(buffer, (time_t) date, tags, "%s", message);
	JRELEASESTRING(tags, jtags);
	JRELEASESTRING(message, jmessage);
}

JNIEXPORT jlong JNICALL Java_eu_rationality_thetruth_Weechat_buffer_1new
  (JNIEnv *env, jclass class, jstring jname)
{
	UNUSED(class);

	JGETSTRING(name, jname);
	struct t_gui_buffer *buf = weechat_buffer_new(name,
	                                              &xmpp_input_cb, NULL, NULL,
	                                              &xmpp_close_cb, NULL, NULL);
	JRELEASESTRING(name, jname);

	return pointer_to_bufferid(buf);
}

JNIEXPORT void JNICALL Java_eu_rationality_thetruth_Weechat_buffer_1set
  (JNIEnv *env, jclass class, jlong bufferid, jstring jproperty, jstring jval)
{
	UNUSED(class);

	JGETSTRING(property, jproperty);
	JGETSTRING(value,    jval);
	struct t_gui_buffer *buffer = bufferid_to_pointer(bufferid);
	weechat_buffer_set(buffer, property, value);
	JRELEASESTRING(property, jproperty);
	JRELEASESTRING(value,    jval);
}


JNIEXPORT jlong JNICALL Java_eu_rationality_thetruth_Weechat_nicklist_1add_1nick
  (JNIEnv *env, jclass class, jlong bufferid, jstring jnick, jstring jcolor, jstring jprefix)
{
	UNUSED(class);

	JGETSTRING(nick, jnick);
	JGETSTRING(color, jcolor);
	JGETSTRING(prefix, jprefix);
	struct t_gui_buffer *buffer = bufferid_to_pointer(bufferid);
	struct t_gui_nick *nickptr = weechat_nicklist_add_nick(buffer, NULL, nick, color, prefix, "", 1);
	JRELEASESTRING(nick, jnick);
	JRELEASESTRING(color, jcolor);
	JRELEASESTRING(prefix, jprefix);

	return pointer_to_nickid(nickptr);
}

JNIEXPORT void JNICALL Java_eu_rationality_thetruth_Weechat_nicklist_1remove_1nick
  (JNIEnv *env, jclass class, jlong bufferid, jlong nickid)
{
	UNUSED(env);
	UNUSED(class);

	struct t_gui_buffer *buffer = bufferid_to_pointer(bufferid);
	struct t_gui_nick   *nick   = nickid_to_pointer(nickid);
	weechat_nicklist_remove_nick(buffer, nick);
}

JNIEXPORT void JNICALL Java_eu_rationality_thetruth_Weechat_nicklist_1remove_1all
  (JNIEnv *env, jclass class, jlong bufferid)
{
	UNUSED(env);
	UNUSED(class);

	struct t_gui_buffer *buffer = bufferid_to_pointer(bufferid);
	weechat_nicklist_remove_all(buffer);
}

JNIEXPORT void JNICALL Java_eu_rationality_thetruth_Weechat_nicklist_1nick_1set
  (JNIEnv *env, jclass class, jlong bufferid, jlong nickid, jstring jproperty, jstring jval)
{
	UNUSED(class);

	JGETSTRING(property, jproperty);
	JGETSTRING(value, jval);
	struct t_gui_buffer *buffer = bufferid_to_pointer(bufferid);
	struct t_gui_nick *nick = nickid_to_pointer(nickid);
	weechat_nicklist_nick_set(buffer, nick, property, value);
	JRELEASESTRING(value, jval);
	JRELEASESTRING(property, jproperty);
}
