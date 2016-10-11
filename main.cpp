#include <iostream>
#include <cmath>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

std::vector<std::vector<float>> mat2array(const cv::Mat &src_img)
{
  size_t row = src_img.rows;
  size_t col = src_img.cols;
  std::vector<std::vector<float>> img(row, std::vector<float>(col, 0));
  int y, x;
#pragma omp parallel for private(y, x) collapse(2)
  for (y = 0; y < row; y++) {
    for (x = 0; x < col; x++) {
      img.at(y).at(x) = src_img.at<uint8_t>(y, x);
    }
  }
  return img;
}

cv::Mat array2mat(const std::vector<std::vector<float>> array_img)
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
  auto to2base = [](size_t x) -> size_t { return pow(2, ceil(log2(x))); };
  cv::Rect roi_rect;
  roi_rect.width = src.cols;
  roi_rect.height = src.rows;
  cv::Mat padded =
      cv::Mat::zeros(cv::Size(to2base(src.cols), to2base(src.rows)), CV_8U);
  cv::Mat roi(padded, roi_rect);
  src.copyTo(roi);
  return padded;
}

int main()
{
  cv::Mat src_img = load_img("./kato.jpg", 0);
  cv::imshow("src", src_img);
  cv::waitKey(0);
  return 0;
}
