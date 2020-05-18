#!/bin/bash
docker-compose -f docker/build.yml up --abort-on-container-exit
docker-compose -f docker/build.yml down