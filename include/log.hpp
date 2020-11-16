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

  LogLine(const Revision& revision, const string& message, const string& symbol)
      : revision_(revision), message_(message), symbol_(symbol), child_(false), head_(false) {}

  void print() {
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
    vector<Commit> merges;
    for (auto& r : Repository::revisions()) {
      auto c = Commit::parse(r.filepath().string());

      if (!c.parent().has_value()) {
        lines.push_back(LogLine(c.revision(), c.message(), string("o")));
      } else {
        for (auto& l : lines) {
          if (l.revision_ == c.parent().value()) {
            if (l.child_) {
              auto cached_symbol = l.symbol_;
              for (auto& l : lines) {
                if (l.revision_ == l.revision_) {
                  l.symbol_ += "---|";
                } else if (l.revision_ > l.revision_) {
                  l.symbol_ += "   |";
                }
              }
              lines.push_back(LogLine(c.revision(), c.message(), "|   " + cached_symbol));
            } else {
              l.child_ = true;
              lines.push_back(LogLine(c.revision(), c.message(), l.symbol_));
            }
          }
        }
      }

      if (c.merge_parent().has_value()) {
        merges.push_back(c);
      }
    }

    for (auto& c : merges) {
      auto& line = lines.at(c.revision().id());
      replace(lines.back().symbol_.begin(), lines.back().symbol_.end(), ' ', '-');
    }

    lines.at(Repository::get_head().value_or(Revision(0)).id()).head_ = true;
    replace(lines.back().symbol_.begin(), lines.back().symbol_.end(), '|', ' ');
  }

  void print() {
    for (auto it = this->lines.rbegin(); it != lines.rend(); it++) {
      (*it).print();
    }
  }
};

} // namespace lit