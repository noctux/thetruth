package eu.rationality.thetruth;

import java.lang.reflect.InvocationHandler;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.lang.reflect.Proxy;

public class WeechatDelayedExectorInvocationHandler implements InvocationHandler {
	private Object wrapped;
	
	private WeechatDelayedExectorInvocationHandler(Object wrapped) {
		this.wrapped = wrapped;
	}

	@Override
	public Object invoke(Object obj, Method method, Object[] args) throws Throwable {
		// https://docs.oracle.com/javase/10/docs/api/java/lang/reflect/Proxy.html:
		// An invocation of the hashCode, equals, or toString methods declared in
		// java.lang.Object on a proxy instance will be encoded and dispatched to the
		// invocation handler's invoke method in the same manner as interface method
		// invocations are encoded and dispatched, as described above. The declaring
		// class of the Method object passed to invoke will be java.lang.Object.
		if (method.getDeclaringClass().equals(Object.class)) {
			return method.invoke(wrapped, args);
		}
		Weechat.register_pending_operation(() -> {
			try {
				Object ret = method.invoke(wrapped, args);
				// Most but not all callbacks will return an Weechat status code
				try {
					return (Integer) ret;
				} catch (ClassCastException cce) {
					return Weechat.WEECHAT_RC_OK;
				}
			} catch (IllegalAccessException | IllegalArgumentException
					| InvocationTargetException e) {
				// TODO: You most certainly do not swallow 'e' here as
				// it may result in extended debugging sessions :) Why not
				// printErr() it? Ulitmately consider switching to
				// java.util.logging (JUL).
				return Weechat.WEECHAT_RC_ERROR;
			}
		});
		return null;
	}
	
	public static Object createProxy(Object obj, Class<?>[] interfaces) {
		return Proxy.newProxyInstance(obj.getClass().getClassLoader(), interfaces, new WeechatDelayedExectorInvocationHandler(obj));
	}

}
