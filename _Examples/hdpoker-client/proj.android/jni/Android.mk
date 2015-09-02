LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos)
$(call import-add-path, $(LOCAL_PATH))

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes $(LOCAL_PATH)/../../lib/boost_1_58_0/include
LOCAL_WHOLE_STATIC_LIBRARIES := PluginFacebook sdkbox

LOCAL_CPPFLAGS += -std=c++11

FILE_LIST := $(wildcard $(LOCAL_PATH)/../../Classes/*.cpp) $(wildcard $(LOCAL_PATH)/../../Classes/easings/*.cpp)

LOCAL_SRC_FILES := hellocpp/main.cpp $(FILE_LIST:$(LOCAL_PATH)/%=%)

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END

LOCAL_LDLIBS += -L$(LOCAL_PATH)/../../lib/boost_1_58_0/lib/linux -lboost_system -lboost_thread -lgnustl_static -landroid -llog

LOCAL_STATIC_LIBRARIES := cocos2dx_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)
$(call import-module, ./sdkbox)
$(call import-module, ./pluginfacebook)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
