#include <iostream>
#include <vector>

#include "PersonDetection.h"

#include "opencv2/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>

PersonDetection::PersonDetection(std::shared_ptr<EnvironmentInfo> env_info) {
  env_info_ = env_info;

  //  CascadeClassifier
  for (int index = 0; index < kDetector_MAX; index++) 
    is_ready_detectors_[index] = true;

  if (detectors_[kDetector_FrontFace1].load(env_info_->general_.cascade_classifier_face1_path_.c_str()) == false) {
    std::cout << "Cann't find " << env_info_->general_.cascade_classifier_face1_path_.c_str() << std::endl;
    is_ready_detectors_[kDetector_FrontFace1] = false;
  } 

  if (detectors_[kDetector_FrontFace2].load(env_info_->general_.cascade_classifier_face2_path_.c_str()) == false) {
    std::cout << "Cann't find " << env_info_->general_.cascade_classifier_face2_path_.c_str() << std::endl;
    is_ready_detectors_[kDetector_FrontFace2] = false;
  } 
}

bool PersonDetection::detect(const cv::Mat &source_img,
                                           bool draw_rect) {
  cv::Mat img;
  source_img.copyTo(img);

  cvtColor(img, img, CV_BGR2GRAY);
  equalizeHist(img, img);

  std::vector<cv::Rect> detecting_rect[kDetector_MAX];

  bool is_detecton[kDetector_MAX] = {false, };

  //  All detection algorithms are doing.
  if (is_ready_detectors_[kDetector_FrontFace1]) {
    std::vector<cv::Rect> found_face1;
    std::vector<cv::Rect> found_face2;

    detectors_[kDetector_FrontFace1].detectMultiScale(
        img, found_face1, 1.1, 2, 0 | 1, cv::Size(10, 10));

    if (found_face1.size() > 0) {
      detectors_[kDetector_FrontFace2].detectMultiScale(
          img, found_face2, 1.1, 2, 0 | 1, cv::Size(10, 10));

      if (found_face2.size() > 0) {
        for (auto front_face2_rect : found_face2) {
          for (auto front_face1_rect : found_face1) {
            if (abs(front_face1_rect.x - front_face2_rect.x) < 10 && abs(front_face1_rect.y - front_face2_rect.y) < 10 &&
               abs(front_face1_rect.width - front_face2_rect.width) < 10 && abs(front_face1_rect.width - front_face2_rect.width) < 10 &&              
                front_face1_rect.contains(
                    cv::Point(front_face2_rect.x + front_face2_rect.width / 2, front_face2_rect.y + front_face2_rect.height / 2)) &&
                front_face2_rect.contains(
                    cv::Point(front_face1_rect.x + front_face1_rect.width / 2, front_face1_rect.y + front_face1_rect.height / 2))) {
              detecting_rect[kDetector_FrontFace1].push_back(front_face1_rect);
              detecting_rect[kDetector_FrontFace2].push_back(front_face2_rect);
              break;
            }
          }
        }
      }

      if (detecting_rect[kDetector_FrontFace1].size() > 0)
        is_detecton[kDetector_FrontFace1] = true;      

      if (detecting_rect[kDetector_FrontFace2].size() > 0)
        is_detecton[kDetector_FrontFace2] = true;
    }
  }

  if (draw_rect) {
    for (auto rect : detecting_rect[kDetector_FrontFace1]) {
      // The HOG detector returns slightly larger rectangles than the real
      // objects,
      // so we slightly shrink the rectangles to get a nicer output.
      rect.x += cvRound(rect.width * 0.1);
      rect.width = cvRound(rect.width * 0.8);
      rect.y += cvRound(rect.height * 0.07);
      rect.height = cvRound(rect.height * 0.8);

      rectangle(source_img, rect.tl(), rect.br(), cv::Scalar(0, 255, 0), 3);
    }

    for (auto rect : detecting_rect[kDetector_FrontFace2]) {
      // The HOG detector returns slightly larger rectangles than the real
      // objects,
      // so we slightly shrink the rectangles to get a nicer output.
      rect.x += cvRound(rect.width * 0.1);
      rect.width = cvRound(rect.width * 0.8);
      rect.y += cvRound(rect.height * 0.07);
      rect.height = cvRound(rect.height * 0.8);

      rectangle(source_img, rect.tl(), rect.br(), cv::Scalar(0, 0, 255), 1);
    }
  }

  if (is_detecton[kDetector_FrontFace1] && is_detecton[kDetector_FrontFace2])
    return true;

  return false;
}
