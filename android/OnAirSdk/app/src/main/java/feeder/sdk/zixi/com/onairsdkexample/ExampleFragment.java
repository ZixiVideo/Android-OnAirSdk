package feeder.sdk.zixi.com.onairsdkexample;

import android.app.Fragment;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.Toast;

import com.feeder.zixi.zixisdkcore.ZixiAudioEncoderParameters;
import com.feeder.zixi.zixisdkcore.ZixiBonding;
import com.feeder.zixi.zixisdkcore.ZixiConnectionSettings;
import com.feeder.zixi.zixisdkcore.ZixiConnectionStatus;
import com.feeder.zixi.zixisdkcore.ZixiEncryptionType;
import com.feeder.zixi.zixisdkcore.ZixiError;
import com.feeder.zixi.zixisdkcore.ZixiFileTransferParameters;
import com.feeder.zixi.zixisdkcore.ZixiLogLevel;
import com.feeder.zixi.zixisdkcore.ZixiRecordingSession;
import com.feeder.zixi.zixisdkcore.ZixiRtmpSettings;
import com.feeder.zixi.zixisdkcore.ZixiSdk;
import com.feeder.zixi.zixisdkcore.ZixiVideoEncoderParameters;
import com.feeder.zixi.zixisdkcore.interfaces.IZixiLogEvents;
import com.feeder.zixi.zixisdkcore.interfaces.IZixiSdkEvents;

import feeder.zixi.com.zixionairsdkapi.ZixiOnAirFragment;
import feeder.zixi.com.zixionairsdkapi.camera.CameraType;
import feeder.zixi.com.zixionairsdkapi.camera.handlers.CameraHandler;
import feeder.zixi.com.zixionairsdkapi.encoders.parameters.AudioEncoderParameters;
import feeder.zixi.com.zixionairsdkapi.encoders.parameters.VideoEncoderParameters;

/**
 * Created by Zixi on 20-Dec-15.
 */
public class ExampleFragment extends Fragment implements IZixiLogEvents, IZixiSdkEvents {
    private final static String TAG = ExampleFragment.class.getSimpleName();
    private ZixiOnAirFragment mOnAirFragment;
    private final static ZixiVideoEncoderParameters VIDEO_ENCODER_PARAMETERS;
    private final static ZixiAudioEncoderParameters AUDIO_ENCODER_PARAMETERS;
    private final static ZixiConnectionSettings ZIXI_CONNECTION_SETTINGS;
    private final static ZixiFileTransferParameters FILE_TRANSFER_PARAMETERS;
    private final static ZixiRtmpSettings       ZIXI_RTMP_SETTINGS;

    static {
        VIDEO_ENCODER_PARAMETERS = new ZixiVideoEncoderParameters();
        AUDIO_ENCODER_PARAMETERS = new ZixiAudioEncoderParameters();
        ZIXI_CONNECTION_SETTINGS = new ZixiConnectionSettings();
        ZIXI_RTMP_SETTINGS = new ZixiRtmpSettings();
        FILE_TRANSFER_PARAMETERS = new ZixiFileTransferParameters();
        initializeSettings();

    }

    private boolean mFlashOn = false;
    private boolean mFileNotFinalized;

    private static void initializeSettings() {
        VIDEO_ENCODER_PARAMETERS.intervalBetweenKeyFrames = 2;
        VIDEO_ENCODER_PARAMETERS.bitrate = 3000000;
        VIDEO_ENCODER_PARAMETERS.height = 720;
        VIDEO_ENCODER_PARAMETERS.width = 1280;
        VIDEO_ENCODER_PARAMETERS.frameRate = 30;

        AUDIO_ENCODER_PARAMETERS.sampleRate = 48000;
        AUDIO_ENCODER_PARAMETERS.channels = 1;
        AUDIO_ENCODER_PARAMETERS.bitrate = 128000;

        ZIXI_CONNECTION_SETTINGS.hostName = "10.7.0.210";
        ZIXI_CONNECTION_SETTINGS.port = 2088;
        ZIXI_CONNECTION_SETTINGS.enableEncoderFeedback = true;
        ZIXI_CONNECTION_SETTINGS.encryptionType = ZixiEncryptionType.ZIXI_NO_ENCRYPTION;
        ZIXI_CONNECTION_SETTINGS.password = null;
        ZIXI_CONNECTION_SETTINGS.maxLatencyMs = 3000;
        ZIXI_CONNECTION_SETTINGS.streamId = "iphone";

        ZIXI_RTMP_SETTINGS.streamName = "iguru";
        ZIXI_RTMP_SETTINGS.rtmpUrl = "rtmp://10.7.0.210/live";
        ZIXI_RTMP_SETTINGS.password= null;
        ZIXI_RTMP_SETTINGS.userName = null;

        FILE_TRANSFER_PARAMETERS.targetLocation = "test" + System.currentTimeMillis() + ".ts";
        FILE_TRANSFER_PARAMETERS.overwriteTarget = true;
        FILE_TRANSFER_PARAMETERS.transsmitionBitrate = 30000000;
        FILE_TRANSFER_PARAMETERS.fileLocation = "/sdcard/Zixi/zixi.ts";


    }

    private void runToast(final String msg) {
        if (getActivity() != null) {
            getActivity().runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    Toast.makeText(getActivity(), msg, Toast.LENGTH_SHORT).show();
                }
            });
        }
    }

    private Button mToggleConnection;
    private Button mToggleCamera;
    private Button mToggleFlash;
    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View ret = inflater.inflate(R.layout.fragment_example,container,false);
        mOnAirFragment = new ZixiOnAirFragment();
        getActivity().getFragmentManager().beginTransaction().replace(R.id.example_on_air_fragment_container, mOnAirFragment).commit();
        mToggleConnection = (Button)ret.findViewById(R.id.example_toggle_connection);
        mToggleCamera = (Button)ret.findViewById(R.id.example_toggle_camera);
        mToggleFlash = (Button)ret.findViewById(R.id.example_toggle_flash);
        if (mOnAirFragment.setEncoderParameters(VIDEO_ENCODER_PARAMETERS,null) != ZixiError.ZIXI_ERROR_OK) {

        }
        mOnAirFragment.setLoggingCallback(this);

        mOnAirFragment.connectToCamera(CameraType.CT_BACK);
        mToggleFlash.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mFlashOn = !mFlashOn;
                mOnAirFragment.toggleFlash(mFlashOn);
            }
        });
        if (mOnAirFragment.getConnectionStatus() == ZixiConnectionStatus.ZIXI_CONNECTED){
            mToggleConnection.setText("Disconnect");
        } else {
            mToggleConnection.setText("Connect");
        }

        mToggleCamera.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CameraType cameraType = mOnAirFragment.getCameraType();
                if (cameraType == CameraType.CT_BACK) {
                    cameraType = CameraType.CT_FRONT;
                } else {
                    cameraType = CameraType.CT_BACK;
                }
                mOnAirFragment.connectToCamera(cameraType);
            }
        });
        mToggleConnection.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mOnAirFragment.getConnectionStatus() == ZixiConnectionStatus.ZIXI_CONNECTED) {
                    if (!mFileNotFinalized) {
                        mToggleConnection.setText("Connect");
                        mOnAirFragment.disconnect();
                    } else {
                        mOnAirFragment.finalizeFileTransfer();
                        mToggleConnection.setText("Disconnect");
                    }
                } else {
                    mOnAirFragment.setMaxLoggingLevel(ZixiLogLevel.ERROR.getId());
                    mOnAirFragment.setLoggingCallback(ExampleFragment.this);
                    mToggleConnection.setText("Disconnect");

                    // Zixi connection
                    // doZixiConnection();

                    // Zixi connection + auto RTMP out
                    // doZixiConnectionRtmpOut();

                    // RTMP connection
                    // doRtmpConnection();

                    // Zixi connection + Store and forward
             //       doStoreAndForward();

                    // Zixi connection for file transfer
                    doFileTransfer();

                }
                mToggleConnection.setEnabled(false);
            }
        });

        mOnAirFragment.setLoggingCallback(this);
        mOnAirFragment.setEventsHandler(this);
        return ret;
    }
    private void doFileTransfer(){
        mOnAirFragment.connectUploadFile(ZIXI_CONNECTION_SETTINGS, FILE_TRANSFER_PARAMETERS);
    }
    private void doStoreAndForward() {
        mOnAirFragment.connectStoreAndForward(ZIXI_CONNECTION_SETTINGS, FILE_TRANSFER_PARAMETERS);
        mFileNotFinalized = true;
        mToggleConnection.setText("Close file");
    }

    private void doRtmpConnection(){
        mOnAirFragment.connectRtmp(ZIXI_RTMP_SETTINGS);
        mFileNotFinalized = false;
    }
    private void doZixiConnectionRtmpOut() {
        mOnAirFragment.connectWithAutoRtmp(ZIXI_CONNECTION_SETTINGS,ZIXI_RTMP_SETTINGS);
        mFileNotFinalized = false;
    }
    private void doZixiConnection() {
        mOnAirFragment.connect(ZIXI_CONNECTION_SETTINGS);
        mFileNotFinalized = false;
    }

    @Override
    public void onLogMessage(int zixiLogLevel, String msg) {
        Log.println(ZixiLogLevel.fromInt(zixiLogLevel).toAndroidLogLevel(), TAG, msg);
    }

    @Override
    public void onConnected() {
        runToast("connected");
        getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mToggleConnection.setEnabled(true);
            }
        });
    }

    @Override
    public void onFailedToConnect(int zixiError) {
        runToast("failedToConnect " + ZixiError.fromInt(zixiError).name());
        getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mToggleConnection.setText("Connect");
                mToggleConnection.setEnabled(true);
            }
        });
    }

    @Override
    public void onDisconnected() {
        runToast("disconnected");
        getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mToggleConnection.setText("Connect");
                mToggleConnection.setEnabled(true);
            }
        });
    }

    @Override
    public void onEncoderBitrateChanged(int newBitrate) {

    }

    @Override
    public void onRecordingStarted() {
        runToast("recording started");
    }

    @Override
    public void onStartedReconnect() {
        runToast("started auto-reconnect");
    }

    @Override
    public void onConnectionRestored() {
        runToast("connection restored");
    }

    @Override
    public void onFailedToConnectToCamera() {
        runToast("failed to connect to camera");
    }

    @Override
    public void onFailedToStartRecorder() {
        runToast("failed to start audio recorder");
    }

    @Override
    public void onRecordingEnded(ZixiRecordingSession zixiRecordingSession) {
        runToast("recording ended " + (zixiRecordingSession.currentDuration / 1000)  + " seconds");
    }

    @Override
    public void onFileTransferStarted() {
        runToast("file transfer started");
    }

    @Override
    public void onFileTransferPaused() {
        runToast("file transfer paused");
    }

    @Override
    public void onFileTransferResumed() {
        runToast("file transfer resumed");
    }

    @Override
    public void onFileTransferCompleted() {
        runToast("file transfer completed");
        getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mToggleConnection.setEnabled(true);
            }
        });
    }

    @Override
    public void onFileTransferFailed(int zixiError) {
        runToast("file transfer error " + ZixiError.fromInt(zixiError).name());
        getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mToggleConnection.setEnabled(true);
            }
        });
    }
}
