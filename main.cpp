#include <iostream>
#include <vector>
#include "third_part/tgaimage.h"
#include "src/geometry/geometry.h"
#include <Eigen/Dense>
#include "src/core/renderer.h"

using namespace std;
using namespace Eigen;

const TGAColor g_white = TGAColor(0xff, 0xff, 0xff, 0xff);
const TGAColor g_red = TGAColor(0xff, 0, 0, 0xff);

const int FRAME_WIDTH = 600;
const int FRAME_HEIGHT = 400;

TGAImage g_image(300, 300, TGAImage::RGB);
Renderer* g_render = nullptr;

int main()
{
    auto frame_buffer = new FrameBuffer(FRAME_WIDTH, FRAME_HEIGHT);
    auto depth_buffer = new FrameBuffer(FRAME_WIDTH, FRAME_HEIGHT, FrameBuffer::FrameFormatType::Bit8);
    g_render = new Renderer(frame_buffer, depth_buffer);
    vector<Vertex> vertices = {
        { -50, 10, 0 },
        { 50, 10, 0 },
        { 0, 50, 0 }
    };
    vector<int> indices = { 0, 1, 2 };
    Eigen::Matrix4d model;
    model<<1, 0, 0, 0,
           0, 1, 0, 0,
           0, 0, 1, -10,
           0, 0, 0, -1;
    Eigen::Matrix4d view;
    Eigen::Matrix4d projection;
    auto mvp = model;
    g_render->set_data(vertices, indices);
    g_render->set_matrix(mvp);
    g_render->draw_triangles(1);
    TGAImage img(*(g_render->get_frame_buffer()));
    img.flip_vertically();
    img.write_tga_file("output.tga");

    return 0;
}
