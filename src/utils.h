#ifndef __utils_h__
#define __utils_h__

#include <Eigen/Core>
#include <vector>
#include <list>
#include <set>
#include <string>
#include <cmath>

#include <stdint.h>
#include <stdio.h>

#include <sophus/se3.hpp>

namespace corvis
{
    using namespace Eigen;
    using namespace Sophus;
    using namespace std;
  
    const double EPS = 1e-8;
    const double PI = 3.14159265;

    typedef Matrix3f mat3x3;
    typedef Vector3f vec3;
    typedef Matrix4f mat4x4;
    typedef Vector4f vec4;
    
    typedef MatrixXf matx;
    typedef VectorXf vecx;
}

#endif
