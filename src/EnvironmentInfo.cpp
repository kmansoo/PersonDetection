#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream> // stringstream

#include "ccCore/ccString.h"

#include "json/reader.h"
#include "json/value.h"
#include "json/writer.h"

#include "EnvironmentInfo.h"

EnvironmentInfo::EnvironmentInfo() {
  this->general_.only_detection_face_ = false;
  this->general_.image_crop_ = false;
  this->general_.image_crop_x_ = 0;
  this->general_.image_crop_y_ = 0;
  this->general_.image_crop_width_ = 720;    
  this->general_.image_crop_height_ = 480;
}

EnvironmentInfo::~EnvironmentInfo() {}

bool EnvironmentInfo::getEnvironment() {
  std::string line;
  std::string json_data;
  std::ifstream env_file("env.json");

  if (env_file.is_open()) {
    while (env_file.good()) {
      std::getline(env_file, line);

      json_data += line;
    }

    env_file.close();

    Json::Reader json_reader;
    Json::Value environment_info;

    try {
      json_reader.parse(json_data, environment_info);

      try {
        if (!environment_info["General"].isNull()) {
          if (!environment_info["General"]["WALLPAD_IP"].isNull())
          this->general_.wallpad_ip_ = environment_info["General"]["WALLPAD_IP"].asCString();

          if (!environment_info["General"]["SOURCE_VIDEO_PATH"].isNull())
            this->general_.source_video_path_ = environment_info["General"]["SOURCE_VIDEO_PATH"].asCString();
          
          if (!environment_info["General"]["WRITE_VIDEO_PATH"].isNull())
            this->general_.write_video_path_ = environment_info["General"]["WRITE_VIDEO_PATH"].asCString();

          if (!environment_info["General"]["RTSP_SERVER_URI"].isNull())
            this->general_.rtsp_server_uri_ = environment_info["General"]["RTSP_SERVER_URI"].asCString();

          if (!environment_info["General"]["ONLY_DETECTION_FACE"].isNull())
            this->general_.only_detection_face_ = environment_info["General"]["ONLY_DETECTION_FACE"].asBool();        

          if (!environment_info["General"]["IMAGE_CROP"].isNull())
            this->general_.image_crop_ = environment_info["General"]["IMAGE_CROP"].asBool();        

          if (!environment_info["General"]["IMAGE_CROP_RECT"].isNull()) {            
            if (!environment_info["General"]["IMAGE_CROP_RECT"]["x"].isNull())
              this->general_.image_crop_x_ = environment_info["General"]["IMAGE_CROP_RECT"]["x"].asInt();       
            
            if (!environment_info["General"]["IMAGE_CROP_RECT"]["y"].isNull())
              this->general_.image_crop_y_ = environment_info["General"]["IMAGE_CROP_RECT"]["y"].asInt();        
              
            if (!environment_info["General"]["IMAGE_CROP_RECT"]["width"].isNull())
              this->general_.image_crop_width_ = environment_info["General"]["IMAGE_CROP_RECT"]["width"].asInt();         
            
            if (!environment_info["General"]["IMAGE_CROP_RECT"]["height"].isNull())
              this->general_.image_crop_height_ = environment_info["General"]["IMAGE_CROP_RECT"]["height"].asInt();                       
          }
          
          if (!environment_info["General"]["CASCADE_CLASSIFIER_FACE1_PATH"].isNull())
            this->general_.cascade_classifier_face1_path_ = environment_info["General"]["CASCADE_CLASSIFIER_FACE1_PATH"].asCString();     
          
          if (!environment_info["General"]["CASCADE_CLASSIFIER_FACE2_PATH"].isNull())
            this->general_.cascade_classifier_face2_path_ = environment_info["General"]["CASCADE_CLASSIFIER_FACE2_PATH"].asCString();     

        }   
      }
      catch (...) {
        std::cout << "There are some error!" << std::endl;
      }      

      try {
        if (!environment_info["Watson"].isNull()) {
          if (!environment_info["Watson"]["WORKSPACE_ID"].isNull())
            this->watson_.workspace_id_ = environment_info["Watson"]["WORKSPACE_ID"].asCString();

          if (!environment_info["Watson"]["CONVERSATION_USERNAME"].isNull())
            this->watson_.conversation_username_ =
                environment_info["Watson"]["CONVERSATION_USERNAME"].asCString();

          if (!environment_info["Watson"]["CONVERSATION_PASSWORD"].isNull())
            this->watson_.conversation_password_ =
                environment_info["Watson"]["CONVERSATION_PASSWORD"].asCString();

          if (!environment_info["Watson"]["SPEECH_TO_TEXT_USERNAME"].isNull())
            this->watson_.stt_username_ =
                environment_info["Watson"]["SPEECH_TO_TEXT_USERNAME"].asCString();

          if (!environment_info["Watson"]["SPEECH_TO_TEXT_PASSWORD"].isNull())
            this->watson_.stt_password_ =
                environment_info["Watson"]["SPEECH_TO_TEXT_PASSWORD"].asCString();

          if (!environment_info["Watson"]["TEXT_TO_SPEECH_USERNAME"].isNull())
            this->watson_.tts_username_ =
                environment_info["Watson"]["TEXT_TO_SPEECH_USERNAME"].asCString();

          if (!environment_info["Watson"]["TEXT_TO_SPEECH_PASSWORD"].isNull())
            this->watson_.tts_password_ =
                environment_info["Watson"]["TEXT_TO_SPEECH_PASSWORD"].asCString();

          if (!environment_info["Watson"]["VISUAL_RECOGNITION_URI"].isNull())
            this->watson_.visual_recognition_uri_ =
                environment_info["Watson"]["VISUAL_RECOGNITION_URI"].asCString();

          if (!environment_info["Watson"]["VISUAL_RECOGNITION_API_KEY"].isNull())
            this->watson_.visual_recognition_api_key_ =
                environment_info["Watson"]["VISUAL_RECOGNITION_API_KEY"].asCString();
        }
      } catch (...) {
        return false;
      }
      
      try {
        if (!environment_info["GCP"].isNull()) {
          if (!environment_info["GCP"]["APP_KEY"].isNull())
            this->gcp_.app_key_ = environment_info["GCP"]["APP_KEY"].asCString();
        }            
      } catch (...) {
        return false;
      }
    } catch (...) {
      return false;
    }
  } else {
    std::cout << "Unable to open file: env.json" << std::endl;

    return false;
  }

  return true;
}
