package onairsdktester.zixi.com.zixionairsdktester;

import android.os.Build;
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
import com.feeder.zixi.zixisdkcore.ZixiFileUploadSettings;
import com.zixi.onairsdk.ZixiConnectionStatistics;
import com.zixi.onairsdk.ZixiOnAirSdk;
import com.zixi.onairsdk.ZixiRtmpStatistics;
import com.zixi.onairsdk.camera.ZixiCameraPreview;
import com.zixi.onairsdk.events.ZixiLogEvents;
import com.zixi.onairsdk.events.ZixiOnAirEncodedFramesEvents;
import com.zixi.onairsdk.events.ZixiOnAirStatusEvents;
import com.zixi.onairsdk.settings.FileUploadSettings;
import com.zixi.onairsdk.settings.ProtocolSettings;
import com.zixi.onairsdk.settings.RtmpSettings;
import com.zixi.onairsdk.settings.VideoSettings;
import com.zixi.onairsdk.settings.ZixiSettings;

public class MainActivity extends AppCompatActivity {
    public static final String TAG = "ZixiOnAirSDKTester";
    private static final String BROADCASTER_CHANNEL_NAME = "";
    private static final String BROADCASTER_HOST_NAME = "";
    private static final String RTMP_STREAM_NAME = "";
    private static final String RTMP_URL = "";

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


        // Called whenever video encoder bitrate changes (adaptive = true)
        @Override
        public void zixiOnAirVideoEncoderBitrateSet(int set_bitrate, int requested_bitrate) {
            Log.e(TAG,String.format("zixiOnAirVideoEncoderBitrateSet: %d Quality: %.02f",set_bitrate, ((float)set_bitrate/requested_bitrate) * 100));
        }

        // New events

        /**
         * Store and forward file has finished write on disk, and now continues to be sent over net
         * @param file_size     - final file size on the device
         */
        @Override
        public void zixiOnAirFileFinalized( long file_size) {

        }

        /**
         * File upload has finished (store and forward/file transfer)
         * @param transmitted - in bytes, sent in net
         */
        @Override
        public void zixiFileTransferComplete( long transmitted) {

        }

        /**
         * connection has been restored
         */
        @Override
        public void zixiOnAirReconnected() {

        }

        /**
         * lost connection to the server
         */
        @Override
        public void zixiOnAirConnectivityLost() {

        }
        // New events end
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


                handleDeviceSpecificInitialization();
                mZixiReady = true;
                if (mUiCreated) {
                    ZixiOnAirSdk.getInstance().setCameraView(mCameraSurface.getHolder());
                }
            } else {
                setStatus("Failed to init sdk, missing permissions");
            }
        }
    });

    // Call after ZixiOnAirSdk.initInstance returned ok
    private void handleDeviceSpecificInitialization() {
        int [][] cameraMaxResolution  =   getDeviceSpecificCameraConstraints();
        int [] cameraOrientation    =   getDeviceSpecificCameraOrientations();
        int    encoderOrientation   =   getDeviceSpecificEncoderOrientations();

        ZixiCameraPreview.setDeviceSpecificData(cameraMaxResolution,cameraOrientation,encoderOrientation);
    }

    // Returns amount of degrees to rotate the input of the video encoder
    // if (Ugoos...) than rotate 180, else dont
    private int getDeviceSpecificEncoderOrientations() {
        if ( Build.MODEL.compareToIgnoreCase("UGOOS-UT3Plus") == 0) {
            return  180;
        }
        return 0;
    }

    // Rotate raw camera sensor array
    // if nexus5 we want to rotate the back camera in 180 degrees
    //      return[0] -> back camera (rotate 180)
    //      return[1] -> front camrea (dont rotate)
    // else dont rotate anything
    private int[] getDeviceSpecificCameraOrientations() {
        if ( Build.MODEL.compareToIgnoreCase("Nexus 5X") == 0) {
            return new int[]{180,0};
        }
        return null;
    }

    // Do not ask the camera preview to be greater than the resolution
    // return[0]x return[1]
    // in this case
    // if (Ugoos...)
    //  dont use any source resolution greater than 1280x720

    private int[][] getDeviceSpecificCameraConstraints() {
        if ( Build.MODEL.compareToIgnoreCase("UGOOS-UT3Plus") == 0) {
            return new int[][] {{1280, 720}};
        }
        return null;
    }


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
                // ZixiOnAirSdk.getInstance().cameraViewChanged(width,height,getResources().getConfiguration().orientation);
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
        /*if (mZixiReady) {
            ZixiOnAirSdk.getInstance().onPause();
        }*/
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
                            ZixiOnAirSdk.getInstance().manualFocus(x,y);

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
                    ZixiOnAirSdk.getInstance().zoom(zoomValue);

                    // to clear manual focus
                    // ZixiOnAirSdk.getInstance().autoFocus();
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
        if (ZixiOnAirSdk.getInstance() != null && ZixiOnAirSdk.getInstance().haveMoreThanOneCamera()) {
            Log.e(TAG,"toggleCamera");
            ZixiOnAirSdk.getInstance().switchCamera();
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

                // File upload - for file transfer application must have read permissions of the file
                //               for store and forward the application must also have write permissions
                //                  to save the data.
                // settings.server.fileSettings = new FileUploadSettings();
                // settings.server.fileSettings.storeAndForward = true/false; true  -> store and forward
                //                                                            false -> file transfer
                // settings.server.fileSettings.localFileName = <FULL_FILE_PATH>;
                // settings.server.fileSettings.overwrite = true/false; true -> if bx have a file named like localFileName it will be overwritten
                //                                                      false -> if bx have a file named like localFileName connect will fail
                // settings.server.fileSettings.remoteLocation = <PATH> ; where to store the uploaded file (path)
                ZixiOnAirSdk.getInstance().startStreamingWithSettings(settings);

                // In case of store and forward, when wishing to cause the sdk to start cleaning up the
                // file and stopping the recording call
                // ZixiOnAirSdk.getInstance().finalizeFile();
                // when the ZixiOnAirStatusEvents.zixiOnAirFileFinalized event will be called,
                // its after all the sources have been stopped, encoders wrapped, and the file has
                // been stored. When ZixiOnAirStatusEvents.zixiFileTransferComplete event is fired.
                // All data has also been sent.

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
