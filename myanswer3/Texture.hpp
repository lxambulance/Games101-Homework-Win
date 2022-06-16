//
// Created by LEI XU on 4/27/19.
//

#ifndef RASTERIZER_TEXTURE_H
#define RASTERIZER_TEXTURE_H
#include <eigen3/Eigen/Eigen>
#include <opencv2/opencv.hpp>

#include "global.hpp"
class Texture {
 private:
  cv::Mat image_data;

 public:
  Texture(const std::string& name) {
    image_data = cv::imread(name);
    cv::cvtColor(image_data, image_data, cv::COLOR_RGB2BGR);
    width = image_data.cols;
    height = image_data.rows;
  }

  int width, height;

  Eigen::Vector3f getColor(float u, float v) {
    auto u_img = u * width;
    auto v_img = (1 - v) * height;
    auto color = image_data.at<cv::Vec3b>(v_img, u_img);
    return Eigen::Vector3f(color[0], color[1], color[2]);
  }
  Eigen::Vector3f getColorBilinear(float u, float v) {
    int u_img = static_cast<int>(u * width);
    int v_img = static_cast<int>((1 - v) * height);
    // std::cout << u_img << ' ' << v_img << std::endl;
    auto color00 = image_data.at<cv::Vec3b>(v_img, u_img);
    auto color01 = image_data.at<cv::Vec3b>(v_img, u_img + 1);
    auto color10 = image_data.at<cv::Vec3b>(v_img + 1, u_img);
    auto color11 = image_data.at<cv::Vec3b>(v_img + 1, u_img + 1);
    float du = u * width - u_img, dv = (1 - v) * height - v_img;
    // std::cout << du << ' ' << dv << std::endl;
    return Eigen::Vector3f(color00[0], color00[1], color00[2]) * (1 - du) *
               (1 - dv) +
           Eigen::Vector3f(color01[0], color01[1], color01[2]) * du * (1 - dv) +
           Eigen::Vector3f(color10[0], color10[1], color10[2]) * (1 - du) * dv +
           Eigen::Vector3f(color11[0], color11[1], color11[2]) * du * dv;
  }
};
#endif  // RASTERIZER_TEXTURE_H
