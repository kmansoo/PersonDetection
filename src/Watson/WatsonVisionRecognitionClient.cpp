#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream> // stringstream
#include <iterator>

#include "ccCore/ccString.h"
#include "restclient-cpp/restclient.h"

#include "json/reader.h"
#include "json/value.h"
#include "json/writer.h"

#include "../LogManager.h"
#include "WatsonVisionRecognitionClient.h"

WatsonVisionRecognitionClient::WatsonVisionRecognitionClient(std::shared_ptr<EnvironmentInfo> env_info) {

  env_info_ = env_info;

  // initialize RestClient
  RestClient::init();

  rest_conn_ = std::make_shared<RestClient::Connection>(
      "https://gateway-a.watsonplatform.net");

  // set connection timeout to 5s
  rest_conn_->SetTimeout(5);
  rest_conn_->SetUserAgent("COMMAX/SmurfHome");
  rest_conn_->SetSSLVerifyPeer(false);
}

WatsonVisionRecognitionClient::~WatsonVisionRecognitionClient() {}

bool WatsonVisionRecognitionClient::recognize(
    const std::string &filename, std::vector<std::string> &label_list) {

  RestClient::HeaderFields headers;
  headers["Accept"] = "application/json";
  headers["Content-Type"] = "image/png";
  //  headers["Accept-Language"] = "en";

  rest_conn_->SetHeaders(headers);

  std::vector<unsigned char> binary_buffer;
  std::ifstream in_file(filename, std::ios_base::binary | std::ios_base::in);
  std::istream_iterator<unsigned char> end_of_file;
  std::istream_iterator<unsigned char> in_file_iter(in_file);

  FILE *fp = fopen(filename.c_str(), "rb");

  if (fp == NULL) {
    std::cout << "File not found!, " << filename << std::endl;
    return false;
  }

  while (!feof(fp))
    binary_buffer.push_back(fgetc(fp));

  fclose(fp);

  std::string req_uri = "/visual-recognition/api/v3/classify?";

  req_uri += "api_key=";
  req_uri += env_info_->watson_.visual_recognition_api_key_;
  req_uri += "&version=2016-05-20";
  //  req_uri += "&classifier_ids=Umbrella_1561636649";

  std::string req_body(binary_buffer.begin(), binary_buffer.end());
  RestClient::Response response = rest_conn_->post(req_uri, req_body);
  LogManager::instance().addLog("VisionRecognition", false, response.body);

  std::cout << "Response:" << std::endl
            << response.code << std::endl
            << response.body << std::endl;

  if (response.code != 200) {
    std::cout << "Response:" << std::endl
              << response.code << std::endl
              << response.body << std::endl;
    return "";
  }

  try {
    Json::Reader json_reader;
    Json::Value converted_result_info;

    json_reader.parse(response.body, converted_result_info);

    if (!converted_result_info["images"].isNull()) {
      for (int index = 0;
           index <
           converted_result_info["images"][0]["classifiers"][0]["classes"].size();
           index++) {
        label_list.push_back(converted_result_info["images"][0]["classifiers"][0]["classes"][index]["class"].asString());
      }
    }
  } catch (...) {
    return false;
  }

  return true;
}
