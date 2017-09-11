#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../EnvironmentInfo.h"

#include "../VisionRecognitionClient.h"

class GcpVisionRecognitionClient : public VisionRecognitionClient {
public:
  GcpVisionRecognitionClient(std::shared_ptr<EnvironmentInfo> env_info);
  ~GcpVisionRecognitionClient();

public:
  virtual bool recognize(const std::string &imgfile,
                         std::vector<std::string> &label_list);

private:
  std::shared_ptr<EnvironmentInfo> env_info_;
  std::string gcp_key_;
};
