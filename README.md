# obd-hardware
The software stack for the Freematics ONE+ dongle. Future hardware can be added


## Getting Started
This assumes a macOS and Sublime Text development based environment. Termius must be installed in sublime text.

1. Clone the repo (including submodules)
2. Install  PlatformIO (`brew install platformio`)
3. Open `platformio.sublime-project`.
4. Choose `Tools>Build System>PlatformIO`.
5. Open `src/carduino.ino` and choose `Tools>Build with...`. Pick `Upload and Monitor` with the device connected. Command B can be used on subsequent runs.

Alternatively, the project can be run from the command line via:
```bash
pio -c sublimetext run -t monitor
```
