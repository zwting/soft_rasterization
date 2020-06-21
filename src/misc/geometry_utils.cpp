//
// Created by zwt on 2020/6/17.
//

#include "geometry_utils.h"

namespace geo_utils
{

    BoundingBox get_bounding_box(Vertex a, Vertex b, Vertex c)
    {
        float x0 = std::min(std::min(a.x, b.x), c.x);
        float y0 = std::min(std::min(a.y, b.y), c.y);
        float x1 = std::max(std::max(a.x, b.x), c.x);
        float y1 = std::max(std::max(a.y, b.y), c.y);
        return BoundingBox(x0, y0, x1, y1);
    }

    Eigen::Matrix4d get_camera_mat(const Eigen::Vector3d& pos, const Eigen::Vector3d& target, const Eigen::Vector3d& up)
    {
        auto zaxis = (pos - target).normalized();
        auto xaxis = up.cross(zaxis).normalized();
        auto yaxis = zaxis.cross(xaxis).normalized();

        Eigen::Matrix4d mat;
        mat << xaxis(0), xaxis(1), xaxis(2), -xaxis.dot(pos),
            yaxis(0), yaxis(1), yaxis(2), -yaxis.dot(pos),
            zaxis(0), zaxis(1), zaxis(2), -zaxis.dot(pos),
            0, 0, 0, 1;

        return mat;
    }

    Eigen::Matrix4d get_project_mat(float near, float far, float fov, float aspect)
    {
        Eigen::Matrix4d mat;
        float radians = math_utils::Degree2Radians(fov * 0.5f);
        auto tan_half_fov = std::tan(radians);
        mat(0, 0) = 1.0 / (aspect * tan_half_fov);
        mat(1, 1) = 1.0 / tan_half_fov;
        mat(2, 2) = -(far + near) / (far - near);
        mat(2, 3) = -2 * far * near / (far - near);
        mat(3, 2) = -1;
        return mat;
    }
}
