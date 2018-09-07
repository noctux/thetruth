package eu.rationality.thetruth;

import eu.rationality.thetruth.Weechat.WeechatCallException;

public class ServerBuffer extends Buffer {
	private static Server server;
	
	public ServerBuffer(Server server) throws WeechatCallException {
		super(server.getJID());
		Weechat.buffer_set(nativeid, "title", "Account: " + server.getJID());
		Weechat.buffer_set(nativeid, "nicklist", "1");
		Weechat.buffer_set(nativeid, "display", "auto");
		this.server = server;
	}
	
	public void closeCallback() {
		super.closeCallback();
		server.disconnect();
	}
}
