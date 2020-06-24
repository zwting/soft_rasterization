#ifndef _RENDERER_H_
#define _RENDERER_H_
#include "frame_buffer.h"
#include <vector>
#include "Eigen/Dense"
#include "../types.h"

const int CLEAR_FRAME = 0x1;
const int CLEAR_DEPTH = 0x2;

class Renderer
{
 private:
    FrameBuffer* frame_buffer_;
    FrameBuffer* depth_buffer_;
    FrameBuffer* back_buffer_;

    std::vector<Vertexd> verticles_;
    std::vector<int> indicles_;

    PrimitiveType primitive_type_;

    Eigen::Matrix4d mat_mvp_;
    Eigen::Matrix4d mat_model_;
    Eigen::Matrix4d mat_view_;
    Eigen::Matrix4d mat_projection_;

    bool is_model_dirty_;
    bool is_view_dirty_;
    bool is_projection_dirty_;

    Vertexd& get_vertex_(int idx);

    void mid_point_draw_line_(int x0, int y0, int x1, int y1, const Color3B& color);

    //----------------渲染管线------------------
    void render();
    std::vector<Vertexd> vertex_stage_();
    std::vector<Vertexi> setup_primitive_and_rasterization(std::vector<Vertexd>& input_vertexs);
    std::vector<Vertexi> fragment_stage_(std::vector<Vertexi>& input_fragments);

    std::vector<Vertexi> setup_triangle(std::vector<Vertexd>& input_vertex);
    std::vector<Vertexi> test_stage_(std::vector<Vertexi>& fragments);

 public:

    explicit Renderer(FrameBuffer* frame_buffer, FrameBuffer* depth_buffer);
    void set_data(std::vector<Vertexd>& verticles, std::vector<int>& indicles);
    void draw_point(int count);
    void draw_lines(int count);
    void draw_triangles(int count);
    void set_matrix(const Eigen::Matrix4d& matrix) { mat_mvp_ = Eigen::Matrix4d(matrix); }
    void set_model(const Eigen::Matrix4d& model)
    {
        is_model_dirty_ = true;
        mat_model_ = Eigen::Matrix4d(model);
    }
    void set_view(const Eigen::Matrix4d& view)
    {
        is_view_dirty_ = true;
        mat_view_ = Eigen::Matrix4d(view);
    }
    void set_projection(const Eigen::Matrix4d& projection)
    {
        is_projection_dirty_ = true;
        mat_projection_ = Eigen::Matrix4d(projection);
    }

    const Eigen::Matrix4d& get_mvp();

    void clear(int clear_flag, const Color3B& clear_color);
    FrameBuffer* get_frame_buffer() const { return frame_buffer_; }
    FrameBuffer* get_depth_buffer() const { return depth_buffer_; }
};

#endif // _RENDERER_H_
