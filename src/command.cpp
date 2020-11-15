#include <map>
#include <memory>
#include <string>

#include <command.hpp>
#include <commands/checkout.hpp>
#include <commands/commit.hpp>
#include <commands/help.hpp>
#include <commands/init.hpp>
#include <commands/log.hpp>
#include <commands/merge.hpp>
#include <commands/show.hpp>
#include <commands/status.hpp>

map<string, unique_ptr<Command>> Command::create_map() {
  map<string, unique_ptr<Command>> commands;

  commands.insert(make_pair("help", unique_ptr<Command>(new HelpCommand())));
  commands.insert(make_pair("init", unique_ptr<Command>(new InitCommand())));
  commands.insert(make_pair("status", unique_ptr<Command>(new StatusCommand())));
  commands.insert(make_pair("commit", unique_ptr<Command>(new CommitCommand())));
  commands.insert(make_pair("show", unique_ptr<Command>(new ShowCommand())));
  commands.insert(make_pair("checkout", unique_ptr<Command>(new CheckoutCommand())));
  commands.insert(make_pair("merge", unique_ptr<Command>(new MergeCommand())));
  commands.insert(make_pair("log", unique_ptr<Command>(new LogCommand)));

  return commands;
}