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

}

std::vector<Vertex> Renderer::vertex_stage_()
{
    std::vector<Vertex> ret;
    for (std::vector<Vertex>::size_type i = 0; i < indicles_.size(); ++i)
    {
        auto vertex = get_vertex_(indicles_[i]);
        //TODO: mvp变换之后做透视除法, 然后做屏幕映射
        ret.push_back(mat_ * vertex);
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
    for (auto it = input_vertex.begin(); it != input_vertex.end(); it++)
    {
        BoundingBox bbox = geo_utils::get_bounding_box(*it, *(it + 1), *(it + 2));
        int xmin = std::floor(bbox.lb_point.x);
        int xmax = std::ceil(bbox.rt_point.x);
        int ymin = std::floor(bbox.lb_point.y);
        int ymax = std::ceil(bbox.rt_point.y);
        for (int y = ymin; y < ymax; y++)
        {
            for (int x = xmin; x < xmax; x++)
            {
                //1. 过滤掉不在三角形内部的点
                //2. 计算当前片元的重心坐标
                //3. 根据重心坐标计算当前片元的颜色
                //4. 结束
            }
        }
    }
}

std::vector<Vertex> Renderer::fragment_stage_(std::vector<Vertex>& input_fragments)
{
    std::vector<Vertex> ret;

    return ret;
}

Renderer::Renderer(FrameBuffer* frame_buffer, FrameBuffer* depth_buffer)
{
    assert(frame_buffer != nullptr && depth_buffer != nullptr && frame_buffer->is_size_equal(*depth_buffer));
    this->frame_buffer_ = frame_buffer;
    this->depth_buffer_ = depth_buffer;
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

void Renderer::clear(int clear_flag)
{
    //清理frame buffer
    if (clear_flag & static_cast<int>(ClearFlag::FRAME))
    {

    }

    //清理depth buffer
    if (clear_flag & static_cast<int>(ClearFlag::DEPTH))
    {

    }
}
