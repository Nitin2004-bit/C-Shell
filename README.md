# C-Shell: A Custom POSIX-Compliant UNIX Shell in C

[![Language: C](https://img.shields.io/badge/Language-C-00599C.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Platform: Linux](https://img.shields.io/badge/Platform-Linux%20%2F%20POSIX-orange.svg)](https://pubs.opengroup.org/onlinepubs/9699919799/)
[![Standard: POSIX](https://img.shields.io/badge/Standard-POSIX.1-blue.svg)](https://pubs.opengroup.org/)
[![Course: OSN](https://img.shields.io/badge/Course-OSN%20Monsoon%202024-green.svg)](https://github.com/OSN-Monsoon-2024)

A modular, feature-rich Unix command-line interpreter developed in C for the **Operating Systems and Networks (OSN Monsoon 2024)** course at IIIT Hyderabad. 

The shell implements process control, job management, inter-process communication (pipes & I/O redirection), signal trapping, dynamic runtime profiling, network socket communication, terminal configuration, and a custom startup configuration file (`.myshrc`).

---

## Table of Contents

- [Overview](#overview)
- [Architecture & Design](#architecture--design)
- [Repository Structure](#repository-structure)
- [Features & Specifications](#features--specifications)
  - [1. Shell Prompt & Execution Profiling](#1-shell-prompt--execution-profiling)
  - [2. Directory Navigation: `hop`](#2-directory-navigation-hop)
  - [3. Directory Listing: `reveal`](#3-directory-listing-reveal)
  - [4. Command History Management: `log`](#4-command-history-management-log)
  - [5. System Command Execution (Foreground & Background)](#5-system-command-execution-foreground--background)
  - [6. Process State Inspection: `proclore`](#6-process-state-inspection-proclore)
  - [7. Recursive File Search: `seek`](#7-recursive-file-search-seek)
  - [8. Startup Configuration & Customization: `.myshrc`](#8-startup-configuration--customization-myshrc)
  - [9. I/O Redirection](#9-io-redirection)
  - [10. Command Pipelines](#10-command-pipelines)
  - [11. Process Activity Tracking: `activities`](#11-process-activity-tracking-activities)
  - [12. Signal Dispatch: `ping`](#12-signal-dispatch-ping)
  - [13. Signal Trapping & Terminal Control](#13-signal-trapping--terminal-control)
  - [14. Job Control: `fg` and `bg`](#14-job-control-fg-and-bg)
  - [15. Process Creation Monitor: `neonate`](#15-process-creation-monitor-neonate)
  - [16. Networked Manual Fetcher: `iMan`](#16-networked-manual-fetcher-iman)
- [Build and Run](#build-and-run)
- [Testing & Example Sessions](#testing--example-sessions)
- [Authors & Acknowledgments](#authors--acknowledgments)

---

## Overview

```text
+---------------------------------------------------------------------------------+
|                                    C-SHELL                                      |
+---------------------+-------------------------------+---------------------------+
|  Built-in Commands  |       Process Management      |      I/O & Pipelines      |
+---------------------+-------------------------------+---------------------------+
| • hop (cd)          | • Foreground Execution        | • Input Redirection (<)   |
| • reveal (ls)       | • Background Execution (&)    | • Output Redirection (>)  |
| • log (history)     | • proclore (/proc inspection) | • Append Redirection (>>) |
| • seek (find)       | • activities (job table)      | • Arbitrary Multi-Pipes   |
| • .myshrc (aliases) | • fg / bg (job control)       | • Combined Pipe & I/O     |
+---------------------+-------------------------------+---------------------------+
|               Signals               |                  Networking               |
+-------------------------------------+-------------------------------------------+
| • Ctrl + C (SIGINT foreground trap) | • iMan (TCP Socket Client to man.he.net)  |
| • Ctrl + Z (SIGTSTP job suspension) | • HTTP GET & HTML Stream Parser           |
| • Ctrl + D (EOF shell logout)       | • neonate (/proc/loadavg PID polling)     |
| • ping <pid> <signal> (kill dispatch)| • Raw non-blocking terminal mode (termios)|
+-------------------------------------+-------------------------------------------+
```

---

## Architecture & Design

The shell operates through an event-driven Read-Eval-Print Loop (REPL) backed by modular subsystems:

```mermaid
flowchart TD
    Prompt[1. Display Prompt <user@sys:~>] --> Read[2. Read & Tokenize Input]
    Read --> Parse[3. Alias Expansion & Syntax Parsing]
    Parse --> Pipes{Contains Pipes '|'?}
    Pipes -->|Yes| PipeExec[Handle Pipe Chain with pipe and dup2]
    Pipes -->|No| Redir{Contains Redirection '<', '>', '>>'?}
    Redir -->|Yes| RedirExec[Setup File Descriptors & Execute]
    Redir -->|No| Dispatch{Built-in or External?}
    Dispatch -->|Built-in| BuiltinExec[Execute hop, reveal, log, seek, proclore, etc.]
    Dispatch -->|External| ForkExec[fork and execvp: Foreground or Background]
    BuiltinExec --> Prompt
    ForkExec --> Prompt
    PipeExec --> Prompt
    RedirExec --> Prompt
```

---

## Repository Structure

```text
C-Shell/
├── Makefile            # Build configuration with targets: all, shell, clean, run
├── .gitignore          # Build artifacts and runtime scratch files filter
├── .myshrc             # Shell startup script containing aliases and functions
├── README.md           # Master documentation and command specification manual
├── header.h            # Global declarations, structs, constants, and includes
├── main.c              # REPL loop, signal handling setup, process reaping
├── basicreq.c / .h     # Prompt generation and runtime execution time profiling
├── hop.c / .h          # 'hop' directory change command
├── reveal.c / .h       # 'reveal' directory listing command
├── logs.c / .h         # 'log' command history tracker and file persistence
├── execut.c / .h       # Execution routing and background process registration
├── othercom.c / .h     # External process execution via fork() and execvp()
├── proclore.c / .h     # Process inspection reading from /proc filesystem
├── seek.c / .h         # Recursive file/folder search with auto-execute mode
├── myshrc.c / .h       # Parser for aliases and custom functions in .myshrc
├── io.c / .h           # Input (<) and output (>, >>) redirection handler
├── pipes.c / .h        # Multi-stage inter-process communication pipe engine
├── activities.c / .h   # Job table tracker for active background processes
├── ping.c / .h         # Signal dispatch utility for target PIDs
├── fgandbg.c / .h      # Job control commands for terminal and process foregrounding
├── neonate.c / .h      # Real-time process PID monitor with non-blocking keypresses
└── iMan.c / .h         # Internet manual page fetcher via raw TCP sockets
```

---

## Features & Specifications

### 1. Shell Prompt & Execution Profiling
- **Default Format**: `<Username@SystemName:~>`
- **Relative Path Resolution**: Automatically substitutes the directory from which the shell was invoked with `~`. Paths within subdirectories are displayed relative to `~` (e.g. `<nitin@thinkpad:~/OSN>`).
- **Foreground Runtime Profiling**: If a foreground command takes $> 2$ seconds to finish, the elapsed time rounded to the nearest integer is displayed in the next prompt:
  ```text
  <nitin@thinkpad:~> sleep 5
  <nitin@thinkpad:~;sleep 5:5s>
  ```

---

### 2. Directory Navigation: `hop`
Replaces standard `cd` with enhanced sequential path traversal:
- **Shorthand Flags**:
  - `hop ~`: Navigates to the shell's root home directory.
  - `hop .`: Remains in the current working directory.
  - `hop ..`: Navigates to the parent directory.
  - `hop -`: Switches to the previous working directory (maintained across hops).
- **Sequential Navigation**: Supports multiple sequential arguments processed from left to right:
  ```bash
  hop dir1 dir2 ..
  ```
  Prints the resolved absolute path after every transition.

---

### 3. Directory Listing: `reveal`
Replaces standard `ls` with formatted, color-coded output:
- **Supported Flags**:
  - `-a`: Displays all entries including hidden files starting with `.`.
  - `-l`: Detailed listing displaying file permissions, hard links, owner, group, size in bytes, and last modified date.
  - Combined flags: `-a -l`, `-l -a`, `-la`, `-al`.
- **Target Resolution**: Supports relative paths, absolute paths, `~`, `.`, and `..`.
- **Color Coding**:
  - **Green**: Executable files (`stat` check with `S_IXUSR`).
  - **Blue**: Directories (`S_ISDIR`).
  - **White**: Regular text and data files.

---

### 4. Command History Management: `log`
Maintains command history across sessions, persisting up to 15 unique commands into `logs.txt`:
- **Commands**:
  - `log`: Prints history from oldest to newest.
  - `log purge`: Erases history both from memory and `logs.txt`.
  - `log execute <index>`: Re-executes the command at the 1-based index (where index 1 is the most recent command).
- **Smart Filtering**:
  - **Deduplication**: If a command is identical to the immediately preceding command, it is not duplicated in history.
  - **Exclusion**: Commands containing the keyword `log` are ignored to prevent recursive history pollution.

---

### 5. System Command Execution (Foreground & Background)
- **Foreground Execution**: Normal system binaries (e.g. `vim`, `gcc`, `cat`, `ls`) run synchronously. The shell waits for process completion using `waitpid()`.
- **Background Execution (`&`)**: Appending `&` to any valid command spawns it in the background:
  ```bash
  sleep 10 &
  ```
  - Displays the background PID: `[1] 34120`.
  - Does not block the shell prompt.
  - Asynchronously reaps zombie processes via `SIGCHLD` and prints exit status upon termination:
    ```text
    sleep with pid 34120 exited normally
    ```

---

### 6. Process State Inspection: `proclore`
Gathers and displays kernel process metrics by querying `/proc/[pid]/`:
- **Usage**: `proclore [pid]` (defaults to the shell process if no PID is given).
- **Output Fields**:
  - `pid`: Process identifier.
  - `Process Status`: Process state code (`R` for Running, `S` for Sleeping, `Z` for Zombie). Appends `+` if the process is in the foreground group.
  - `Process Group`: Process group ID (`pgrp`).
  - `Virtual Memory`: Memory footprint size in bytes (`vsize`).
  - `Executable Path`: Absolute path to the executing binary (resolved by reading `/proc/[pid]/exe`).

---

### 7. Recursive File Search: `seek`
Recursively traverses directory trees looking for file or directory name matches:
- **Usage**: `seek [-d] [-f] [-e] <search_pattern> [target_directory]`
- **Flags**:
  - `-d`: Search for directories only.
  - `-f`: Search for files only (`-d` and `-f` cannot be used together).
  - `-e`: Execute mode when exactly one match is identified:
    - If the sole match is a file: prints file contents to stdout.
    - If the sole match is a directory: automatically changes directory into it.
- Prints `No match found!` if no files match the criteria.

---

### 8. Startup Configuration & Customization: `.myshrc`
Executed automatically when the shell boots to register aliases and custom compound functions:
- **Aliases**:
  ```bash
  alias reveall=reveal -l
  alias home=hop ~
  ```
- **Shell Functions**:
  ```bash
  mk_hop() {
      mkdir $1
      hop $1
  }
  ```

---

### 9. I/O Redirection
Supports standard Unix file descriptor reassignment:
- `< input_file`: Redirects standard input.
- `> output_file`: Redirects standard output, overwriting existing contents.
- `>> output_file`: Appends standard output to the file.
- **Combined Redirection**:
  ```bash
  cat < input.txt > output.txt
  sort < names.txt >> all_names.txt
  ```
- Validates file existence and permissions before execution, returning clear diagnostic error messages.

---

### 10. Command Pipelines
Chains multiple commands together using anonymous pipes (`|`):
- Connects stdout of command $i$ directly to stdin of command $i+1$:
  ```bash
  cat names.txt | grep Smith | sort | uniq -c
  ```
- Works seamlessly in combination with input and output redirection:
  ```bash
  cat < input.txt | grep pattern > filtered.txt
  ```

---

### 11. Process Activity Tracking: `activities`
Displays an inventory of all active background processes spawned by the current shell session:
- **Usage**: `activities`
- **Output**:
  ```text
  1234 : sleep 10 - Running
  1238 : vim notes.txt - Stopped
  ```
- Entries are sorted lexicographically by command name.

---

### 12. Signal Dispatch: `ping`
Sends a signal to a process identified by PID:
- **Usage**: `ping <pid> <signal_number>`
- The signal number is applied modulo 32:
  ```bash
  ping 1234 9     # Sends SIGKILL
  ping 1238 18    # Sends SIGCONT
  ```

---

### 13. Signal Trapping & Terminal Control
- **`Ctrl + C` (`SIGINT`)**: Sends `SIGINT` to the running foreground process group, interrupting it without terminating the shell.
- **`Ctrl + Z` (`SIGTSTP`)**: Sends `SIGTSTP` to the current foreground process, pushing it into the background and marking it as `Stopped`. The shell immediately regains terminal control.
- **`Ctrl + D` (`EOF`)**: Gracefully terminates the shell session, reaping child processes and cleaning up resources.

---

### 14. Job Control: `fg` and `bg`
- **`fg <pid>`**:
  - Brings a background process (running or stopped) into the foreground.
  - Hands over terminal control via `tcsetpgrp()`.
  - Sends `SIGCONT` if stopped.
  - Waits for process completion or signal suspension.
- **`bg <pid>`**:
  - Resumes a stopped background process in the background by sending `SIGCONT`.

---

### 15. Process Creation Monitor: `neonate`
Monitors the kernel for process generation:
- **Usage**: `neonate -n [time_interval]`
- Queries `/proc/loadavg` periodically every `time_interval` seconds and prints the PID of the most recently spawned process.
- Switches terminal to raw non-canonical mode via POSIX `termios`.
- The user can terminate monitoring at any time by pressing `'x'`.

---

### 16. Networked Manual Fetcher: `iMan`
Fetches manual pages dynamically over the web using low-level POSIX TCP sockets:
- **Usage**: `iMan <command_name>`
- Opens a socket connection to `man.he.net` on port 80.
- Transmits an HTTP request:
  ```http
  GET /?topic=<command_name>&section=all HTTP/1.1
  Host: man.he.net
  ```
- Reads the response stream, parses out HTTP headers, filters HTML tags, and prints clean manual page text directly to the console.

---

## Build and Run

### Prerequisites
A Linux environment (Ubuntu 20.04+, Debian, Fedora, or WSL2 on Windows) with `gcc` and `make`:

```bash
sudo apt-get update
sudo apt-get install -y build-essential
```

---

### Compilation

Clone the repository and build using the provided `Makefile`:

```bash
# Clone repository
git clone https://github.com/Nitin2004-bit/C-Shell.git
cd C-Shell

# Compile the shell binary
make

# Run the shell
./shell
```

### Makefile Targets
- `make` or `make all`: Compiles all source files with `-Wall -Wextra -g` into the `shell` executable.
- `make run`: Builds and immediately launches the shell.
- `make clean`: Removes all compiled `.o` object files and executables.

---

## Testing & Example Sessions

### 1. Navigation & Listing
```text
<nitin@laptop:~> hop OSN
/home/nitin/OSN
<nitin@laptop:~/OSN> reveal -l
-rw-r--r-- 1 nitin nitin 19486 Oct 24 20:24 README.md
-rwxr-xr-x 1 nitin nitin 59888 Oct 24 22:21 shell
<nitin@laptop:~/OSN> hop -
/home/nitin
```

### 2. Pipelines & I/O Redirection
```text
<nitin@laptop:~> cat < sample.txt | grep "test" | wc -l > count.txt
<nitin@laptop:~> cat count.txt
42
```

### 3. Background Job Control
```text
<nitin@laptop:~> sleep 30 &
[1] 45120
<nitin@laptop:~> activities
45120 : sleep 30 - Running
<nitin@laptop:~> ping 45120 19
Sent signal 19 to process with pid 45120
<nitin@laptop:~> activities
45120 : sleep 30 - Stopped
<nitin@laptop:~> fg 45120
(Process brought to foreground...)
```

### 4. Network Manual Fetching
```text
<nitin@laptop:~> iMan sleep
NAME
       sleep - delay for a specified amount of time

SYNOPSIS
       sleep NUMBER[SUFFIX]...
...
```

---

## Authors & Acknowledgments

- **Nitin** ([@Nitin2004-bit](https://github.com/Nitin2004-bit))
- Developed for **Operating Systems and Networks (OSN Monsoon 2024)**, International Institute of Information Technology, Hyderabad (IIIT-H).
