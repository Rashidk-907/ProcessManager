#include "../Include/shell.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>

Shell::Shell() : m_running(true) {}

std::vector<std::string> Shell::parseString(const std::string &input)
{
  std::vector<std::string> tokens;
  std::stringstream ss(input);
  std::string token;

  while (ss >> token)
  {
    tokens.push_back(token);
  }

  return tokens;
}

void Shell::run(const std::string &name, const std::vector<std::string> &data)
{
  std::vector<std::string> arguments;
  std::string processName;
  std::string command;

  if (data.size() == 2)
  {
    // Case: "run ls","run pwd", "run echo"
    processName = data[1];
    command = data[1];
  }
  else if (data.size() >= 3)
  {
    processName = name;
    command = data[2];
    for (size_t i = 3; i < data.size(); i++)
    {
      arguments.push_back(data[i]);
    }
  }
  else
  {
    printError("run", "Usage: run <name> <command> [args...] or run <command>");
    return;
  }

  if (m_manager.create(processName, command, arguments))
  {
    std::cout << "[" << processName << "] started \n";
  }
  else
  {
    printError(processName, "failed to start");
  }
}

void Shell::send(const std::string &name, const std::vector<std::string> &data)
{
  if (data.size() < 3)
  {
    printError("send", "Usage: send <name> <message>");
    return;
  }

  // Rebuild message from remaining tokens (preserve spaces)
  std::string message;
  for (size_t i = 2; i < data.size(); i++)
  {
    if (i > 2)
      message += " ";
    message += data[i];
  }

  if (!m_manager.sendInput(name, message))
  {
    printError(name, "failed to send input");
  }
}

void Shell::listAll()
{
  auto list = m_manager.listAll();
  if (list.empty())
  {
    std::cout << "(no processes)\n";
  }
  else
  {
    std::cout << "\n";
    for (const auto &name : list)
    {
      std::cout << "  " << name << "  →  " << m_manager.status(name) << "\n";
    }
    std::cout << "\n";
  }
}

void Shell::output(const std::string &name)
{
  auto result = m_manager.getOutput(name);
  if (result.has_value())
  {
    std::cout << result.value() << "\n";
  }
  else
  {
    std::cerr<<"" << name << ": failed to get output\n";
  }
  
}

void Shell::execute(const std::string &input)
{
  if (input.empty())
    return;

  auto data = parseString(input);
  if (data.empty())
    return;

  const std::string &cmd = data[0];

  // Commands without arguments
  if (cmd == "help" || cmd == "h" || cmd == "?")
  {
    printHelp();
    return;
  }

  if (cmd == "cls" || cmd == "clear")
  {
    std::cout << "\033[2J\033[1;1H"; // ANSI escape code to clear the terminal
    return;
  }

  if (cmd == "exit" || cmd == "quit")
  {
    std::cout << "exit\n";
    m_running = false;
    return;
  }

  if (cmd == "list" || cmd == "ls")
  {
    listAll();
    return;
  }

  if (cmd == "stopall")
  {
    m_manager.stopAll();
    std::cout << "all processes stopped\n";
    return;
  }

  // Commands with arguments
  if (data.size() < 2)
  {
    printError(cmd, "missing argument");
    return;
  }

  const std::string &name = data[1];

  if (cmd == "run" || cmd == "start")
  {
    if (data.size() < 2)
    {
      printError("run", "Usage: run <name> <command> [args...]");
      return;
    }
    run(name, data);
    return;
  }

  if (cmd == "stop" || cmd == "kill")
  {
    if (m_manager.stop(name))
    {
      std::cout << "[" << name << "] stopped\n";
    }
    else
    {
      printError("stop", name + " not found");
    }
    return;
  }

  if (cmd == "status" || cmd == "stat")
  {
    std::cout << m_manager.status(name) << "\n";
    return;
  }

  if (cmd == "send" || cmd == "in")
  {
    send(name, data);
    return;
  }

  if (cmd == "output" || cmd == "out")
  {
    output(name);
    return;
  }

  // @todo add remove command

  // if (cmd == "remove" || cmd == "rm") {
  //     if (m_manager.remove(name)) {
  //         std::cout << "[" << name << "] removed\n";
  //     } else {
  //         printError("remove", "failed to remove " + name);
  //     }
  //     return;
  // }

  std::cout << "unknown command: " << cmd << " (try help)\n";
}

void Shell::printError(const std::string &name, const std::string &msg) const
{
  std::cerr << name << ": " << msg << "\n";
}

void Shell::printHelp() const
{
  std::cout << "\n"
            << "  run     <name> <cmd> [args]   start a process\n"
            << "  list                          list all processes\n"
            << "  status  <name>                show process status\n"
            << "  send    <name> <text>         send input to process\n"
            << "  output  <name>                get output from process\n"
            << "  stop    <name>                stop a process\n"
            << "  stopall                       stop all processes\n"
            << "  remove  <name>                remove stopped process\n"
            << "  help                          show this help\n"
            << "  exit                          exit shell\n"
            << "\n"
            << "  Aliases: ls=list, kill=stop, in=send, out=output, rm=remove\n"
            << "\n";
}

void Shell::stop()
{
  m_running = false;
}

bool Shell::isRunning() const
{
  return m_running;
}