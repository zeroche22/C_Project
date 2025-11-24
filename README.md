# Keylogger Project

**Members:**  
- Manzhos Yaroslav  
- Zaporoshchuk Valeria  
- Lapukhov Darii

## Overview
Two-part keylogger written in C: a Windows client that captures keystrokes and a server that stores and analyzes the captured events. The client serializes keystrokes to JSON, saves them locally, and streams the same payload to the server over TCP. The server keeps raw payloads per client and runs a simple text analyzer to surface interesting snippets.

## Repository Structure
- `README.md` — project description and usage notes.
- `keylogger/` — Windows client: `main/main_keylogger.c`, bundled `cJSON` sources, `Makefile` producing `main/main_keylogger.exe`.
- `server/` — POSIX server: socket listener with threaded client handling, storage, key decoding, and analysis helpers plus `Makefile` for `json_server`.

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
│   │
│   └── Makefile
│
├── .gitignore
└── README.md


## How It Works
1. **Client hook & buffering**: `keylogger/main/main_keylogger.c` installs a low-level keyboard hook (WinAPI `WH_KEYBOARD_LL`). Pressed keys are normalized (letters, digits, special keys like `SPACE`, `ENTER`, `BACKSPACE`) and appended to an in-memory ring (max 30 entries).
2. **Flush to JSON**: On buffer fill or shutdown, the client builds a JSON object with an array of `{ "key": "<KEY>", "time": "<YYYY-MM-DD HH:MM:SS>" }` entries, writes it to a timestamped file (`keylog_<date>_<time>.json`), and sends the same string to the server. Connection details are set via `SERVER_IP` and `SERVER_PORT` macros.
3. **Server ingest**: `server/json_server` listens on TCP port 8080 by default. Each client connection runs in its own thread. Every received JSON payload is saved under `data/clients/<ip>/raw/<index>.json`, where `<ip>` is the client address and `<index>` is an incrementing counter per client.
4. **Decoding & analysis**: The server extracts readable text from the JSON keys (handling spacing, newlines, backspaces, and ignoring shift markers). It then scans the reconstructed text for `@` symbols, capturing surrounding context for quick inspection. Results are written to `data/clients/<ip>/analysis/<index>_result.json`.

## Building & Running
- **Server (Linux/macOS):**
  - Build and run:  
    ```sh
    cd server
    make run
    ```
  - Change the port by passing an argument: `./json_server 9000`.
  - Server data lives under `data/clients/`.
- **Client (Windows):**
  - Build with MinGW or MSVC toolchain that provides Winsock2 headers/libraries:  
    ```sh
    cd keylogger
    make
    ```
  - Set `SERVER_IP`/`SERVER_PORT` in `keylogger/main/main_keylogger.c` to point at your server.
  - Run `main/main_keylogger.exe`; it will log to local JSON files and stream to the server if reachable.
