#!/usr/bin/env bash

# Stop script if compilation fails
set -e

# To remove all previous builds
rm -rf build*

TEST=$1

if [[ "$TEST" = "y" ]]; then

  # Full client + server build

  cmake -S . -B build

  cmake --build build -j$(nproc)

  # Test client + server

  ctest --test-dir build --output-on-failure

  # Client standalone build

  cmake -S . -B build-client \
    -DHEGADB_BUILD_SERVER=OFF \
    -DHEGADB_BUILD_CLIENT=ON \
    -DHEGADB_BUILD_TESTS=OFF

  cmake --build build-client -j$(nproc)

  # Server standalone build

  cmake -S . -B build-server \
    -DHEGADB_BUILD_SERVER=ON \
    -DHEGADB_BUILD_CLIENT=OFF \
    -DHEGADB_BUILD_TESTS=OFF

  cmake --build build-server -j$(nproc)

fi

# Setting up a tmux session with 1 window and 2 planes (horizontally split)

tmux new-session -d -s hega-db
tmux split-window -h -t hega-db
tmux new-window -t hega-db -n editor

# Sending commands to start server followed by client

if [[ "$TEST" = "y" ]]; then

  tmux send-keys -t hega-db:0.0 "./build-server/server/hegadb_server_app 2> server-run_error.log" Enter
  tmux send-keys -t hega-db:0.1 "./build-client/client/hegadb_client 2> client-run_error.log" Enter

fi

# Attaching the tmux session to foreground

tmux attach -t hega-db
