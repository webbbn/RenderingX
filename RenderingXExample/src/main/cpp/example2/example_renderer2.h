//
// Created by Consti10 on 08/10/2019.
//

#ifndef RENDERINGX_EXAMPLE_RENDERER2_H
#define RENDERINGX_EXAMPLE_RENDERER2_H

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <jni.h>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <GLProgramVC.h>
#include <GLProgramText.h>
#include <GLProgramTexture.h>
#include <Helper/ColoredGeometry.hpp>
#include <Helper/TexturedGeometry.hpp>
#include <TextAssetsHelper.hpp>

#include <GLProgramLine.h>
#include <Chronometer.h>
#include <FPSCalculator.h>

#include "vr/gvr/capi/include/gvr.h"
#include "vr/gvr/capi/include/gvr_types.h"

class ExampleRenderer2{
public:
   ExampleRenderer2(JNIEnv* env,jobject androidContext,gvr_context *gvr_context,jfloatArray undistData);
    void onSurfaceCreated(JNIEnv* env,jobject context);
    void onSurfaceChanged(int width, int height);
    void onDrawFrame();
    void drawEye(bool whichEye);
private:
    std::unique_ptr<gvr::GvrApi> gvr_api_;
    //Camera/Projection matrix constants
    static constexpr float MAX_Z_DISTANCE=5.0f;
    static constexpr float MIN_Z_DISTANCE=0.001f;
    static constexpr float CAMERA_POSITION=0.7f;
    static constexpr float TEXT_Y_OFFSET=-1.0f;

//the projection matrix. Should be re calculated in each onSurfaceChanged with new width and height
    glm::mat4x4 projection;
//the view matrix, without any IPD
    glm::mat4x4 eyeView,leftEyeView,rightEyeView;
//used to render vertex-color geometry
    GLProgramVC* glProgramVC;
    GLProgramVC* glProgramVC2;
    //
    GLProgramTexture* glProgramTexture;

//holds colored geometry vertices
    GLuint glBufferVC;

    GLuint glBufferVCDistorted1;
    GLuint glBufferVCDistorted2;

    GLuint glBufferTextured;
    GLuint glBufferTextured1;
    GLuint glBufferTextured2;
    GLuint glBufferTexturedIndices;

    GLuint glBufferCoordinateSystemLines;
    DistortionManager* distortionManager;

//simplifies debugging/benchmarking
    Chronometer cpuFrameTime{"CPU frame time"};
    FPSCalculator fpsCalculator{"OpenGL FPS",2000}; //print every 2 seconds

    static constexpr float VR_InterpupilaryDistance=0.2f;

    static constexpr int TESSELATION=6;
    static constexpr int N_COLORED_VERTICES=6*(TESSELATION+1)*(TESSELATION+1);
    static constexpr int N_TEXTURED_VERTICES=6*11*11;
    int ViewPortW=0,ViewPortH=0;

    static constexpr int TEXTURE_TESSELATION_FACTOR=30;
    static constexpr int N_TEXTURED_INDICES=6*TEXTURE_TESSELATION_FACTOR*TEXTURE_TESSELATION_FACTOR;
};

#endif //RENDERINGX_EXAMPLE_RENDERER2_H
