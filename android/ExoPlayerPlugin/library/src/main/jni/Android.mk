# ZIXI_PATH points to the root of zixi source tree

JNI_PATH := $(call my-dir)
LIB_PATH := $(call my-dir)/../zixi/clientLib

#r5 build
include $(LIB_PATH)/Android.mk


include $(JNI_PATH)/jwrapper/Android.mk

