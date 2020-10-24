#pragma once

#ifdef __WIN32__
#define popen  _popen
#define pclose _pclose
#endif

#include <string>
#include <array>

std::pair<int, std::string> system_popen(const std::string& command) {
  std::array<char, 128> buffer;
  std::string result;

  auto pipe = popen(command.c_str(), "r");

  if (!pipe) throw std::runtime_error("popen() failed!");
  while (!feof(pipe)) {
    if (fgets(buffer.data(), 128, pipe) != nullptr) result += buffer.data();
  }
  auto status = pclose(pipe);
  
  return make_pair(status, result);
}