//
// Created by Consti10 on 11/12/2019.
//

#ifndef CARDBOARD_MPOLYNOMIALRADIALDISTORTION_H
#define CARDBOARD_MPOLYNOMIALRADIALDISTORTION_H

#include <array>
#include <vector>

//Based on @cardboard/PolynomialRadialDistortion

// PolynomialRadialDistortion implements a radial distortion based using
// a set of coefficients describing a polynomial function.
// See http://en.wikipedia.org/wiki/Distortion_(optics).
//
// Unless otherwise stated, the units used in this class are tan-angle units
// which can be computed as distance on the screen divided by distance from the
// virtual eye to the screen.
class PolynomialRadialDistortion {
public:
    // Construct a PolynomialRadialDistortion with coefficients for
    // the radial distortion equation:
    //
    //   p' = p (1 + K1 r^2 + K2 r^4 + ... + Kn r^(2n))
    //
    // where r is the distance in tan-angle units from the optical center,
    // p the input point and p' the output point.
    // The provided vector contains the coefficients for the even monomials
    // in the distortion equation: coefficients[0] is K1, coefficients[1] is K2,
    // etc.  Thus the polynomial used for distortion has degree
    // (2 * coefficients.size()).
    explicit PolynomialRadialDistortion(const std::vector<float>& coefficients={0,0});

    // Given a 2d point p, returns the corresponding distorted point.
    // The units of both the input and output points are tan-angle units,
    // which can be computed as the distance on the screen divided by
    // distance from the virtual eye to the screen. For both the input
    // and output points, the intersection of the optical axis of the lens
    // with the screen defines the origin, the x axis points right, and
    // the y axis points up.
    std::array<float, 2> Distort(const std::array<float, 2>& p) const;

    // Given a 2d point p, returns the point that would need to be passed to
    // Distort to get point p (approximately).
    std::array<float, 2> DistortInverse(const std::array<float, 2>& p) const;

    // Given a radius (measuring distance from the optical axis of the lens),
    // returns the distortion factor for that radius.
    float DistortionFactor(float r_squared) const;

    // Given a radius (measuring distance from the optical axis of the lens),
    // returns the corresponding distorted radius.
    float DistortRadius(float r) const;

    //Given a radius r, returns the radius that would need to be passed
    //to DistortRadius to get r (approximately).
    float DistortRadiusInverse(float r)const;

    //Convert into human-readable string for debugging
    std::string toString()const;

    //Vertex displacement distortion correction needs to obtain the coefficients since it uses
    //them in the vertex shader. Immutability of coefficients is untouched
    std::vector<float> getCoeficients()const;
private:
    //Immutable except trough constructor
    std::vector<float> coefficients_;
};


//Inverse polynomial is a close approximation of the inverse in the range [0..MAX_RAD_SQ]
class PolynomialRadialInverse:private PolynomialRadialDistortion{
public:
    //Takes a PolynomialRadialDistortion and creates the approximate inverse for the given range.
    //More coefficients do often, but not always result in a better fit
    PolynomialRadialInverse(const PolynomialRadialDistortion& parent,const float maxRad,const int NUM_COEFFICIENTS):
        PolynomialRadialDistortion(PolynomialRadialInverse::getApproximateInverseCoefficients(parent,maxRad,NUM_COEFFICIENTS)),maxRadSq(maxRad*maxRad){
    };
    //Default constructor: create a identity distortion (the inverse of 0 is also 0)
    PolynomialRadialInverse(const int NUM_COEFFICIENTS=2):PolynomialRadialDistortion(std::vector<float>((unsigned)NUM_COEFFICIENTS,0)),maxRadSq(100000){
    };
public:
    using PolynomialRadialDistortion::getCoeficients;
public:
    using PolynomialRadialDistortion::Distort;
    /*float DistortionFactor(float r_squared) const{
        if(r_squared>maxRadSq){
            r_squared=maxRadSq;
        }
        return PolynomialRadialDistortion::DistortionFactor(r_squared);
    }*/
    using PolynomialRadialDistortion::DistortionFactor;

    using PolynomialRadialDistortion::DistortRadius;
    //PolynomialRadialDistortion::DistortInverse makes no sense - we are already a inverse function
    std::string toStringX()const;
    float getMaxRadSq()const{ return maxRadSq;};
public:
    //calculate coefficients for a polynomial function that describes the inverse of this distortion
    static std::vector<float> getApproximateInverseCoefficients(const PolynomialRadialDistortion& distortion,float maxRadius, int numCoefficients);

    //The un-distortion function created by getApproximateInverseCoefficients is not a perfect fit
    //calculate the deviation between the real un-distortion value (as obtained by @DistortInverse)
    //{Note: this value is also not 'absolute true'}
    //and the value generated by the inverse polynomial distortion function
    static float calculateDeviation(const PolynomialRadialDistortion& distortion,const PolynomialRadialInverse& inverseDistortion,const float radius);

    //Given a polynomialRadialDistortion and a inverse defined in the range [0..maxRadSq]
    //Loop trough the defined range using step size and return the largest deviation
    //also see @calculateDeviation
    static float calculateMaxDeviation(const PolynomialRadialDistortion& distortion,const PolynomialRadialInverse& inverse,const float stepSize=0.01f);

    //TODO
    //Calculate RMSE (Root mean square error) of the inverse in the range [0..maxRadSq]
public:
    float maxRadSq;
};


#endif //CARDBOARD_MPOLYNOMIALRADIALDISTORTION_H