#pragma once

#include <filesystem>
#include <string>

#include <sub-process.hpp>

namespace fs = std::filesystem;

class Patch {
  public:
  static void apply(fs::path patchfile) {
    system_popen("patch -s -p0 -i " + patchfile.string());
  }

  static void apply(fs::path patchfile, fs::path wd) {
    system_popen("patch -d " + wd.string() + " -s -p0 -i " + patchfile.string());
  }
};