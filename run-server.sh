#!/bin/bash
set -e

# NUSANTARA ROLEPLAY - open.mp Server Runner
# Runs open.mp via qemu-i386-static in /dev/shm to avoid btrfs 64-bit inode EOVERFLOW.
# Automatically syncs gamemodes and database to/from ~/workspace.

SRC_DIR="/home/hatch/workspace/openmp-gamemode/Server"
RUN_DIR="/dev/shm/openmp-server"

echo "=== [open.mp] Preparing Nusantara Roleplay Server ==="

mkdir -p "$RUN_DIR"
mkdir -p "$RUN_DIR/gamemodes"
mkdir -p "$RUN_DIR/plugins"
mkdir -p "$RUN_DIR/components"
mkdir -p "$RUN_DIR/scriptfiles"

# Extract base open.mp server if not already present
if [ ! -f "$RUN_DIR/omp-server" ]; then
    echo "[1/4] Extracting base open.mp binaries and components..."
    tar -xzf "/home/hatch/workspace/openmp-gamemode/openmp-server.tar.gz" --strip-components=1 -C "$RUN_DIR"
fi

# Sync plugins and components
echo "[2/4] Syncing plugins, components, and libraries..."
if [ -f "$SRC_DIR/plugins/mysql-R41-4-Debian/plugins/mysql.so" ]; then
    cp "$SRC_DIR/plugins/mysql-R41-4-Debian/plugins/mysql.so" "$RUN_DIR/plugins/"
    cp "$SRC_DIR/plugins/mysql-R41-4-Debian/log-core.so" "$RUN_DIR/"
fi
if [ -f "$SRC_DIR/plugins/samp_bcrypt.so" ]; then
    cp "$SRC_DIR/plugins/samp_bcrypt.so" "$RUN_DIR/plugins/"
fi
if [ -f "$SRC_DIR/plugins/components/sscanf.so" ]; then
    cp "$SRC_DIR/plugins/components/sscanf.so" "$RUN_DIR/components/"
fi

# Sync compiled AMX and configuration
echo "[3/4] Syncing main.amx and config.json..."
cp "$SRC_DIR/gamemodes/main.amx" "$RUN_DIR/gamemodes/main.amx"
cp "$SRC_DIR/config.json" "$RUN_DIR/config.json"

# Ensure config settings (disable public announce)
sed -i 's/"announce": true/"announce": false/' "$RUN_DIR/config.json"

echo "[4/4] Starting open.mp server on port 7777..."

cleanup() {
    echo ""
    echo "=== [open.mp] Server stopped ==="
    if [ -f "$RUN_DIR/log.txt" ]; then
        cp "$RUN_DIR/log.txt" "$SRC_DIR/server_latest.log"
    fi
}
trap cleanup EXIT INT TERM

cd "$RUN_DIR"
exec qemu-i386-static -d unimp,guest_errors -L /usr/i386-linux-gnu ./omp-server "$@"
