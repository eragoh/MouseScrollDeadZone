# Mouse Scroll Dead Zone

Prevents accidental scrolling when middle-clicking or barely touching the scroll on Linux mice with high-resolution
scroll wheels. It worked perfectly with my MX Master 2s.

## Installation

**1. Install dependencies:**

```bash
# Fedora
sudo dnf install libevdev-devel gcc-c++
```

Replace `/dev/input/event22` in the code with your mouse device (find it with `sudo libinput list-devices`).

**2. Compile and run:**

```bash
g++ -o mouse-scroll-dead-zone main.cpp -levdev
sudo ./mouse-scroll-dead-zone
```

## Auto-start on Boot

**1. Copy to system:**

```bash
sudo cp mouse-scroll-dead-zone /usr/local/bin/
```

**2. Create service file:**

```bash
sudo nano /etc/systemd/system/mouse-scroll-dead-zone.service
```

Paste this:

```ini
[Unit]
Description = Mouse Scroll Dead Zone Filter
After = multi-user.target

[Service]
Type = simple
ExecStart = /usr/local/bin/mouse-scroll-dead-zone
Restart = always
User = root

[Install]
WantedBy = multi-user.target
```

**3. Enable it:**

```bash
sudo systemctl enable mouse-scroll-dead-zone
sudo systemctl start mouse-scroll-dead-zone
```

## Adjust Sensitivity

Edit `main.cpp` and change:

```cpp
constexpr int DEAD_ZONE = 60;  // Higher = less sensitive
```

Then recompile and restart the service.

## Uninstall

```bash
sudo systemctl stop mouse-scroll-dead-zone
sudo systemctl disable mouse-scroll-dead-zone
sudo rm /usr/local/bin/mouse-scroll-dead-zone
sudo rm /etc/systemd/system/mouse-scroll-dead-zone.service
```

## License

MIT License - do whatever you want with it.