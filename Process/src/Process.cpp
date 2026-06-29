#include "../include/Process.hpp"
#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <optional>
#include <fcntl.h>
#include <random>
#include <signal.h>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

//   Constructors
Process::Process(Process &&other) noexcept
    : m_pid(other.m_pid), m_state(other.m_state), m_exitCode(other.m_exitCode),
      m_readfd(other.m_readfd), m_writefd(other.m_writefd)
{
  other.m_pid = -1;
  other.m_state = State::Killed;
}

Process &Process::operator=(Process &&other) noexcept
{

  if (this != &other)
  {
    if (m_pid != -1 && m_state == State::Running)
    {
      kill(m_pid, SIGKILL);
    }
    m_pid = other.m_pid;
    m_exitCode = other.m_exitCode;
    m_state = other.m_state;
    other.m_pid = -1;
    other.m_state = State::Killed;
    other.m_readfd = m_readfd;
    other.m_writefd = m_writefd;
  }
  return *this;
}

Process::~Process()
{
  if (m_state == State::Running)
  {
    kill(m_pid, SIGKILL);
    wait();
  }
  if (m_readfd != -1)
    close(m_readfd);
  if (m_writefd != -1)
    close(m_writefd);
}

// Process Control

void Process::spawn(const std::string path, const std::vector<std::string> &args)
{
  if (m_state == State::Running)
  {
    std::cerr << "Already Running\n";
    return;
  }

  int to_child[2];
  int from_child[2];
  if (pipe(to_child) == -1 || pipe(from_child) == -1)
  {
    std::cerr << "Pipe() failed\n";
    return;
  }

  m_pid = fork();
  if (m_pid == -1)
  {
    std::cerr << "Fork() failed\n";
    ::close(to_child[0]);
    ::close(to_child[1]);
    ::close(from_child[0]);
    ::close(from_child[1]);
    return;
  }

  if (m_pid == 0)
  {
    auto argv = getArguments(path, args);
    ::close(to_child[1]);
    ::close(from_child[0]);

    ::dup2(to_child[0], STDIN_FILENO);
    ::dup2(from_child[1], STDOUT_FILENO);

    ::close(to_child[0]);
    ::close(from_child[0]);

    execvp(path.data(), argv.data());
    std::cerr << "execvp failed : " << std::strerror(errno) << "\n";
    return;
  }

  ::close(to_child[0]);
  ::close(from_child[1]);

  m_readfd = from_child[0];
  m_writefd = to_child[1];
  m_state = State::Running;
}

void Process::wait()
{

  if (m_state != State::Running)
    return;

  int status;
  int result = waitpid(m_pid, &status, 0);

  if (result == -1)
    std::cerr << "Waitpid Failed\n";

  m_state = State::Killed;
  if (WIFEXITED(status))
  {
    m_exitCode = WEXITSTATUS(status);
  }
  else if (WIFSIGNALED(status))
  {
    m_exitCode = -WTERMSIG(status);
  }
}

void Process::terminate()
{
  if (m_state == State::Running)
  {
    kill(m_pid, SIGKILL);
  }
}

std::vector<char *> Process::getArguments(const std::string &path, const std::vector<std::string> &args)
{
  std::vector<char *> argv;
  argv.push_back(const_cast<char *>(path.c_str()));
  for (const auto &arg : args)
  {
    argv.push_back(const_cast<char *>(arg.c_str()));
  }
  argv.push_back(nullptr);
  return argv;
}

//  Pipe I/O

void Process::sendInput(std::string &data)
{
  if (m_writefd == -1)
  {
    std::cerr << "No Pipe\n";
    return;
  }
  std::string withNewline = data + "\n";
  ssize_t wr = ::write(m_writefd, data.data(), data.size());
  if (wr < 0)
  {
    std::cerr << "Error Reading File\n";
    return;
  }
}

void Process::closeInput()
{
  if (m_writefd != -1)
  {
    ::close(m_writefd);
    m_writefd = -1;
  }
}

std::optional<std::string> Process::getOutput() {
    if (m_readfd == -1) {
        return std::nullopt;
    }

    // Set pipe to non-blocking
    int flags = fcntl(m_readfd, F_GETFL, 0);
    fcntl(m_readfd, F_SETFL, flags | O_NONBLOCK);

    std::string data;
    char buffer[4096];
    ssize_t bytes;

    // Read until no more data available (non-blocking)
    while ((bytes = ::read(m_readfd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes] = '\0';
        data += buffer;
    }

    // Restore blocking mode
    fcntl(m_readfd, F_SETFL, flags);

    return data.empty() ? std::nullopt : std::optional<std::string>(data);
}

pid_t Process::getPid() const
{
  return m_pid;
}
bool Process::isRunning() const
{
  return m_state == State::Running;
}

Process::State Process::getStatus() const
{
  return m_state;
}

int Process::getExitCode() const
{
  return m_exitCode;
}