# 🚦 Traffic Simulation System

A **multi-threaded traffic simulation system** developed for an **Operating Systems course**. It models traffic flow through multiple intersections with support for emergency vehicles and intelligent traffic light controllers.

---

## ✨ Features

### 🔄 Multi-threaded Architecture
Specialized threads for:
- 🚦 Traffic light control  
- 🚗 Vehicle generation  
- 🚨 Emergency vehicle handling  
- 🛣️ Vehicle movement between roads  
- 📟 Real-time GUI updates  
- ⚙️ Configuration management  
- 📊 System performance monitoring  

### ⚙️ Dynamic Configuration
- 🔄 **Hot-reloading** of `config.txt`  
- Adjustable parameters:
  - ⏱️ Simulation duration  
  - 🔁 Number of intersections and roads  
  - 🚘 Vehicle generation rates  
  - 🚨 Emergency vehicle limits  
  - 🧠 Scheduling policies  

### 🖥️ Real-time GUI (via ncurses)
- 📍 Visual representation of intersections  
- 🚦 Traffic light states: RED / GREEN / YELLOW  
- 🔢 Vehicle counts per road  
- 🚨 Emergency vehicle indicators  
- 📊 System statistics dashboard  

### 🚨 Emergency Handling
- ⏱️ **Priority scheduling** for emergency vehicles  
- ⚠️ Special logic for critical situations  
- 📈 Emergency vehicle statistics tracking  

### 📈 Performance Monitoring
- 🧠 Real-time CPU usage  
- 🧮 Memory consumption tracking  
- 📊 Detailed traffic statistics  

### 📝 Logging System
- ⏰ Timestamped event logging  
- 📚 Comprehensive simulation records  
- 💾 Persistent log storage (`traffic_sim.log`)  

---

## 🧠 Operating System Concepts Implemented

### 🧵 Multi-threading
- POSIX `pthreads` for concurrent execution  
- Mutexes for thread-safe operations  
- Condition variables for synchronization  

### 🔒 Process Synchronization
- Mutex locks for shared resource protection  
- Condition variables for efficient waiting  
- Atomic operations on shared data  

### 🔄 Inter-Process Communication (IPC)
- `inotify` for dynamic config monitoring  
- Shared memory for intersection states  
- Conditional signaling between threads  

### ⚙️ Resource Management
- Dynamic memory allocation  
- File I/O for config and logging  
- System-level resource tracking  

### 🗂️ Scheduling Algorithms
- Priority-based scheduling for emergency vehicles  
- Adaptive traffic light timing  
- Load-based duration calculation  

### ⏱️ Real-time Systems
- Timed operations with `usleep`  
- Periodic thread execution  
- Real-time GUI/statistics updates  

---

## 🧰 Prerequisites

- 🐧 **Linux environment**  
- 🛠️ GCC compiler  
- 📚 Ncurses library  
- 🔗 POSIX Threads (`pthreads`) support  

---

## 📁 File Structure (optional)

```bash
traffic_simulation/
├── src/
│   ├── config.c
│   ├── gui.c
│   ├── ipc.c
│   └── logger.c
│   ├── main.c
│   └── main.h
│   ├── monitoring.c
│   └── scheduler.c
├── config.txt
├── Makefile
└── README.md
├── traffic_sim.log (this will be created at the run time)
```

---

## 🏁 Getting Started

```bash
# Clone the repo
git clone https://github.com/kinza7124/traffic_simulation.git
cd traffic_simulation

# Build the project
make clean
make 

# Run the simulation
./traffic_sim
```

---


## 📜 License

This project is licensed under the MIT License. See `LICENSE` for more information.
