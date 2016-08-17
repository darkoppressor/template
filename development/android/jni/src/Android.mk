LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
PRIVATE_APP_STL := $(APP_STL)
PRIVATE_APP_STL := $(PRIVATE_APP_STL:_shared=)
PRIVATE_APP_STL := $(PRIVATE_APP_STL:_static=)
LOCAL_MODULE := libgpg-1
LOCAL_SRC_FILES := $(LOCAL_PATH)/../$(TARGET_ARCH_ABI)/libgpg.a
LOCAL_STATIC_LIBRARIES := $(APP_STL)
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../include/gpg
LOCAL_EXPORT_LDLIBS := -lz -latomic -llog
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := SDL2_static
LOCAL_SRC_FILES := $(LOCAL_PATH)/../$(TARGET_ARCH_ABI)/libSDL2.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../include/SDL2/include
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := SDL2_image_static
LOCAL_SRC_FILES := $(LOCAL_PATH)/../$(TARGET_ARCH_ABI)/libSDL2_image.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../include/SDL2_image
LOCAL_STATIC_LIBRARIES := SDL2_static
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := SDL2_mixer_static
LOCAL_SRC_FILES := $(LOCAL_PATH)/../$(TARGET_ARCH_ABI)/libSDL2_mixer.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../include/SDL2_mixer
LOCAL_STATIC_LIBRARIES := SDL2_static
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := RakNet_static
LOCAL_SRC_FILES := $(LOCAL_PATH)/../$(TARGET_ARCH_ABI)/libRakNet.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../include/RakNet/..
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := Cheese-Engine
LOCAL_SRC_FILES := $(LOCAL_PATH)/../$(TARGET_ARCH_ABI)/libCheese-Engine.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../include/cheese-engine
LOCAL_STATIC_LIBRARIES := SDL2_static SDL2_image_static SDL2_mixer_static RakNet_static
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := main

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../include/boost

# Add your application source files here...
LOCAL_SRC_FILES := $(wildcard $(LOCAL_PATH)/../../../../*.cpp)

LOCAL_STATIC_LIBRARIES := libgpg-1 SDL2_static SDL2_image_static SDL2_mixer_static RakNet_static Cheese-Engine

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog -lz -landroid

include $(BUILD_SHARED_LIBRARY)
