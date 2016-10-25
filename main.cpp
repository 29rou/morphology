#include <iostream>
#include <cmath>
#include <vector>
#include <numeric>
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
    const std::vector<std::vector<bool>> src_array, const bool padd_with)
{
  std::vector<std::vector<bool>> padded_array(
      src_array.size() + 2,
      std::vector<bool>(src_array.data()->size() + 2, padd_with));
  int i, j;
#pragma omp parallel for private(i, j) collapse(2)
  for (i = 0; i < src_array.size(); i++) {
    for (j = 0; j < src_array.data()->size(); j++) {
      padded_array.at(i + 1).at(j + 1) = src_array.at(i).at(j);
    }
  }
  return padded_array;
}

std::vector<std::vector<bool>> suppressing(
    const std::vector<std::vector<bool>> src_array)
{
  std::vector<std::vector<bool>> suppressed_array(
      src_array.size() - 2,
      std::vector<bool>(src_array.data()->size() - 2, false));
  int i, j;
#pragma omp parallel for private(i, j) collapse(2)
  for (i = 0; i < suppressed_array.size(); i++) {
    for (j = 0; j < suppressed_array.data()->size(); j++) {
      suppressed_array.at(i).at(j) = src_array.at(i + 1).at(j + 1);
    }
  }
  return suppressed_array;
}

std::vector<std::vector<bool>> dilation(
    const std::vector<std::vector<bool>> src_array)
{
  std::vector<std::vector<bool>> dilated_array(
      src_array.size(), std::vector<bool>(src_array.data()->size(), false));
  int i, j;
#pragma omp parallel for private(i, j) collapse(2)
  for (i = 1; i < src_array.size() - 1; i++) {
    for (j = 1; j < src_array.data()->size() - 1; j++) {
      dilated_array.at(i).at(j) =
          src_array.at(i - 1).at(j - 1) || src_array.at(i - 1).at(j) ||
          src_array.at(i - 1).at(j + 1) || src_array.at(i).at(j - 1) ||
          src_array.at(i).at(j) || src_array.at(i).at(j + 1) ||
          src_array.at(i + 1).at(j - 1) || src_array.at(i + 1).at(j) ||
          src_array.at(i + 1).at(j + 1);
    }
  }
  return dilated_array;
}

std::vector<std::vector<bool>> erosion(
    const std::vector<std::vector<bool>> src_array)
{
  std::vector<std::vector<bool>> erosed_array(
      src_array.size(), std::vector<bool>(src_array.data()->size(), false));
  int i, j;
#pragma omp parallel for private(i, j) collapse(2)
  for (i = 1; i < src_array.size() - 1; i++) {
    for (j = 1; j < src_array.data()->size() - 1; j++) {
      erosed_array.at(i).at(j) =
          src_array.at(i - 1).at(j - 1) && src_array.at(i - 1).at(j) &&
          src_array.at(i - 1).at(j + 1) && src_array.at(i).at(j - 1) &&
          src_array.at(i).at(j) && src_array.at(i).at(j + 1) &&
          src_array.at(i + 1).at(j - 1) && src_array.at(i + 1).at(j) &&
          src_array.at(i + 1).at(j + 1);
    }
  }
  return erosed_array;
}

std::vector<std::vector<bool>> opening(
    const std::vector<std::vector<bool>> src_array, const int n)
{
  auto computed_array = src_array;
  for (int i = 0; i < n; i++) {
    computed_array = dilation(computed_array);
  }
  return computed_array;
}

std::vector<std::vector<bool>> closing(
    const std::vector<std::vector<bool>> src_array, const int n)
{
  auto computed_array = src_array;
  for (int i = 0; i < n; i++) {
    computed_array = erosion(computed_array);
  }
  return computed_array;
}

std::vector<std::vector<bool>> morphology_main_func(
    const std::vector<std::vector<bool>> src_array, const int count)
{
  std::cout << "padding" << std::endl;
  auto padded_array = padding(src_array, false);
  std::cout << "opening" << std::endl;
  auto opened_array = opening(padded_array, count);
  std::cout << "closing" << std::endl;
  auto closed_array = closing(opened_array, count);
  std::cout << "suppressing" << std::endl;
  auto suppressed_array = suppressing(closed_array);
  return suppressed_array;
}

int main(int argc, char *argv[])
{
  cv::Mat src_img = load_img(argv[1], 0);
  cv::imshow("src", src_img);
  cv::waitKey(1);
  auto array_img = mat2array(src_img);
  std::cout << "start" << std::endl;
  auto computed_array = morphology_main_func(array_img, atoi(argv[2]));
  auto result_img = array2mat(computed_array);
  cv::imshow("result", result_img);
  cv::waitKey(0);
  return 0;
}
