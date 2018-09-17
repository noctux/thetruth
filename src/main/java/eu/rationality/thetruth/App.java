package eu.rationality.thetruth;

import org.jivesoftware.smack.*;
import org.jivesoftware.smack.tcp.*;
import org.jxmpp.jid.EntityBareJid;
import org.jxmpp.jid.impl.JidCreate;
import org.jivesoftware.smack.chat2.*;
import org.jivesoftware.smack.packet.Message;
import org.jivesoftware.smack.roster.Roster;

/**
 * Hello world!
 *
 */
public class App
{
    public static void main( String[] args )
    {
		try {
			AbstractXMPPConnection connection = new XMPPTCPConnection("thetruthtest", System.getenv("TRUTH_PASS"), "dismail.de");
			connection.connect().login();
			ChatManager chatManager = ChatManager.getInstanceFor(connection);
			chatManager.addIncomingListener(new IncomingChatMessageListener() {
				public void newIncomingMessage(EntityBareJid from, Message message, Chat chat) {
					System.out.println("Received message from " + from + ": " + message);
				}
			});
			// TODO: Use JidCreate.entityBareFrom() instead of from()
			EntityBareJid jid = JidCreate.from("noctux@jabber.ccc.de").asEntityBareJidIfPossible();
			Chat chat = chatManager.chatWith(jid);
			chat.send("HEY, what's up?");
			
			var roster = Roster.getInstanceFor(connection);
			// TODO: The roster should already be loaded if I'm not mistaken, so no need to call reloadAndWait()
			roster.reloadAndWait();
			System.out.println("Entries: " + roster.getEntries().size());
			roster.getEntries().stream().forEach(x -> System.out.println(x.getName() + " " + x.getJid() + " " + x.toString()));
			
			while(true) { Thread.sleep(10); }
			//connection.disconnect();
		} catch (Exception e) {
			System.err.println("An error occured" + e);
		};
        System.out.println( "Hello World!" );
    }
}
