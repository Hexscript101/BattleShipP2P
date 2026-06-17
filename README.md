# BattleShipP2P

A peer-to-peer networked Battleship game written in C, built on top of an existing local two-player implementation. Two machines connect over TCP and play against each other — each client manages its own game state locally, exchanging only attack coordinates and hit results over the network.

## Authors

- [Hexscript101](https://github.com/Hexscript101) 
- [Edoelel](https://github.com/Edoelel)
- [Toothbrush](https://github.com/Toothbrush)

## Project Structure
 
```
BattleShipP2P/
├── original/        # Original local two-player implementation and early sketches
├── version 1/
│   ├── mainPort.c       # Self-contained networked version (single file, fully functional)
│   ├── main.c           # Modular refactor entry point (in progress)
│   ├── functions.c      # Game and network logic (in progress)
│   └── functions.h      # Header for modular version (in progress)
└── README.md
```


## How It Works

The game uses a minimal peer-to-peer architecture over TCP:

- The first player to launch acts as the **server** and listens for an incoming connection
- The second player connects as the **client**
- Once connected, players take turns exchanging moves:
  - **Attack packet** — `[row][col]`
  - **Result packet** — hit or miss
- All game logic (hit detection, sinking, win condition) runs **locally** on each machine — only the minimum necessary data is sent over the network

## Build
 
Compile from the single-file version:
 
```bash
gcc -std=c99 -o battleship "version 1/mainPort.c"
```
 
Or, for the modular version:
 
```bash
gcc -std=c99 -o battleship main.c functions.c
```

### Makefile:
```bash
make
```
then:
```bash
./battleship <mod>
```
## Usage
 
**Player 1 — Server** (launches first, waits for connection on port `4444`):
 
```bash
./battleship server
```
 
**Player 2 — Client** (connects to Player 1's machine):
 
```bash
./battleship client
```
 
The client will prompt for the server's IP address and port interactively.
 
### Fleet composition
 
Each player gets a randomly placed fleet on a 10×10 grid:
 
| Ship | Cells |
|---|---|
| Frigate × 2 | 2 each |
| Submarine | 3 |
| Battleship | 4 |
| Carrier | 5 |


## License

MIT