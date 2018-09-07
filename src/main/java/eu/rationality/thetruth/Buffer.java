package eu.rationality.thetruth;

import java.util.Set;

import eu.rationality.thetruth.Weechat.WeechatCallException;

public abstract class Buffer {
	enum Tags {
		MUC,
		PRIVMSG,
		SELFMSG,
		ACTION,
	};
	
	protected long   nativeid;
	private String name;

	public Long nativeID() {
		return nativeid;
	}

	public void print(String str) {
		Weechat.print(nativeID(), str);
	}
	
	public void printErr(String str) {
		Weechat.print_prefix(nativeID(), "error", str);
	}
	
	public void printMsgDateTags(long time, String sender, String data,String tags) {
		Weechat.print_date_tags(nativeid, time, tags + ",nick_"+sender+",host_"+sender, data);
	}
	
	public void sendMsg() {
		
	}
	
	public int receiveCommand(String cmd, String args[]) {
		return Weechat.WEECHAT_RC_OK;
	}
	
	public int handleInput(String input) {
		print("echo: " + input);
		return Weechat.WEECHAT_RC_OK;
	}
	
	public Buffer(String name) throws WeechatCallException {
		this.name = name;
		this.nativeid = createNativeBuffer(name);
		BufferManager.getinstance().register(this);
	}
	
	private static long createNativeBuffer(String name) throws WeechatCallException {
		long nativeid = Weechat.buffer_new(name);
		if (nativeid == 0) {
			throw new Weechat.WeechatCallException();
		}
		return nativeid;
	}

	public void closeCallback() {
	}
}
