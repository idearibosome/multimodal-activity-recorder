package kr.ac.yonsei.mcml.mmrclient;

import android.app.Application;
import android.content.ComponentName;
import android.content.ServiceConnection;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;

import org.qtproject.qt5.android.bindings.QtApplication;

public class MainApplication extends QtApplication {
	
	public AndroidService androidService = null;
	public boolean isAndroidServiceConnected = false;
	public String pendingOpenExternalFile = null;
	
	public ServiceConnection AndroidServiceConnection = new ServiceConnection() {
		@Override
		public void onServiceConnected(ComponentName componentName, IBinder iBinder) {
			androidService = ((AndroidService.AndroidServiceBinder) iBinder).getService();
			isAndroidServiceConnected = true;
			
			androidService.initService();
		}

		@Override
		public void onServiceDisconnected(ComponentName componentName) {
			isAndroidServiceConnected = false;
			androidService = null;
		}
	};
}
