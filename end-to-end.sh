#!/usr/bin/env bash

# Usage:
#   ./end-to-end.sh y   Clean, build (full + standalone catalog/server/client),
#                       run the tests, then start everything in tmux.
#   ./end-to-end.sh     Skip the build and start everything from the existing
#                       standalone builds.

# Stop script if compilation fails
set -e

TEST=$1

SESSION="hega-db"

# These ports are hardcoded in the applications:
#   catalog : catalog/src/service/main.cpp  (0.0.0.0:50052)
#   server  : server/src/main.cpp           (0.0.0.0:50051)
# and are the defaults of the client (client/src/ClientConfig.cpp).
CATALOG_PORT=50052
SERVER_PORT=50051

CATALOG_BIN="./build-catalog/catalog/catalog_microservice"
SERVER_BIN="./build-server/server/hegadb_server_app"
CLIENT_BIN="./build-client/client/hegadb_client"

# Waits until something is listening on 127.0.0.1:<port>
# Usage: wait_for_port <port> [timeout-seconds]
wait_for_port() {
  local port=$1
  local timeout=${2:-30}
  local i

  for ((i = 0; i < timeout * 10; i++)); do
    if (exec 3<>"/dev/tcp/127.0.0.1/${port}") 2>/dev/null; then
      return 0
    fi
    sleep 0.1
  done

  return 1
}

if [[ "$TEST" = "y" ]]; then

  # To remove all previous builds
  rm -rf build*

  # Full build (catalog + server + client) with tests

  cmake -S . -B build

  cmake --build build -j$(nproc)

  # Test everything

  ctest --test-dir build --output-on-failure

  # Catalog standalone build

  cmake -S . -B build-catalog \
    -DHEGADB_BUILD_CATALOG=ON \
    -DHEGADB_BUILD_SERVER=OFF \
    -DHEGADB_BUILD_CLIENT=OFF \
    -DHEGADB_BUILD_TESTS=OFF

  cmake --build build-catalog -j$(nproc)

  # Server standalone build

  cmake -S . -B build-server \
    -DHEGADB_BUILD_CATALOG=OFF \
    -DHEGADB_BUILD_SERVER=ON \
    -DHEGADB_BUILD_CLIENT=OFF \
    -DHEGADB_BUILD_TESTS=OFF

  cmake --build build-server -j$(nproc)

  # Client standalone build

  cmake -S . -B build-client \
    -DHEGADB_BUILD_CATALOG=OFF \
    -DHEGADB_BUILD_SERVER=OFF \
    -DHEGADB_BUILD_CLIENT=ON \
    -DHEGADB_BUILD_TESTS=OFF

  cmake --build build-client -j$(nproc)

fi

# Make sure the standalone binaries exist before opening tmux

for bin in "$CATALOG_BIN" "$SERVER_BIN" "$CLIENT_BIN"; do
  if [[ ! -x "$bin" ]]; then
    echo "Missing $bin - run './end-to-end.sh y' to build first." >&2
    exit 1
  fi
done

# Setting up a tmux session:
#   window 1: 3 panes side by side -> catalog | server | client
#   window 2: editor
# Pane ids are used instead of pane numbers so this also works when
# tmux is configured with a non-zero pane-base-index.

tmux kill-session -t "$SESSION" 2>/dev/null || true

CATALOG_PANE=$(tmux new-session -d -s "$SESSION" -n services -P -F '#{pane_id}')
SERVER_PANE=$(tmux split-window -h -t "$CATALOG_PANE" -P -F '#{pane_id}')
CLIENT_PANE=$(tmux split-window -h -t "$SERVER_PANE" -P -F '#{pane_id}')
tmux select-layout -t "$CATALOG_PANE" even-horizontal

tmux new-window -t "$SESSION" -n editor

# Start in order: catalog -> server -> client.
# Each service is given time to start listening before the next one starts,
# because the client fetches the catalog as soon as it starts.

tmux send-keys -t "$CATALOG_PANE" "$CATALOG_BIN 2> catalog-run_error.log" Enter
if ! wait_for_port "$CATALOG_PORT" 30; then
  echo "Warning: catalog service is not listening on port $CATALOG_PORT" >&2
fi

tmux send-keys -t "$SERVER_PANE" "$SERVER_BIN 2> server-run_error.log" Enter
if ! wait_for_port "$SERVER_PORT" 30; then
  echo "Warning: server is not listening on port $SERVER_PORT" >&2
fi

tmux send-keys -t "$CLIENT_PANE" "$CLIENT_BIN 2> client-run_error.log" Enter

# Attach to the first window (services) with the client pane active

tmux select-window -t "$CATALOG_PANE"
tmux select-pane -t "$CLIENT_PANE"

# Attaching the tmux session to foreground

tmux attach -t "$SESSION"
