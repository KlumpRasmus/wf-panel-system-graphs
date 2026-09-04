#!/bin/sh
set -eu

project_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
build_dir="$project_dir/build"

if [ "$(id -u)" -eq 0 ]; then
    printf '%s\n' "Run this installer as your normal desktop user, not as root."
    exit 1
fi

if command -v findmnt >/dev/null 2>&1 &&
        [ "$(findmnt -n -o FSTYPE / 2>/dev/null || true)" = "overlay" ]; then
    printf '%s\n' \
        "WARNING: / is an overlay filesystem." \
        "The plugin will disappear after reboot unless you first disable Overlay FS" \
        "in raspi-config and reboot."
    printf '%s' "Continue with a temporary installation? [y/N] "
    IFS= read -r answer
    case "$answer" in
        y|Y|yes|YES) ;;
        *) exit 1 ;;
    esac
fi

printf '%s\n' "Installing build tools and the small native panel dependencies..."
sudo apt update
sudo apt install -y \
    build-essential meson ninja-build pkg-config \
    libgtkmm-3.0-dev libglm-dev wf-panel-pi-dev lxtask

multiarch=$(dpkg-architecture -qDEB_HOST_MULTIARCH)

if [ -d "$build_dir" ]; then
    meson setup --wipe "$build_dir" \
        --prefix=/usr --libdir="lib/$multiarch"
else
    meson setup "$build_dir" \
        --prefix=/usr --libdir="lib/$multiarch"
fi

meson compile -C "$build_dir"
meson test -C "$build_dir" --print-errorlogs
sudo meson install -C "$build_dir"

printf '\n%s\n' \
    "Installed CPU Custom, GPU Custom, and Memory." \
    "Log out and back in so wf-panel-pi loads the new libraries." \
    "Then remove the built-in CPU/GPU entries and add the three custom graphs."
