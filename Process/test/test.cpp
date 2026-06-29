#include "../include/Process.hpp"
#include <iostream>

using namespace std;

int main()
{
  Process p;

  // Spawn a command
  p.spawn("cat"); // Or use: p.Spawn("ls", {"-l", "-a"});

  std::string argv = "holaa";
  p.sendInput(argv);
  p.closeInput();
  auto output = p.getOutput();
  if (output)
  {
    cout << output.value() << endl;
  }
  else
  {
    cout << "No Output\n";
  }
  // Wait for it to complete
  p.wait();

  return 0;
}
