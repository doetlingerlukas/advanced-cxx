#pragma once

#include <string>

namespace lit {
  constexpr std::string_view DIR          = ".lit";
  constexpr std::string_view PREVIOUS_DIR = ".lit/previous";
  constexpr std::string_view REVISION_DIR = ".lit/revisions";
  constexpr std::string_view HEAD         = ".lit/head";
  constexpr std::string_view INDEX        = ".lit/index";
  constexpr std::string_view MERGE_SOURCE = ".lit/m_src";
  constexpr std::string_view MERGE_TARGET = ".lit/m_trg";
}