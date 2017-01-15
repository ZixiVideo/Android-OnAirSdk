package onairsdktester.zixi.com.zixionairsdktester;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.SeekBar;
import android.widget.TextView;

import com.feeder.zixi.zixisdkcore.ZixiErrors;
import com.zixi.onairsdk.ZixiConnectionStatistics;
import com.zixi.onairsdk.ZixiOnAirSdk;
import com.zixi.onairsdk.ZixiRtmpStatistics;
import com.zixi.onairsdk.events.ZixiLogEvents;
import com.zixi.onairsdk.events.ZixiOnAirEncodedFramesEvents;
import com.zixi.onairsdk.events.ZixiOnAirStatusEvents;
import com.zixi.onairsdk.settings.ProtocolSettings;
import com.zixi.onairsdk.settings.RtmpSettings;
import com.zixi.onairsdk.settings.VideoSettings;
import com.zixi.onairsdk.settings.ZixiSettings;

public class MainActivity extends AppCompatActivity {
    public static final String TAG = "ZixiOnAirSDKTester";

    // Ui Properties
    private SurfaceView mCameraSurface;
    private TextView mStatusText;
    private SeekBar mSetZoom;

    private SingleTapConfirm mTapDetector = new SingleTapConfirm();

    private class SingleTapConfirm extends GestureDetector.SimpleOnGestureListener {

        @Override
        public boolean onSingleTapUp(MotionEvent event) {
            return true;
        }
    }

    // Init states
    private boolean mUiCreated = false;
    private boolean mUiGotMeasures = false;
    private boolean mZixiReady = false;

    // Toggle between setting mode 1 and 2
    private boolean mSettingsToggle = false;

    // Full screen mode or not
    private boolean mFullScreen = false;

    // Totals of this run
    private long mEncodedVideo =0 ;  // Encoded video frames
    private long mEncodedAudio = 0;  // Encoded audio frames
    private long mTotalAudio = 0;    // total video bitstream size in bytes
    private long mTotalVideo = 0;   // total audio bitstream size in bytes

    // Ui view measures
    private int mSurfaceW;
    private int mSurfaceH;


    // Log callback
    private ZixiLogEvents mLogCallback = new ZixiLogEvents() {
        @Override
        public void logMessage(int level, String who, String what) {
            Log.println(level,who,what);
        }
    };

    // Sdk connectivity events
    private ZixiOnAirStatusEvents mOnAirCallbacks = new ZixiOnAirStatusEvents() {
        @Override
        public void zixiOnAirWillStart() {
            setStatus("zixiOnAirWillStart");
        }

        @Override
        public void zixiOnAirDidStart(){ setStatus("zixiOnAirDidStart");}

        @Override
        public void zixiOnAirFailedToStart(int error) {
            setStatus("zixiOnAirFailedToStart " + error + " - " + ZixiErrors.toString(error));
        }

        @Override
        public void zixiOnAirDidFinish(int error) {
            setStatus("zixiOnAirDidFinish");
            mSettingsToggle = !mSettingsToggle;
        }

        @Override
        public void zixiOnAirStatistics(Object o) {
            if (o instanceof ZixiConnectionStatistics) {
                ZixiConnectionStatistics statistics = (ZixiConnectionStatistics)o;
                Log.e(TAG,"zixiOnAirStatistics zixi" + statistics.bitrate);
            } else if (o instanceof ZixiRtmpStatistics) {
                ZixiRtmpStatistics r = (ZixiRtmpStatistics)o;
                Log.e(TAG,"zixiOnAirStatistics rtmp" + r.bitrate);
            } else
                Log.e(TAG,"zixiOnAirStatistics");
        }

        @Override
        public void zixiOnAirCaptureInfo(int width, int height, float fps) {
            Log.e(TAG,"zixiOnAirCaptureInfo [" + width + "x" + height + "]@" +String.format("%.02f",fps));
        }
    };

    // Encoded frames callbacks
    private ZixiOnAirEncodedFramesEvents mEncodedFramesCallbacks = new ZixiOnAirEncodedFramesEvents() {
        @Override
        public void onEncodedAudio(byte[] data, int size, long pts) {
            mEncodedAudio++;
            mTotalAudio+=size;
            if ((mEncodedAudio % 100) == 0) {
                Log.e(TAG,"Total encoded audio frames " + mEncodedAudio + " " + mTotalAudio + " bytes");
            }
        }

        @Override
        public void onEncodedVideo(byte[] data, int size, long pts, long dts) {
            mEncodedVideo++;
            mTotalVideo+=size;

            if ((mEncodedVideo % 100) == 0) {
                Log.e(TAG,"Total encoded video frames " + mEncodedVideo + " " + mTotalVideo + " bytes");
            }
        }
    };
    private Thread mStartZixiThread = new Thread(new Runnable() {
        @Override
        public void run() {
            if (ZixiOnAirSdk.initInstance(MainActivity.this) != null) {
                ZixiOnAirSdk.getInstance().setLogCallback(mLogCallback);
                ZixiOnAirSdk.getInstance().setStatusEventsHandler(mOnAirCallbacks);
                ZixiOnAirSdk.getInstance().setEncodedFramesEventsHandler(mEncodedFramesCallbacks);
                mZixiReady = true;
                if (mUiCreated) {
                    ZixiOnAirSdk.getInstance().setCameraView(mCameraSurface.getHolder());
                }
                if (mUiGotMeasures) {
                    int r = getResources().getConfiguration().orientation;
                    if (r == 0) {
                        r = 2;
                    } else if (r == 1) {
                        r = 3;
                    } else if (r == 2) {
                        r = 0;
                    } else {
                        r = 1;
                    }
                    ZixiOnAirSdk.getInstance().cameraViewChanged(mSurfaceW,mSurfaceH,r);
                }
            } else {
                setStatus("Failed to init sdk, missing permissions");
            }
        }
    });


    private void setStatus(final String status) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (mStatusText != null) {
                    mStatusText.setText(status);
                    Log.e(TAG,"zixiOnAirWillStart");
                }
            }
        });
    }
    private SurfaceHolder.Callback mSurfaceCallbacks = new SurfaceHolder.Callback() {
        @Override
        public void surfaceCreated(SurfaceHolder holder) {
            if (mZixiReady){
                ZixiOnAirSdk.getInstance().setCameraView(mCameraSurface.getHolder());
            } else {
                mUiCreated = true;
            }
        }

        @Override
        public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
            mSurfaceH = height;
            mSurfaceW = width;
            if (mZixiReady) {
                Log.e(TAG,"surfaceChanged ["  + width + "x" + height + "] " + getResources().getConfiguration().orientation);
                ZixiOnAirSdk.getInstance().cameraViewChanged(width,height,getResources().getConfiguration().orientation);
            } else {
                mUiGotMeasures = true;
            }
        }

        @Override
        public void surfaceDestroyed(SurfaceHolder holder) {
            mUiCreated = false;
            mUiGotMeasures = false;
            if (mZixiReady) {
                ZixiOnAirSdk.getInstance().setCameraView(null);
            }
        }
    };


    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (mZixiReady && ZixiOnAirSdk.getInstance().connected()) {
            ZixiOnAirSdk.getInstance().stopStreaming();
        }
        if (mZixiReady) {
            ZixiOnAirSdk.getInstance().onPause();
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mStartZixiThread.start();
        mCameraSurface = (SurfaceView)findViewById(R.id.camera_surface);
        mCameraSurface.getHolder().addCallback(mSurfaceCallbacks);
        mStatusText = (TextView) findViewById(R.id.status_text);
        mSetZoom = (SeekBar)findViewById(R.id.main_set_zoom);

        /*((LinearLayout)findViewById(R.id.test))*/mCameraSurface.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {

                if (mTapDetector.onSingleTapUp(event) ) {
                    if (event.getAction() == MotionEvent.ACTION_UP) {
                        float x = event.getX();
                        float y = event.getY();
                        if (mZixiReady)
                            ZixiOnAirSdk.getInstance().cameraPreview().setManualFocusPoint(x,y);

                        Log.i(TAG, "Touch [" + x + "x" + y + "]");
                    }
                    return true;
                }
                return false;
            }
        });
        mSetZoom.setMax(100);
        mSetZoom.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                // zoom between 1 and 6;
                float zoomValue =1.0f+ ((float)progress/20);
                Log.e(TAG,"set zoom " + zoomValue);
                if (mZixiReady) {
                    ZixiOnAirSdk.getInstance().cameraPreview().setZoom(zoomValue);
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
    }

    public void toggleCameraClicked(View view) {
        if (ZixiOnAirSdk.getInstance() != null && ZixiOnAirSdk.getInstance().cameraPreview() != null) {
            Log.e(TAG,"toggleCamera");
            ZixiOnAirSdk.getInstance().cameraPreview().switchCamera();
        }
    }

    // Called from "CONNECT" button
    public void toggleOnClick(View view) {
        if (ZixiOnAirSdk.getInstance().canChangeConnectivity()) {
            if (ZixiOnAirSdk.getInstance().connected()){
                ZixiOnAirSdk.getInstance().stopStreaming();
            } else {
                ZixiSettings settings = new ZixiSettings();
                settings.server.rtmpFwd = null;
                settings.server.channelName = BROADCASTER_CHANNEL_NAME;
                settings.server.hostName = BROADCASTER_HOST_NAME;
                settings.protocol.protocolId = ProtocolSettings.PROTCOL_ZIXI;
                settings.server.bonding = false;
                settings.server.rtmpFwd = new RtmpSettings();
                settings.server.rtmpFwd.streamName = RTMP_STREAM_NAME;
                settings.server.rtmpFwd.URL = RTMP_URL;
                settings.server.rtmpFwd.username = "";
                settings.server.rtmpFwd.password = "";


                /* RTMP CONNECTION
                settings.protocol.protocolId = ProtocolSettings.PROTCOL_RTMP;
                settings.rtmp.streamName = RTMP_STREAM_NAME;
                settings.rtmp.URL = RTMP_URL;
                settings.rtmp.password = "";
                settings.rtmp.username = "";
                 */
                if (mSettingsToggle) {
                    settings.video.frameSizePreset = VideoSettings.ZixiFrameSizePreset1280x720;
                }    else {
                    settings.video.frameSizePreset = VideoSettings.ZixiFrameSizePreset640x360;
                }

                ZixiOnAirSdk.getInstance().startStreamingWithSettings(settings,this);
            }
        }
        Log.e("Activity","toggleOnClick - done");
    }

    public void toggleFullScreen(View view) {
        mFullScreen = !mFullScreen;
        int options = 5894;
        if (mFullScreen) {
            getWindow().getDecorView().setSystemUiVisibility(options);
        } else {
            getWindow().getDecorView().setSystemUiVisibility( 0);
        }
    }
}
