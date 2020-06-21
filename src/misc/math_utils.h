//
// Created by zwt on 2020/6/15.
//

#ifndef SOFT_RASTERIZATION_SRC_MISC_MATH_UTILS_H_
#define SOFT_RASTERIZATION_SRC_MISC_MATH_UTILS_H_

namespace math_utils
{
    const double PI = 3.1415926;
    template<typename T>
    T Degree2Radians(const T degree)
    {
        return degree * PI / 180.0;
    }

    template<typename T>
    T Radians2Degree(const T radians)
    {
        return radians / PI * 180.0;
    }
}

#endif //SOFT_RASTERIZATION_SRC_MISC_MATH_UTILS_H_
