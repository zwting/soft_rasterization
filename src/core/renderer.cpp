#include "renderer.h"
#include <cassert>
#include <misc/geometry_utils.h>
#include <iostream>

Vertex& Renderer::get_vertex_(int idx)
{
    assert(idx >= 0 && idx < indicles_.size());
    const std::vector<int>::value_type vertex_idx = indicles_[idx];
    assert(vertex_idx >= 0 && vertex_idx < verticles_.size());
    return verticles_[vertex_idx];
}

void Renderer::mid_point_draw_line_(int x0, int y0, int x1, int y1, const Color3B& color)
{
    bool is_swap = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1))
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
        is_swap = true;
    }

    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int k = 0;
    if (std::abs(y1 - y0) > 0)
    {
        k = std::abs(y1 - y0) / (y1 - y0);
    }
    int a = y0 - y1;
    int b = x1 - x0;
    int y = y0;
    int d = a * (x0 + 1) + b * (y + k * 0.5) + x0 * y1 - x1 * y0;
    for (int x = x0; x < x1; x++)
    {
        if (is_swap)
        {
            frame_buffer_->set_pixel(x, y, color);
        }
        else
        {
            frame_buffer_->set_pixel(y, x, color);
        }
        if (d * k >= 0)
        {
            d += a;
        }
        else
        {
            d += a + b * k;
            y += k;
        }
    }
}

void Renderer::render()
{
    auto verticles = vertex_stage_();
    auto fragments = setup_primitive_and_rasterization(verticles);
    fragments = fragment_stage_(fragments);

    //深度测试
    //alpha测试
    //模板测试

    //写入frame_buffer
    for (auto frag : fragments)
    {
        Color3B c(frag.c.r, frag.c.g, frag.c.b);
        this->frame_buffer_->set_pixel(frag.x, frag.y, c);
    }
}

std::vector<Vertex> Renderer::vertex_stage_()
{
    std::vector<Vertex> ret;
    Eigen::Vector4d v;
    Eigen::Matrix4d view_port_mat;
    int half_w = frame_buffer_->get_width() / 2;
    int half_h = frame_buffer_->get_height() / 2;
    view_port_mat << half_w, 0, 0, half_w,
        0, half_h, 0, half_h,
        0, 0, 1, 0,
        0, 0, 0, 1;
    auto mvp = this->get_mvp();
    for (std::vector<Vertex>::size_type i = 0; i < indicles_.size(); ++i)
    {
        auto vertex = get_vertex_(indicles_[i]);
        v << vertex.x, vertex.y, vertex.z, 1;
        // mvp 变换
        v = mvp * v;
        //透视除法
        v(0) /= v(3);
        v(1) /= v(3);
        v(2) /= v(3);
        v(3) = 1;

        //视口变换
        v = view_port_mat * v;
        ret.push_back(Vertex(v(0), v(1), v(2), vertex.c));
    }
    return ret;
}

std::vector<Vertex> Renderer::setup_primitive_and_rasterization(std::vector<Vertex>& input_vertexs)
{
    std::vector<Vertex> ret;
    switch (primitive_type_)
    {
        case PrimitiveType::Triangle:
        {
            ret = setup_triangle(input_vertexs);
            break;
        }
    }
    return ret;
}
std::vector<Vertex> Renderer::setup_triangle(std::vector<Vertex>& input_vertex)
{
    std::vector<Vertex> ret;
    auto cross = [](const Eigen::Vector2i& a, const Eigen::Vector2i& b)
    {
      return a(0) * b(1) - a(1) * b(0);
    };
    int height = frame_buffer_->get_height();
    for (auto it = input_vertex.begin(); it != input_vertex.end(); it += 3)
    {
        BoundingBox bbox = geo_utils::get_bounding_box(*it, *(it + 1), *(it + 2));
        int xmin = std::floor(bbox.lb_point.x);
        int xmax = std::ceil(bbox.rt_point.x);
        int ymin = std::floor(bbox.lb_point.y);
        int ymax = std::ceil(bbox.rt_point.y);

        Eigen::Vector2i v0, v1, v2;
        v0 << (it->x), (it->y);
        v1 << ((it + 1)->x), ((it + 1)->y);
        v2 << ((it + 2)->x), ((it + 2)->y);

        Eigen::Vector2i v01 = v1 - v0;
        Eigen::Vector2i v12 = v2 - v1;
        Eigen::Vector2i v20 = v0 - v2;

//        std::cout << xmin << "," << ymin << "," << xmax << "," << ymax << std::endl;
        auto f = [](const Eigen::Vector2i& a, const Eigen::Vector2i& b, const int x, const int y)
        {
          int A = a(1) - b(1);
          int B = b(0) - a(0);
          return A * x + B * y + a(0) * b(1) - a(1) * b(0);
        };
        double f_alpha = f(v0, v1, v2(0), v2(1));
        double f_beta = f(v1, v2, v0(0), v0(1));
        double f_gamma = f(v2, v0, v1(0), v1(1));
        for (int y = ymin; y < ymax; y++)
        {
            for (int x = xmin; x < xmax; x++)
            {
                //1. 过滤掉不在三角形内部的点
                Eigen::Vector2i vp(x, y);
                Eigen::Vector2i v0p = vp - v0;
                Eigen::Vector2i v1p = vp - v1;
                Eigen::Vector2i v2p = vp - v2;

                if (cross(v01, v0p) < 0 || cross(v12, v1p) < 0 || cross(v20, v2p) < 0)
                {
                    continue;
                }

                //2. 计算当前片元的重心坐标
                double alpha = f(v0, v1, x, y) / f_alpha;
                double beta = f(v1, v2, x, y) / f_beta;
                double gamma = 1 - alpha - beta;

                //3. 根据重心坐标计算当前片元的颜色
                Color4B c = (it + 2)->c * alpha + (it)->c * beta + (it + 1)->c * gamma;

                //4. 结束
                ret.push_back(Vertex(x, height - y, it->z, c));
            }
        }
    }

    return ret;
}

std::vector<Vertex> Renderer::fragment_stage_(std::vector<Vertex>& input_fragments)
{
    std::vector<Vertex> ret;
    for (auto it = input_fragments.begin(); it != input_fragments.end(); it++)
    {
        ret.push_back(Vertex(it->x, it->y, it->z, it->c));
    }
    return ret;
}

Renderer::Renderer(FrameBuffer* frame_buffer, FrameBuffer* depth_buffer)
{
    assert(frame_buffer != nullptr && depth_buffer != nullptr && frame_buffer->is_size_equal(*depth_buffer));
    this->frame_buffer_ = frame_buffer;
    this->depth_buffer_ = depth_buffer;

    this->is_model_dirty_ = false;
    this->is_view_dirty_ = false;
    this->is_projection_dirty_ = false;
}

void Renderer::set_data(std::vector<Vertex>& verticles, std::vector<int>& indicles)
{
    this->verticles_ = verticles;
    this->indicles_ = indicles;
}

void Renderer::draw_point(int count)
{
}

void Renderer::draw_lines(int count)
{
}

void Renderer::draw_triangles(int count)
{
    primitive_type_ = PrimitiveType::Triangle;
    render();
}

void Renderer::clear(int clear_flag, const Color3B& clear_color)
{
    //清理frame buffer
    if (clear_flag & CLEAR_FRAME)
    {
        frame_buffer_->clear(clear_color);
    }

    //清理depth buffer
    if (clear_flag & CLEAR_DEPTH)
    {
        depth_buffer_->clear(Color3B(0, 0, 0));
    }
}
const Eigen::Matrix4d& Renderer::get_mvp()
{
    if (is_projection_dirty_ || is_view_dirty_ || is_model_dirty_)
    {
        mat_mvp_ = mat_projection_ * mat_view_ * mat_model_;
        is_projection_dirty_ = false;
        is_view_dirty_ = false;
        is_model_dirty_ = false;
    }

    return mat_mvp_;
}

