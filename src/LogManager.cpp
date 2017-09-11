#include "LogManager.h"

#include <ctime>
#include <iomanip>
#include <iostream>

// 2017.6.28, Mansoo(mansoo.kim@icloud.com), I'll implement it as soon as
// possible.

LogManager::LogManager() { log_file_.open("log.txt"); }

LogManager::~LogManager() {}

void LogManager::addLog(const std::string &tag, bool isReq,
                        const std::string &log) {
  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);

  log_file_ << "[" << tag << "] ";

  if (isReq)
    log_file_ << "[REQ] ";
  else
    log_file_ << "[***] ";

  log_file_ << std::put_time(&tm, "%d-%m-%Y %H-%M-%S: ") << log << std::endl;
}
