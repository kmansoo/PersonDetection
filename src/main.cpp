#include <iostream>

#include "opencv2/core.hpp"

#include "SmartCamera.h"

const char *keys = {"{a  ai_engine|gcp      | ai (gcp or watson) }"
                    "{c  camera   |         | use camera or not}"
                    "{d  display  |         | use displey to show video}"
                    "{f  filename |         | source video file        }"
                    "{m  method   |mog2     | method (knn or mog2) }"
                    "{r  rtsp     |         | use RTSP or not}"
                    "{s  smooth   |         | smooth the mask }"
                    "{w  writefile|         | write video file}"
};

// this is a sample for foreground detection functions
int main(int argc, const char **argv) {
  cv::CommandLineParser parser(argc, argv, keys);

  SmartCamera sc(parser);
  sc.run();

  return 0;
}
