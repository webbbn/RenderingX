package constantin.renderingx.core;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.graphics.ColorMatrixColorFilter;
import android.os.Build;
import android.os.PowerManager;
import android.util.Log;
import android.view.Display;
import android.view.LayoutInflater;
import android.view.View;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.ImageButton;
import android.widget.Toast;

import androidx.lifecycle.Lifecycle;
import androidx.lifecycle.LifecycleObserver;
import androidx.lifecycle.LifecycleOwner;
import androidx.lifecycle.OnLifecycleEvent;

import com.google.vr.cardboard.DisplaySynchronizer;
import com.google.vr.ndk.base.GvrApi;
import com.google.vr.ndk.base.GvrUiLayout;
import com.google.vr.sdk.base.AndroidCompat;

import static android.content.Context.POWER_SERVICE;

//The GvrLayout does not allow users to create a 'normal' context when selected headset==Daydream
//Simple workaround, you can use this as a drop-in replacement of GvrLayout when using your own presentationView anyway
//(E.g. using GLSurfaceView and custom renderer)
//TODO what is the problem with DisplaySynchronizer ? It runs okay on FPV_VR_OS, but not in RenderingXExample

//Uses LifecycleObserver to handle resume, pause and destroy
//(On GvrLayout you had to call them manually from your activity)

//No default constructor because we explicitly need the LifecycleOwner !
@SuppressLint("ViewConstructor")
public class MyVRLayout extends FrameLayout implements LifecycleObserver {
    private static final String TAG="MyVRLayout";

    private GvrApi gvrApi;
    private DisplaySynchronizer displaySynchronizer;

    public <T extends Activity & LifecycleOwner> MyVRLayout(final T parent) {
        super(parent);
        init(false);
        parent.getLifecycle().addObserver(this);
    }

    public <T extends Activity & LifecycleOwner> MyVRLayout(final T parent,final boolean createDisplaySynchronizer) {
        super(parent);
        init(createDisplaySynchronizer);
        parent.getLifecycle().addObserver(this);
    }


    private void init(final boolean createDisplaySynchronizer){
        LayoutInflater.from(getContext()).inflate(R.layout.my_vr_layout, this, true);
        final Activity activity=(Activity)getContext();
        final Display display=activity.getWindowManager().getDefaultDisplay();
        displaySynchronizer=null;
        if(createDisplaySynchronizer){
            final Context context=activity;
            displaySynchronizer=new DisplaySynchronizer(activity,display);
        }
        gvrApi=new GvrApi(getContext(),displaySynchronizer);
        findViewById(R.id.vr_overlay_settings_button).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                GvrUiLayout.launchOrInstallGvrApp(activity);
                activity.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(activity,"VR Headset changes require an activity restart",Toast.LENGTH_LONG).show();
                    }
                });
            }
        });
        findViewById(R.id.vr_overlay_back_button).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                activity.finish();
            }
        });
        //In VR, always enable sustained performance if possible
        enableSustainedPerformanceIfPossible(activity);
        //Dim the screen to n percent TODO
        //Enable Immersive mode
        FullscreenHelper.enableImmersiveSticky(activity);
        //Keep screen on
        activity.getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        //Invert color of drawable
       final float[] NEGATIVE = {
                -1.0f,     0,     0,    0, 255, // red
                0, -1.0f,     0,    0, 255, // green
                0,     0, -1.0f,    0, 255, // blue
                0,     0,     0, 1.0f,   0  // alpha
        };
       //This button allows re-centering of head tracking
       final ImageButton recenter_button=((ImageButton)findViewById(R.id.vr_overlay_recenter_button));
       recenter_button.getDrawable().setColorFilter(new ColorMatrixColorFilter(NEGATIVE));
       recenter_button.setOnClickListener(new OnClickListener() {
           @Override
           public void onClick(View v) {
               gvrApi.recenterTracking();
           }
       });
    }

    //Disable / enable the UI overlay with settings button and seperator line
    public void setVrOverlayEnabled(boolean enabled){
        final int wantedVisibility=enabled ? VISIBLE : INVISIBLE;
        findViewById(R.id.vr_overlay_seperator).setVisibility(wantedVisibility);
        findViewById(R.id.vr_overlay_back_button).setVisibility(wantedVisibility);
        findViewById(R.id.vr_overlay_settings_button).setVisibility(wantedVisibility);
        findViewById(R.id.vr_overlay_recenter_button).setVisibility(wantedVisibility);
    }

    public GvrApi getGvrApi(){
        return gvrApi;
    }

    public void setPresentationView(View presentationView){
        addView(presentationView,0);
    }

    @OnLifecycleEvent(Lifecycle.Event.ON_RESUME)
    protected void resumeX() {
        gvrApi.resumeTracking();
        if(displaySynchronizer!=null)displaySynchronizer.onResume();
        enableSustainedPerformanceIfPossible((Activity)getContext());
    }

    @OnLifecycleEvent(Lifecycle.Event.ON_PAUSE)
    protected void pauseX(){
        gvrApi.pauseTracking();
        if(displaySynchronizer!=null)displaySynchronizer.onPause();
        disableSustainedPerformanceIfEnabled((Activity)getContext());
    }

    @OnLifecycleEvent(Lifecycle.Event.ON_DESTROY)
    protected void destroyX(){
        gvrApi.shutdown();
        if(displaySynchronizer!=null)displaySynchronizer.shutdown();
    }

    public static void enableSustainedPerformanceIfPossible(Activity c){
        if (Build.VERSION.SDK_INT >= 24) {
            final PowerManager powerManager = (PowerManager)c.getSystemService(POWER_SERVICE);
            if(powerManager!=null){
                if (powerManager.isSustainedPerformanceModeSupported()) {
                    //slightly lower, but sustainable clock speeds
                    //I also enable this mode (if the device supports it) when not doing front buffer rendering,
                    //because when the user decides to render at 120fps or more (disable vsync/60fpsCap)
                    //the App benefits from sustained performance, too
                    AndroidCompat.setSustainedPerformanceMode(c,true);
                    Log.d(TAG,"Sustained performance successfully set");
                }else{
                    Log.d(TAG,"Sustained performance not available");
                }
            }
        }
    }

    public static void disableSustainedPerformanceIfEnabled(Activity c){
        if (Build.VERSION.SDK_INT >= 24) {
            final PowerManager powerManager = (PowerManager)c.getSystemService(POWER_SERVICE);
            if(powerManager!=null){
                if (powerManager.isSustainedPerformanceModeSupported()) {
                    AndroidCompat.setSustainedPerformanceMode(c, false);
                }
            }
        }
    }

    //VR Mode crashes, also I am not sure what it does for non-daydream devices
    public static void enableAndroidVRModeIfPossible(Activity c){
        if(Build.VERSION.SDK_INT>=24){
            boolean succ= AndroidCompat.setVrModeEnabled(c,true);
            if(!succ){
                Log.d(TAG,"Cannot enable vr mode");
            }
        }
    }
    public static void disableAndroidVRModeIfEnabled(Activity c){
        if (Build.VERSION.SDK_INT >= 24) {
            AndroidCompat.setVrModeEnabled(c,false);
        }
    }

}