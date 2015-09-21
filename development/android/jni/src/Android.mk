LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := SDL2
LOCAL_SRC_FILES := $(LOCAL_PATH)/../$(TARGET_ARCH_ABI)/libSDL2.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../include/SDL2/include
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := SDL2_image
LOCAL_SRC_FILES := $(LOCAL_PATH)/../$(TARGET_ARCH_ABI)/libSDL2_image.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../include/SDL2_image
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := SDL2_mixer
LOCAL_SRC_FILES := $(LOCAL_PATH)/../$(TARGET_ARCH_ABI)/libSDL2_mixer.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../include/SDL2_mixer
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := RakNet
LOCAL_SRC_FILES := $(LOCAL_PATH)/../$(TARGET_ARCH_ABI)/libRakNet.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../include/RakNet/..
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := Cheese-Engine
LOCAL_SRC_FILES := $(LOCAL_PATH)/../$(TARGET_ARCH_ABI)/libCheese-Engine.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../include/cheese-engine
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := main

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../include/boost

# Add your application source files here...
LOCAL_SRC_FILES := $(wildcard $(LOCAL_PATH)/../../../../*.cpp)

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image SDL2_mixer RakNet Cheese-Engine

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)
