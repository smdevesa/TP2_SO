# Custom Kernel from Scratch

This project is a custom-built kernel designed to demonstrate low-level OS functionality and efficient resource management from the ground up. Developed entirely from scratch, this kernel includes essential operating system components with features aimed at achieving both simplicity and modularity.

### Key Features

- **Dual Memory Management**: Supports two memory allocators—a naïve memory manager for basic allocation and deallocation, and a buddy allocator for more sophisticated, efficient memory usage in dynamic environments.
  
- **Named Semaphore System**: Implements a named semaphore mechanism to manage resource access and synchronization across processes, using assembly-level `XCHG` operations for locking and safe concurrent usage.

- **Blocking I/O Management**: Includes functionality for blocking I/O operations, enabling processes to wait on I/O tasks, thereby improving resource handling and system responsiveness.

- **Unidirectional Pipe System**: Provides one-way pipes for inter-process communication, allowing efficient data transfer between processes.

- **Preemptive Round-Robin Scheduler with Priorities**: Utilizes a preemptive round-robin scheduler with priority support to manage multitasking. Each process is assigned a priority level, which influences CPU allocation and ensures that high-priority tasks receive timely execution.

## Installation Instructions

### Prerequisites
- **Docker**: Ensure Docker is installed and running on your system. You can download it from [Docker’s official website](https://www.docker.com/).

### Steps

1. **Clone the Repository**:
   ```bash
   git clone git@github.com:smdevesa/TP2_SO.git
   cd TP2_SO.git
   ```

2. **Pull and run the Docker image**:
   ```bash
   docker pull agodio/itba-so-multi-platform:3.0
   docker run -v ${PWD}:/root --security-opt seccomp:unconfined -ti agodio/itba-so-multi-platform:3.0
   ```
3. **Inside the container, compile the kernel**:
   ```bash
   cd root/Toolchain/
   make all
   cd ..
   make all # or 'make buddy' to use buddy allocator
   exit
   ```

## Usage Instructions

First of all you have to execute the script:
```bash
./run.sh
```

To demonstrate the full functionality of the kernel, several user-space applications are provided. Most of these applications serve as wrappers for system calls and showcase how different kernel features operate.

### Available Applications

Below is a list of available applications, along with a brief description of their usage and functionality.

---

### Shell and Utilities

- **sh**: User shell to execute applications. 
  - **Foreground/Background Execution**: Add `&` at the end of a command to run it in the background.
  - **Pipe**: Connect two applications via a pipe by using `|`, e.g., `app1 | app2`.
  - **Special Key Support**: Supports `Ctrl+D` for end-of-file and `Ctrl+C` to terminate the foreground process.

- **help**: Displays a list of all available commands.

---

### Physical Memory Management

- **mem**: Displays the current state of the memory.

---

### Process and Scheduling Management

- **ps**: Shows a list of all processes along with properties such as name, PID, priority, stack base pointer and other relevant details.

- **loop**: Prints its process ID and a message every specified number of seconds. Usage example:
  ```bash
  loop <interval_in_ticks>
  ```

- **kill**: Terminates a process by its PID. Example usage:
  ```bash
  kill <pid>
  ```

- **nice**: Changes the priority of a process by specifying its PID and new priority level. Example usage:
    ```bash
    nice <pid> <new_priority>
    ```

- **block**: Blocks a process by its PID. Example usage:
    ```bash
    block <pid> 
    ```

- **unblock**: Unblocks a process by its PID. Example usage:
    ```bash
    unblock <pid> 
    ```

### Inter-Process Communication

- **cat**: Reads from stdin and outputs the text as it receives it.

- **wc**: Counts the number of lines in the input. Can be used in combination with `ps` or other streams. Example usage:
    ```bash
    ps | wc
    ```

- **filter**: Filters vowels from the input and displays the result. Example usage:
    ```bash
    cat ps | filter
    ```

- **phylo**: Demonstrates the dining philosophers problem, managing synchronization and resource-sharing among processes. Example usage:
    ```bash
    phylo
    ```

## Authors

- **Santiago Devesa**
- **Tiziano Fuchinecco**
- **Tomás Rafael Balboa Koolen**