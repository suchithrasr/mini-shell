# Mini Shell

A Unix-like command-line shell built from scratch in C using Linux system calls.

## Features
- Execute external commands (ls, cat, grep, date, sleep, etc.)
- Built-in commands: cd, pwd, echo, exit, jobs, fg, bg
- Piping between multiple commands: `ls -l | grep main | wc -l`
- Background execution using `&`
- Job control — suspend with Ctrl+Z, resume with fg/bg
- Signal handling — SIGINT, SIGTSTP, SIGCHLD
- Custom prompt rename using PS1=
- Auto-cleanup of background zombie processes via SIGCHLD

## Technologies Used
- Language: C
- OS: Linux (Ubuntu)
- Concepts: fork(), exec(), waitpid(), pipe(), dup2(), signals, linked list

## How to Compile
```bash
make
```

## How to Run
```bash
./shell
```

## Usage Examples
```bash
myshell$: ls -l
myshell$: ls -l | wc
myshell$: sleep 30 &
myshell$: jobs
myshell$: fg
myshell$: PS1=suchi$:
myshell$: cd /home
myshell$: echo $$
myshell$: cat file.txt | grep hello | wc -l
```

## Project Structure
| File | Purpose |
|---|---|
| main.c | Global variables and entry point |
| scan_input.c | Main shell loop, fork, signal handler |
| commands.c | Command classification and execution |
| linked_list.c | Job table for background and stopped processes |
| header.h | All declarations and includes |
| ext_cmds.txt | List of supported external commands |