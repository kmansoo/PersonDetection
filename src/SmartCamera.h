#include <iostream>
#include <thread>

#include "ccCore/ccCoreAPI.h"

#include "opencv2/core.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/videoio.hpp"

#include "ObjectDetection.h"
#include "PersonDetection.h"
#include "VisionRecognitionClient.h"
#include "EnvironmentInfo.h"

class SmartCamera {
public:
  SmartCamera(cv::CommandLineParser &parser);
  ~SmartCamera();

public:
  bool run();

private:
  void init();
  void open_video_resource();
  void recognize();
  bool detect_persion(cv::Mat& original);

  void backend_worker();

private:
  bool use_camera_;
  bool smooth_mask_;
  bool use_display_;
  bool use_rtsp_;
  bool use_writer_;

  std::string ai_engine_;
  std::string recognition_image_name_;

  std::thread thread_;

  bool is_stop_thread_;
  bool is_ready_recogniztion_;
  bool is_detected_object_;
  bool is_detected_person_;;

  int detection_count_;
  int detected_persion_number_;
  
  bool check_time_;
  double start_time_;

  cv::Mat img_for_object_detection_;

  std::shared_ptr<EnvironmentInfo> env_;
  
  cv::VideoCapture video_cap_;

  std::shared_ptr<VisionRecognitionClient> vrc_;
  std::shared_ptr<ObjectDetection> obj_detection_;
  std::shared_ptr<PersonDetection> person_detection_;
};
