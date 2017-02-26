//
// Created by roy on 21-Feb-16.
//

#include <jni.h>
#include <android/log.h>
#include "zixi_context.h"

#include <sys/system_properties.h>
static int isArtEnabled() {
    char buf[PROP_VALUE_MAX] = {};
    __system_property_get("persist.sys.dalvik.vm.lib.2", buf);
    // This allows libartd.so to be detected as well.
    return (strncmp("libart", buf, 6) == 0)?1:0;
}

#define running_under_art isArtEnabled()

ZIXI_DEFINE_APP(zixi_app_client_interface);
unsigned long long g_protocol_iv;


inline int getReleaseBufferFlag() {
    return running_under_art? JNI_COMMIT : JNI_ABORT;
}

#define LOGE(TAG,...) __android_log_print(ANDROID_LOG_ERROR  , TAG,__VA_ARGS__)


static void log (void *user_data , int level, const char *msg) {
    LOGE("ZIXI_NATIVE", "%s", msg);
}
// com.google.android.exoplayer2.upstream.ZixiDataSourceImpl
// Java_com_google_android_exoplayer2_upstream_ZixiDataSourceImpl
JNIEXPORT int Java_com_google_android_exoplayer2_upstream_ZixiDataSourceImpl_nativeZixiConnect(
    JNIEnv * env,
    jobject thiz,
    jlongArray jCtxOut,
    jstring     jUrl,
    jstring     jDeviceId) {

    zixi_client_configure_logging(4, log,(void*)0);
    LOGE("ZixiJni", "About to connect");

    const char* url = (*env)->GetStringUTFChars(env,jUrl,0);
    const char * device_id = (*env)->GetStringUTFChars(env,jDeviceId,0);
    jlong * out_ptr = (*env)->GetLongArrayElements(env,jCtxOut,0);
    LOGE("ZixiJni", "Creating context");
    void * ptr = createContext();
    LOGE("ZixiJni", "Calling Connect");
    int ret = connectContext(ptr, url,device_id,(void*)0,200);
    LOGE("ZixiJni", "Connect returned %d",ret);
    *out_ptr = 0;
    *out_ptr = *((jlong*)&ptr);
    (*env)->ReleaseLongArrayElements(env, jCtxOut, out_ptr,getReleaseBufferFlag());
    (*env)->ReleaseStringUTFChars(env, jDeviceId, device_id);
    (*env)->ReleaseStringUTFChars(env, jUrl,url);
    return ret;
}
JNIEXPORT int Java_com_google_android_exoplayer2_upstream_ZixiDataSourceImpl_nativeZixiDisconnect(
    JNIEnv * env,
    jobject thiz,
    jlong   jCtx) {

    disconnectContext((void*)jCtx);
    deleteContext((void*)jCtx);
    return 0;
}

JNIEXPORT int Java_com_google_android_exoplayer2_upstream_ZixiDataSourceImpl_nativeZixiRead(
    JNIEnv * env,
    jobject thiz,
    jlong jCtx,
    jbyteArray jOutBuffer,
    jint        jOffset,
    jint        jReadLen,
    jintArray   jOutRead) {
    int r = 0;
   /* if (jOutBuffer != NULL && jOutReadt != NULL) {*/
        jbyte *out_buffer = (*env)->GetByteArrayElements(env, jOutBuffer, 0);
        jint *out_read = (*env)->GetIntArrayElements(env, jOutRead, 0);
        r= readContext((void *) jCtx, out_buffer, jOffset, jReadLen, (int *) out_read);
        (*env)->ReleaseByteArrayElements(env, jOutBuffer, out_buffer, getReleaseBufferFlag());
        (*env)->ReleaseIntArrayElements(env, jOutRead, out_read, getReleaseBufferFlag());
    /*}*/
    return r;
}

JNIEXPORT int Java_com_google_android_exoplayer2_upstream_ZixiDataSourceImpl_nativeZixiGetStatistics(
    JNIEnv * env,
    jobject thiz,
    jlong   jCtx,
    jlongArray jOutData) {

    jlong * out_data = (*env)->GetLongArrayElements(env,jOutData,0);
    int ret = getContextStatistics((void*)jCtx,out_data);
    (*env)->ReleaseLongArrayElements(env,jOutData,out_data,getReleaseBufferFlag());
    return ret;
}
JNIEXPORT int Java_com_google_android_exoplayer2_upstream_ZixiDataSourceImpl_nativeZixiGetStatisticsArrSize(
    JNIEnv * env,
    jobject thiz) {
    return getStatisticsArraySize();
}

jint JNI_OnLoad(JavaVM* vm, void * reserved) {
    LOGE("JNI","JNI_OnLoad");
    return JNI_VERSION_1_6;
}

void JNI_OnUnload(JavaVM *vm, void *reserved){
    LOGE("JNI","JNI_OnUnload");
}