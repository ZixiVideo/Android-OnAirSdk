package com.google.android.exoplayer2.upstream;

import android.net.Uri;
import android.os.Build;
import android.util.Log;

import com.google.android.exoplayer2.C;

import java.io.IOException;
import java.lang.ref.WeakReference;

/**
 * Created by roy on 2/5/2017.
 */

public interface ZixiDataSource extends DataSource, IZixiStatisticsProvider {

    interface Factory extends DataSource.Factory {
        @Override
        ZixiDataSource createDataSource();
    }

    abstract class BaseFactory implements ZixiDataSource.Factory {
        public final ZixiDataSource createDataSource() {
            return createDataSourceInternal();
        }

        protected abstract ZixiDataSource createDataSourceInternal();
    }
}
