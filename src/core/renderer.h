#ifndef _RENDERER_H_
#define _RENDERER_H_
#include "frame_buffer.h"
#include <vector>
#include "Eigen/Dense"
#include "../types.h"

class Renderer
{
 public:
    enum class ClearFlag { FRAME = 0x1, DEPTH = 0x2 };

 private:
    FrameBuffer* frame_buffer_;
    FrameBuffer* depth_buffer_;

    std::vector<Vertex> verticles_;
    std::vector<int> indicles_;

    PrimitiveType primitive_type_;

    Eigen::Matrix4d mat_;

    Vertex& get_vertex_(int idx);

    void mid_point_draw_line_(int x0, int y0, int x1, int y1, const Color3B& color);

    //----------------渲染管线------------------
    void render();
    std::vector<Vertex> vertex_stage_();
    std::vector<Vertex> setup_primitive_and_rasterization(std::vector<Vertex>& input_vertexs);
    std::vector<Vertex> fragment_stage_(std::vector<Vertex>& input_fragments);

    std::vector<Vertex> setup_triangle(std::vector<Vertex>& input_vertex);

 public:

    explicit Renderer(FrameBuffer* frame_buffer, FrameBuffer* depth_buffer);
    void set_data(std::vector<Vertex>& verticles, std::vector<int>& indicles);
    void draw_point(int count);
    void draw_lines(int count);
    void draw_triangles(int count);
    void set_matrix(const Eigen::Matrix4d& matrix) { mat_ = matrix; }

    void clear(int clear_flag = static_cast<int>(ClearFlag::FRAME));
    FrameBuffer* get_frame_buffer() const { return frame_buffer_; }
    FrameBuffer* get_depth_buffer() const { return depth_buffer_; }
};

#endif // _RENDERER_H_
