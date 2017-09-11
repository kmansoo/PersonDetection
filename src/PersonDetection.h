#pragma once

#include "opencv2/core.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/video/background_segm.hpp"

#include <string>

#include "EnvironmentInfo.h"

class PersonDetection {

public:
  PersonDetection(std::shared_ptr<EnvironmentInfo> env_info);
  ~PersonDetection(){};

public:
  bool detect(const cv::Mat &source_img, bool draw_rect = false);

private:
  std::shared_ptr<EnvironmentInfo> env_info_;

  enum DetectorName {
    kDetector_FrontFace1 = 0,
    kDetector_FrontFace2,  
    kDetector_MAX
  };

  cv::CascadeClassifier detectors_[kDetector_MAX];
  bool is_ready_detectors_[kDetector_MAX];  
};