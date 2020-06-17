//
// Created by zwt on 2020/6/15.
//

#ifndef SOFT_RASTERIZATION_SRC_MISC_GEOMETRY_UTILS_H_
#define SOFT_RASTERIZATION_SRC_MISC_GEOMETRY_UTILS_H_

#include <geometry/geometry.h>

namespace geo_utils
{
    BoundingBox get_bounding_box(Vertex a, Vertex b, Vertex c)
    {
        float x0 = std::min(a.x, b.x);
        x0 = std::min(x0, c.x);

        float y0 = std::min(a.y, b.y);
        y0 = std::min(y0, c.y);

        float x1 = std::max(a.x, b.x);
        x1 = std::max(x1, c.x);

        float y1 = std::max(a.y, b.y);
        y1 = std::max(y1, c.y);

        return BoundingBox(x0, y0, x1, y1);
    }

    Eigen::Matrix4d get_camera_mat(const Eigen::Vector3d &pos, const Eigen::Vector3d &target, const Eigen::Vector3d &up)
    {
        auto zaxis = (pos - target).normalized();
        auto xaxis = up.cross(zaxis).normalized();
        auto yaxis = zaxis.cross(xaxis).normalized();

        Eigen::Matrix4d mat;
        mat<< xaxis.x, xaxis.y, xaxis.z, -xaxis.dot(pos),
              yaxis.x, yaxis.y, yaxis.z, -yaxis.dot(pos),
              zaxis.x, zaxis.y, zaxis.z, -zaxis.dot(pos),
              0, 0, 0, 1;

        return mat;
    }

    Eigen::Matrix4d get_project_mat(float near, float far, float fov, float aspect)
    {
        Eigen::Matrix4d mat;
        // double tan_half_fov = s
        // mat<<
        return mat;
    }
} // namespace geo_utils

#endif //SOFT_RASTERIZATION_SRC_MISC_GEOMETRY_UTILS_H_
