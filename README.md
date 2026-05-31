# BattleShipP2P

A peer-to-peer networked Battleship game written in C, built on top of an existing local two-player implementation. Two machines connect over TCP and play against each other — each client manages its own game state locally, exchanging only attack coordinates and hit results over the network.

## Authors

- [Hexscript101](https://github.com/Hexscript101) 
- [Edoelel](https://github.com/Edoelel)
- [Toothbrush](https://github.com/Toothbrush)

## Project Structure

```
BattleShipP2P/
├── original/        # Original local two-player implementation and a raw sketch with some ideas
└── README.md
```

> The networked version is under active development

## How It Works

The game uses a minimal peer-to-peer architecture over TCP:

- The first player to launch acts as the **server** and listens for an incoming connection
- The second player connects as the **client**
- Once connected, players take turns exchanging moves:
  - **Attack packet** — `[row][col]`
  - **Result packet** — hit or miss
- All game logic (hit detection, sinking, win condition) runs **locally** on each machine — only the minimum necessary data is sent over the network

## Build


> Build instructions will be updated once the networked codebase is added.

## Requirements

- GCC or any C99-compatible compiler
- POSIX-compatible OS (Linux, macOS) — uses BSD sockets (`sys/socket.h`)
- Local network connectivity between the two machines

## Status

| Component | Status |
|---|---|
| Local two-player game | ✅ Complete |
| TCP connection setup | 🔄 In progress |
| Network protocol | 🔄 In progress |
| Turn management | ⏳ Planned |

## License

MIT