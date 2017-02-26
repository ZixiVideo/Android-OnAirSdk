package com.google.android.exoplayer2.upstream;

import android.net.Uri;

import java.io.IOException;

/**
 * Created by roy on 2/12/2017.
 */

public class ZixiDataSourceFactory extends ZixiDataSource.BaseFactory {
    private final TransferListener<? super DataSource> listener;


    public ZixiDataSourceFactory(TransferListener<? super DataSource> listener) {
        this.listener = listener;
    }
    @Override
    protected ZixiDataSource createDataSourceInternal() {
        return new ZixiDataSourceImpl(listener);
    }
}
