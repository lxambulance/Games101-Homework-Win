#include <cmath>
#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "Triangle.hpp"
#include "rasterizer.hpp"

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos) {
  Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

  Eigen::Matrix4f translate;
  translate << 1, 0, 0, -eye_pos[0], 0, 1, 0, -eye_pos[1], 0, 0, 1, -eye_pos[2],
      0, 0, 0, 1;

  view = translate * view;

  return view;
}

Eigen::Matrix4f get_model_matrix_with_any_vector(float a, Eigen::Vector3f v) {
  Eigen::Matrix4f m = Eigen::Matrix4f::Identity();

  Eigen::Matrix4f m1, m2;
  m1 << v[0] * v[0], v[0] * v[1], v[0] * v[2], 0, v[1] * v[0], v[1] * v[1],
      v[1] * v[2], 0, v[2] * v[0], v[2] * v[1], v[2] * v[2], 0, 0, 0, 0, 1;
  m2 << 0, -v[2], v[1], 0, v[2], 0, -v[0], 0, -v[1], v[0], 0, 0, 0, 0, 0, 1;
  a = -a / 180 * MY_PI;
  m = m1 + m2 * (m - m1) * sin(a) + (m - m1) * cos(a);

  return m;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle) {
  Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

  // TODO: Implement this function
  // Create the model matrix for rotating the triangle around the Z axis.
  // Then return it.

  Eigen::Matrix4f rotate;
  float a = cos(-rotation_angle / 180 * MY_PI),
        b = sin(-rotation_angle / 180 * MY_PI);
  rotate << a, -b, 0, 0, b, a, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1;

  model = rotate * model;

  return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float zNear, float zFar) {
  // Students will implement this function

  Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

  // TODO: Implement this function
  // Create the projection matrix for the given parameters.
  // Then return it.

  Eigen::Matrix4f pers2otho;
  pers2otho << zNear, 0, 0, 0, 0, zNear, 0, 0, 0, 0, zNear + zFar,
      -zNear * zFar, 0, 0, 1, 0;
  projection = pers2otho * projection;

  double n = -zNear, f = -zFar, tmp = n * tan(eye_fov / 360 * MY_PI);
  double b = -tmp, t = tmp, l = -tmp * aspect_ratio, r = tmp * aspect_ratio;
  Eigen::Matrix4f transform;
  transform << 2 / (r - l), 0, 0, (l + r) / (l - r), 0, 2 / (t - b), 0,
      (b + t) / (b - t), 0, 0, 2 / (f - n), (n + f) / (n - f), 0, 0, 0, 1;
  projection = transform * projection;

  return projection;
}

int main(int argc, const char** argv) {
  float angle = 0;
  bool command_line = false;
  std::string filename = "output.png";
  Eigen::Vector3f axis{0, 0, -1};

  if (argc >= 3) {
    command_line = true;
    angle = std::stof(argv[2]);  // -r by default
    if (argc == 4) {
      filename = std::string(argv[3]);
    } else if (argc == 5) {
      command_line = false;
      float x = std::stof(argv[2]), y = std::stof(argv[3]),
            z = std::stof(argv[4]);
      float l = sqrt(x * x + y * y + z * z);
      axis = {x / l, y / l, z / l};
    } else
      return 0;
  }

  rst::rasterizer r(700, 700);

  Eigen::Vector3f eye_pos = {0, 0, 5};

  std::vector<Eigen::Vector3f> pos{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};

  std::vector<Eigen::Vector3i> ind{{0, 1, 2}};

  auto pos_id = r.load_positions(pos);
  auto ind_id = r.load_indices(ind);

  int key = 0;
  int frame_count = 0;

  if (command_line) {
    r.clear(rst::Buffers::Color | rst::Buffers::Depth);

    r.set_model(get_model_matrix(angle));
    r.set_view(get_view_matrix(eye_pos));
    r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

    r.draw(pos_id, ind_id, rst::Primitive::Triangle);
    cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
    image.convertTo(image, CV_8UC3, 1.0f);

    cv::imwrite(filename, image);

    return 0;
  }

  while (key != 27) {
    r.clear(rst::Buffers::Color | rst::Buffers::Depth);

    // std::cout << get_model_matrix_with_any_vector(angle, axis) << std::endl;
    // std::cout << get_model_matrix(angle) << std::endl;
    r.set_model(get_model_matrix_with_any_vector(angle, axis));
    r.set_view(get_view_matrix(eye_pos));
    r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

    r.draw(pos_id, ind_id, rst::Primitive::Triangle);

    cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
    image.convertTo(image, CV_8UC3, 1.0f);
    cv::imshow("image", image);
    key = cv::waitKey(10);

    std::cout << "frame count: " << frame_count++ << '\n';

    if (key == 'a') {
      angle += 10;
    } else if (key == 'd') {
      angle -= 10;
    }
  }

  return 0;
}
