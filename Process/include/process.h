#ifndef PROCESS_H
#define PROCESS_H

#include <fcntl.h>
#include <iomanip>
#include <optional>
#include <string>
#include <sys/types.h>
#include <vector>

class Process
{

public:
  // state
  enum class State
  {
    Idle,
    Running,
    Killed
  };

  // constructors
  Process() = default;
  Process(const Process &obj) = delete;
  Process operator=(const Process &obj) = delete;
  Process(Process &&other) noexcept;
  Process &operator=(Process &&other) noexcept;
  ~Process();

  // process control
  void spawn(const std::string path,
             const std::vector<std::string> &args = {});
  void wait();
  void terminate();

  // pipe I/O
  void sendInput(std::string &data);
  void closeInput();
  std::optional<std::string> getOutput();

  // getters
  pid_t getPid() const;
  int getExitCode() const;
  State getStatus() const;
  std::optional<int> exitCode() const;
  bool isRunning() const;

private:
  std::vector<char *> getArguments(const std::string &path,
                                   const std::vector<std::string> &args);

private:
  pid_t m_pid{-1};
  State m_state{State::Idle};
  int m_exitCode{-1};
  int m_readfd{-1};
  int m_writefd{-1};

};

#endif // PROCESSRESOURCE_H