#!/bin/bash
# run_replier.sh

# 기본 설정값
HOST="tcp:localhost:55555"
USER="default@default"
TOPIC="solace/samples/intro"

echo "Starting BasicReplier..."
./bin/BasicReplier -c $HOST -u $USER -t $TOPIC
