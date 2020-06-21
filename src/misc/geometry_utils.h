//
// Created by zwt on 2020/6/15.
//

#ifndef SOFT_RASTERIZATION_SRC_MISC_GEOMETRY_UTILS_H_
#define SOFT_RASTERIZATION_SRC_MISC_GEOMETRY_UTILS_H_

#include <geometry/geometry.h>
#include "math_utils.h"

namespace geo_utils
{
    BoundingBox get_bounding_box(Vertex a, Vertex b, Vertex c);

    Eigen::Matrix4d get_camera_mat(const Eigen::Vector3d& pos, const Eigen::Vector3d& target, const Eigen::Vector3d& up);

    Eigen::Matrix4d get_project_mat(float near, float far, float fov, float aspect);

} // namespace geo_utils

#endif //SOFT_RASTERIZATION_SRC_MISC_GEOMETRY_UTILS_H_
