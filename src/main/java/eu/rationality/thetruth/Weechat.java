package eu.rationality.thetruth;

import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.function.Supplier;

import org.jivesoftware.smack.SmackConfiguration;
import org.jivesoftware.smack.util.StringUtils;
import org.jxmpp.jid.EntityBareJid;
import org.jxmpp.jid.impl.JidCreate;
import org.jxmpp.jid.util.JidUtil;

class Weechat {
	@SuppressWarnings("serial")
	static class WeechatCallException extends Exception {
	};
	
	/* return codes for plugin functions */
	static final int WEECHAT_RC_OK     =  0;
	static final int WEECHAT_RC_OK_EAT =  1;
	static final int WEECHAT_RC_ERROR  = -1;
	
	/* return codes for config read functions/callbacks */
	static final int WEECHAT_CONFIG_READ_OK              =  0;
	static final int WEECHAT_CONFIG_READ_MEMORY_ERROR    = -1;
	static final int WEECHAT_CONFIG_READ_FILE_NOT_FOUND  = -2;

	/* return codes for config write functions/callbacks */
	static final int WEECHAT_CONFIG_WRITE_OK             =  0;
	static final int WEECHAT_CONFIG_WRITE_ERROR          = -1;
	static final int WEECHAT_CONFIG_WRITE_MEMORY_ERROR   = -2;
	
	/* null value for option */
	static final String WEECHAT_CONFIG_OPTION_NULL       = "null";

	/* return codes for config option set */
	static final int WEECHAT_CONFIG_OPTION_SET_OK_CHANGED       =  2;
	static final int WEECHAT_CONFIG_OPTION_SET_OK_SAME_VALUE    =  1;
	static final int WEECHAT_CONFIG_OPTION_SET_ERROR            =  0;
	static final int WEECHAT_CONFIG_OPTION_SET_OPTION_NOT_FOUND = -1;

	/* return codes for config option unset */
	static final int WEECHAT_CONFIG_OPTION_UNSET_OK_NO_RESET    =  0;
	static final int WEECHAT_CONFIG_OPTION_UNSET_OK_RESET       =  1;
	static final int WEECHAT_CONFIG_OPTION_UNSET_OK_REMOVED     =  2;
	static final int WEECHAT_CONFIG_OPTION_UNSET_ERROR          = -1;


	
	// Weechat is singlethreaded: therefore we need a mechanism to transform
	// asynchronous operations into callbacks triggered from the main loop
	// Callback management for native callbacks that need to be synchronized
	private static ConcurrentLinkedQueue<Supplier<Integer>> pendingOperations =
			new ConcurrentLinkedQueue<Supplier<Integer>>();
	public  static void register_pending_operation(Supplier<Integer> op) {
		pendingOperations.add(op);
		trigger_pending_operations();
	}
	public static int process_pending_operations() {
		Supplier<Integer> op;
		while((op = pendingOperations.poll()) != null) {
			op.get();
		}
		return WEECHAT_RC_OK;
	}
	native static void trigger_pending_operations();
	

	// Write to buffer with native bufferid
	native static void print(long bufferid, String str);
	// Write to buffer with native bufferid with the specified weechat prefix
	native static void print_prefix(long bufferid, String prefix, String str);
	// Write with date (in seconds since epoch) and tags
	native static void print_date_tags(long bufferid, long date, String tags, String message);
	// Create a named buffer returning the native buffer id
	native static long buffer_new(String name);
	// Set a property for buffer bufferid
	native static void buffer_set(long bufferid, String property, String value);
	// Callback for input received
	public static int buffer_input_callback(long bufferid, String data) {
		Buffer b = BufferManager.getinstance().byid(bufferid);
		if (b == null) {
			printerr(0, "Input callback received for buffer " + Long.toHexString(bufferid) + " which is not managed by the plugin");
			return WEECHAT_RC_ERROR;
		}
		return b.handleInput(data);
	}
	// Callback for buffer close events
	public static int buffer_close_callback(long bufferid) {
		BufferManager.getinstance().deregister(bufferid);
		return WEECHAT_RC_OK;
	}

	// Nicklist related
	native static long nicklist_add_nick(long bufferid, String nick, String color, String prefix);
	native static void nicklist_remove_nick(long bufferid, long nickid);
	native static void nicklist_remove_all(long bufferid);
	native static void nicklist_nick_set(long bufferid, long nickid, String property, String value);

	public static void printerr(long bufferid, String str) {
		print_prefix(bufferid, "error", str);
	}
	
	public static void print_backtrace(Throwable t) {
		var frames = t.getStackTrace();
		Weechat.printerr(0, "Backtrace:\n");
		for (var f : frames) {
			Weechat.printerr(0,  "   " + f.toString());
		}
	}

	public static void test(int a) {
		print(0, "Test " + a);
		printerr(0, "This is an example error");
		try {
			print(0, Integer.toString(a));
		} catch (Exception e) {
			System.err.println("Exception occured: " + e);
		}
		print(0, "Hello from jar file");
		print(0, "Afternative " + a);
		long bufferid = buffer_new("test");
		buffer_set(bufferid, "display", "auto");
		buffer_set(bufferid, "title", "jabber test buffer");
		buffer_set(bufferid, "nicklist", "1");
		long nickid = nicklist_add_nick(bufferid, "hugo", "blue", "@+");
		print(bufferid, "Hello world to buffer " + bufferid);
		print(bufferid, "added nick with id " + nickid);
		print(bufferid, "new message");
		Supplier<Integer> v = () -> {print(0, "Hello from pending operation"); return WEECHAT_RC_OK;};
		register_pending_operation(v);
		print(bufferid, "after reg message");
	}
	
	public static int init() {
		print(0, "Java Init");
		SmackConfiguration.DEBUG = true;
		try {
			// Todo: parse from cfg
			String pw = System.getenv("TRUTH_PASS");
			String jidEnv = System.getenv("TRUTH_JID");

			final String user, domain;
			if (jidEnv != null && JidUtil.isTypicalValidEntityBareJid(jidEnv)) {
				EntityBareJid jid = JidCreate.entityBareFrom(jidEnv);
				user = jid.getLocalpart().toString();
				domain = jid.getDomain().toString();
			} else {
				throw new Exception("Invalid JID specified from environment");
			}
			Server[] servers = {
					new Server(domain, user, pw, null),
			};
			
			for(Server s: servers) {
				s.connect();
			}
		} catch (Exception e) {
			printerr(0, "Java Init failed");
			return WEECHAT_RC_ERROR;
		}
		
		return WEECHAT_RC_OK;
	}

	public static void loadLibrary(String soname) {
		try {
			System.load(soname);
		} catch (Exception e) {
			print(0, "Error while loading " + soname);
		}
		print(0, "Successfully registered " + soname);
	}

	public static int command_callback(long bufferid, String cmd, String[] args) {
		Buffer b = BufferManager.getinstance().byid(bufferid);
		if (b == null) {
			// For hooked commands: legit behaviour: hooked command invoked on other (e.g. irc) buffer
			return WEECHAT_RC_OK;
		}
		return b.receiveCommand(cmd, args);
	}
	
	// Longs are native ids
	native static boolean config_boolean(long option);
	native static String config_color(long option);
	native static void config_free(long config_file);
	native static long config_get(String option_name);
	native static int config_integer(long option);
	native static long config_new(String name);
	public static int config_reload_callback(long config_file_id) {
		// TODO
		return WEECHAT_CONFIG_READ_OK;
	}
	/*
	 * weechat.config_new
	 * weechat.config_new_option
	 * weechat.config_new_section
	 * weechat.config_option_free
	 * weechat.config_option_set
	 * weechat.config_read
	 * weechat.config_reload
	 * weechat.config_search_option
	 * weechat.config_string
	 * weechat.config_write
	 * weechat.config_write_line
	 * weechat.config_write_option
	 */
}
