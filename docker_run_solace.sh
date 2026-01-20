#!/bin/bash
CONTAINER_NAME="solace-broker"
IMAGE="solace/solace-pubsub-standard"

if [ "$(docker ps -aq -f name=${CONTAINER_NAME})" ]; then
    echo "Existing container found."
    if [ "$(docker ps -q -f name=${CONTAINER_NAME})" ]; then
        echo "Container is already running."
    else
        echo "Starting container..."
        docker start ${CONTAINER_NAME}
    fi
else
    echo "Creating new container..."
    docker run -d -p 8080:8080 -p 55555:55555 -p 8008:8008 --shm-size=2g --env username_admin_globalaccesslevel=admin --env username_admin_password=admin --name ${CONTAINER_NAME} ${IMAGE}
    echo "Starting..."
fi

echo "--------------------------------------------------"
echo "Management UI: http://localhost:8080"
echo "Messaging Port: 55555"
echo "Admin User: admin / admin"
echo "--------------------------------------------------"