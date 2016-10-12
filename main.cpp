#include <iostream>
#include <cmath>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

std::vector<std::vector<bool>> mat2array(const cv::Mat &src_img)
{
  size_t row = src_img.rows;
  size_t col = src_img.cols;
  std::vector<std::vector<bool>> img(row, std::vector<bool>(col, false));
  int y, x;
#pragma omp parallel for private(y, x) collapse(2)
  for (y = 0; y < row; y++) {
    for (x = 0; x < col; x++) {
      if (src_img.at<uint8_t>(y, x) != 0) {
        img.at(y).at(x) = true;
      }
    }
  }
  return img;
}

cv::Mat array2mat(const std::vector<std::vector<bool>> array_img)
{
  int row = array_img.size();
  int col = array_img.data()->size();
  cv::Mat compute_img = cv::Mat::zeros(cv::Size(col, row), CV_8U);
  int y, x;
#pragma omp parallel for private(y, x) collapse(2)
  for (y = 0; y < row; y++) {
    for (x = 0; x < col; x++) {
      if (array_img.at(y).at(x)) {
        compute_img.at<uint8_t>(y, x) = 255;
      }
    }
  }
  return compute_img;
}

cv::Mat load_img(std::string path, int n)
{
  cv::Mat src = cv::imread(path, n);
  cv::threshold(src, src, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
  return src;
}

std::vector<std::vector<bool>> padding(
    const std::vector<std::vector<bool>> src_array)
{
  std::vector<std::vector<bool>> padded_array(
      src_array.size() + 2, std::vector<bool>(src_array.data()->size(), false));
  int i, j;
#pragma omp parallel for private(i, j) collapse(2)
  for (i = 0; i < src_array.size(); i++) {
    for (j = 0; j < src_array.data()->size(); j++) {
      padded_array.at(i + 1).at(j + 1) = src_array.at(i).at(j);
    }
  }
  return padded_array;
}

std::vector<std::vector<bool>> dilation(
    const std::vector<std::vector<bool>> src_array)
{
  std::vector<std::vector<bool>> dilated_array(
      src_array.size(), std::vector<bool>(src_array.data()->size(), false));
  return dilated_array;
}

std::vector<std::vector<bool>> erosion(
    const std::vector<std::vector<bool>> src_array)
{
  std::vector<std::vector<bool>> erosed_array(
      src_array.size(), std::vector<bool>(src_array.data()->size(), false));
  return erosed_array;
}

std::vector<std::vector<bool>> morphology_main_func(
    const std::vector<std::vector<bool>> src_array)
{
  auto padded_array = padding(src_array);
  std::vector<std::vector<bool>> compute_array(
      src_array.size(), std::vector<bool>(src_array.data()->size()));
  return compute_array;
}

int main()
{
  cv::Mat src_img = load_img("./kato.jpg", 0);
  cv::imshow("src", src_img);
  auto array_img = mat2array(src_img);
  auto result_img = array2mat(array_img);
  cv::imshow("result", result_img);
  cv::waitKey(0);
  return 0;
}
