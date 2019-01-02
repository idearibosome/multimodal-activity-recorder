package kr.ac.yonsei.mcml.mmrclient;

import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.Manifest;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.view.KeyEvent;
import android.util.Log;

import org.qtproject.qt5.android.bindings.QtActivity;

import static android.app.Activity.RESULT_OK;

public class MainActivity extends QtActivity {
  
  public MainApplication mainApplication;
  public boolean fullDestroy;
  
  private Handler mainHandler;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    fullDestroy = false;
        
    if (!isTaskRoot()) {
      final Intent intent = getIntent();
      final String intentAction = intent.getAction();
      if (intentAction != null) {
        Log.w("Loading", "There is already an intent. Finishing Main Activity instead of launching.");
        finish();
        return;
      }
    }

    super.onCreate(savedInstanceState);
    fullDestroy = true;

    createService();
  }
    
  @Override
  protected void onDestroy() {		
    if (fullDestroy) {
        
      if (mainApplication != null && mainApplication.androidService != null) {
        mainApplication.androidService.destroyService();
        unbindService(mainApplication.AndroidServiceConnection);
      }
      stopService(new Intent(this, AndroidService.class));
    }
        
    System.exit(0);
    super.onDestroy();
  }
  
  @Override
  public void onConfigurationChanged(Configuration newConfig) {
    super.onConfigurationChanged(newConfig);
  }
  
  @Override
  public boolean onKeyUp(int keycode, KeyEvent e) {
    return super.onKeyUp(keycode, e);
  }

  @Override
  public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
  }
  
  @Override
  public void onActivityResult(int requestCode, int resultCode, Intent resultData) {
  }

  public void createService() {
    mainApplication = (MainApplication) getApplication();

    startService(new Intent(this, AndroidService.class));
    bindService(new Intent(this, AndroidService.class), mainApplication.AndroidServiceConnection, Context.BIND_AUTO_CREATE);
  }
}
