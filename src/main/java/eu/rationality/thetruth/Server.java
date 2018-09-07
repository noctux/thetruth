package eu.rationality.thetruth;

import java.io.IOException;
import java.util.Collection;

import org.jivesoftware.smack.ConnectionListener;
import org.jivesoftware.smack.SmackException;
import org.jivesoftware.smack.XMPPConnection;
import org.jivesoftware.smack.XMPPException;
import org.jivesoftware.smack.chat2.Chat;
import org.jivesoftware.smack.chat2.ChatManager;
import org.jivesoftware.smack.chat2.IncomingChatMessageListener;
import org.jivesoftware.smack.packet.Message;
import org.jivesoftware.smack.packet.Presence;
import org.jivesoftware.smack.roster.Roster;
import org.jivesoftware.smack.roster.RosterListener;
import org.jivesoftware.smack.tcp.XMPPTCPConnection;
import org.jivesoftware.smack.tcp.XMPPTCPConnectionConfiguration;
import org.jxmpp.jid.EntityBareJid;
import org.jxmpp.jid.Jid;
import org.jxmpp.stringprep.XmppStringprepException;

public class Server {
	private XMPPTCPConnection con;
	
	private String domain;
	private String user;
	private String password;
	private Integer port;
	private ServerBuffer serverbuffer;
	
	public Server(String domain, String user, String password, Integer port) {
		super();
		this.domain = domain;
		this.user = user;
		this.password = password;
		this.port = port;
	}

	public String getDomain() {
		return domain;
	}

	public void setDomain(String domain) {
		this.domain = domain;
	}

	public String getUser() {
		return user;
	}

	public void setUser(String user) {
		this.user = user;
	}

	public Integer getPort() {
		return port;
	}

	public void setPort(Integer port) {
		this.port = port;
	}

	public ServerBuffer getServerbuffer() {
		return serverbuffer;
	}

	public String getJID() {
		return user + "@" + domain;
	}

	private XMPPTCPConnectionConfiguration buildConfig() throws XmppStringprepException {
			var builder = XMPPTCPConnectionConfiguration.builder()
					.setXmppDomain(domain)
					.setUsernameAndPassword(user, password);
			if (port != null) {
				builder = builder.setPort(port);
			}
			return builder.build();
	}
	
	public void connect() throws Weechat.WeechatCallException, SmackException, IOException, XMPPException, InterruptedException {
		if (this.serverbuffer == null)  {
			this.serverbuffer = new ServerBuffer(this);
		}
		var conf = buildConfig();
		con = new XMPPTCPConnection(conf);
		Roster roster = Roster.getInstanceFor(con);
		roster.addRosterListener(new RosterListener() {
			
			@Override
			public void presenceChanged(Presence presence) {
				// TODO Auto-generated method stub
				
			}
			
			@Override
			public void entriesUpdated(Collection<Jid> addresses) {
				// TODO Auto-generated method stub
				
			}
			
			@Override
			public void entriesDeleted(Collection<Jid> addresses) {
				// TODO Auto-generated method stub
				
			}
			
			@Override
			public void entriesAdded(Collection<Jid> addresses) {
				// TODO Auto-generated method stub
				for(Jid j : addresses) {
					Weechat.nicklist_add_nick(serverbuffer.nativeID(), j.toString(), "", "");
				}
				
			}
		});
		serverbuffer.print("Connecting to server");
		con.addConnectionListener(new ConnectionListener() {
			@Override
			public void connectionClosedOnError(Exception e) {
				Weechat.register_pending_operation(() -> {
					serverbuffer.printErr("Connection closed on error("
				                          + e.getClass() +  "): " + e.getMessage());
					return Weechat.WEECHAT_RC_OK;
				});
			}
			
			@Override
			public void connectionClosed() {
				Weechat.register_pending_operation(() -> {
					serverbuffer.print("Connection to server closed");
					return Weechat.WEECHAT_RC_OK;
				});
				
			}
			
			@Override
			public void connected(XMPPConnection connection) {
				Weechat.register_pending_operation(() -> {
					serverbuffer.print("Connection to server " + connection.getHost()
					                  +  " on port " + connection.getPort() + " succeeded");
					return Weechat.WEECHAT_RC_OK;
				});
				
			}
			
			@Override
			public void authenticated(XMPPConnection connection, boolean resumed) {
				Weechat.register_pending_operation(() -> {
					serverbuffer.print("Authenticated as " + connection.getUser().toString());
					return Weechat.WEECHAT_RC_OK;
				});
			}
		});
		ChatManager.getInstanceFor(con).addIncomingListener(new IncomingChatMessageListener() {
			
			@Override
			public void newIncomingMessage(EntityBareJid from, Message message, Chat chat) {
				Weechat.register_pending_operation(() -> {
					serverbuffer.printMsgDateTags(System.currentTimeMillis() / 1000L, from.toString(), message.getBody(), "notify_private,log1");
					return Weechat.WEECHAT_RC_OK;
				});
				
			}
		});
		con.connect().login();
	}

	public void disconnect() {
		if (con.isConnected()) {
			con.disconnect();
		}
	}
}
