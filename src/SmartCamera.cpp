#include <vector>

#include "ccCore/ccString.h"
#include "restclient-cpp/restclient.h"

#include "GCP/GcpVisionRecognitionClient.h"
#include "Watson/WatsonVisionRecognitionClient.h"

#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>

#include "SmartCamera.h"

SmartCamera::SmartCamera(cv::CommandLineParser &parser) {
  RestClient::init();

  is_stop_thread_ = false;
  is_ready_recogniztion_ = true;
  is_detected_object_ = false;
  is_detected_person_ = false;

  check_time_ = false;
  detection_count_ = 0;
  detected_persion_number_ = 0;
  start_time_ = (double)cv::getTickCount();
  
  use_camera_ = parser.has("camera");
  use_display_ = parser.has("display");
  smooth_mask_ = parser.has("smooth");
  use_rtsp_ = parser.has("rtsp");
  use_writer_ = parser.has("writefile");
  ai_engine_ = parser.get<std::string>("ai_engine");
 
  parser.printMessage();

  env_ = std::make_shared<EnvironmentInfo>();
  env_->getEnvironment();  

  obj_detection_ = std::make_shared<ObjectDetection>("mog");
  person_detection_ = std::make_shared<PersonDetection>(env_);

  init();
}

SmartCamera::~SmartCamera() {
  is_stop_thread_ = true;
  thread_.join();
}

void SmartCamera::init() {
  open_video_resource();

  //  Create the instance for Vision Recognition Client
  if (ai_engine_ == "gcp") {
    vrc_ = std::make_shared<GcpVisionRecognitionClient>(env_); //  for GCP

    std::cout << "The AI Engine is chosen by 'GCP'" << std::endl;
  } else {
    vrc_ = std::make_shared<WatsonVisionRecognitionClient>(env_); //  for Watson
    std::cout << "The AI Engine is chosen by 'Watson'" << std::endl;
  }

  thread_ = std::thread(std::bind(&SmartCamera::backend_worker, this));

  if (use_display_) {
    if (env_->general_.only_detection_face_ == false) {
      cv::namedWindow("image", cv::WINDOW_NORMAL);
      cv::namedWindow("ObjectDetection", cv::WINDOW_NORMAL);
    }

    cv::namedWindow("PersonDetection", cv::WINDOW_NORMAL);    
  }
}

void SmartCamera::open_video_resource() {
  if (video_cap_.isOpened())
    video_cap_.release();

  if (use_camera_)
    video_cap_.open(0);
  else {
    if (use_rtsp_)
      video_cap_.open(env_->general_.rtsp_server_uri_);
    else
      video_cap_.open(env_->general_.source_video_path_);
  }
}

void SmartCamera::backend_worker() {
  is_ready_recogniztion_ = false;

  int display_count = 0;
  int detection_count = 0;

  cv::Mat fg_mask_frame;

  while (is_stop_thread_ == false) {
    if (is_ready_recogniztion_ == true) {
      
      if (check_time_) {
        double cur_time = (double)cv::getTickCount() - start_time_;
  
        cur_time = (cur_time * 1000. / cv::getTickFrequency()); //  seconds
  
        //  check timeout
        if (cur_time > 10 * 1000) {
          display_count = 0;

          detection_count_ = 0;

          check_time_ = false;
          is_detected_object_ = false;
          is_detected_person_ = false;

          std::cout << "Timeout!" << std::endl;

          detection_count = 0;
        }
      }

      //
      //  If env_->general_.only_detection_face_ is true, the SmartCamera is going to detect only faces in video.
      //
      if (env_->general_.only_detection_face_) {
        person_detection_->detect(img_for_object_detection_, true);
        
        if (use_display_)
          cv::imshow("PersonDetection", img_for_object_detection_);             
      }
      else {
        //
        //  If env_->general_.only_detection_face_ is false, the SmartCamera is going to detect whether the object is an persion or not after detection an object.
        //  Also the SmartCamera doesn't work during 10 seconds when it detected the person.
        //
        bool status = obj_detection_->detect(img_for_object_detection_, fg_mask_frame);
      
        if (use_display_)
          cv::imshow("ObjectDetection", fg_mask_frame);  
      
        if (is_detected_object_ == false) {
          if (status) {
            is_detected_person_ = false;
            is_detected_object_ = true;

            std::cout << "An object is detected so it's going to detect whether the object is a person or not!!" << std::endl;

            check_time_ = true;
            start_time_ = (double)cv::getTickCount();
            detection_count = 0;
          }
        }

        if (is_detected_object_ == true && is_detected_person_ == false) {
          // convert to grayscale
          if (person_detection_->detect(img_for_object_detection_, false)) {

            std::cout << "A person is detected!" << std::endl;

            std::string image_name;

            Luna::ccString::format(image_name, "detected_persion_%03d.png",
                                  detected_persion_number_++);

            std::cout << "Number of Detection Person: "
                      << detected_persion_number_ << std::endl;

            cv::imwrite(image_name, img_for_object_detection_);

            recognition_image_name_ = image_name;
            
            recognize();

            if (detected_persion_number_ >= 100)
              detected_persion_number_ = 0;

            is_detected_person_ = true;
            detection_count_ = 0;

            check_time_ = true;
            start_time_ = (double)cv::getTickCount();

            if (use_display_)
              cv::imshow("PersonDetection", img_for_object_detection_);
          }     
        }
      }
    }

    is_ready_recogniztion_ = false;

    Luna::sleep(1);
  }
}

void SmartCamera::recognize() {

  std::vector<std::string> label_list;
  vrc_->recognize(recognition_image_name_, label_list);

  std::string recognize_info = "person";

  for (auto label : label_list) {
    if (label == "sunglasses") {
      recognize_info = label;
      std::cout << "I found a sunglasses!" << std::endl;
      break;
    }

    if (label == "umbrella") {
      recognize_info = label;
      std::cout << "I found a umbrella!" << std::endl;
      break;
    }

    if (label == "cap") {
      recognize_info = label;
      std::cout << "I found a cap!" << std::endl;
      break;
    }    
  }

  //  연결된 곳으로 보냄.
  std::string wallpad_http_server;
  Luna::ccString::format(wallpad_http_server, "http://%s:8000", env_->general_.wallpad_ip_.c_str());

  std::shared_ptr<RestClient::Connection> rest_conn = std::make_shared<RestClient::Connection>(wallpad_http_server);
  
  rest_conn->SetTimeout(2);
  rest_conn->SetUserAgent("COMMAX/SmurfHome");
  rest_conn->SetSSLVerifyPeer(false);

  RestClient::HeaderFields headers;
  headers["Content-Type"] = "application/json";
  rest_conn->SetHeaders(headers);

  std::string req_uri = "/api/v1/smartcamera";
  std::string req_body;

  Luna::ccString::format(req_body, R"(
{
  "recognition": "%s"
}
)", recognize_info.c_str());

  RestClient::Response response = rest_conn->post(req_uri, req_body);  
}

bool SmartCamera::run() {
  if (!video_cap_.isOpened()) {
    std::cout << "can not open camera or video file" << std::endl;
    return false;
  }

  cv::Mat source_img;
  cv::Mat show_img;

  for (;;) {
    video_cap_ >> source_img;

    if (source_img.empty()) {
      std::cout << "There are no video data, so it's going to try reopening the video source!" << std::endl;
      open_video_resource();
      cv::waitKey(500);
      continue;
    }

    if (env_->general_.image_crop_ == true) {
      cv::Rect rect;

      rect = cv::Rect(cv::Point(env_->general_.image_crop_x_, env_->general_.image_crop_y_), 
                      cv::Size(env_->general_.image_crop_width_, env_->general_.image_crop_height_));

      show_img = source_img(rect);     
    }
    else {
      float rate = 640.0f / source_img.cols;
      
      cv::resize(source_img, show_img,
                 cv::Size(source_img.cols * rate, source_img.rows * rate), cv::INTER_LINEAR);        
    }
    
    if (is_ready_recogniztion_ == false) {
      img_for_object_detection_ = show_img.clone();

      is_ready_recogniztion_ = true;
    }

    if (use_display_ && env_->general_.only_detection_face_ == false)
      cv::imshow("image", show_img);           

    cv::waitKey(1);
  }

  return 0;
}
