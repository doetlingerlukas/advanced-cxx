#include <iostream>
#include <string>
#include <vector>

#include <command.hpp>

using namespace std;

class HelpCommand: public Command {
  public:
    string name() const {
      return "help";
    }

    string description() const {
      return "Displays the different sub-commands and their options.";
    }

    int execute(vector<string> arguments) const {
      if (!arguments.empty()) {
        cerr << "No arguments expected for 'help'." << endl;
        return 1;
      }

      cout << "This is lit, a simple version control system." << endl;

      for (auto& command : Command::create_map()) {
        cout << command.second->name() << ":" << endl;
        cout << "\t" << command.second->description() << endl;
      }

      return 0;
    }
};