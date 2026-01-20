#!/bin/bash
# docker_reset_solace.sh

CONTAINER_NAME="solace-broker"

echo "!!! WARNING !!!"
echo "This will DELETE the existing Solace broker container and ALL data."
echo "You have 5 seconds to cancel (Ctrl+C)..."
sleep 5

echo "Stopping and removing container: ${CONTAINER_NAME}..."

# 컨테이너가 존재하는지 확인 후 삭제
if [ "$(docker ps -aq -f name=${CONTAINER_NAME})" ]; then
    docker rm -f ${CONTAINER_NAME}
    echo "Container removed."
else
    echo "Container not found. Nothing to remove."
fi

echo "Starting fresh Solace broker..."
./docker_run_solace.sh
