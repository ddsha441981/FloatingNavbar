# FloatingNavbar - AI-Powered Interview Assistant

[![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)]()
[![Qt6](https://img.shields.io/badge/Qt-6-green.svg)](https://www.qt.io/product/qt6)
[![C++20](https://img.shields.io/badge/C++-20-blue.svg)]()
[![Groq API](https://img.shields.io/badge/Groq-API-orange.svg)](https://groq.com)
[![Platform](https://img.shields.io/badge/platform-Windows-lightgrey.svg)]()
[![License](https://img.shields.io/badge/license-MIT-blue.svg)]()

---

## 📌 Overview

**FloatingNavbar** is a **C++ Qt6 application** designed to assist with **technical interviews**.
It provides **real-time audio transcription**, **screenshot analysis**, and **AI-powered question answering**, with seamless integration to external apps via **named pipes**.

---

## ✨ Features

* 🎙️ **Real-time Audio Capture**: System audio + voice activity detection
* 🤖 **AI Integration**: Groq API for intelligent answers
* 🖼️ **Screenshot Analysis**: Automated capture & AI-driven insights
* ❓ **Question Extraction**: Detect & classify interview questions
* 🔌 **Named Pipe Communication**: Inter-process integration
* 📌 **Floating UI**: Always-on-top toolbar
* 🪟 **Multi-Window Interface**: Separate windows for transcript, prompts, and responses

---

## 🛠️ Prerequisites

### Dependencies

* **Qt6** (Core, Widgets, Network, Multimedia, WebSockets, Concurrent)
* **CMake** (>= 3.10)
* **C++20** compiler

**External Libraries (included as submodules):**

* [QHotkey](https://github.com/Skycoder42/QHotkey) → global hotkeys
* [libfvad](https://github.com/dpirch/libfvad) → voice activity detection

### System Requirements

* **Windows** (primary supported platform with named pipes)
* **Audio System** with system audio capture
* **Internet** for Groq API integration

---
## Images

![This is an alt text.](/snapshoots/image1.JPG "This is a sample image.")
![This is an alt text.](/snapshoots/image2.JPG "This is a sample image.")

---

## 📂 Project Structure

```
FloatingNavbar/
├── src/                          # Core source files
│   ├── main.cpp                  # Entry point
│   ├── navbar.cpp/.h             # Floating toolbar
│   ├── AudioCaptureManager.cpp/.h # Audio capture
│   ├── GroqStreamer.cpp/.h       # Groq API integration
│   ├── QuestionExtractor.cpp/.h  # Question analysis
│   └── ...
├── external/                     # Dependencies
│   ├── QHotkey/                  
│   └── libfvad/                  
├── audioserver/                  # Go-based audio server
├── audio-source/                 # Audio capture binaries
├── screen-capture/               # Screenshot processor
└── configs/                      # Config files
```

---

## 🚀 Installation

### 1. Clone Repo

```bash
git clone <repository-url>
cd FloatingNavbar
git submodule update --init --recursive
```

### 2. Install Qt6

**Windows (via vcpkg):**

```bash
vcpkg install qt6-base qt6-multimedia qt6-websockets qt6-concurrent
```

### 3. Configure Build

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
```

### 4. Build Project

```bash
cmake --build . --config Release
```

### 5. Setup Config

```bash
mkdir -p ~/mytools/data/
echo "API_KEY=your_groq_api_key_here" > ~/mytools/data/config.txt
```

`configs/wsocket.ini`:

```ini
[WebSocket]
url=ws://localhost:8080/ws
```

### 6. External Components

* **Audio Server** → Go binary in `../audioserver/`
* **Audio Capture** → binaries in `../audio-source/`
* **Screen Capture** → binary in `../screen-capture/`

---

## 📖 Usage

### Start App

```bash
./FloatingNavbar
```

It will:

* Start pipe servers
* Launch Go audio server
* Show floating navbar

### Main Controls

* **Prompt** → Load predefined prompts
* **Capture** → Take & analyze screenshots
* **Ask/Help** → Send transcript to AI
* **Start/Stop Listening** → Toggle audio capture
* **Transcript** → Show/hide transcript window
* **More** → Settings, model selection, reset

### Hotkeys

* `Ctrl + Alt + Shift + N` → Toggle navbar

---

## ⚙️ Configuration

### Audio

* VAD sensitivity
* 16kHz sample rate
* 1000ms frame chunks
* Silence detection: 120 chunks

### AI

* Default model: `llama-3.3-70b-versatile`
* Supports streaming responses
* Confidence-based question extraction
* Custom prompt file: `~/mytools/data/prompt.txt`

### UI

* Always-on-top
* Smooth animations
* Dark theme + transparency

---

## 🐛 Troubleshooting

**Audio not working**

* Check binaries & permissions
* Ensure WebSocket server is running

**AI not responding**

* Check API key & internet
* Verify Groq quota

**Screenshot fails**

* Ensure binary is executable
* Check screen recording permissions

**Pipe errors**

* Run as admin (Windows)
* Verify pipe names aren’t conflicting

**Debug mode**:

```bash
QT_LOGGING_RULES="*=true"
```

---

## 👨‍💻 Development

* Modular C++ design
* Qt signal-slot event architecture
* Asynchronous UI operations
* Resource-safe memory handling

---

## 📜 License

[MIT](LICENSE)

## 🤝 Contributing

PRs welcome! See [CONTRIBUTING.md](CONTRIBUTING.md)

## 📬 Support

For issues/requests → open a GitHub issue

---
