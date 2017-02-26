APP_OPTIM:= release
NDK_TOOLCHAIN_VERSION := 4.9
APP_PLATFORM := android-23
APP_CFLAGS =

APP_STL = c++_shared

APP_ABI = armeabi-v7a arm64-v8a

APP_CPPFLAGS := -D_GLIBCXX__PTHREADS
APP_CPPFLAGS += -DBOOST_ASIO_DISABLE_EPOLL -DBOOST_THREAD_USE_LIB
APP_CPPFLAGS += -DANDROID -D__ANDROID__  -D_REENTRANT
APP_CPPFLAGS += -DZIXI_BOOST_NO_SSM

#android libs
APP_LDLIBS := -ldl -llog -lc -Wl,-shared,-Bsymbolic -Wl,--gc-sections

ZIXI_INTERNAL_MODULES += zixi_player_client

APP_MODULES = jwrapper_exo $(ZIXI_INTERNAL_MODULES)

ANDROID_PATH = $(call my-dir)
ANDROID_PATH := $(abspath $(ANDROID_PATH))

# APP_MODULES is cleared by ANDROID internal makefiles, so pass on MY_APP_MODULES instead
MY_APP_MODULES := $(APP_MODULES)
