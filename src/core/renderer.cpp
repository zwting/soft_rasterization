#include "renderer.h"
#include <cassert>
#include <misc/geometry_utils.h>
#include <iostream>

Vertexd& Renderer::get_vertex_(int idx)
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
    fragments = test_stage_(fragments);

    //blend阶段
    //写入frame_buffer
    for (auto frag : fragments)
    {
        Color3B c(frag.c.r, frag.c.g, frag.c.b);
        this->back_buffer_->set_pixel(frag.x, frag.y, c);
    }

    //复制到前缓冲
    std::memcpy(this->frame_buffer_->get_data(), this->back_buffer_->get_data(), this->back_buffer_->size());
}

std::vector<Vertexd> Renderer::vertex_stage_()
{
    std::vector<Vertexd> ret;
    Eigen::Vector4d v;
    Eigen::Matrix4d view_port_mat;
    int half_w = back_buffer_->get_width() / 2;
    int half_h = back_buffer_->get_height() / 2;
    view_port_mat << half_w, 0, 0, half_w,
        0, half_h, 0, half_h,
        0, 0, 1, 0,
        0, 0, 0, 1;
    auto mvp = this->get_mvp();
    for (std::vector<Vertexd>::size_type i = 0; i < indicles_.size(); ++i)
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
        ret.emplace_back(Vertexd(v(0), v(1), v(2), vertex.c));
    }
    return ret;
}

std::vector<Vertexi> Renderer::setup_primitive_and_rasterization(std::vector<Vertexd>& input_vertexs)
{
    std::vector<Vertexi> ret;
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
std::vector<Vertexi> Renderer::setup_triangle(std::vector<Vertexd>& input_vertex)
{
    std::vector<Vertexi> ret(100);
    auto height = back_buffer_->get_height();
    for (decltype(input_vertex.size()) i = 0; i < input_vertex.size(); i += 3)
    {
        BoundingBox bbox = geo_utils::get_bounding_box(input_vertex[i], input_vertex[i + 1], input_vertex[i + 2]);
        int xmin = static_cast<int>(std::floor(bbox.lb_point.x));
        int ymin = static_cast<int>(std::floor(bbox.lb_point.y));
        int xmax = static_cast<int>(std::ceil(bbox.rt_point.x));
        int ymax = static_cast<int>(std::ceil(bbox.rt_point.y));

        Vec2<double> v0(input_vertex[i].x, input_vertex[i].y),
            v1(input_vertex[i + 1].x, input_vertex[i + 1].y),
            v2(input_vertex[i + 2].x, input_vertex[i + 2].y);

        auto f = [](const Vec2<double>& a, const Vec2<double>& b, const int x, const int y)
        {
          double A = a.y - b.y;
          double B = b.x - a.x;
          return A * x + B * y + a.x * b.y - a.y * b.x;
        };
        double f_alpha = f(v0, v1, v2.x, v2.y);
        double f_beta = f(v1, v2, v0.x, v0.y);
        double f_gamma = f(v2, v0, v1.x, v1.y);
        for (int y = ymin; y < ymax; y++)
        {
            for (int x = xmin; x < xmax; x++)
            {
                //1. 计算当前片元的重心坐标
                double alpha = f(v0, v1, x, y) / f_alpha;
                double beta = f(v1, v2, x, y) / f_beta;
                double gamma = f(v2, v0, x, y) / f_gamma;

                //2. 过滤掉不在三角形内部的点
                if (alpha < 0 || beta < 0 || gamma < 0)
                    continue;

                //3. 根据重心坐标计算当前片元的颜色
                Color4B c = input_vertex[i + 2].c * alpha + input_vertex[i].c * beta + input_vertex[i + 1].c * gamma;
                double z = input_vertex[i + 2].z * alpha + input_vertex[i].z * beta + input_vertex[i + 1].z * gamma;

                //4. 结束

                ret.emplace_back(Vertexi(x,height - y, z, c));
            }
        }
    }

    return ret;
}

std::vector<Vertexi> Renderer::fragment_stage_(std::vector<Vertexi>& input_fragments)
{
    std::vector<Vertexi> ret(input_fragments.size());
    for (decltype(input_fragments.size()) i = 0; i < input_fragments.size(); i++)
    {
        ret.emplace_back(Vertexi(input_fragments[i]));
    }
    return ret;
}

std::vector<Vertexi> Renderer::test_stage_(std::vector<Vertexi>& fragments)
{
//    std::vector<Vertex> ret;
//
//    return ret;
    return fragments;
}

Renderer::Renderer(FrameBuffer* frame_buffer, FrameBuffer* depth_buffer)
{
    assert(frame_buffer != nullptr && depth_buffer != nullptr && frame_buffer->is_size_equal(*depth_buffer));
    this->frame_buffer_ = frame_buffer;
    this->depth_buffer_ = depth_buffer;

    this->back_buffer_ = new FrameBuffer(*frame_buffer);

    //初始化z-buffer
//    std::memset(depth_buffer->get_data(), 0xff, sizeof)

    this->is_model_dirty_ = false;
    this->is_view_dirty_ = false;
    this->is_projection_dirty_ = false;
}

void Renderer::set_data(std::vector<Vertexd>& verticles, std::vector<int>& indicles)
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
        back_buffer_->clear(clear_color);
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


