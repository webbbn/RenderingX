package constantin.renderingx.example;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Spinner;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import constantin.renderingx.core.GLESInfo.AWriteGLESInfo;
import constantin.renderingx.core.GLESInfo.GLESInfo;
import constantin.renderingx.example.Mono.AExampleRendering;
import constantin.renderingx.example.StereoVR.AExampleVRRendering;
import constantin.renderingx.example.supersync.AExampleSuperSync;

public class MainActivity extends AppCompatActivity {
    private Context context;
    private static final String[] REQUIRED_PERMISSION_LIST = new String[]{
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.ACCESS_FINE_LOCATION,
    };
    private final List<String> missingPermission = new ArrayList<>();
    private static final int REQUEST_PERMISSION_CODE = 12345;

    private Spinner mSpinner;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        context=this;
        setContentView(R.layout.activity_main);
        mSpinner=findViewById(R.id.spinner_360_video_type);

        //This retreives any HW info needed for the app
        AWriteGLESInfo.writeGLESInfoIfNeeded(this);

        findViewById(R.id.button).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivity(new Intent().setClass(context, AExampleRendering.class));
            }
        });
        findViewById(R.id.button2).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(isSuperSyncSupported()){
                    startActivity(new Intent().setClass(context, AExampleSuperSync.class));
                }else{
                    Toast.makeText(context,"SuperSync is not supported on this phone",Toast.LENGTH_LONG).show();
                }
            }
        });
        findViewById(R.id.button3).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                final Intent intent=new Intent().setClass(context, AExampleVRRendering.class);
                intent.putExtra(AExampleVRRendering.KEY_SPHERE_MODE,0);
                startActivity(intent);
            }
        });
        findViewById(R.id.button4).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                final Intent intent=new Intent().setClass(context, AExampleVRRendering.class);
                final int position = mSpinner.getSelectedItemPosition()==Spinner.INVALID_POSITION ? 0 : mSpinner.getSelectedItemPosition();
                final int SPHERE_MODE;
                final String VIDEO_FILENAME;
                if(position==0){
                    SPHERE_MODE=AExampleVRRendering.SPHERE_MODE_GVR_EQUIRECTANGULAR;
                    VIDEO_FILENAME="360DegreeVideos/testRoom1_1920Mono.mp4";
                }else if(position==1){
                    SPHERE_MODE=AExampleVRRendering.SPHERE_MODE_GVR_EQUIRECTANGULAR;
                    VIDEO_FILENAME="360DegreeVideos/paris_by_diego.mp4";
                } else if(position==2){
                    SPHERE_MODE=AExampleVRRendering.SPHERE_MODE_INSTA360_TEST;
                    VIDEO_FILENAME="360DegreeVideos/insta_webbn_1_shortened.h264";
                }else{
                    SPHERE_MODE=AExampleVRRendering.SPHERE_MODE_INSTA360_TEST;
                    VIDEO_FILENAME="360DegreeVideos/insta_webbn_1_shortened.h264";
                }
                intent.putExtra(AExampleVRRendering.KEY_SPHERE_MODE,SPHERE_MODE);
                intent.putExtra(AExampleVRRendering.KEY_VIDEO_FILENAME,VIDEO_FILENAME);
                startActivity(intent);
            }
        });
        checkAndRequestPermissions();
    }

    @Override
    protected void onResume() {
        super.onResume();
    }


    private void checkAndRequestPermissions(){
        missingPermission.clear();
        for (String eachPermission : REQUIRED_PERMISSION_LIST) {
            if (ContextCompat.checkSelfPermission(this, eachPermission) != PackageManager.PERMISSION_GRANTED) {
                missingPermission.add(eachPermission);
            }
        }
        if (!missingPermission.isEmpty()) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                final String[] asArray=missingPermission.toArray(new String[0]);
                Log.d("PermissionManager","Request: "+ Arrays.toString(asArray));
                ActivityCompat.requestPermissions(this, asArray, REQUEST_PERMISSION_CODE);
            }
        }
    }
    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        // Check for granted permission and remove from missing list
        if (requestCode == REQUEST_PERMISSION_CODE) {
            for (int i = grantResults.length - 1; i >= 0; i--) {
                if (grantResults[i] == PackageManager.PERMISSION_GRANTED) {
                    missingPermission.remove(permissions[i]);
                }
            }
        }
        if (!missingPermission.isEmpty()) {
            checkAndRequestPermissions();
        }

    }

    public boolean isSuperSyncSupported(){
        return GLESInfo.isExtensionAvailable(context,GLESInfo.EGL_ANDROID_front_buffer_auto_refresh) && GLESInfo.isExtensionAvailable(context,GLESInfo.EGL_KHR_mutable_render_buffer);
    }

}
