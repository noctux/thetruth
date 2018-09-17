#include <libgen.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include <jni.h>
#include <weechat-plugin.h>

#include "xmpp.h"
#include "signaling.h"

#define WEECHAT_JAPI_CLASS "eu/rationality/thetruth/Weechat"
#define JAR_NAME           "the-truth-1.0-SNAPSHOT-jar-with-dependencies.jar"

WEECHAT_PLUGIN_NAME("thetruth")
WEECHAT_PLUGIN_DESCRIPTION("XMPP-Plugin for WeeChat")
WEECHAT_PLUGIN_AUTHOR("Simon Schuster <git@schuster.re>")
WEECHAT_PLUGIN_VERSION("0.1")
WEECHAT_PLUGIN_LICENSE("AGPL3")

struct t_weechat_plugin *weechat_plugin = NULL;
static JavaVM *jvm = NULL;
static JNIEnv *env = NULL;
static jclass api_class;

static bool create_vm(const char *);
static bool destroy_vm(void);
int weechat_plugin_test(void);

#define report_error(__message, __argz...)                  \
	weechat_printf(0, "xmpp.so: %s" __message, weechat_prefix("error"), ##__argz);

#define DELETEREF(ref) \
	(*env)->DeleteLocalRef(env, ref);

#define REPORT_EXCEPTION() \
	report_jni_exception(__FILE__,__LINE__)

#define CHECK_EXCEPTION \
	(*env)->ExceptionCheck(env)

static inline jstring get_jni_class_name(jobject obj, jclass clazz)
{
    jmethodID mid = (*env)->GetMethodID(env, clazz, "getClass", "()Ljava/lang/Class;");
    jobject class = (*env)->CallObjectMethod(env, obj, mid);
    jclass mclazz = (*env)->GetObjectClass(env, class);
    mid = (*env)->GetMethodID(env, mclazz, "getName", "()Ljava/lang/String;");
    jstring class_name = (jstring) (*env)->CallObjectMethod(env, class, mid);
	DELETEREF(mclazz);
	DELETEREF(class);
	return class_name;
}

static inline void report_jni_exception(const char *file, const unsigned int line)
{
	// Adapted from https://stackoverflow.com/a/27072507 (User fluffy) under CC-SA-BY
	jthrowable e = (*env)->ExceptionOccurred(env);
	(*env)->ExceptionClear(env); // clears the exception; e seems to remain valid

	jclass clazz  = (*env)->GetObjectClass(env, e);
	jstring classname = get_jni_class_name(e, clazz);
	const char *cstr = (*env)->GetStringUTFChars(env, classname, NULL);

	jmethodID getMessage = (*env)->GetMethodID(env,
	                                           clazz,
	                                           "getMessage",
	                                           "()Ljava/lang/String;");
	jstring message = (jstring) (*env)->CallObjectMethod(env, e, getMessage);
	const char *mstr = (*env)->GetStringUTFChars(env, message, NULL);

	report_error("%s:%u: Uncaught exception %s: %s", file, line, cstr, mstr);
#ifdef DEBUG
	jmethodID mid    = (*env)->GetStaticMethodID(env, api_class, "print_backtrace", "(Ljava/lang/Throwable;)V");
	if (mid == NULL) {
		report_error("%s: Failed to resolve method Weechat.print_backtrace, skipping backtrace");
	} else {
		(*env)->CallStaticVoidMethod(env, api_class, mid, e);
	}
#endif

	(*env)->ReleaseStringUTFChars(env, classname, cstr);
	(*env)->ReleaseStringUTFChars(env, message, mstr);
	(*env)->DeleteLocalRef(env, classname);
	(*env)->DeleteLocalRef(env, message);
	(*env)->DeleteLocalRef(env, clazz);
	(*env)->DeleteLocalRef(env, e);
}

static inline jstring
convert_string(const char *str)
{
	jstring jstr = (*env)->NewStringUTF(env, str);
	if (jstr == NULL) {
		report_error("Failed to allocate java string: %s", str);
	}
	return jstr;
}

static jobjectArray
convert_chararray(int len, char **array) {
	jobjectArray javaarray = NULL;
	jstring empty          =  convert_string("");
	javaarray = (jobjectArray) (*env)->NewObjectArray(env,
	               len, (*env)->FindClass(env, "java/lang/String"), empty);
	DELETEREF(empty);
	if (javaarray == NULL) {
		report_error("Failed to allocate object array");
		return javaarray;
	}

	for(int i = 0; i < len; i++) {
		jstring str = convert_string(array[i]);
		if (str == NULL) {
			report_error("Failed to allocate object array element");
			DELETEREF(javaarray);
			return NULL;
		}
		(*env)->SetObjectArrayElement(env, javaarray, i, str);
		// No error handling necessary: Throws: ArrayIndexOutOfBounds and ArrayStoreException
		// Both cannot happen here
		DELETEREF(str);
	}

	return javaarray;
}

int
xmpp_command_cb (const void *pointer, void *data,
            struct t_gui_buffer *buffer,
            int argc, char **argv, char **argv_eol)
{
	/* make C compiler happy */
	UNUSED(data);
	UNUSED(argv_eol);

	// callbacks may run after plugin_end was called
	if (!jvm) return WEECHAT_RC_OK;

	jint ret = WEECHAT_RC_ERROR;

	jlong   bufferid = pointer_to_bufferid(buffer);
	jstring cmd = convert_string((const char *) pointer);
	if (cmd == NULL) {
		goto error_cmd;
	}

#if 1
	jobjectArray args = convert_chararray(argc, argv);
	if (args == NULL) {
		goto error_args;
	}
#else
	jobjectArray args = NULL;
	jstring empty     =  (*env)->NewStringUTF(env, "");
	args = (jobjectArray) (*env)->NewObjectArray(env,
			argc, (*env)->FindClass(env, "java/lang/String"), empty);
	if (args == NULL) {
		// TODO: errorhandling
	}

	for(int i = 0; i < argc; i++) {
		jstring str = (*env)->NewStringUTF(env, argv[i]);
		if (str == NULL) {
			// TODO: errorhandling
		}
		(*env)->SetObjectArrayElement(env, args, i, (*env)->NewStringUTF(env, *(argv + i)));
		// TODO: errorhandling
		(*env)->DeleteLocalRef(env, str);
	}
	DELETEREF(empty);
#endif

	jmethodID mid = (*env)->GetStaticMethodID(env, api_class, "command_callback", "(JLjava/lang/String;[Ljava/lang/String;)I");
	if (mid == NULL) {
		report_error("Failed to resolve Weechat.command_callback");
		goto error_mid;

	};
	ret = (*env)->CallStaticIntMethod(env, api_class, mid, bufferid, cmd, args);
	if (CHECK_EXCEPTION) {
		REPORT_EXCEPTION();
		ret = WEECHAT_RC_ERROR;
	}

error_mid:
	DELETEREF(args);
error_args:
	DELETEREF(cmd);
error_cmd:
	return ret;
}

int
xmpp_input_cb (const void *pointer,
               void *data,
               struct t_gui_buffer *buffer,
               const char *input_data)
{
	UNUSED(pointer);
	UNUSED(data);

	// callbacks may run after plugin_end was called
	if (!jvm) return WEECHAT_RC_OK;

	jstring input    = convert_string(input_data);
	if (input == NULL) {
		return WEECHAT_RC_ERROR;
	}
	jlong   bufferid = pointer_to_bufferid(buffer);
	jmethodID mid    = (*env)->GetStaticMethodID(env, api_class, "buffer_input_callback", "(JLjava/lang/String;)I");
	if (mid == NULL) {
		report_error("%s: Failed to resolve method Weechat.buffer_input_callback");
		DELETEREF(input);
		return WEECHAT_RC_ERROR;
	}

	jint ret = (*env)->CallStaticIntMethod(env, api_class, mid, bufferid, input);
	if (CHECK_EXCEPTION) {
		REPORT_EXCEPTION();
		ret = WEECHAT_RC_ERROR;
	}

	DELETEREF(input);

	return ret;
}

int xmpp_close_cb (const void *pointer,
                   void *data,
                   struct t_gui_buffer *buffer)
{
	UNUSED(pointer);
	UNUSED(data);

	// callbacks may run after plugin_end was called
	if (!jvm) return WEECHAT_RC_OK;

	jlong   bufferid = pointer_to_bufferid(buffer);
	jmethodID mid    = (*env)->GetStaticMethodID(env, api_class, "buffer_close_callback", "(J)I");
	if (mid == NULL) {
		report_error("Failed to resolve method Weechat.buffer_close_callback");
		return WEECHAT_RC_ERROR;
	}
	jint ret = (*env)->CallStaticIntMethod(env, api_class, mid, bufferid);
	if (CHECK_EXCEPTION) {
		REPORT_EXCEPTION();
		ret = WEECHAT_RC_ERROR;
	}

	return ret;
}

int
xmpp_pending_operations_cb(const void *pointer, void *data, int fd)
{
	UNUSED(pointer);
	UNUSED(data);

	reset_fd_signaling();

	// callbacks may run after plugin_end was called
	if (!jvm) return WEECHAT_RC_OK;

	jmethodID mid    = (*env)->GetStaticMethodID(env, api_class, "process_pending_operations", "()I");
	if (mid == NULL) {
		report_error("Failed to resolve method Weechat.process_pending_operations");
		return WEECHAT_RC_ERROR;
	}
	jint ret = (*env)->CallStaticIntMethod(env, api_class, mid);
	if (CHECK_EXCEPTION) {
		REPORT_EXCEPTION();
		ret = WEECHAT_RC_ERROR;
	}

	return ret;
}



static bool
create_vm(const char *classpath)
{
	JavaVMInitArgs vm_args;
	JavaVMOption options[2];
	const char *classpathflag = "-Djava.class.path=";
	char cp[strlen(classpathflag) + 1 + strlen(classpath) + 1];
	snprintf(cp, sizeof(cp)/sizeof(*cp), "%s%s", classpathflag, classpath);
	options[0].optionString = cp;
#ifdef DEBUG
	options[1].optionString = "-Dsmack.debugEnabled=true";
	vm_args.nOptions = 2;
#else
	vm_args.nOptions = 1;
#endif
	vm_args.version = JNI_VERSION_10;
	vm_args.options = options;
	vm_args.ignoreUnrecognized = false;
	int ret = JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);
	if (ret < 0) {
		env = NULL;
		weechat_printf(NULL, "Error creating vm: %d\n", ret);
		return false;
	}
	api_class = (*env)->FindClass(env, WEECHAT_JAPI_CLASS);
	if (api_class == NULL) {
		weechat_printf(NULL, "Error resolving java api class in classpath: %s\n", WEECHAT_JAPI_CLASS);
		destroy_vm();
		return false;
	}

	return true;
}

static bool
destroy_vm(void)
{
	if (jvm) {
		jint ret = (*jvm)->DestroyJavaVM(jvm);
		if (ret != JNI_OK) {
			report_error("Error destroying VM");
			/* Report error */
			return false;
		}
		jvm = NULL;
	}
	api_class = NULL;
	return true;
}

int
weechat_plugin_test(void)
{
	jmethodID mid = (*env)->GetStaticMethodID(env, api_class, "test", "(I)V");
	if (mid == NULL) {
		report_error("Failed to resolve method Weechat.test");
		return WEECHAT_RC_ERROR;
	}
	(*env)->CallStaticVoidMethod(env, api_class, mid, 100);
	if (CHECK_EXCEPTION) {
		REPORT_EXCEPTION();
		return WEECHAT_RC_ERROR;
	}
	return WEECHAT_RC_OK;
}

static int
weechat_java_init(void)
{
	jmethodID mid = (*env)->GetStaticMethodID(env, api_class, "init", "()I");
	if (mid == NULL) {
		report_error("Failed to resolve method Weechat.init");
		return WEECHAT_RC_ERROR;
	}
	jint ret = (*env)->CallStaticIntMethod(env, api_class, mid);
	if (CHECK_EXCEPTION) {
		REPORT_EXCEPTION();
		return WEECHAT_RC_ERROR;
	}
	return ret;
}

int
weechat_plugin_init (struct t_weechat_plugin *plugin,
                     int argc, char *argv[])
{
	(void) argc;
	(void) argv;
    weechat_plugin = plugin;

	weechat_printf(NULL, "starting up\n");

	// Setting up signaling infrastructure
	create_hook_fd_signaling();

	// Get the directory of the so (we assume the jar to be located there as well)
	// and construct the classpath from this information
	char filecopy[strlen(plugin->filename) + 1];
	strcpy(filecopy, plugin->filename);
	char *pluginbasedir = dirname(filecopy);
	char classpath[strlen(pluginbasedir) + 1 + strlen(JAR_NAME) + 1];
	snprintf(classpath, sizeof(classpath)/sizeof(*classpath), "%s/%s", pluginbasedir, JAR_NAME);

	weechat_printf(NULL, "Creating vm with classpath: %s\n", classpath);

	//if (!create_vm(classpath)) {
	if (!create_vm(classpath)) {
		return WEECHAT_RC_ERROR;
	}

	weechat_printf(NULL, "Registering .so at: %s\n", plugin->filename);

	{
		// Load the .so itself from Java so the JVM can find native methods
		jmethodID mid = (*env)->GetStaticMethodID(env, api_class, "loadLibrary", "(Ljava/lang/String;)V");
		if (mid == NULL) {
			report_error("Failed to resolve method Weechat.loadLibrary");
			destroy_vm();
			return WEECHAT_RC_ERROR;
		}

		// The jvm does not seem to like symlinks for libs
		char filename_resolved[PATH_MAX];
		jstring soname = NULL;
		if (realpath(plugin->filename, filename_resolved) == NULL) {
			weechat_printf(NULL, "Failed to resolve realpath(%s), falling back to plain value\n", plugin->filename);
			soname = (*env)->NewStringUTF(env, plugin->filename);
		} else {
			soname = (*env)->NewStringUTF(env, filename_resolved);
		}
		(*env)->CallStaticVoidMethod(env, api_class, mid, soname);
		DELETEREF(soname);

		if (CHECK_EXCEPTION) {
			REPORT_EXCEPTION();
			destroy_vm();
			return WEECHAT_RC_ERROR;
		}
	}

	weechat_printf(NULL, "Running tests\n");

	weechat_java_init();
	// TODO: mayba report errors from there

	weechat_hook_command ("jdouble",
	                      "Display two times a message "
	                      "or execute two times a command",
	                      "message | command",
	                      "message: message to display two times\n"
	                      "command: command to execute two times",
	                      NULL,
	                      &xmpp_command_cb, "jdouble", NULL);

    return WEECHAT_RC_OK;
}

int
weechat_plugin_end (struct t_weechat_plugin *plugin)
{
    /* make C compiler happy */
    (void) plugin;

	bool success = true;

	destroy_hook_fd_signaling();
	success = success && destroy_vm();


	if (success)
    	return WEECHAT_RC_OK;
	else
		return WEECHAT_RC_ERROR;
}


