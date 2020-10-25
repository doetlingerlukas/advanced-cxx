#pragma once

#include <fstream>

#include <constants.hpp>
#include <revision.hpp>
#include <optional>

namespace lit {

using namespace std;

class Repository {
  public:
    static optional<Revision> get_head() {
      ifstream head_file;
      head_file.open(string(lit::HEAD));

      string head_revision;
      getline(head_file, head_revision);
      head_file.close();

      if (head_revision.empty()) {
        return nullopt;
      }
      return optional(Revision::from_id(head_revision));
    }

    static void set_head(const Revision& revision) {
      ofstream head_file;
      head_file.open(string(lit::HEAD), ofstream::out | ofstream::trunc);

      head_file << revision.id();
      head_file.close();
    }
};

} // namespace lit
