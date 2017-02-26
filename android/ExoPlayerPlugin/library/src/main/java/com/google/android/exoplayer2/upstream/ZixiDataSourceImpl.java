package com.google.android.exoplayer2.upstream;

import android.net.Uri;
import android.os.Build;
import android.util.Log;

import com.google.android.exoplayer2.C;

import java.io.IOException;
import java.lang.ref.WeakReference;

/**
 * Created by roy on 2/12/2017.
 */

public class ZixiDataSourceImpl implements ZixiDataSource {
    // private final static String TAG = ZixiDataSourceImpl.class.getSimpleName();
    private final WeakReference<TransferListener<? super DataSource>> mListener;
    private DataSpec                        mDataSpec;
    private boolean                         mOpened;
    private int []                          mActualWrittenBuffer;
    private long                            mZixiHandle;
    private long[]                          mStatisticsBuffer;
    private final static String             TAG = ZixiDataSource.class.getSimpleName();
    private final static int                ZIXI_STATISTICS_ARR_SIZE;
    private Object                          mStatisticsLock;

    private static native int nativeZixiConnect(long [] outCtx,String url, String deviceId);
    private static native int nativeZixiDisconnect(long ctx);
    private static native int nativeZixiRead(long ctx, byte[] data, int offset, int readLength, int[] actual);
    private static native int nativeZixiGetStatistics(long ctx,long[] data);
    private static native int nativeZixiGetStatisticsArrSize();
    static {
        System.loadLibrary("jwrapper_exo");
        ZIXI_STATISTICS_ARR_SIZE = nativeZixiGetStatisticsArrSize();
    }

    public ZixiDataSourceImpl(TransferListener<? super DataSource> listener) {
        mListener = new WeakReference<TransferListener<? super DataSource>>(listener);
        mActualWrittenBuffer = new int[1];
        mStatisticsLock = new Object();
    }
    @Override
    public long open(DataSpec dataSpec) throws IOException {
        if (mDataSpec == null || !mOpened) {
            mDataSpec = dataSpec;
            String uri = dataSpec.uri.toString();
            Log.d(TAG, "open " + uri);
            long[] raw_ptr = new long[1];
            int zixi_ret = nativeZixiConnect(raw_ptr, uri, Build.DEVICE);
            if (zixi_ret == 0) {
                Log.d(TAG, "connected to " + uri);
                mZixiHandle = raw_ptr[0];
            } else {
                Log.d(TAG, "failed to connect to " + uri);
                throw new ZixiConnectionException(zixi_ret, uri);
            }
            mOpened = true;
            if (mListener != null && mListener.get() != null) {
                mListener.get().onTransferStart(this,mDataSpec);
            }
        } else if (mOpened) {
            Log.e(TAG,"OPEN!!!");
        }
        return C.LENGTH_UNSET;
    }

    @Override
    public int read(byte[] buffer, int offset, int readLength) throws IOException {
        int zixi_ret = nativeZixiRead(mZixiHandle,buffer,offset,readLength, mActualWrittenBuffer);
        if (zixi_ret == 0) {
            if (mListener != null && mListener.get() != null) {
                mListener.get().onBytesTransferred(this,mActualWrittenBuffer[0]);
                if (mActualWrittenBuffer[0] != readLength && mActualWrittenBuffer[0] != 0) {
                    Log.e(TAG, "requested " + readLength + " got " + mActualWrittenBuffer[0]);
                }
            }
        } else {
            throw new ZixiConnectionException(zixi_ret, mDataSpec.uri.toString());
        }
        return readLength;
    }

    @Override
    public Uri getUri() {
        return mDataSpec == null?null:mDataSpec.uri;
    }

    @Override
    public void close() throws IOException {
        Log.d(TAG,"close");
        if (mOpened) {
            synchronized (mStatisticsLock) {
                nativeZixiDisconnect(mZixiHandle);
                mZixiHandle = 0;
            }
            mOpened = false;
            if (mListener != null && mListener.get() != null) {
                mListener.get().onTransferEnd(this);
            }
        }
    }

    @Override
    public int getStatistics(ZixiStatistics zixiStatistics) {
        int ret = -3; // not initialized
        synchronized (mStatisticsLock) {
            if (mZixiHandle != 0) {
                ret = nativeZixiGetStatistics(mZixiHandle, mStatisticsBuffer);
                zixiStatistics.fromLongArray(mStatisticsBuffer);
            }
        }
        return ret;
    }
}
