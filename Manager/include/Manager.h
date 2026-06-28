#pragma once
#include "../../Process/include/process.h"
#include <memory>
#include <string>
#include <unordered_map>
class Manager {
private:
  // Choice 1: By name (easy lookup)
  std::unordered_map<std::string, std::unique_ptr<Process>> m_processes;

public:
  // Create and start a process
  bool create(const std::string &name, const std::string &path,
              const std::vector<std::string> &args);

  // Stop a specific process
  bool stop(const std::string &name);
  // Send input to a specific process
  bool sendInput(const std::string &name, const std::string &data);

  // Get output from a specific process
  std::optional<std::string> getOutput(const std::string &name);

  // Stop ALL processes
  void stopAll();

  // Listl all processes
  std::vector<std::string> listAll();

  // Get status of a specific process
  std::string status(const std::string &name);
};
