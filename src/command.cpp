#include <string>
#include <map>
#include <memory>

#include <command.hpp>
#include <commands/help.hpp>
#include <commands/init.hpp>
#include <commands/status.hpp>
#include <commands/commit.hpp>
#include <commands/show.hpp>

map<string, unique_ptr<Command>> Command::create_map() {
  map<string, unique_ptr<Command>> commands;

  commands.insert(make_pair("help", unique_ptr<Command>(new Help())));
  commands.insert(make_pair("init", unique_ptr<Command>(new Init())));
  commands.insert(make_pair("status", unique_ptr<Command>(new Status())));
  commands.insert(make_pair("commit", unique_ptr<Command>(new Commit())));
  commands.insert(make_pair("show", unique_ptr<Command>(new Show())));

  return commands;
}