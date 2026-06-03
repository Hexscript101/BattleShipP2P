### TCP Client-Server
Basic TCP client and server implementation from scratch.

**What it covers:**
- Socket creation and configuration
- Address structures (`sockaddr_in`, byte order conversion with `htons`)
- Connection handling (`connect()`, `bind()`, `listen()`, `accept()`)
- Data transmission (`send()`, `recv()`)
- Multi-connection server loops

**How to compile:**
```bash
g++ -o server server.cpp
g++ -o client client.cpp
```

**Usage:**
```bash
./server          # Starts server on localhost:{line 35 of "server.cpp" file}
./client          # Connects to server
```