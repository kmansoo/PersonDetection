#pragma once

#include <memory>
#include <string>

#include "../VisionRecognitionClient.h"
#include "../EnvironmentInfo.h"

class WatsonVisionRecognitionClient : public VisionRecognitionClient {
public:
  WatsonVisionRecognitionClient(std::shared_ptr<EnvironmentInfo> env_info);
  ~WatsonVisionRecognitionClient();

public:
  virtual bool recognize(const std::string &imgfile,
                         std::vector<std::string> &label_list);

private:
  std::shared_ptr<EnvironmentInfo> env_info_;
  std::string token_;
};
