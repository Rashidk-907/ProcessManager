# Process Manager

A simple command-line tool to manage external processes in C++.

## What it does

- Start and stop processes
- Send input to running processes
- Read output from processes
- List all running processes
- Built with C++17, uses Linux system calls (fork, execvp, pipe)

## How it works
Each process runs in a child spawned via fork(). 
Two pipes connect parent to child — one for stdin, one for stdout. 
The shell loop maps process names to ProcessResource objects stored in a map.

## Requirements

- Linux (or WSL on Windows)
- C++17 compiler (g++ 7+)
- Make

## Building

```bash
git clone https://github.com/Rashidk-907/ProcessManager.git
cd ProcessManager
make
```

## Usage

```bash
./Pshell
```

## Example
```
# Start a process
pshell > run ls
[ls] started 

# List running processes
pshell > list
  ls  →  Running (PID: 1234)

# Send input and read output
pshell > run cat /bin/cat
[cat] started

pshell > send cat hello world
pshell > output cat
hello world

# Stop a process
pshell > stop cat
[cat] stopped

# Exit
pshell > exit
exit
```
