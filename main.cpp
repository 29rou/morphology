#include <iostream>
#include <cmath>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

std::vector<std::vector<bool>> mat2array(const cv::Mat &src_img)
{
  size_t row = src_img.rows + 2;
  size_t col = src_img.cols + 2;
  std::vector<std::vector<bool>> img(row, std::vector<bool>(col, 0));
  int y, x;
#pragma omp parallel for private(y, x) collapse(2)
  for (y = 1; y < row - 1; y++) {
    for (x = 1; x < col - 1; x++) {
      (src_img.at<uint8_t>(y - 1, x - 1) != 0) ? img.at(y).at(x) = 255 : 0;
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
      compute_img.at<uint8_t>(y, x) = array_img.at(y).at(x);
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

std::vector<std::vector<float>> morphology_main_func(
    const std::vector<std::vector<float>> src_array)
{
  std::vector<std::vector<float>> compute_array(
      src_array.size() - 2, std::vector<float>(src_array.data()->size() - 2));
  return compute_array;
}

int main()
{
  cv::Mat src_img = load_img("./kato.jpg", 0);
  cv::imshow("src", src_img);
  auto array_img = mat2array(src_img);
  auto result_img = array2mat(array_img);
  cv::waitKey(0);
  return 0;
}
