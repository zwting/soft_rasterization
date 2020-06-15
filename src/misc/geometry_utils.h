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
}

#endif //SOFT_RASTERIZATION_SRC_MISC_GEOMETRY_UTILS_H_
