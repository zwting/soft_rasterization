#include <iostream>
#include <vector>
#include "third_part/tgaimage.h"
#include "src/geometry/geometry.h"
#include <Eigen/Dense>
#include <misc/geometry_utils.h>
#include "src/core/renderer.h"
#include "unistd.h"

#include <opencv2/opencv.hpp>

using namespace std;
using namespace Eigen;

const TGAColor g_white = TGAColor(0xff, 0xff, 0xff, 0xff);
const TGAColor g_red = TGAColor(0xff, 0, 0, 0xff);

const int FRAME_WIDTH = 600;
const int FRAME_HEIGHT = 400;

TGAImage g_image(300, 300, TGAImage::RGB);
Renderer* g_render = nullptr;

Eigen::Matrix4d get_model_mat(float angle)
{
    Eigen::Matrix4d model;
    double radians = math_utils::Degree2Radians(angle);
    double s = std::sin(radians), c = std::cos(radians);
    model(0, 0) = c;
    model(1, 0) = s;
    model(0, 1) = -s;
    model(1, 1) = c;
    model(2, 2) = 1;
    model(3, 3) = 1;
    return model;
}

int main()
{
    auto frame_buffer = new FrameBuffer(FRAME_WIDTH, FRAME_HEIGHT);
    auto depth_buffer = new FrameBuffer(FRAME_WIDTH, FRAME_HEIGHT, FrameBuffer::FrameFormatType::Bit8);
    g_render = new Renderer(frame_buffer, depth_buffer);
    vector<Vertex> vertices = {
        { -2, -1, 0, 0xff, 0, 0, 0xff },
        { 2, -1, 0, 0, 0xff, 0, 0xff },
        { 0, 1, 0, 0, 0, 0xff, 0xff }
    };
    vector<int> indices = { 0, 1, 2 };
    float angle = 0;
    Eigen::Matrix4d model = get_model_mat(angle);

    Eigen::Matrix4d view = geo_utils::get_camera_mat(Eigen::Vector3d(0, 0, 10),
        Eigen::Vector3d(0, 0, 0),
        Eigen::Vector3d(0, 1, 0));
    Eigen::Matrix4d projection = geo_utils::get_project_mat(0.3, 100, 45, FRAME_WIDTH / (float)FRAME_HEIGHT);

    Eigen::Vector3i v1(1, 2, 0);
    Eigen::Vector3i v2(0, 1, 0);
    std::cout << v1.cross(v2) << std::endl;

    g_render->set_model(model);
    g_render->set_view(view);
    g_render->set_projection(projection);

    g_render->set_data(vertices, indices);
    g_render->draw_triangles(1);

    cv::Mat image(FRAME_HEIGHT, FRAME_WIDTH, CV_8UC3, g_render->get_frame_buffer()->get_data());
    int key = 0;
    auto bg_color = Color3B(100, 100, 100);
    while (key != 27)
    {
        namedWindow("Display Image", cv::WINDOW_AUTOSIZE);
        imshow("Display Image", image);

        key = cv::waitKey(0);
        if (key == 'a')
        {
            angle += 2;
        }
        else if (key == 'd')
        {
            angle -= 2;
        }
        std::cout << "key=" << key <<"angle="<<angle<< std::endl;
        g_render->clear(CLEAR_FRAME, bg_color);
        g_render->set_model(get_model_mat(angle));
        g_render->draw_triangles(1);
        std::memcpy(image.data, g_render->get_frame_buffer()->get_data(), g_render->get_frame_buffer()->size());
    }

//    TGAImage img(*(g_render->get_frame_buffer()));
//    img.write_tga_file("output.tga");

    return 0;
}
