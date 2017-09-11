#include "GcpVisionRecognitionClient.h"

#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream> // stringstream
#include <thread>

#include "ccCore/ccCoreAPI.h"
#include "ccCore/ccString.h"
#include "restclient-cpp/restclient.h"

#include "json/reader.h"
#include "json/value.h"
#include "json/writer.h"

#include "../LogManager.h"

GcpVisionRecognitionClient::GcpVisionRecognitionClient(std::shared_ptr<EnvironmentInfo> env_info) {
  env_info_ = env_info;

  gcp_key_ = env_info_->gcp_.app_key_;

  // initialize RestClient
  RestClient::init();

  rest_conn_ =
      std::make_shared<RestClient::Connection>("https://vision.googleapis.com");

  // set connection timeout to 5s
  rest_conn_->SetTimeout(5);
  rest_conn_->SetUserAgent("COMMAX/SmurfHome");
  rest_conn_->SetSSLVerifyPeer(false);
}

GcpVisionRecognitionClient::~GcpVisionRecognitionClient() {}

bool GcpVisionRecognitionClient::recognize(
    const std::string &filename, std::vector<std::string> &label_list) {

  std::ifstream fin(filename.c_str());

  if (!fin) {
    std::cout << "File not found!, " << filename << std::endl;
    return false;
  }

  Luna::ccString converted_image_data;
  Luna::ccString::encode_base64(converted_image_data, fin);

  std::string req_uri = "/v1/images:annotate?key=";

  req_uri += gcp_key_;

  RestClient::HeaderFields headers;
  headers["Content-Type"] = "application/json";
  rest_conn_->SetHeaders(headers);

  std::string req_body = R"(
  {
    "requests": [
      {
        "image": {
          "content": ")";
  req_body += converted_image_data;
  req_body += R"("
        },
        "features": [
          {
            "type": "LABEL_DETECTION"
          }
        ]
      }
    ]
  }
  )";

  RestClient::Response response;

  std::thread call_api ([&]() {
      response = rest_conn_->post(req_uri, req_body);
    }
  );

  call_api.join();

  LogManager::instance().addLog("VisionRecognition", false, response.body);
 
  if (response.code != 200) {
    std::cout << "Response:" << std::endl
              << response.code << std::endl
              << response.body << std::endl;
    return "";
  }
  else {
    /*
      std::cout << "Response:" << std::endl
      << response.code << std::endl
      << response.body << std::endl;
    */
  }

  try {
    Json::Reader json_reader;
    Json::Value converted_result_info;

    json_reader.parse(response.body, converted_result_info);

    if (!converted_result_info["responses"].isNull()) {
      for (int index = 0;
           index <
           converted_result_info["responses"][0]["labelAnnotations"].size();
           index++) {

            Json::Value value = converted_result_info["responses"][0]["labelAnnotations"][index];

            std::string label = value["description"].asString();

            if (label == "sunglasses" || label == "umbrella" || label == "cap") {
              label_list.push_back(label);
            }
      }
    }
  } catch (...) {
    return false;
  }

  return true;
}
