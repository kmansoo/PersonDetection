#pragma once

#include <fstream>
#include <memory>
#include <string>

#include "json/value.h"

class EnvironmentInfo {
public:
  EnvironmentInfo();
  ~EnvironmentInfo();

public:
  bool getEnvironment();

public:
  struct GeneralInfo {
    std::string wallpad_ip_;
    std::string source_video_path_;
    std::string write_video_path_;
    std::string rtsp_server_uri_;
    bool only_detection_face_;
    bool image_crop_;
    int image_crop_x_;
    int image_crop_y_;
    int image_crop_width_;
    int image_crop_height_;

    std::string cascade_classifier_face1_path_; //  for face detection
    std::string cascade_classifier_face2_path_; //  for face detection
  };

  struct WatsonInfo {
    std::string workspace_id_;
    std::string conversation_username_;
    std::string conversation_password_;
  
    std::string stt_username_;
    std::string stt_password_;
  
    std::string tts_username_;
    std::string tts_password_;
  
    std::string visual_recognition_uri_;
    std::string visual_recognition_api_key_;       
  };  

  struct GCPInfo {
    std::string app_key_;    
  };    

public:
  GeneralInfo general_;
  WatsonInfo  watson_;
  GCPInfo     gcp_;
};
