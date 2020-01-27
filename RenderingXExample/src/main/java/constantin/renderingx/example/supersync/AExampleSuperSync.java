package constantin.renderingx.example.supersync;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;

import constantin.renderingx.core.PerformanceHelper;
import constantin.renderingx.core.ViewSuperSync;

public class AExampleSuperSync extends AppCompatActivity {
    private ViewSuperSync mViewSuperSync;
    private GLRExampleSuperSync mGLRStereoSuperSync;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mViewSuperSync=new ViewSuperSync(this);
        mGLRStereoSuperSync = new GLRExampleSuperSync(this,mViewSuperSync.getGvrApi());
        mViewSuperSync.setRenderer(mGLRStereoSuperSync);
        setContentView(mViewSuperSync);
    }

    @Override
    protected void onResume() {
        super.onResume();
        System.out.println("YYY onResume()");
        PerformanceHelper.setImmersiveSticky(this);
        PerformanceHelper.enableAndroidVRModeIfPossible(this);
        PerformanceHelper.enableSustainedPerformanceIfPossible(this);
        mViewSuperSync.resume();
    }

    @Override
    protected void onPause(){
        super.onPause();
        System.out.println("YYY onPause()");
        mViewSuperSync.pause();
        PerformanceHelper.disableSustainedPerformanceIfEnabled(this);
        PerformanceHelper.disableAndroidVRModeIfEnabled(this);
    }

    @Override
    protected void onDestroy(){
        super.onDestroy();
        mViewSuperSync.destroy();
        mViewSuperSync=null;
        mGLRStereoSuperSync=null;
    }
}
