package kr.ac.yonsei.mcml.mmrclient;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.Build;
import android.os.Handler;
import android.os.IBinder;
import android.support.v4.app.NotificationCompat;

public class AndroidService extends Service {
    
  public class AndroidServiceBinder extends Binder {
    public AndroidService getService() {
      return AndroidService.this;
    }
  }
  private final IBinder binder = new AndroidServiceBinder();
  
  private NotificationManager notificationManager;
  private NotificationCompat.Builder notificationCompatBuilder;
  private int notificationID = 94251136;
  private String notificationChannelID = "mmrclient_service";
  private PendingIntent notificationIntent;
  private Notification notification;
  
  private Handler mainHandler;


  @Override
  public IBinder onBind(Intent intent) {
    return binder;
  }

  @Override
  public void onCreate() {
    super.onCreate();
    
    this.mainHandler = new Handler();
  }

  public void initService() {
    Intent mainIndent = new Intent(this, MainActivity.class);
    mainIndent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_SINGLE_TOP);
    this.notificationIntent = PendingIntent.getActivity(this, 0, mainIndent, PendingIntent.FLAG_UPDATE_CURRENT);

    this.notificationCompatBuilder = new NotificationCompat.Builder(this)
      .setSmallIcon(R.drawable.statusbar_logo)
      .setContentTitle("MMRClient")
      .setContentText("Running")
      .setPriority(NotificationCompat.PRIORITY_DEFAULT)
      .setContentIntent(notificationIntent)
      .setAutoCancel(false);
    
    this.notificationManager = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);
        
    // Oreo: notification channel
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
      NotificationChannel channel = new NotificationChannel(notificationChannelID, "Default", NotificationManager.IMPORTANCE_LOW);
      channel.enableLights(false);
      channel.enableVibration(false);
      channel.setSound(null, null);
      channel.setShowBadge(false);
      this.notificationManager.createNotificationChannel(channel);
    }

    this.notification = this.notificationCompatBuilder.build();
    
    showNotification();
  }

  public void destroyService() {
    stopForeground(true);
  }

  public void showNotification() {
    startForeground(notificationID, notification);
  }
}




