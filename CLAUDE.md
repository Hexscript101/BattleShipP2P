# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

A peer-to-peer networked Battleship game written in C using TCP sockets. Two machines connect over TCP/IP and play against each other, with each client managing its own game state locally.

## Project Structure

```
BattleShipP2P/
├── original/              # Original local two-player implementation
│   └── battagliaNavale.c  # Single-player vs AI local game
├── version1/              # Network-enabled version (active development)
│   ├── main.c             # Entry point (modular refactor)
│   ├── func.c             # Game logic and network functions
│   ├── functions.h        # Header file with declarations
│   ├── mainPort.c         # Self-contained networked version (fully functional)
│   ├── Makefile           # Build configuration
│   └── BUGS_REPORT.md     # Documented bugs and suggested fixes
├── References/            # Reference implementations
│   └── client_server_tcp/ # TCP client/server examples in C++
└── README.md              # Project documentation
```

## Build Commands

```bash
# Build the modular version
make

# Or compile directly with gcc
gcc -std=c99 -Wall -Wextra -O2 -o battleship main.c func.c

# Build the self-contained version
gcc -std=c99 -o battleship "version1/mainPort.c"

# Clean build artifacts
make clean
```

## Run Commands

**Player 1 (Server)** - launches first, waits for connection on port 4444:
```bash
./battleship server
```

**Player 2 (Client)** - connects to Player 1's machine:
```bash
./battleship client
```
Then enter the server's IP address and port when prompted.

## Architecture

### Network Protocol
- Player 1 acts as TCP server on port 4444
- Player 2 connects as TCP client
- Minimal protocol: attack coordinates (row, col) sent as chars, hit/miss status returned

### Game Boards
- `boardG1M` / `boardG2M` - Main boards (ships hidden from opponent)
- `boardG1S` / `boardG2S` - Shot tracking boards ('X' = hit, 'O' = miss)

### Ship Representation
- `'A'` = Frigate (2 cells) - stem cell + `'+'` connecting cell
- `'V'` = Submarine (3 cells) - stem cell + `'+'` connecting cells (vertical)
- `'<'` = Battleship (4 cells) - stem cell + `'+'` connecting cells (horizontal)
- `'>'` = Carrier (5 cells) - stem cell + `'+'` connecting cells

### Key Functions (func.c)
- `create_socket()` - Create TCP socket
- `set_up_server()` - Bind, listen, accept connection
- `set_up_client()` - Connect to server
- `send_attack()` - Send row/col as chars
- `recv_char()` - Receive a char from network
- `send_status()` - Send hit (1) or miss (0) status
- `game_loop()` - Main game loop handling turns
- `gen_ships()` - Generate random ship placement

## Known Issues

See `version1/BUGS_REPORT.md` for documented bugs including:
- Off-by-one bounds checking errors
- Incorrect turn-based logic (players defending when they should attack)
- Missing input validation and buffer clearing
- Potential buffer over-reads in ship placement

## Development Notes

- Uses global variables for game state (could be refactored into a struct)
- Turn logic is currently based on a single counter that determines both attacker and defender roles
- The `mainPort.c` version is self-contained and fully functional
- The modular version (`main.c` + `func.c`) is still in progress