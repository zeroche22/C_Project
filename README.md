# Keylogger Project

**Members:**  
- Manzhos Yaroslav  
- Zaporoshchuk Valeria  
- Lapukhov Darii

## Overview
Two-part keylogger written in C: a Windows client captures keystrokes and streams JSON payloads to a server. The server stores raw events per client and runs a lightweight analyzer to surface interesting snippets.

## Architecture (high level)
```
Windows client
  hook (WH_KEYBOARD_LL) 
  buffer 
  JSON (keys+timestamps)
  local file (keylog_*.json)
  TCP send to server

server
  listener (TCP 8080) 
  thread per client 
  storage (data/clients/<ip>/raw/*.json)
  key_decoder (JSON → text)
  analysis (find '@' contexts)
  results (data/clients/<ip>/analysis/*_result.json)
```

## Environment Setup
**Client (Windows)**
- Build with MinGW-w64 or MSVC that provides Winsock2 headers/libs.
- Update `SERVER_IP` and `SERVER_PORT` in `keylogger/main/main_keylogger.c`.
- Use a shell with `make` support (MinGW/MSYS or similar) and ensure `-lws2_32` links.

## Repository Structure
```
keylogger/
├── include/
│   ├── cJSON.h
│   └── .
├── main/
│   └── main_keylogger.c
├── src/
│   └── cJSON.c
├── Makefile
│
├── server/
│   ├── include/
│   │   ├── analysis.h
│   │   ├── client_handler.h
│   │   ├── key_decoder.h
│   │   ├── server.h
│   │   └── storage.h
│   │
│   ├── src/
│   │   ├── analysis.c
│   │   ├── client_handler.c
│   │   ├── key_decoder.c
│   │   ├── server.c
│   │   └── storage.c
│   ├── tests/
│   │   ├── test_all.cpp
│   │
│   └── Makefile
│
├── .gitignore
└── README.md
```

## Building & Running
**Server**
```sh
cd server
make
./json_server
```
Server data is written under `data/clients/`.

**Run tests**
```sh
cd server
make test
```

**Client**
```sh
cd keylogger
make
./main/main_keylogger.exe
```
The client writes `keylog_<date>_<time>.json` locally and sends the same payload to the server if reachable.

## Key Modules and Functions
- `server/src/server.c`: `run_server(port)` validates/binds/listens; spawns `pthread` per connection; `main` sets up storage and starts the loop.
- `server/src/client_handler.c`: `handle_client(sock, ip)` receives JSON chunks, picks next index, saves raw payload, decodes text, and triggers analysis; closes the socket on exit.
- `server/src/storage.c`: manages `data/clients/<ip>/raw` and `analysis` directories; `storage_get_next_index` inspects existing files; `storage_save_raw`/`storage_save_analysis` write numbered files.
- `server/src/key_decoder.c`: `extract_text_from_json(json, out, out_size)` walks `"key"` fields, handling `SPACE`, `ENTER`, `BACKSPACE`, ignoring `SHIFT`, and truncating safely.
- `server/src/analysis.c`: `analyze_text_and_store(ip, index, text)` scans decoded text for `@`, captures surrounding context, JSON-escapes snippets, and stores results with a match count.

## Key Implementation Details
- File layout per client: `data/clients/<ip>/raw/%04d.json` and `data/clients/<ip>/analysis/%04d_result.json`, where the index increments from existing files.
- Backspace handling mutates the in-memory buffer before saving analysis, mirroring user corrections.
- The analyzer throttles adjacent `@` matches (skips if closer than 5 chars) to reduce noise.
- Tests (`server/tests/test_all.cpp`) cover key decoding, storage indexing/saving, and the analysis output format.
