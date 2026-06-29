#include "../include/Manager.hpp"
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>
bool Manager::create(const std::string &name, const std::string &path,
                     const std::vector<std::string> &args={})
{
  if (m_processes.find(name) != m_processes.end())
  {
    std::cerr << "Process : " << name << " already exists\n";
    return false;
  }
  auto process = std::make_unique<Process>();
  process->spawn(path, args);
  if (process->getStatus() != Process::State::Running)
  {
    return false;
  }
  m_processes[name] = std::move(process);
  return true; // Placeholder return value
}

bool Manager::stop(const std::string &name)
{
  auto it = m_processes.find(name);
  
  if (it == m_processes.end())
  {
    std::cerr << "Process : " << name << " not found\n";
    return false;
  }
  if(it->second->getStatus() != Process::State::Running)
  {
    return false;
  }
  it->second->terminate();
  it->second->wait();
  return true;
}

bool Manager::sendInput(const std::string &name, const std::string &data)
{
  auto it = m_processes.find(name);
  if (it == m_processes.end())
  {
    std::cerr << "Process : " << name << " not found\n";
    return false;
  }
  auto send = data;
  it->second->sendInput(send);
  return true;
}

std::optional<std::string> Manager::getOutput(const std::string &name)
{
  auto it = m_processes.find(name);
  if (it == m_processes.end())
  {
    std::cerr << "Process : " << name << " not found\n";
    return std::nullopt;
  }

  auto output = it->second->getOutput();
  return output;
}

void Manager::stopAll()
{
  for (auto &it : m_processes)
  {
    if (it.second->getStatus() == Process::State::Running)
    {
      it.second->terminate();
      it.second->wait();
    }
  }
}

std::vector<std::string> Manager::listAll()
{
  std::vector<std::string> names;
  for (const auto &it : m_processes)
  {
    names.push_back(it.first);
  }
  return names;
}

std::string Manager::status(const std::string &name)
{
  auto it = m_processes.find(name);
  if (it == m_processes.end())
  {
    return "Not found";
  }
  switch (it->second->getStatus())
  {
  case Process::State::Running:
    return "Running (PID: " + std::to_string(it->second->getPid()) + ")";
  case Process::State::Killed:
    return "Killed (Exit Code: " + std::to_string(it->second->getExitCode()) +
           " )";
  case Process::State::Idle:
    return "Idle (PID: " + std::to_string(it->second->getPid()) + ")";
  default:
    return "Unknown";
  }
}
