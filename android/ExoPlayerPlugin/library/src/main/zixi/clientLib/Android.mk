LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := zixi_player_client

$(info zixiClientStatic for $(TARGET_ARCH) $(APP_STL))

ifeq ($(TARGET_ARCH),arm)
ifeq ($(APP_STL),stlport_static)
LOCAL_SRC_FILES := libzixiClient_static_android_stlport.a
else
LOCAL_SRC_FILES := libzixiClient_static_android_libstdc++.a
endif
endif

ifeq ($(TARGET_ARCH),arm64)
ifeq ($(APP_STL),stlport_static)
LOCAL_SRC_FILES := libzixiClient_static_android_arm64_stlport.a
else
LOCAL_SRC_FILES := libzixiClient_static_android_arm64_libstdc++.a
endif
endif

ifeq ($(TARGET_ARCH),x86)
ifeq ($(APP_STL),stlport_static)
LOCAL_SRC_FILES := libzixiClient_static_android_x86_stlport.a
else
LOCAL_SRC_FILES := libzixiClient_static_android_x86_libstdc++.a
endif
endif

include $(PREBUILT_STATIC_LIBRARY)
