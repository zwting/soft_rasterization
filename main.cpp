#include <iostream>
#include <vector>
#include "third_part/tgaimage.h"
#include "src/geometry/geometry.h"
#include <Eigen/Dense>
#include <misc/geometry_utils.h>
#include "src/core/renderer.h"

#include <opencv2/opencv.hpp>

using namespace std;
using namespace Eigen;

const int FRAME_WIDTH = 600;
const int FRAME_HEIGHT = 400;

Renderer* g_render = nullptr;

Eigen::Matrix4d get_model_mat(float angle, const Vec3<double>& translate = Vec3<double>(0, 0, 0))
{
    Eigen::Matrix4d model;
    double radians = math_utils::Degree2Radians(angle);
    double s = std::sin(radians), c = std::cos(radians);
    model << c, -s, 0, translate.x,
        s, c, 0, translate.y,
        0, 0, 1, translate.z,
        0, 0, 0, 1;


    return model;
}

long long get_time_stamps()
{
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    );
    return ms.count();
}

int main()
{
    auto frame_buffer = new FrameBuffer(FRAME_WIDTH, FRAME_HEIGHT);
    auto depth_buffer = new FrameBuffer(FRAME_WIDTH, FRAME_HEIGHT, FrameBuffer::FrameFormatType::Bit8);
    g_render = new Renderer(frame_buffer, depth_buffer);
    vector<Vertexd> vertices = {
        { -1.5, -1.0, 0.0, 0xff, 0, 0, 0xff },
        { 1.5, -1.0, 0.0, 0, 0xff, 0, 0xff },
        { 0.0, 1.0, 0.0, 0, 0, 0xff, 0xff },
//        { -1.0, -1.0, -1.0, 0xff, 0, 0, 0xff },
//        { -1.0, 1.0, -1.0, 0, 0xff, 0, 0xff },
//        { 1.0, 1.0, -1.0, 0, 0, 0xff, 0xff },
//        { 1, -1, -1, 0xff, 0, 0xff, 0xff },
//        { -1, -1, 1, 0, 0xff, 0xff, 0xff },
//        { -1, 1, 1, 0xff, 0xff, 0, 0xff },
//        { 1, 1, 1, 0, 0xff, 0xff, 0xff },
//        { 1, -1, 1, 0, 0, 0, 0xff },
    };
    vector<int> indices = {
        0, 1, 2
//        0, 2, 1, 0, 3, 2,
//        4, 1, 5, 4, 0, 1,
//        4, 3, 0, 4, 7, 3,
//        1, 6, 5, 1, 2, 6,
//        4, 6, 7, 4, 5, 6,
//        3, 6, 2, 3, 7, 6,
    };
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

    cv::Mat image(FRAME_HEIGHT, FRAME_WIDTH, CV_8UC3, g_render->get_frame_buffer()->get_data());
    int key = 0;
    auto bg_color = Color3B(100, 100, 100);
    Eigen::IOFormat format(3);
    cout << model.format(format) << projection.format(format) << view.format(format) << endl;
    auto prev_time = get_time_stamps();
    auto translate = Vec3<double>(2, 0, 0);
    auto translate1 = Vec3<double>(2, 2, 0);
    while (key != 27)
    {
        static float angle = 0;
        g_render->clear(CLEAR_FRAME, bg_color);

        g_render->set_model(get_model_mat(angle));
        g_render->draw_triangles(1);

        g_render->set_model(get_model_mat(90 - angle, translate));
        g_render->draw_triangles(1);

        g_render->set_model(get_model_mat(90 - angle, translate1));
        g_render->draw_triangles(1);

        std::memcpy(image.data, g_render->get_frame_buffer()->get_data(), g_render->get_frame_buffer()->size());

        namedWindow("Display Image", cv::WINDOW_AUTOSIZE);
        imshow("Display Image", image);

        key = cv::waitKey(1);
        if (key == 'a')
        {
            angle += 2;
        }
        else if (key == 'd')
        {
            angle -= 2;
        }
        angle += 1;

        auto cur_time = get_time_stamps();
        auto time_delta = cur_time - prev_time;
        prev_time = cur_time;
        cout << "FPS: " << 1000 / time_delta << endl;
    }

//    TGAImage img(*(g_render->get_frame_buffer()));
//    img.write_tga_file("output.tga");

    return 0;
}
