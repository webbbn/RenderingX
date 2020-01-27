//
// Created by Consti10 on 12/10/2019.
//

#ifndef RENDERINGX_NDKHELPER_H
#define RENDERINGX_NDKHELPER_H

#include <jni.h>
#include <string>

//links to @Java / NDKHelper

class NDKHelper {
private:
    static constexpr const auto CLASS_NAME="constantin/renderingx/core/NDKHelper";
public:
    //documentation @Java/NDKHelper/uploadAssetImageToGPU()
    static void uploadAssetImageToGPU(JNIEnv *env, jobject androidContext,const char* filename,const bool extractAlpha){
        jclass NDKHelper_ = env->FindClass(CLASS_NAME);
        jstring filenameDistanceField_=env->NewStringUTF(filename);
        jmethodID uploadAssetImageToGPU_=env->GetStaticMethodID(
                NDKHelper_,"uploadAssetImageToGPU", "(Landroid/content/Context;Ljava/lang/String;Z)V");
        env->CallStaticVoidMethod(NDKHelper_, uploadAssetImageToGPU_,androidContext,filenameDistanceField_,(jboolean)extractAlpha);
        env->DeleteLocalRef(NDKHelper_);
    }
    //documentation @Java/NDKHelper/getFloatArrayFromAssets()
    static void getFloatArrayFromAssets(JNIEnv *env, jobject androidContext,const char* filename,float* array,const int arrayLength){
        jclass NDKHelper_ = env->FindClass(CLASS_NAME);
        jfloatArray aFontWidthsU=env->NewFloatArray(arrayLength);
        jstring filenameFontWidthsU_=env->NewStringUTF(filename);
        jmethodID getFloatArrayFromAssets_=env->GetStaticMethodID(
                NDKHelper_,"getFloatArrayFromAssets", "(Landroid/content/Context;Ljava/lang/String;[F)V");
        env->CallStaticVoidMethod(NDKHelper_,getFloatArrayFromAssets_,androidContext,filenameFontWidthsU_,aFontWidthsU);
        jfloat *arrayP=env->GetFloatArrayElements(aFontWidthsU,nullptr);
        std::memcpy(array,arrayP,(size_t)arrayLength*sizeof(float));
        env->ReleaseFloatArrayElements(aFontWidthsU,arrayP,0);
        env->DeleteLocalRef(NDKHelper_);
    }
};


#endif //RENDERINGX_NDKHELPER_H
