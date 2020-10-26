#include <string>
#include <map>
#include <memory>

#include <command.hpp>
#include <commands/help.hpp>
#include <commands/init.hpp>
#include <commands/status.hpp>
#include <commands/commit.hpp>
#include <commands/show.hpp>
#include <commands/checkout.hpp>

map<string, unique_ptr<Command>> Command::create_map() {
  map<string, unique_ptr<Command>> commands;

  commands.insert(make_pair("help", unique_ptr<Command>(new HelpCommand())));
  commands.insert(make_pair("init", unique_ptr<Command>(new InitCommand())));
  commands.insert(make_pair("status", unique_ptr<Command>(new StatusCommand())));
  commands.insert(make_pair("commit", unique_ptr<Command>(new CommitCommand())));
  commands.insert(make_pair("show", unique_ptr<Command>(new ShowCommand())));
  commands.insert(make_pair("checkout", unique_ptr<Command>(new CheckoutCommand())));

  return commands;
}