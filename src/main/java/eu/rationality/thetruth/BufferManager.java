package eu.rationality.thetruth;

import java.util.HashMap;
import java.util.Map;

public class BufferManager {
	private static BufferManager instance = null;
	private Map<Long, Buffer> id2buffer;
	
	public static BufferManager getinstance() {
		if (instance == null) {
			instance = new BufferManager();
		}
		return instance;
	}
	
	private BufferManager() {
		this.id2buffer = new HashMap<>();
	}
	
	public Buffer byid(long id) {
		return id2buffer.get(id);
	}
	
	public void register(Buffer b) {
		id2buffer.put(b.nativeID(), b);
	}
	
	public void deregister(Long nativeid) {
		Buffer removed = id2buffer.remove(nativeid);
		if (removed == null) {
			Weechat.printerr(0, "Closing buffer " + Long.toHexString(nativeid)
			                    + " which is not registered with the Buffermanager");
		} else {
			removed.closeCallback();
		}
	}
}
