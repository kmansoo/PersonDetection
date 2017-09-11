#pragma once

#include "opencv2/core.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/video/background_segm.hpp"

#include <string>

class ObjectDetection {

public:
  ObjectDetection(const std::string &algorithm_name = "mog2");
  ~ObjectDetection(){};

public:
  bool detect(const cv::Mat &source_img);
  bool detect(const cv::Mat &source_img, cv::Mat &fgmask);

private:
  bool use_background_subtractor(const cv::Mat &source_img, cv::Mat &fgmask);
  bool use_fast_object_detection(const cv::Mat &source_img, cv::Mat &fgmask);

private:
  cv::Ptr<cv::BackgroundSubtractor> bg_model_;
  cv::Mat fgmask_;

  //  for fast object detection
  cv::Mat last_frame_;
  bool is_first_frame_;  
};