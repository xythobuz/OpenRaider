/*! \file include/Light.h
 * \brief The GL light class
 *
 * \author Mongoose
 */

#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <MatMath.h>

class Light {
public:
    typedef enum {
        typePoint = 1,
        typeSpot = 2,
        typeDirectional = 3
    } LightType;

    // These aren't used anywhere? -- xythobuz
    //float mAmbient[4]; //! Ambient color
    //float mDiffuse[4]; //! Diffuse color
    //float mSpecular[4]; //! Specular color

    vec4_t mPos; //! Light position in 3 space
    vec3_t mDir; //! Light direction
    float mAtt;
    vec4_t mColor; //! Color of light
    vec_t mCutoff; //! Fade out distance
    LightType mType; //! Type of light
};

#endif
