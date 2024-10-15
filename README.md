# LumettaOS

**LumettaOS** is a Unix-based operating system designed and implemented as part of my semester-long project for ECE 391. The kernel was developed from scratch using C and AT&T x86 assembly, modeled after the Linux kernel.

## Features

**Paged Virtual Memory Management**
   - Implemented paged virtual memory for effective memory isolation between user and kernel space, ensuring memory protection and efficient memory utilization.
  
**Interrupt Handling**
   - Custom interrupt descriptor table (IDT) setup for handling hardware interrupts (e.g., keyboard input) and software interrupts (e.g., system calls).
  
**Preemptive Task Scheduling**
   - Support for round-robin scheduling of multiple user processes. The OS can interrupt and switch between tasks based on a virtualized clock, ensuring efficient CPU utilization.

4. **In-Memory File System**
   - A lightweight, fast, in-memory file system to manage file operations within the kernel space, avoiding disk I/O for simplicity in this educational setting.

5. **Multiple Terminals**
   - Simultaneous support for multiple user terminals, allowing users to run and interact with independent processes in different sessions.

6. **User and Kernel Privileges**
   - The OS enforces privilege separation between user-mode and kernel-mode operations, ensuring that user processes cannot interfere with system-level functionality.

7. **System Calls**
   - Basic system calls were implemented to allow user programs to interact with the OS for performing essential operations, such as process creation, file management, and I/O.

8. **USB/PS2 Keyboard Input**
   - Integrated support for USB and PS2 keyboard devices, allowing real-time user input to be processed by the OS.

9. **Full-Color VGA Mode-X Output**
   - Support for graphical output in VGA Mode-X, enabling a wide range of visual operations with full-color display capabilities.

10. **Virtualized Clock**
   - The OS leverages a virtual clock to manage task scheduling, timing of interrupts, and other time-sensitive processes in a predictable and manageable way.

## How to Build and Run

1. **Requirements**:
   - x86 emulator (e.g., QEMU or Bochs)
   - GNU toolchain for cross-compiling C and assembly code (GCC, Binutils)

2. **Building LumettaOS**:
   - Clone this repository:
     ```bash
     git clone https://github.com/your-username/LumettaOS.git
     ```
   - Navigate to the source directory:
     ```bash
     cd LumettaOS/student-distrib
     ```
   - Install dependencies:
     ```bash
     make dep
     ```
   - Build the OS (bootimg) and create the QEMU disk image (mp3.img):
     ```bash
     sudo make
     ```

## Acknowledgments

This project was completed as part of **ECE 391: Computer Systems Engineering** at the University of Illinois at Urbana-Champaign, taught by **Prof. Steven Lumetta**. I want to thank the numerous TAs for their constant help and advice, and my fellow students I worked so many late nights with, who provided valuable feedback and guidance throughout the development process.