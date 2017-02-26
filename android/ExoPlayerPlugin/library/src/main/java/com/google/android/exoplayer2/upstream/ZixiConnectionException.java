package com.google.android.exoplayer2.upstream;

import java.io.IOException;

/**
 * Created by roy on 23-Feb-16.
 */
public class ZixiConnectionException extends IOException {
    public ZixiConnectionException(int zixiRet, String url) {
        super("Failed to connect to url " + url + " : " + getErrorString(zixiRet) + " (" + zixiRet + ")");
    }

    private static String getErrorString(int zixi_ret) {
        String ret = "";
        switch (zixi_ret) {
            case 0:
                ret = "No Error";
                break;
            case -1:
                ret = "Failed";
                break;
            case -2:
                ret = "Timeout";
                break;
            case -3:
                ret = "Not initialized";
                break;
            case -4:
                ret = "Not connected";
                break;
            case -5:
                ret = ".So file not found";
                break;
            case -6:
                ret = "Function not found";
                break;
            case -7:
                ret = "Authorization failed";
                break;
            case -23:
                ret = "No source connected to channel";
                break;
        }

        return ret;

    }
}
