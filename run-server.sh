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
mkdir -p "$RUN_DIR/scriptfiles/data"
mkdir -p "$SRC_DIR/scriptfiles/data"

# Copy base server components if not already there
if [ ! -f "$RUN_DIR/omp-server" ]; then
    echo "[1/4] Extracting base open.mp binaries and components..."
    tar -xzf "/home/hatch/workspace/openmp-gamemode/openmp-server.tar.gz" --strip-components=1 -C "$RUN_DIR"
fi

# Sync compiled AMX and configs from workspace
echo "[2/4] Syncing gamemode and configs from workspace..."
cp "$SRC_DIR/gamemodes/roleplay.amx" "$RUN_DIR/gamemodes/roleplay.amx"
if [ -f "$SRC_DIR/scriptfiles/data/roleplay.db" ]; then
    cp "$SRC_DIR/scriptfiles/data/roleplay.db" "$RUN_DIR/scriptfiles/data/roleplay.db"
fi

# Ensure config.json uses roleplay gamemode and announce is false
sed -i 's/"announce": true/"announce": false/' "$RUN_DIR/config.json"
sed -i 's/"enable": true/"enable": false/' "$RUN_DIR/config.json" # disable web artwork server
sed -i 's/"gungame 1"/"roleplay 1"/' "$RUN_DIR/config.json"

echo "[3/4] Starting open.mp server on port 7777..."

# Cleanup trap to sync database back on exit
cleanup() {
    echo ""
    echo "=== [open.mp] Shutting down server and syncing data back to workspace ==="
    if [ -f "$RUN_DIR/scriptfiles/data/roleplay.db" ]; then
        cp -u "$RUN_DIR/scriptfiles/data/roleplay.db" "$SRC_DIR/scriptfiles/data/roleplay.db" || cp "$RUN_DIR/scriptfiles/data/roleplay.db" "$SRC_DIR/scriptfiles/data/roleplay.db"
        echo "[Sync] roleplay.db successfully synced to workspace!"
    fi
    if [ -f "$RUN_DIR/log.txt" ]; then
        cp "$RUN_DIR/log.txt" "$SRC_DIR/server_latest.log"
    fi
}
trap cleanup EXIT INT TERM

cd "$RUN_DIR"
exec qemu-i386-static -d unimp,guest_errors -L /usr/i386-linux-gnu ./omp-server "$@"
