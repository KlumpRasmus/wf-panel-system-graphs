# Custom system graphs for wf-panel-pi

Three native Raspberry Pi OS taskbar plugins for `wf-panel-pi`: **CPU Custom**,
**GPU Custom**, and **Memory**. They share the same wide graph renderer, colours,
and centered `C:16%`, `G:16%`, and `M:16%` label style. Each opens LXTask when
left-clicked. A long press/right-click continues to open the standard panel
menu.

The plugins read the same native Linux/Raspberry Pi statistics interfaces used
by the standard indicators and have no GNOME or Flatpak runtime dependency.

## Supported system

- Raspberry Pi OS with the Wayland/labwc desktop and `wf-panel-pi` 1.10+
- 32-bit or 64-bit Raspberry Pi OS (the installer detects the library path)

Tested on a Raspberry Pi 5 running Raspberry Pi OS Trixie with
`wf-panel-pi` 1.13.

It is not an LXPanel plugin and will not appear on the older X11/LXPanel
desktop.

The Meson dependency intentionally has no numeric version constraint because
some Raspberry Pi OS releases report the installed `wf-panel-pi` pkg-config
version as `undefined`.

## Install

Clone the repository and run the installer:

```sh
git clone https://github.com/KlumpRasmus/wf-panel-system-graphs.git
cd wf-panel-system-graphs
chmod +x install.sh uninstall.sh
./install.sh
```

Alternatively, download and extract a GitHub source archive, open a terminal
in the extracted directory, then run:

```sh
chmod +x install.sh uninstall.sh
./install.sh
```

The installer adds the compiler and development packages using APT, builds all
three plugins, runs their parser tests, and installs six files under `/usr`. It
also installs LXTask if needed.

Log out and back in after installation because `wf-panel-pi` keeps loaded plugin
libraries in memory. Then right-click the panel and choose **Add / Remove
Plugins**. Remove the standard **CPU** and **GPU** entries and add **CPU Custom**,
**GPU Custom**, and **Memory**.

If Raspberry Pi OS Overlay FS is enabled, disable it in `raspi-config` and
reboot before installing if you want the installation to survive a reboot.

## Use and configuration

- Left-click: open LXTask.
- Hover: show the current usage. Memory also shows used and total RAM.
- Right-click or long-press: open the regular panel menu.
- In the panel's plugin preferences: toggle the percentage and choose graph
  foreground/background colours.

The installer also creates `~/.config/wf-panel-system-graphs.conf`. It controls
the three graph colours independently and the shared background, text, and
shadow colours:

```ini
cpu=#53c86b
gpu=#53c86b
memory=#53c86b
background=rgba(64,64,64,0.55)
text=white
shadow=rgba(0,0,0,0.65)
```

The file takes precedence over the equivalent panel colour preferences. GTK
colour names, `#RRGGBB`, and `rgba(...)` values are accepted. Saved changes are
picked up automatically within about 1.5 seconds; no panel restart is needed.

Memory uses `MemTotal - MemAvailable`. CPU uses deltas from `/proc/stat`. GPU
uses the Pi V3D `gpu_stats` interface, with the older debugfs `gpu_usage`
interface as a fallback.

## Uninstall

Remove the plugin from the panel first, then run:

```sh
./uninstall.sh
```

Log out and back in afterward.

## Build manually

```sh
sudo apt install build-essential meson ninja-build pkg-config \
  libgtkmm-3.0-dev libglm-dev wf-panel-pi-dev lxtask
meson setup build --prefix=/usr \
  --libdir="lib/$(dpkg-architecture -qDEB_HOST_MULTIARCH)"
meson compile -C build
meson test -C build --print-errorlogs
sudo meson install -C build
```

## License

BSD-3-Clause. The module interface and compatible CPU/GPU sampling behavior are
based on Raspberry Pi Ltd's BSD-licensed `pplug-cpu` and `pplug-gpu` projects.
