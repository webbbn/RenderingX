//
// Created by Consti10 on 04/12/2019.
//

#ifndef CARDBOARD_MLENSDISTORTION_H
#define CARDBOARD_MLENSDISTORTION_H

#include <array>
#include <memory>
#include <math.h>

#include "PolynomialRadialDistortion.h"

//Based on @cardboard/LensDistortion
//CardboardLensDistortion

constexpr float kDefaultBorderSizeMeters = 0.003f;

struct MDeviceParams{
    const float screen_width_meters;
    const float screen_height_meters;
    const float screen_to_lens_distance;
    const float inter_lens_distance;
    const int vertical_alignment;
    const float tray_to_lens_distance;
    const std::array<float,4> device_fov_left;
    const std::vector<float> radial_distortion_params;
};

// All values in tanangle units.
class MLensDistortion {
public:
    struct ViewportParams{
        float width;
        float height;
        float x_eye_offset;
        float y_eye_offset;
    };
    //Same data as above, but used in a different context
    //(Normalized device coordinates for half screen viewport)
    struct ViewportParamsNDC{
        float width;
        float height;
        float x_eye_offset;
        float y_eye_offset;
    };
public:
    static float GetYEyeOffsetMeters(const int vertical_alignment,
                                     const float tray_to_lens_distance,
                                     const float screen_height_meters);
    static std::array<float, 4> CalculateFov(
            const std::array<float, 4> device_fov,
            const float GetYEyeOffsetMeters,
            const float screen_to_lens_distance, const float inter_lens_distance,
            const PolynomialRadialDistortion &distortion, float screen_width_meters,
            float screen_height_meters);
    static void CalculateViewportParameters(int eye,
                                            const float GetYEyeOffsetMeters,
                                            const float screen_to_lens_distance,
                                            const float inter_lens_distance,
                                            const std::array<float, 4> &fov,
                                            float screen_width_meters,
                                            float screen_height_meters,
                                            ViewportParams *screen_params,
                                            ViewportParams *texture_params);

    //calculates the viewport parameters to use with
    //@UndistortedNDCForDistortedNDC
    //Left eye viewport is left half of screen, right eye viewport is right half of screen
    static void CalculateViewportParameters_NDC(int eye,
                                            const float GetYEyeOffsetMeters,
                                            const float screen_to_lens_distance,
                                            const float inter_lens_distance,
                                            const std::array<float, 4> &fov,
                                            float screen_width_meters,
                                            float screen_height_meters,
                                            ViewportParams& screen_params,
                                            ViewportParams& texture_params);


    //Use isInverse==true when passing a distortion that is already a inverse
    static std::array<float, 2> UndistortedUvForDistortedUv(
            const PolynomialRadialDistortion &distortion,
            const ViewportParams &screen_params, const ViewportParams &texture_params,
            const std::array<float, 2> &in,bool isInverse=false);


    //This one takes and returns values between -1 and 1 (e.g. OpenGL viewport coordinates)
    //Viewport Parameters have to be calculated accordingly @CalculateViewportParameters_NDC
    //Uses n*x+t instead of n*x-t (compared to the cardboard original)
    static std::array<float, 2> UndistortedNDCForDistortedNDC(
            const PolynomialRadialDistortion &inverseDistortion,
            const ViewportParams &screen_params, const ViewportParams &texture_params,
            const std::array<float, 2> &in,const bool isInverse=true);

    static std::string MDeviceParamsAsString(const MDeviceParams& dp);

    static std::string ViewportParamsAsString(const ViewportParams& screen_params,const ViewportParams& texture_params);
    static std::string ViewportParamsNDCAsString(const ViewportParamsNDC& screen_params,const ViewportParamsNDC& texture_params);
};




#endif //CARDBOARD_MLENSDISTORTION_H
