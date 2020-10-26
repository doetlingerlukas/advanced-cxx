#pragma once

#include <string>
#include <filesystem>

#include <sub-process.hpp>

namespace fs = std::filesystem;

class Patch {
  public:
    static void apply(fs::path patchpath) {
      system_popen("patch -s -p0 < " + patchpath.string());
    }
};