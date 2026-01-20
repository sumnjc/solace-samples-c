#!/bin/bash
# run_async_requestor.sh

# 기본 설정값
HOST="tcp:localhost:55555"
USER="default@default"
TOPIC="solace/samples/intro"

echo "Starting AsyncRequestor (Async)..."
./bin/AsyncRequestor -c $HOST -u $USER -t $TOPIC
