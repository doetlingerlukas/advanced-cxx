#include <deque>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include <commit.hpp>
#include <constants.hpp>
#include <repository.hpp>
#include <revision.hpp>

namespace fs = std::filesystem;

namespace lit {

using namespace std;

class LogLine {
  Revision revision_;
  string message_;
  string symbol_;
  bool child_;
  bool head_;
  bool merge_;

  LogLine(const Revision& revision, const string& message, const string& symbol, optional<Revision> merge)
      : revision_(revision), message_(message), symbol_(symbol), child_(false), head_(false), merge_(merge.has_value()) {}

  void print() {
    if (merge_) {
      replace(symbol_.begin(), symbol_.end(), ' ', '-');
    }
    for (auto i = 0; i < 12 - symbol_.size(); i++) {
      symbol_ += ' ';
    }
    cout << symbol_ << (head_ ? "<- " : "   ") << revision_.to_string() << "  '" << message_ << "'" << endl;
  }

  friend class Log;
};

class Log {
  vector<LogLine> lines;

  public:
  Log() {
    for (auto& r : Repository::revisions()) {
      auto c = Commit::parse(r.filepath().string());

      if (!c.parent().has_value()) {
        lines.push_back(LogLine(c.revision(), c.message(), string("o"), c.merge_parent()));
      } else {
        for (auto& l : lines) {
          if (l.revision_ == c.parent().value()) {
            if (l.child_) {
              auto cached_symbol = l.symbol_;
              update_lines(l.revision_);
              lines.push_back(LogLine(c.revision(), c.message(), "|   " + cached_symbol, c.merge_parent()));
            } else {
              l.child_ = true;
              lines.push_back(LogLine(c.revision(), c.message(), l.symbol_, c.merge_parent()));
            }
          }
        }
      }
    }

    lines.at(Repository::get_head().value_or(Revision(0)).id()).head_ = true;
    if (!lines.back().merge_) {
      replace(lines.back().symbol_.begin(), lines.back().symbol_.end(), '|', ' ');
    }
  }

  void update_lines(const Revision revision) {
    for (auto& l : lines) {
      if (l.revision_ == revision) {
        l.symbol_ += "---|";
      } else if (l.revision_ > revision) {
        l.symbol_ += "   |";
      }
    }
  }

  void print() {
    for (auto it = this->lines.rbegin(); it != lines.rend(); it++) {
      (*it).print();
    }
  }
};

} // namespace lit