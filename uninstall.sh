#!/bin/sh
set -eu

multiarch=$(dpkg-architecture -qDEB_HOST_MULTIARCH)
memory_plugin="/usr/lib/$multiarch/wf-panel-pi/libmemory.so"
cpu_plugin="/usr/lib/$multiarch/wf-panel-pi/libcpugraph.so"
gpu_plugin="/usr/lib/$multiarch/wf-panel-pi/libgpugraph.so"
memory_metadata="/usr/share/wf-panel-pi/metadata/memory.xml"
cpu_metadata="/usr/share/wf-panel-pi/metadata/cpugraph.xml"
gpu_metadata="/usr/share/wf-panel-pi/metadata/gpugraph.xml"

sudo rm -f -- \
    "$memory_plugin" "$cpu_plugin" "$gpu_plugin" \
    "$memory_metadata" "$cpu_metadata" "$gpu_metadata"

printf '%s\n' \
    "Removed the custom CPU, GPU, and Memory plugin files." \
    "Log out and back in to unload them from wf-panel-pi."
