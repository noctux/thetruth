package eu.rationality.thetruth;

import java.util.Arrays;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.function.Supplier;
import java.util.*;

class Weechat {
	@SuppressWarnings("serial")
	static class WeechatCallException extends Exception {
	};
	
	static final int WEECHAT_RC_OK     =  0;
	static final int WEECHAT_RC_OK_EAT =  1;
	static final int WEECHAT_RC_ERROR  = -1;
	
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
		print(0, "Echo " + data);
		return WEECHAT_RC_OK;
	}
	// Callback for buffer close events
	public static int buffer_close_callback(long bufferid) {
		print(0, "Buffer " + bufferid + " closed");
		return WEECHAT_RC_OK;
	}

	// Nicklist related
	native static long nicklist_add_nick(long bufferid, String nick, String color, String prefix);
	native static void nicklist_remove_nick(long bufferid, long nickid);
	native static void nicklist_remove_all(long bufferid);

	public static void printerr(long bufferid, String str) {
		print_prefix(bufferid, "error", str);
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
		try {
			// Todo: parse from cfg
			String pw = System.getenv("TRUTH_PASS");
			Server[] servers = {
					new Server("dismail.de", "thetruthtest", pw, null)
			};
			
			for(Server s: servers) {
				s.connect();
			}
		} catch (Exception e) {
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
		String reconstructed = Arrays.stream(args).reduce("/" + cmd, (a, b) -> a + " " + b);
		print(0, reconstructed);
		return WEECHAT_RC_OK;
	}
}