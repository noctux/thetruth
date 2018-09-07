import java.io.*;
import java.util.Arrays;
import java.util.stream.Stream;

class Main {
	static final int WEECHAT_RC_OK     =  0;
	static final int WEECHAT_RC_OK_EAT =  1;
	static final int WEECHAT_RC_ERROR  = -1;

	// Write to buffer wiht native bufferid
	native static void print(long bufferid, String str);
	// Write to buffer wiht native bufferid with the specified weechat prefix
	native static void print_prefix(long bufferid, String prefix, String str);
	// Create a named buffer returning the native buffer id
	native static long buffer_new(String name);
	// Set a property for buffer bufferid
	native static void buffer_set(long bufferid, String property, String value);
	// Callback for input received
	public static int buffer_input_callback(long bufferid, String data) {
		Main.print(0, "Echo " + data);
		return WEECHAT_RC_OK;
	}
	// Callback for buffer close events
	public static int buffer_close_callback(long bufferid) {
		Main.print(0, "Buffer " + bufferid + " closed");
		return WEECHAT_RC_OK;
	}

	// Nicklist related
	native static long nicklist_add_nick(long bufferid, String nick, String color, String prefix);
	native static void nicklist_remove_nick(long bufferid, long nickid);
	native static void nicklist_remove_all(long bufferid);

	public static void printerr(long bufferid, String str) {
		Main.print_prefix(bufferid, "error", str);
	}

	public static void test(int a) {
		Main.print(0, "Test " + a);
		Main.printerr(0, "This is an example error");
		try {
			Main.print(0, Integer.toString(a));
		} catch (Exception e) {
			System.err.println("Exception occured: " + e);
		}
		Main.print(0, "Afternative " + a);
		long bufferid = buffer_new("test");
		buffer_set(bufferid, "display", "auto");
		buffer_set(bufferid, "title", "jabber test buffer");
		buffer_set(bufferid, "nicklist", "1");
		//long nickid = nicklist_add_nick(bufferid, "hugo", "blue", "@+");
		Main.print(bufferid, "Hello world to buffer " + bufferid);
		//Main.print(bufferid, "added nick with id " + nickid);
	}

	public static void loadLibrary(String soname) {
		try {
			System.load(soname);
		} catch (Exception e) {
			Main.print(0, "Error while loading " + soname);
		}
		Main.print(0, "Successfully registered " + soname);
	}

	public static int command_callback(long bufferid, String cmd, String[] args) {
		String reconstructed = Arrays.stream(args).reduce("/" + cmd, (a, b) -> a + " " + b);
		Main.print(0, reconstructed);
		return WEECHAT_RC_OK;
	}
}
