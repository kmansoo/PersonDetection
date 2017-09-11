#include <iostream>
#include <vector>

#include "ObjectDetection.h"

#include "opencv2/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>

ObjectDetection::ObjectDetection(const std::string &algorithm_name) {
  bg_model_ = algorithm_name == "knn"
                  ? cv::createBackgroundSubtractorKNN()
                        .dynamicCast<cv::BackgroundSubtractor>()
                  : cv::createBackgroundSubtractorMOG2()
                        .dynamicCast<cv::BackgroundSubtractor>();

  is_first_frame_ = false;
}

bool ObjectDetection::detect(const cv::Mat &source_img) {
  return detect(source_img, fgmask_);
}

bool ObjectDetection::detect(const cv::Mat &source_img, cv::Mat &fgmask) {
  return use_background_subtractor(source_img, fgmask);
  //return use_fast_object_detection(source_img, fgmask);
}

bool ObjectDetection::use_background_subtractor(const cv::Mat &source_img,
                                                cv::Mat &fgmask) {
  // update the model
  bool update_bg_model = true;

  cv::Mat frame, gray_frame;
  
  // scale down image
  float rate = 200 / source_img.cols;

  cv::resize(source_img, frame, cv::Size(source_img.cols * rate, source_img.rows * rate), 0.33, 0.33);

  // convert to grayscale
  cvtColor(frame, gray_frame, CV_BGR2GRAY);

  // add blur
  cv::GaussianBlur(gray_frame, gray_frame, cv::Size(21, 21), 0);

  bg_model_->apply(gray_frame, fgmask, update_bg_model ? -1 : 0);

  std::vector<std::vector<cv::Point>> contours;

  // 연결 구성요소의 외곽선 가져오기
  cv::findContours(fgmask,
                   contours,               // 외곽선 벡터
                   cv::RETR_CCOMP,         // 외부와 내부 외곽선 탐색
                   cv::CHAIN_APPROX_NONE); // 각 외곽선의 모든 화소 탐색 ,
                                           // CHAIN_APPROX_SIMPLE = 4개의
                                           // 포인터만 저장

  //  std::cout << "contours.size() = " << contours.size() << std::endl;
  if (contours.size() >= 10)
    return true;

  return false;
}

bool ObjectDetection::use_fast_object_detection(const cv::Mat &source_img, cv::Mat &fgmask) {
  cv::Mat frame, gray_frame, delta_frame;

  // scale down image
  float rate = 200 / source_img.cols;

  cv::resize(source_img, frame, cv::Size(source_img.cols * rate, source_img.rows * rate), 0, 0, CV_INTER_NN);

  // convert to grayscale
  cvtColor(frame, gray_frame, CV_BGR2GRAY);

  // add blur
  cv::GaussianBlur(gray_frame, gray_frame, cv::Size(source_img.cols * rate, source_img.rows * rate), 0);

  if (is_first_frame_ == false) {
    is_first_frame_ = true;

    last_frame_ = gray_frame;
  }

  // create difference frame
  cv::absdiff(last_frame_, gray_frame, delta_frame);
  cv::threshold(delta_frame, delta_frame, 50, 255, cv::THRESH_BINARY);

  int iterations = 2;
  cv::dilate(delta_frame, delta_frame, cv::Mat(), cv::Point(-1, -1),
             iterations);

  std::vector<std::vector<cv::Point>> contours;

  cv::findContours(delta_frame,
                   contours,               // 외곽선 벡터
                   cv::RETR_CCOMP,         // 외부와 내부 외곽선 탐색
                   cv::CHAIN_APPROX_NONE); // 각 외곽선의 모든 화소 탐색 ,
                                           // CHAIN_APPROX_SIMPLE = 4개의
                                           // 포인터만 저장

  if (contours.size() >= 1)
    return true;

  return false;
}
