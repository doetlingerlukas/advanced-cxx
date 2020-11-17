#pragma once

#include <filesystem>
#include <string>

#include <subprocess.hpp>

namespace fs = std::filesystem;

namespace lit {

class Patch {
  public:
  static void apply(fs::path patchfile) {
    system_popen("patch -s -p0 -i " + patchfile.string());
  }

  static void apply(fs::path patchfile, fs::path wd) {
    system_popen("patch -d " + wd.string() + " -s -p0 -i " + patchfile.string());
  }
};

} // namespace lit