#pragma once

#include "restclient-cpp/connection.h"
#include <string>

class VisionRecognitionClient {
public:
  VisionRecognitionClient() {}
  ~VisionRecognitionClient() {}

public:
  virtual bool recognize(const std::string &imgfile,
                         std::vector<std::string> &label_list) = 0;

protected:
  std::shared_ptr<RestClient::Connection> rest_conn_;
};
